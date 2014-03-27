#!/bin/bash

#######################################################################
# Script to be included in the cronjob. It checks if there are requests
# (create model, test model or project model) in a specific directory 
# and runs them. It depends on a configuration file that must be passed
# as a parameter (otherwise it will try to read a file called 
# server.conf in the same directory).
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

# Process oldest sampling request
ls -t $TICKET_DIRECTORY/samp_req.* 2> /dev/null | tail -n -1 | while read req; do

  ticket="${req##*.}"
  moved=$TICKET_DIRECTORY"/samp_proc."$ticket
  resp=$TICKET_DIRECTORY"/samp_resp."$ticket
  log=$TICKET_DIRECTORY"/"$ticket
  prog=$TICKET_DIRECTORY"/prog."$ticket
  meta=$TICKET_DIRECTORY"/job."$ticket

  # Rename file, avoiding that another process take it
  mv "$req" "$moved"

  # if condor_integration then create the script and submit
  if [[ "$CONDOR_INTEGRATION" == "yes" ]]; then
    echo "universe        = vanilla" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "arguments = --xml-req "$moved" --result "$resp" --prog-file "$prog" --config-file "$OM_CONFIGURATION>> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "executable = "$NODE_BIN_DIR"/om_pseudo " >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "environment = "$NODE_ENVIRONMENT >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "output          = "$TICKET_DIRECTORY"/om_samp_log"$ticket".out" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "error           = "$TICKET_DIRECTORY"/om_samp_log"$ticket".err" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "log             = "$TICKET_DIRECTORY"/om_samp_log"$ticket".log" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "getenv = true" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "should_transfer_files = yes" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "when_to_transfer_output = on_exit" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "queue" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "" >> $TICKET_DIRECTORY"/condor_sc."$ticket

    export CONDOR_CONFIG="$CONDOR_CONFIG"
    "$CONDOR_BIN_DIR"/condor_submit $TICKET_DIRECTORY"/condor_sc."$ticket
  else
    # no Condor - execute om_pseudo directly
    "$OM_BIN_DIR"/om_pseudo --xml-req "$moved" --result "$resp" --log-file "$log" --prog-file "$prog" --config-file "$OM_CONFIGURATION"

    # If this is part of an experiment
    if [ -e $meta ]; then
      if grep -q "EXP=" "$meta"; then
        # Run experiment manager to trigger other actions
        "$OM_BIN_DIR"/omws_manager --config "$CONFIG" --ticket "$ticket" 
      fi  
    fi

    finished=$TICKET_DIRECTORY"/done."$ticket
    touch "$finished"
  fi
done

# Process oldest model request
ls -t $TICKET_DIRECTORY/model_req.* 2> /dev/null | tail -n -1 | while read req; do

  ticket="${req##*.}"
  moved=$TICKET_DIRECTORY"/model_proc."$ticket
  resp=$TICKET_DIRECTORY"/model_resp."$ticket
  log=$TICKET_DIRECTORY"/"$ticket
  prog=$TICKET_DIRECTORY"/prog."$ticket
  meta=$TICKET_DIRECTORY"/job."$ticket

  # Rename file, avoiding that another process take it
  mv "$req" "$moved"

  # if condor_integration then create the script and submit
  if [[ "$CONDOR_INTEGRATION" == "yes" ]]; then
    echo "universe        = vanilla" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "arguments = --xml-req "$moved" --model-file "$resp" --prog-file "$prog" --config-file "$OM_CONFIGURATION>> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "executable = "$NODE_BIN_DIR"/om_model " >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "environment = "$NODE_ENVIRONMENT >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "output          = "$TICKET_DIRECTORY"/om_model_log"$ticket".out" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "error           = "$TICKET_DIRECTORY"/om_model_log"$ticket".err" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "log             = "$TICKET_DIRECTORY"/om_model_log"$ticket".log" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "getenv = true" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "should_transfer_files = yes" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "when_to_transfer_output = on_exit" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "queue" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "" >> $TICKET_DIRECTORY"/condor_sc."$ticket

    export CONDOR_CONFIG="$CONDOR_CONFIG"
    "$CONDOR_BIN_DIR"/condor_submit $TICKET_DIRECTORY"/condor_sc."$ticket
  else
    # no Condor - execute om_model directly
    "$OM_BIN_DIR"/om_model --xml-req "$moved" --model-file "$resp" --log-file "$log" --prog-file "$prog" --config-file "$OM_CONFIGURATION"

    # If this is part of an experiment
    if [ -e $meta ]; then
      if grep -q "EXP=" "$meta"; then
        # Run experiment manager to trigger other actions
        "$OM_BIN_DIR"/omws_manager --config "$CONFIG" --ticket "$ticket" 
      fi  
    fi

    finished=$TICKET_DIRECTORY"/done."$ticket
    touch "$finished"
  fi  
