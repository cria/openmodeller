#!/bin/bash

#######################################################################
# Script to be included in the cronjob. It checks if there are requests
# (sample points, create, test, evaluate or project model) in a 
# specific directory and runs the corresponding commands. It depends on
# a configuration file that must be passed as a parameter (otherwise it 
# will try to read a file called server.conf in the same directory).
# This script can process both OMWS 1.0 and OMWS 2.0 requests, so you
# can have a single scheduler even if the same machine has both service 
# versions running.
# Parameter 1: path to configuration file.
# Parameter 2: sleep interval
#######################################################################

# Function to read configuration file
function readconf {
 
  while read line; do
    # skip comments
    [[ ${line:0:1} == "#" ]] && continue
 
    # skip empty lines
    [[ -z "$line" ]] && continue
 
    # got a config line eval it
    eval $line
 
  done < "$1"
}

# Function to read XML
# source: http://stackoverflow.com/questions/893585/how-to-parse-xml-in-bash
read_dom () {
    local IFS=\>
    read -d \< ENTITY CONTENT
    local ret=$?
    TAG_NAME=${ENTITY%% *}
    ATTRIBUTES=${ENTITY#* }
    return $ret
}

# Function to read key value pairs (for XML attributes)
# Created to avoid using "eval" on external content
read_pairs () {
    local IFS="="
    read KEY VALUE
    local ret=$?
    return $ret
}

# Find layer id and try to open it with openModeller
find_id_and_check_layer () {

  while read_pairs; do

    if [[ $KEY == "Id" ]] ; then

      # Remove first and last quotes
      VALUE="${VALUE%\"}"
      VALUE="${VALUE#\"}"

      # This won't exit the program if the layer doesn't exist or can't be opened
      # (in this case users will get the error later, when the job fails). However 
      # this command may cache remote layers locally.
      "$OM_BIN_DIR"/om_layer --check "$VALUE" --config-file "$OM_CONFIGURATION"
    fi
  done
}

# Configuration file can be passed as a parameter
if [[ "$1" ]]; then
  CONFIG=$1
else
  CONFIG="server.conf"
fi  

# If configuration file exists, read the configuration
if [ -f $CONFIG ]; then
  # read configuration
  readconf $CONFIG
else
  echo "Error: no configuration file available"
  exit 0
fi

if [[ "$SYSTEM_STATUS" eq 2 ]]; then
  echo "System disabled by configuration (SYSTEM_STATUS)"
  exit 0
fi

# Second parameter is sleep interval so we can be running several
# schedulers at different intervals
sleep $2

# Don't allow number of concurrent jobs to exceed a maximum
if [[ "$CONDOR_INTEGRATION" == "no" ]]; then

  if [ ! -d "$PID_DIRECTORY" ]; then

    mkdir "$PID_DIRECTORY"
  fi

  PROCS=0

  for PID in `find $PID_DIRECTORY/* -type f -printf "%f\n" 2> /dev/null`; do

    TEST_RUNNING=`ps -p $PID | grep $PID`

    if [ -z "${TEST_RUNNING}" ]; then
      # Process is not running for some reason, so remove pid file
      rm -f "$PID_DIRECTORY/$PID"
    else
      # Process is running
      PROCS=$((PROCS+1))
    fi
  done

  if [ $PROCS -ge $MAX_SHELL_JOBS ]; then

    exit 1
  fi

  touch "$PID_DIRECTORY/$$"
fi

# Process oldest request
ls -t $TICKET_DIRECTORY/*_req.* 2> /dev/null | tail -n -1 | while read req; do

  # variable req contains the full path

  # strip everything before the ticket string, which is separated by a dot, to get the ticket
  # note: ## = longest possible match, so directory names can contain dot in their names
  ticket="${req##*.}" 
  filename="${req##*/}" # strip directories
  jobtype="${filename%%_*}" # remove everything after _ to get job type

  moved=$TICKET_DIRECTORY"/"$jobtype"_proc."$ticket

  # Rename file, avoiding that another process takes it
  mv "$req" "$moved"

  # Exit program if last command fails
  if [ $? -ne 0 ]; then
    echo "Warning: request file not found. Aborting."
    exit 1
  fi

  log=$TICKET_DIRECTORY"/"$ticket
  prog=$TICKET_DIRECTORY"/prog."$ticket
  meta=$TICKET_DIRECTORY"/job."$ticket

  # Try to open all layers beforehand, depending on configuration
  if [[ "$CHECK_LAYERS" == "yes" ]]; then

    while read_dom; do
      # Avoid closing tags
      if [[ ${TAG_NAME:0:1} != "/" ]] ; then
        # Map or Mask tags, with or without namespace prefix
        if [[ $TAG_NAME == "Map" || ${TAG_NAME:(-4)} == ":Map" || $TAG_NAME == "Mask" || ${TAG_NAME:(-5)} == ":Mask" ]] ; then
          # TODO: avoid duplications, as the same layer may be repeated
          # TODO: parallelize with xargs

          # Convert white spaces to newlines and feed check_layer
          echo "$ATTRIBUTES" | sed 's/ /\n/g' | find_id_and_check_layer
        fi
      fi
    done < "$moved"
  fi  

  # Projection jobs need additional variables
  if [[ "$jobtype" == "proj" ]]; then

    map_base=$DISTRIBUTION_MAP_DIRECTORY"/proc_"$ticket

    # define image file extension based on filetype
    filetype=`cat $moved | grep -Eo 'FileType="\w+"'`
    proj_type=`echo $filetype | sed 's/FileType="//; s/".*//'`
    case "$proj_type" in
      GreyTiff)
        img_ext=".tif"
        map_file=$map_base$img_ext ;;
      GreyTiff100)
        img_ext=".tif"
        map_file=$map_base$img_ext ;;
      FloatingTiff)
        img_ext=".tif"
        map_file=$map_base$img_ext ;;
      GreyBMP)
        img_ext=".bmp" 
        map_file=$map_base$img_ext ;;
      ByteASC)
        img_ext=".asc" 
        map_file=$map_base$img_ext ;;
      FloatingASC)
        img_ext=".asc" 
        map_file=$map_base$img_ext ;;
      *)
        img_ext=".img"
        map_file=$map_base$img_ext ;;
    esac

    map_ige=$map_base".ige"
    stats=$TICKET_DIRECTORY"/stats."$ticket
  else
    # All other jobs produce a _resp. file
    resp=$TICKET_DIRECTORY"/"$jobtype"_resp."$ticket
  fi  

  # if condor_integration then create the script and submit
  if [[ "$CONDOR_INTEGRATION" == "yes" ]]; then
    echo "universe        = vanilla" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    if [[ "$jobtype" == "samp" ]]; then
      echo "arguments = --xml-req "$moved" --result "$resp" --prog-file "$prog" --config-file "$OM_CONFIGURATION>> $TICKET_DIRECTORY"/condor_sc."$ticket
      echo "executable = "$NODE_BIN_DIR"/om_pseudo " >> $TICKET_DIRECTORY"/condor_sc."$ticket
    fi
    if [[ "$jobtype" == "model" ]]; then
      echo "arguments = --xml-req "$moved" --model-file "$resp" --prog-file "$prog" --config-file "$OM_CONFIGURATION>> $TICKET_DIRECTORY"/condor_sc."$ticket
      echo "executable = "$NODE_BIN_DIR"/om_model " >> $TICKET_DIRECTORY"/condor_sc."$ticket
    fi
    if [[ "$jobtype" == "eval" ]]; then
      echo "arguments = --xml-req "$moved" --result "$resp" --prog-file "$prog" --config-file "$OM_CONFIGURATION>> $TICKET_DIRECTORY"/condor_sc."$ticket
      echo "executable = "$NODE_BIN_DIR"/om_evaluate " >> $TICKET_DIRECTORY"/condor_sc."$ticket
    fi
    if [[ "$jobtype" == "test" ]]; then
      echo "arguments = --xml-req "$moved" --result-file "$resp" --prog-file "$prog" --config-file "$OM_CONFIGURATION>> $TICKET_DIRECTORY"/condor_sc."$ticket
      echo "executable = "$NODE_BIN_DIR"/om_test " >> $TICKET_DIRECTORY"/condor_sc."$ticket
    fi
    if [[ "$jobtype" == "proj" ]]; then
      echo "arguments = --xml-req "$moved" --dist-map "$DISTRIBUTION_MAP_DIRECTORY"/"$ticket$img_ext" --stat-file "$stats" --prog-file "$prog" --config-file "$OM_CONFIGURATION>> $TICKET_DIRECTORY"/condor_sp."$ticket
      echo "executable = "$NODE_BIN_DIR"/om_project" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    fi
    echo "environment = "$NODE_ENVIRONMENT >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "output      = "$TICKET_DIRECTORY"/om_"$jobtype"_log"$ticket".out" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "error       = "$TICKET_DIRECTORY"/om_"$jobtype"_log"$ticket".err" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "log         = "$TICKET_DIRECTORY"/om_"$jobtype"_log"$ticket".log" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "getenv = true" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "should_transfer_files = yes" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "when_to_transfer_output = on_exit" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "queue" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "" >> $TICKET_DIRECTORY"/condor_sc."$ticket

    # TODO: create done.ticket file in the end!

    # TODO: post-process projections

    export CONDOR_CONFIG="$CONDOR_CONFIG"
    "$CONDOR_BIN_DIR"/condor_submit $TICKET_DIRECTORY"/condor_sc."$ticket
  else
    # no Condor - execute command directly
    if [[ "$jobtype" == "samp" ]]; then
      "$OM_BIN_DIR"/om_pseudo --xml-req "$moved" --result "$resp" --log-file "$log" --prog-file "$prog" --config-file "$OM_CONFIGURATION"
    fi
    if [[ "$jobtype" == "model" ]]; then
      "$OM_BIN_DIR"/om_model --xml-req "$moved" --model-file "$resp" --log-file "$log" --prog-file "$prog" --config-file "$OM_CONFIGURATION"
    fi
    if [[ "$jobtype" == "eval" ]]; then
      "$OM_BIN_DIR"/om_evaluate --xml-req "$moved" --result "$resp" --log-file "$log" --prog-file "$prog" --config-file "$OM_CONFIGURATION"
    fi
    if [[ "$jobtype" == "test" ]]; then
      "$OM_BIN_DIR"/om_test --xml-req "$moved" --result "$resp" --log-file "$log" --prog-file "$prog" --config-file "$OM_CONFIGURATION"
    fi
    if [[ "$jobtype" == "proj" ]]; then
      "$OM_BIN_DIR"/om_project --xml-req "$moved" --dist-map "$map_file" --stat-file "$stats" --log-file "$log" --prog-file "$prog" --config-file "$OM_CONFIGURATION"

      finalmap_file=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket$img_ext
      finalmap_ige=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".ige"

      if [ -e $map_ige ]; then
        mv "$map_ige" "$finalmap_ige"
      fi

      # ASCII format creates additional files, so rename them first
      if [[ "$proj_type" == "ByteASC" || "$proj_type" == "FloatingASC" ]]; then

        orig_deriv1=$map_base".asc.aux.xml"
        orig_deriv2=$map_base".prj"

        final_deriv1=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".asc.aux.xml"
        final_deriv2=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".prj"

        mv "$orig_deriv1" "$final_deriv1"
        mv "$orig_deriv2" "$final_deriv2"
      fi

      # This must be the last step, since getProgress will only return 100% if
      # the final map exists
      mv "$map_file" "$finalmap_file"
    fi

    # If this is part of an experiment
    if [ -e $meta ]; then
      if grep -q "EXP=" "$meta"; then
        # Run experiment manager to trigger other actions
        "$OM_BIN_DIR"/omws_manager --config "$CONFIG" --ticket "$ticket" > /dev/null 2> /dev/null
      fi
    fi

    finished=$TICKET_DIRECTORY"/done."$ticket
    touch "$finished"
  fi

  # Read configuration again to check if status changed
  readconf $CONFIG
  if [[ "$SYSTEM_STATUS" eq 2 ]]; then
    echo "System disabled by configuration (SYSTEM_STATUS)"
    exit 0
  fi

done

if [[ "$CONDOR_INTEGRATION" == "no" ]]; then
  rm -f "$PID_DIRECTORY/$$"
fi
