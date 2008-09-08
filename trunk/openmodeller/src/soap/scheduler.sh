# Script to be included in the cronjob. It checks if there are requests
#### (create model or project model) in a specific directory and runs them

#read configuration Function
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

#configuration file
CONFIG="scheduler.conf"

#if configuration file exists, read the configuration
if [ -f $CONFIG ]; then 
  #configuration section
  readconf "default"
else 
  CONDOR_INTEGRATION=no 
  PARALLEL_CONDOR=no
fi

# TODO: check number of running processes and don't exceed a certain limit

# First parameter is the ticket repository directory
ticket_repository=$1

# Second parameter is the map repository directory
map_repository=$2

# Third parameter is sleep interval so we can be running several
# schedulers at different intervals
sleep $3

# model requests
for req in `find $ticket_repository/model_req.* -type f 2> /dev/null`; do
ticket="${req##*.}"
moved=$ticket_repository"/model_proc."$ticket
resp=$ticket_repository"/model_resp."$ticket
log=$ticket_repository"/"$ticket

model_prog=$ticket_repository"/prog."$ticket
# rename file
mv "$req" "$moved"
# execute om_create

  # if condor_integration then create the script and submit
  if [[ "$CONDOR_INTEGRATION" == "yes" ]]; then
      # create a parallel script 
      if [[ "$PARALLEL_CONDOR" == "yes" ]]; then
        echo "universe        = vanilla" >> $ticket_repository"/condor_sc."$ticket
        echo "arguments = --xml-req "$moved" --model-file "$resp" --prog-file "$model_prog>> $ticket_repository"/condor_sc."$ticket
        echo "executable = /home/silvio/om_serial/bin/om_create " >> $ticket_repository"/condor_sc."$ticket
        echo "environment = LD_LIBRARY_PATH=/home/silvio/om_serial/lib:/home/om/bin/lib_extern/lib" >> $ticket_repository"/condor_sc."$ticket
        echo "output          = "$ticket_repository"/om_create_log"$ticket".out" >> $ticket_repository"/condor_sc."$ticket
        echo "error           = "$ticket_repository"/om_create_log"$ticket".err" >> $ticket_repository"/condor_sc."$ticket
        echo "log             = "$ticket_repository"/om_create_log"$ticket".log" >> $ticket_repository"/condor_sc."$ticket
        echo "getenv = true" >> $ticket_repository"/condor_sc."$ticket
        echo "should_transfer_files = yes" >> $ticket_repository"/condor_sc."$ticket
        echo "when_to_transfer_output = on_exit" >> $ticket_repository"/condor_sc."$ticket
        echo "queue" >> $ticket_repository"/condor_sc."$ticket
        echo "" >> $ticket_repository"/condor_sc."$ticket
      # create a serial script 
      else
        echo "universe        = vanilla" >> $ticket_repository"/condor_sc."$ticket
        echo "arguments = --xml-req "$moved" --model-file "$resp" --prog-file "$model_prog>> $ticket_repository"/condor_sc."$ticket
        echo "executable = /home/silvio/om_serial/bin/om_create " >> $ticket_repository"/condor_sc."$ticket
        echo "environment = LD_LIBRARY_PATH=/home/silvio/om_serial/lib:/home/om/bin/lib_extern/lib" >> $ticket_repository"/condor_sc."$ticket
        echo "output          = "$ticket_repository"/om_create_log"$ticket".out" >> $ticket_repository"/condor_sc."$ticket
        echo "error           = "$ticket_repository"/om_create_log"$ticket".err" >> $ticket_repository"/condor_sc."$ticket
        echo "log             = "$ticket_repository"/om_create_log"$ticket".log" >> $ticket_repository"/condor_sc."$ticket
        echo "getenv = true" >> $ticket_repository"/condor_sc."$ticket
        echo "should_transfer_files = yes" >> $ticket_repository"/condor_sc."$ticket
        echo "when_to_transfer_output = on_exit" >> $ticket_repository"/condor_sc."$ticket
        echo "queue" >> $ticket_repository"/condor_sc."$ticket
        echo "" >> $ticket_repository"/condor_sc."$ticket      
      fi
      export CONDOR_CONFIG=/application/condor/etc/condor_config
      /application/condor/bin/condor_submit $ticket_repository"/condor_sc."$ticket
  else
    /usr/local/bin/om_create --xml-req "$moved" --model-file "$resp" --log-file "$log" --prog-file "$model_prog"
  fi  

exit 1
done

