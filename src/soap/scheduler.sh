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

# Process model requests
for req in `find $TICKET_DIRECTORY/model_req.* -type f 2> /dev/null`; do

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
    echo "arguments = --xml-req "$moved" --model-file "$resp" --calc-matrix --calc-roc --prog-file "$model_prog>> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "executable = /home/silvio/om_serial/bin/om_model " >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "environment = LD_LIBRARY_PATH=/home/silvio/om_serial/lib:/home/om/bin/lib_extern/lib" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "output          = "$TICKET_DIRECTORY"/om_model_log"$ticket".out" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "error           = "$TICKET_DIRECTORY"/om_model_log"$ticket".err" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "log             = "$TICKET_DIRECTORY"/om_model_log"$ticket".log" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "getenv = true" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "should_transfer_files = yes" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "when_to_transfer_output = on_exit" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "queue" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "" >> $TICKET_DIRECTORY"/condor_sc."$ticket

    export CONDOR_CONFIG=/application/condor/etc/condor_config
    /application/condor/bin/condor_submit $TICKET_DIRECTORY"/condor_sc."$ticket
  else
    # no Condor - execute om_model directly
    "$OM_BIN_DIR"/om_model --xml-req "$moved" --model-file "$resp" --calc-matrix --calc-roc --log-file "$log" --prog-file "$model_prog"
  fi  
  exit 1
done

# Process test requests
for req in `find $TICKET_DIRECTORY/test_req.* -type f 2> /dev/null`; do

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
    echo "executable = /home/silvio/om_serial/bin/om_test " >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "environment = LD_LIBRARY_PATH=/home/silvio/om_serial/lib:/home/om/bin/lib_extern/lib" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "output          = "$TICKET_DIRECTORY"/om_test_log"$ticket".out" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "error           = "$TICKET_DIRECTORY"/om_test_log"$ticket".err" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "log             = "$TICKET_DIRECTORY"/om_test_log"$ticket".log" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "getenv = true" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "should_transfer_files = yes" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "when_to_transfer_output = on_exit" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "queue" >> $TICKET_DIRECTORY"/condor_sc."$ticket
    echo "" >> $TICKET_DIRECTORY"/condor_sc."$ticket

    export CONDOR_CONFIG=/application/condor/etc/condor_config
    /application/condor/bin/condor_submit $TICKET_DIRECTORY"/condor_sc."$ticket
  else
    # no Condor - execute om_test directly
    "$OM_BIN_DIR"/om_test --xml-req "$moved" --result "$resp" --log-file "$log" --prog-file "$test_prog"
  fi  
  exit 1
done

# Process projection requests
for req in `find $TICKET_DIRECTORY/proj_req.* -type f 2> /dev/null`; do

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
    echo "executable = /home/silvio/om_serial/bin/om_project" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "environment = LD_LIBRARY_PATH=/home/silvio/om_serial/lib:/home/om/bin/lib_extern/lib" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "output		= "$TICKET_DIRECTORY"/om_project_LOG"$ticket".out" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "error		= "$TICKET_DIRECTORY"/om_project_LOG"$ticket".err" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "log		= "$TICKET_DIRECTORY"/om_project_LOG"$ticket".log" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "getenv = true" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "should_transfer_files = yes" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "when_to_transfer_output = on_exit" >> $TICKET_DIRECTORY"/condor_sp."$ticket
    echo "queue" >> $TICKET_DIRECTORY"/condor_sp."$ticket

    export CONDOR_CONFIG=/application/condor/etc/condor_config
    /application/condor/bin/condor_submit $TICKET_DIRECTORY"/condor_sp."$ticket
  else
    # no Condor - execute om_project directly
    "$OM_BIN_DIR"/om_project --xml-req "$moved" --dist-map "$map_img" --stat-file "$stats" --log-file "$log" --prog-file "$proj_prog"
  fi

  finalmap_img=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".img"
  finalmap_ige=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".ige"

  if [ -e $map_ige ]; then
    mv "$map_ige" "$finalmap_ige"
  fi

  # Create a PNG with pseudocolor (requires GDAL command-line tools)
  if [[ "$CREATE_PNG" == "yes" ]]; then
    if [ -e $map_img ]; then
      finalmap_png=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".png"
      tempmap_png=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".png.tmp"
      # Create a virtual raster from an existing template
      vrt_file=$DISTRIBUTION_MAP_DIRECTORY"/"$ticket".vrt"
      # Get x and y dimensions from raster using gdalinfo
      info=`$GDAL_BIN_DIR/gdalinfo $map_img`
      match=`echo $info | sed -n -e 's#.*Size\sis\s\([0-9\.]*\),\s\([0-9\.]*\).*$#\1 \2#p'`
      match_len=${#match}
      x_size=0
      y_size=0
      if [ $match_len -gt 0 ]; then
        x_size=`echo $match | awk '{print $1}'`
        y_size=`echo $match | awk '{print $2}'`
      fi
      # Replace values in the virtual raster file
      sed 's/\$x/'$x_size'/' "$VRT_TEMPLATE" | sed 's/\$y/'$y_size'/' | sed 's#\$file_name#'$map_img'#' > "$vrt_file"
      # Convert the virtual raster to PNG
      "$GDAL_BIN_DIR"/gdal_translate "$vrt_file" -ot Byte -of PNG "$tempmap_png"
      mv "$tempmap_png" "$finalmap_png"
      rm -f "$vrt_file"
    fi
  fi

  # This must be the last step, since getProgress will only return 100% if
  # the final map exists
  mv "$map_img" "$finalmap_img"

  exit 1
done