done

# Process oldest evaluation request
ls -t $TICKET_DIRECTORY/eval_req.* 2> /dev/null | tail -n -1 | while read req; do

  ticket="${req##*.}"
  moved=$TICKET_DIRECTORY"/eval_proc."$ticket
  resp=$TICKET_DIRECTORY"/eval_resp."$ticket
  log=$TICKET_DIRECTORY"/"$ticket
  prog=$TICKET_DIRECTORY"/prog."$ticket
  meta=$TICKET_DIRECTORY"/job."$ticket

  # Rename file, avoiding that another process take it
  mv "$req" "$moved"

  # if condor_integration then create the script and submit
  if [[ "$CONDOR_INTEGRATION" == "yes" ]]; then
    echo "universe        = vanilla" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "arguments = --xml-req "$moved" --result "$resp" --prog-file "$prog" --config-file "$OM_CONFIGURATION>> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "executable = "$NODE_BIN_DIR"/om_evaluate " >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "environment = "$NODE_ENVIRONMENT >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "output          = "$TICKET_DIRECTORY"/om_eval_log"$ticket".out" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "error           = "$TICKET_DIRECTORY"/om_eval_log"$ticket".err" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "log             = "$TICKET_DIRECTORY"/om_eval_log"$ticket".log" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "getenv = true" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "should_transfer_files = yes" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "when_to_transfer_output = on_exit" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "queue" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "" >> $TICKET_DIRECTORY"/condor_sc."$ticket

    export CONDOR_CONFIG="$CONDOR_CONFIG"
    "$CONDOR_BIN_DIR"/condor_submit $TICKET_DIRECTORY"/condor_sc."$ticket
  else
    # no Condor - execute om_evaluate directly
    "$OM_BIN_DIR"/om_evaluate --xml-req "$moved" --result "$resp" --log-file "$log" --prog-file "$prog" --config-file "$OM_CONFIGURATION"

    # If this is part of an experiment
    if [ -e $meta ]; then
      if grep -q "EXP=" "$meta"; then
        # Run experiment manager to trigger other actions
        "$OM_BIN_DIR"/omws_manager --config "$CONFIG" --ticket "$ticket" 
      fi  
    fi

    finished=$TICKET_DIRECTORY"/done."$ticket
    touch "$finished"
  fi  
done

# Process oldest test request
ls -t $TICKET_DIRECTORY/test_req.* 2> /dev/null | tail -n -1 | while read req; do

  ticket="${req##*.}"
  moved=$TICKET_DIRECTORY"/test_proc."$ticket
  resp=$TICKET_DIRECTORY"/test_resp."$ticket
  log=$TICKET_DIRECTORY"/"$ticket
  prog=$TICKET_DIRECTORY"/prog."$ticket
  meta=$TICKET_DIRECTORY"/job."$ticket

  # Rename file, avoiding that another process take it
  mv "$req" "$moved"

  # if condor_integration then create the script and submit
  if [[ "$CONDOR_INTEGRATION" == "yes" ]]; then
    echo "universe        = vanilla" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "arguments = --xml-req "$moved" --result-file "$resp" --prog-file "$prog" --config-file "$OM_CONFIGURATION>> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "executable = "$NODE_BIN_DIR"/om_test " >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "environment = "$NODE_ENVIRONMENT >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "output          = "$TICKET_DIRECTORY"/om_test_log"$ticket".out" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "error           = "$TICKET_DIRECTORY"/om_test_log"$ticket".err" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "log             = "$TICKET_DIRECTORY"/om_test_log"$ticket".log" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "getenv = true" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "should_transfer_files = yes" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "when_to_transfer_output = on_exit" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "queue" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "" >> $TICKET_DIRECTORY"/condor_sc."$ticket

    export CONDOR_CONFIG="$CONDOR_CONFIG"
    "$CONDOR_BIN_DIR"/condor_submit $TICKET_DIRECTORY"/condor_sc."$ticket
  else
    # no Condor - execute om_test directly
    "$OM_BIN_DIR"/om_test --xml-req "$moved" --result "$resp" --log-file "$log" --prog-file "$prog" --config-file "$OM_CONFIGURATION"

    # If this is part of an experiment
    if [ -e $meta ]; then
      if grep -q "EXP=" "$meta"; then
        # Run experiment manager to trigger other actions
        "$OM_BIN_DIR"/omws_manager --config "$CONFIG" --ticket "$ticket" 
      fi  
    fi

    finished=$TICKET_DIRECTORY"/done."$ticket
    touch "$finished"
  fi  