# projection requests
for req in `find $ticket_repository/proj_req.* -type f 2> /dev/null`; do
ticket="${req##*.}"
moved=$ticket_repository"/proj_proc."$ticket
map_base=$map_repository"/proc_"$ticket
map_img=$map_base".img"
map_ige=$map_base".ige"
stats=$ticket_repository"/stats."$ticket
log=$ticket_repository"/"$ticket
proj_prog=$ticket_repository"/prog."$ticket
# rename file
mv "$req" "$moved"
# execute om_project

  # if condor_integration then create the script and submit
  if [[ "$CONDOR_INTEGRATION" == "yes" ]]; then
    # create a parallel script 
    if [[ "$PARALLEL_CONDOR" == "yes" ]]; then
      echo "universe 	= vanilla" >> $ticket_repository"/condor_sp."$ticket
      echo "arguments = --xml-req "$moved" --dist-map "$map_repository"/"$ticket".img --stat-file "$stats" --prog-file "$proj_prog >> $ticket_repository"/condor_sp."$ticket
      echo "executable = /home/silvio/om_serial/bin/om_project" >> $ticket_repository"/condor_sp."$ticket
      echo "environment = LD_LIBRARY_PATH=/home/silvio/om_serial/lib:/home/om/bin/lib_extern/lib" >> $ticket_repository"/condor_sp."$ticket
      echo "output		= "$ticket_repository"/om_project_LOG"$ticket".out" >> $ticket_repository"/condor_sp."$ticket
      echo "error		= "$ticket_repository"/om_project_LOG"$ticket".err" >> $ticket_repository"/condor_sp."$ticket
      echo "log		= "$ticket_repository"/om_project_LOG"$ticket".log" >> $ticket_repository"/condor_sp."$ticket
      echo "getenv = true" >> $ticket_repository"/condor_sp."$ticket
      echo "should_transfer_files = yes" >> $ticket_repository"/condor_sp."$ticket
      echo "when_to_transfer_output = on_exit" >> $ticket_repository"/condor_sp."$ticket
      echo "queue" >> $ticket_repository"/condor_sp."$ticket
    # create a serial script 
    else
      echo "universe 	= vanilla" >> $ticket_repository"/condor_sp."$ticket
      echo "arguments = --xml-req "$moved" --dist-map "$map_repository"/"$ticket".img --stat-file "$stats" --prog-file "$proj_prog >> $ticket_repository"/condor_sp."$ticket
      echo "executable = /home/silvio/om_serial/bin/om_project" >> $ticket_repository"/condor_sp."$ticket
      echo "environment = LD_LIBRARY_PATH=/home/silvio/om_serial/lib:/home/om/bin/lib_extern/lib" >> $ticket_repository"/condor_sp."$ticket
      echo "output		= "$ticket_repository"/om_project_LOG"$ticket".out" >> $ticket_repository"/condor_sp."$ticket
      echo "error		= "$ticket_repository"/om_project_LOG"$ticket".err" >> $ticket_repository"/condor_sp."$ticket
      echo "log		= "$ticket_repository"/om_project_LOG"$ticket".log" >> $ticket_repository"/condor_sp."$ticket
      echo "getenv = true" >> $ticket_repository"/condor_sp."$ticket
      echo "should_transfer_files = yes" >> $ticket_repository"/condor_sp."$ticket
      echo "when_to_transfer_output = on_exit" >> $ticket_repository"/condor_sp."$ticket
      echo "queue" >> $ticket_repository"/condor_sp."$ticket
    fi

    export CONDOR_CONFIG=/application/condor/etc/condor_config
    /application/condor/bin/condor_submit $ticket_repository"/condor_sp."$ticket
  else
    /usr/local/bin/om_project --xml-req "$moved" --dist-map "$map_img" --stat-file "$stats" --log-file "$log" --prog-file "$proj_prog"
  fi

finalmap_img=$map_repository"/"$ticket".img"
finalmap_ige=$map_repository"/"$ticket".ige"
if [ -e $map_ige ]; then
mv "$map_ige" "$finalmap_ige"
fi

# Create a PNG with pseudocolor (requires GDAL command-line tools)
#if [ -e $map_img ]; then
#finalmap_png=$map_repository"/"$ticket".png"
#tempmap_png=$map_repository"/"$ticket".png.tmp"
# Create a virtual raster from an existing template
#vrt_template=REPLACE_WITH_VRT_TEMPLATE_FILE_PATH
#vrt_file=$map_repository"/"$ticket".vrt"
# Get x and y dimensions from raster using gdal_info
#info=`gdalinfo $map_img`
#match=`echo $info | sed -n -e 's#.*Size\sis\s\([0-9\.]*\),\s\([0-9\.]*\).*$#\1 \2#p'`
#match_len=${#match}
#x_size=0
#y_size=0
#if [ $match_len -gt 0 ]; then
#  x_size=`echo $match | awk '{print $1}'`
#  y_size=`echo $match | awk '{print $2}'`
#fi
# Replace values in the virtual raster file
#sed 's/\$x/'$x_size'/' "$vrt_template" | sed 's/\$y/'$y_size'/' | sed 's#\$file_name#'$map_img'#' > "$vrt_file"
# Convert the virtual raster to PNG
#gdal_translate "$vrt_file" -ot Byte -of PNG "$tempmap_png"
#mv "$tempmap_png" "$finalmap_png"
#rm -f "$vrt_file"
#fi

# This must be the last step, since getProgress will only return 100% if
# the final map exists
#mv "$map_img" "$finalmap_img"

exit 1
done