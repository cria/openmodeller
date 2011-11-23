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
function readconf() {
 
  while read line; do
    # skip comments
    [[ ${line:0:1} == "#" ]] && continue
 
    # skip empty lines
    [[ -z "$line" ]] && continue
 
    # got a config line eval it
    eval $line
 
  done < "$CONFIG"
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
  readconf
else
  echo "Error: no configuration file available"
  exit 0
fi

# TODO: check number of running processes and don't exceed a certain limit

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

# Process oldest model request
ls -t $TICKET_DIRECTORY/model_req.* 2> /dev/null | tail -n -1 | while read req; do

  ticket="${req##*.}"
  moved=$TICKET_DIRECTORY"/model_proc."$ticket
  resp=$TICKET_DIRECTORY"/model_resp."$ticket
  log=$TICKET_DIRECTORY"/"$ticket

  model_prog=$TICKET_DIRECTORY"/prog."$ticket

  # Rename file, avoiding that another process take it
  mv "$req" "$moved"

  # if condor_integration then create the script and submit
  if [[ "$CONDOR_INTEGRATION" == "yes" ]]; then
    echo "universe        = vanilla" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "arguments = --xml-req "$moved" --model-file "$resp" --prog-file "$model_prog>> $TICKET_DIRECTORY"/condor_sc."$ticket
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
    "$OM_BIN_DIR"/om_model --xml-req "$moved" --model-file "$resp" --log-file "$log" --prog-file "$model_prog"
  fi  
done

# Process oldest test request
ls -t $TICKET_DIRECTORY/test_req.* 2> /dev/null | tail -n -1 | while read req; do

  ticket="${req##*.}"
  moved=$TICKET_DIRECTORY"/test_proc."$ticket
  resp=$TICKET_DIRECTORY"/test_resp."$ticket
  log=$TICKET_DIRECTORY"/"$ticket

  test_prog=$TICKET_DIRECTORY"/prog."$ticket

  # Rename file, avoiding that another process take it
  mv "$req" "$moved"

  # if condor_integration then create the script and submit
  if [[ "$CONDOR_INTEGRATION" == "yes" ]]; then
    echo "universe        = vanilla" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "arguments = --xml-req "$moved" --result-file "$resp" --prog-file "$test_prog>> $TICKET_DIRECTORY"/condor_sc."$ticket
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
    "$OM_BIN_DIR"/om_test --xml-req "$moved" --result "$resp" --log-file "$log" --prog-file "$test_prog"
  fi  
done

# Process oldest projection request
ls -t $TICKET_DIRECTORY/proj_req.* 2> /dev/null | tail -n -1 | while read req; do

  ticket="${req##*.}"
  moved=$TICKET_DIRECTORY"/proj_proc."$ticket
  map_base=$DISTRIBUTION_MAP_DIRECTORY"/proc_"$ticket
  map_img=$map_base".img"
  map_ige=$map_base".ige"
  stats=$TICKET_DIRECTORY"/stats."$ticket
  log=$TICKET_DIRECTORY"/"$ticket
  proj_prog=$TICKET_DIRECTORY"/prog."$ticket

  # Rename file, avoiding that another process take it
  mv "$req" "$moved"

  # if condor_integration then create the script and submit
  if [[ "$CONDOR_INTEGRATION" == "yes" ]]; then
    echo "universe 	= vanilla" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "arguments = --xml-req "$moved" --dist-map "$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".img --stat-file "$stats" --prog-file "$proj_prog >> $TICKET_DIRECTORY"/condor_sp."$ticket
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
    "$OM_BIN_DIR"/om_project --xml-req "$moved" --dist-map "$map_img" --stat-file "$stats" --log-file "$log" --prog-file "$proj_prog"
  fi

  finalmap_img=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".img"
  finalmap_ige=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".ige"

  if [ -e $map_ige ]; then
    mv "$map_ige" "$finalmap_ige"
  fi

  # Create PNG and KMZ files with pseudocolor (requires GDAL command-line tools)
  if [[ "$CREATE_PNG" == "yes" ] || [ "$CREATE_KMZ" == "yes" ]]; then
    if [ -e $map_img ]; then
      # Create a virtual raster
      initial_vrt_file=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".tmp.vrt"
      "$GDAL_BIN_DIR"/gdal_translate "$map_img" -of VRT "$initial_vrt_file"
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

  # This must be the last step, since getProgress will only return 100% if
  # the final map exists
  mv "$map_img" "$finalmap_img"
done

if [[ "$CONDOR_INTEGRATION" == "no" ]]; then
  rm -f "$PID_DIRECTORY/$$"
fi