done

# Process oldest projection request
ls -t $TICKET_DIRECTORY/proj_req.* 2> /dev/null | tail -n -1 | while read req; do

  ticket="${req##*.}"
  moved=$TICKET_DIRECTORY"/proj_proc."$ticket

  # Rename file, avoiding that another process take it
  mv "$req" "$moved"

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
  log=$TICKET_DIRECTORY"/"$ticket
  prog=$TICKET_DIRECTORY"/prog."$ticket
  meta=$TICKET_DIRECTORY"/job."$ticket

  # if condor_integration then create the script and submit
  if [[ "$CONDOR_INTEGRATION" == "yes" ]]; then
    echo "universe 	= vanilla" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "arguments = --xml-req "$moved" --dist-map "$DISTRIBUTION_MAP_DIRECTORY"/"$ticket$img_ext" --stat-file "$stats" --prog-file "$prog" --config-file "$OM_CONFIGURATION>> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "executable = "$NODE_BIN_DIR"/om_project" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "environment = "$NODE_ENVIRONMENT >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "output		= "$TICKET_DIRECTORY"/om_project_LOG"$ticket".out" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "error		= "$TICKET_DIRECTORY"/om_project_LOG"$ticket".err" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "log		= "$TICKET_DIRECTORY"/om_project_LOG"$ticket".log" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "getenv = true" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "should_transfer_files = yes" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "when_to_transfer_output = on_exit" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "queue" >> $TICKET_DIRECTORY"/condor_sp."$ticket

    export CONDOR_CONFIG="$CONDOR_CONFIG"
    "$CONDOR_BIN_DIR"/condor_submit $TICKET_DIRECTORY"/condor_sp."$ticket
  else
    # no Condor - execute om_project directly
    "$OM_BIN_DIR"/om_project --xml-req "$moved" --dist-map "$map_file" --stat-file "$stats" --log-file "$log" --prog-file "$prog" --config-file "$OM_CONFIGURATION"
  fi

  finalmap_file=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket$img_ext
  finalmap_ige=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".ige"

  if [ -e $map_ige ]; then
    mv "$map_ige" "$finalmap_ige"
  fi

  # Create PNG and KMZ files with pseudocolor (requires GDAL command-line tools).
  # This is still an unofficial feature!! Works only for ByteHFA
  if [[ "$proj_type" == "ByteHFA" ]]; then
    if [[ "$CREATE_PNG" == "yes" || "$CREATE_KMZ" == "yes" ]]; then
      if [ -e $map_file ]; then
        # Create a virtual raster
        initial_vrt_file=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".tmp.vrt"
        "$GDAL_BIN_DIR"/gdal_translate "$map_file" -of VRT "$initial_vrt_file"
        if [ -e $initial_vrt_file ]; then
          # Inject color table in VRT before </VRTRasterBand>
          final_vrt_file=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".vrt"
          colors=`cat $COLOR_TABLE`
          cat $initial_vrt_file | while read line;
          do
            trimmedline=`echo $line`
            if [ "$trimmedline" == "</VRTRasterBand>" ]; then
              echo "$colors" >> $final_vrt_file
            fi
            echo "$line" >> $final_vrt_file
          done
          if [ -e $final_vrt_file ]; then
            # Create PNG expanding to RGB
            png_file=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".png"
            "$GDAL_BIN_DIR"/gdal_translate "$final_vrt_file" -of PNG -expand rgb "$png_file"
            if [ -e $png_file ]; then
              if [[ "$CREATE_KMZ" == "yes" ]]; then
                # Create KMZ file based on PNG
                kmz_file=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".kmz"
                "$GDAL_BIN_DIR"/gdal_translate "$png_file" -of KMLSUPEROVERLAY -co format=png "$kmz_file"
              fi
              png_xml_file=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".png.aux.xml"
              if [ -e $png_xml_file ]; then
                rm -f "$png_xml_file"
              fi
            fi
            rm -f "$final_vrt_file"
          fi
          rm -f "$initial_vrt_file"
        fi
      fi
    fi
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

  # If this is part of an experiment
  if [ -e $meta ]; then
    if grep -q "EXP=" "$meta"; then
      # Run experiment manager to trigger other actions
      "$OM_BIN_DIR"/omws_manager --config "$CONFIG" --ticket "$ticket" 
    fi  
  fi

  finished=$TICKET_DIRECTORY"/done."$ticket
  touch "$finished"
done

if [[ "$CONDOR_INTEGRATION" == "no" ]]; then
  rm -f "$PID_DIRECTORY/$$"
fi