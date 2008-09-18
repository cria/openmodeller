#######################################################################
# Script to be included in the cronjob. It checks if there are requests
# (create model, test model or project model) in a specific directory 
# and runs them. It depends on a configuration file that must be passed
# as a parameter (otherwise it will try to read a file called 
# scheduler.conf in the same directory).
#######################################################################

# Function to read configuration file
function readconf() {
 
  match=0
 
  while read line; do
    # skip comments
    [[ ${line:0:1} == "#" ]] && continue
 
    # skip empty lines
    [[ -z "$line" ]] && continue
 
    # still no match? lets check again
    if [ $match == 0 ]; then
 
      # do we have an opening tag ?
      if [[ ${line:$((${#line}-1))} == "{" ]]; then
 
          # strip "{"
          group=${line:0:$((${#line}-1))}
          # strip whitespace
          group=${group// /}
 
          # do we have a match ?
          if [[ "$group" == "$1" ]]; then
            match=1
            continue
          fi
 
          continue
      fi
 
    # found closing tag after config was read - exit loop
    elif [[ ${line:0} == "}" && $match == 1 ]]; then
      break
 
    # got a config line eval it
    else
        eval $line
    fi
 
  done < "$CONFIG"
}

# Configuration file can be passed as a parameter
if [[ "$1" ]]; then
  CONFIG=$1
else
  CONFIG="scheduler.conf"
fi  

# If configuration file exists, read the configuration
if [ -f $CONFIG ]; then
  # default configuration section
  readconf "default"
else
  echo "Error: no configuration file available"
  exit 0
fi

# TODO: check number of running processes and don't exceed a certain limit

# Second parameter is sleep interval so we can be running several
# schedulers at different intervals
sleep $2

# Process model requests
for req in `find $TICKET_REPOSITORY/model_req.* -type f 2> /dev/null`; do

  ticket="${req##*.}"
  moved=$TICKET_REPOSITORY"/model_proc."$ticket
  resp=$TICKET_REPOSITORY"/model_resp."$ticket
  log=$TICKET_REPOSITORY"/"$ticket

  model_prog=$TICKET_REPOSITORY"/prog."$ticket

  # Rename file, avoiding that another process take it
  mv "$req" "$moved"

  # if condor_integration then create the script and submit
  if [[ "$CONDOR_INTEGRATION" == "yes" ]]; then
    echo "universe        = vanilla" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "arguments = --xml-req "$moved" --model-file "$resp" --prog-file "$model_prog>> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "executable = /home/silvio/om_serial/bin/om_model " >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "environment = LD_LIBRARY_PATH=/home/silvio/om_serial/lib:/home/om/bin/lib_extern/lib" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "output          = "$TICKET_REPOSITORY"/om_model_log"$ticket".out" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "error           = "$TICKET_REPOSITORY"/om_model_log"$ticket".err" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "log             = "$TICKET_REPOSITORY"/om_model_log"$ticket".log" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "getenv = true" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "should_transfer_files = yes" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "when_to_transfer_output = on_exit" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "queue" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "" >> $TICKET_REPOSITORY"/condor_sc."$ticket

    export CONDOR_CONFIG=/application/condor/etc/condor_config
    /application/condor/bin/condor_submit $TICKET_REPOSITORY"/condor_sc."$ticket
  else
    # no Condor - execute om_model directly
    "$OM_BIN_DIR"/om_model --xml-req "$moved" --model-file "$resp" --log-file "$log" --prog-file "$model_prog"
  fi  
  exit 1
done

# Process test requests
for req in `find $TICKET_REPOSITORY/test_req.* -type f 2> /dev/null`; do

  ticket="${req##*.}"
  moved=$TICKET_REPOSITORY"/test_proc."$ticket
  resp=$TICKET_REPOSITORY"/test_resp."$ticket
  log=$TICKET_REPOSITORY"/"$ticket

  # Rename file, avoiding that another process take it
  mv "$req" "$moved"

  # if condor_integration then create the script and submit
  if [[ "$CONDOR_INTEGRATION" == "yes" ]]; then
    echo "universe        = vanilla" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "arguments = --xml-req "$moved" --result-file "$resp>> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "executable = /home/silvio/om_serial/bin/om_test " >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "environment = LD_LIBRARY_PATH=/home/silvio/om_serial/lib:/home/om/bin/lib_extern/lib" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "output          = "$TICKET_REPOSITORY"/om_test_log"$ticket".out" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "error           = "$TICKET_REPOSITORY"/om_test_log"$ticket".err" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "log             = "$TICKET_REPOSITORY"/om_test_log"$ticket".log" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "getenv = true" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "should_transfer_files = yes" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "when_to_transfer_output = on_exit" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "queue" >> $TICKET_REPOSITORY"/condor_sc."$ticket
    echo "" >> $TICKET_REPOSITORY"/condor_sc."$ticket

    export CONDOR_CONFIG=/application/condor/etc/condor_config
    /application/condor/bin/condor_submit $TICKET_REPOSITORY"/condor_sc."$ticket
  else
    # no Condor - execute om_test directly
    "$OM_BIN_DIR"/om_test --xml-req "$moved" --result-file "$resp" --log-file "$log"
  fi  
  exit 1
done

# Process projection requests
for req in `find $TICKET_REPOSITORY/proj_req.* -type f 2> /dev/null`; do

  ticket="${req##*.}"
  moved=$TICKET_REPOSITORY"/proj_proc."$ticket
  map_base=$MAP_REPOSITORY"/proc_"$ticket
  map_img=$map_base".img"
  map_ige=$map_base".ige"
  stats=$TICKET_REPOSITORY"/stats."$ticket
  log=$TICKET_REPOSITORY"/"$ticket
  proj_prog=$TICKET_REPOSITORY"/prog."$ticket

  # Rename file, avoiding that another process take it
  mv "$req" "$moved"

  # if condor_integration then create the script and submit
  if [[ "$CONDOR_INTEGRATION" == "yes" ]]; then
    echo "universe 	= vanilla" >> $TICKET_REPOSITORY"/condor_sp."$ticket
    echo "arguments = --xml-req "$moved" --dist-map "$MAP_REPOSITORY"/"$ticket".img --stat-file "$stats" --prog-file "$proj_prog >> $TICKET_REPOSITORY"/condor_sp."$ticket
    echo "executable = /home/silvio/om_serial/bin/om_project" >> $TICKET_REPOSITORY"/condor_sp."$ticket
    echo "environment = LD_LIBRARY_PATH=/home/silvio/om_serial/lib:/home/om/bin/lib_extern/lib" >> $TICKET_REPOSITORY"/condor_sp."$ticket
    echo "output		= "$TICKET_REPOSITORY"/om_project_LOG"$ticket".out" >> $TICKET_REPOSITORY"/condor_sp."$ticket
    echo "error		= "$TICKET_REPOSITORY"/om_project_LOG"$ticket".err" >> $TICKET_REPOSITORY"/condor_sp."$ticket
    echo "log		= "$TICKET_REPOSITORY"/om_project_LOG"$ticket".log" >> $TICKET_REPOSITORY"/condor_sp."$ticket
    echo "getenv = true" >> $TICKET_REPOSITORY"/condor_sp."$ticket
    echo "should_transfer_files = yes" >> $TICKET_REPOSITORY"/condor_sp."$ticket
    echo "when_to_transfer_output = on_exit" >> $TICKET_REPOSITORY"/condor_sp."$ticket
    echo "queue" >> $TICKET_REPOSITORY"/condor_sp."$ticket

    export CONDOR_CONFIG=/application/condor/etc/condor_config
    /application/condor/bin/condor_submit $TICKET_REPOSITORY"/condor_sp."$ticket
  else
    # no Condor - execute om_project directly
    "$OM_BIN_DIR"/om_project --xml-req "$moved" --dist-map "$map_img" --stat-file "$stats" --log-file "$log" --prog-file "$proj_prog"
  fi

  finalmap_img=$MAP_REPOSITORY"/"$ticket".img"
  finalmap_ige=$MAP_REPOSITORY"/"$ticket".ige"

  if [ -e $map_ige ]; then
    mv "$map_ige" "$finalmap_ige"
  fi

  # Create a PNG with pseudocolor (requires GDAL command-line tools)
  if [[ "$CREATE_PNG" == "yes" ]]; then
    if [ -e $map_img ]; then
      finalmap_png=$MAP_REPOSITORY"/"$ticket".png"
      tempmap_png=$MAP_REPOSITORY"/"$ticket".png.tmp"
      # Create a virtual raster from an existing template
      vrt_template=REPLACE_WITH_VRT_TEMPLATE_FILE_PATH
      vrt_file=$MAP_REPOSITORY"/"$ticket".vrt"
      # Get x and y dimensions from raster using gdalinfo
      info=`gdalinfo $map_img`
      match=`echo $info | sed -n -e 's#.*Size\sis\s\([0-9\.]*\),\s\([0-9\.]*\).*$#\1 \2#p'`
      match_len=${#match}
      x_size=0
      y_size=0
      if [ $match_len -gt 0 ]; then
        x_size=`echo $match | awk '{print $1}'`
        y_size=`echo $match | awk '{print $2}'`
      fi
      # Replace values in the virtual raster file
      sed 's/\$x/'$x_size'/' "$vrt_template" | sed 's/\$y/'$y_size'/' | sed 's#\$file_name#'$map_img'#' > "$vrt_file"
      # Convert the virtual raster to PNG
      gdal_translate "$vrt_file" -ot Byte -of PNG "$tempmap_png"
      mv "$tempmap_png" "$finalmap_png"
      rm -f "$vrt_file"
    fi
  fi

  # This must be the last step, since getProgress will only return 100% if
  # the final map exists
  mv "$map_img" "$finalmap_img"

  exit 1
done
