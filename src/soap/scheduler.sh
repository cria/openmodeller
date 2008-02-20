# Script to be included in the cronjob. It checks if there are requests
# (create model or project model) in a specific directory and runs them

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
om_create --xml-req "$moved" --model-file "$resp" --log-file "$log" --prog-file "$model_prog"
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
om_project --xml-req "$moved" --dist-map "$map_img" --stat-file "$stats" --log-file "$log" --prog-file "$proj_prog"
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
#/usr/bin/gdal_translate "$vrt_file" -ot Byte -of PNG "$tempmap_png"
#mv "$tempmap_png" "$finalmap_png"
#rm -f "$vrt_file"
#fi

# This must be the last step, since getProgress will only return 100% if
# the final map exists
mv "$map_img" "$finalmap_img"

exit 1
done
