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
om_create "$moved" "$resp" "$log" "$model_prog"
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
om_project "$moved" "$map_img" "$stats" "$log" "$proj_prog"
finalmap_img=$map_repository"/"$ticket".img"
finalmap_ige=$map_repository"/"$ticket".ige"
if [ -e $map_ige ]; then
mv "$map_ige" "$finalmap_ige"
fi

# Create a JPEG file with pseudocolor
#if [ -e $map_img ]; then
#finalmap_jpg=$map_repository"/"$ticket".jpg"
#tempmap_jpg=$map_repository"/"$ticket".jpg.tmp"
#palete=$map_repository"/REPLACE_WITH_PALETE_FILE_NAME"
# This requires GDAL command-line tools:
#/usr/bin/gdal_translate "$map_img" -ot Byte -of JPEG "$tempmap_jpg"
#/usr/bin/pct2rgb.py -of JPEG "$tempmap_jpg" "$tempmap_jpg"
# This requires Imagemagick >= 6.3.5:
#/usr/bin/convert -opaque "#F00" -fill "#FFF" "$tempmap_jpg" "$tempmap_jpg"
#/usr/bin/convert -equalize -colorspace RGB "$tempmap_jpg" "$tempmap_jpg"
#/usr/bin/convert "$tempmap_jpg" "$palete" -clut "$tempmap_jpg"
#mv "$tempmap_jpg" "$finalmap_jpg"
#fi

# This must be the last step, since getProgress will only return 100% if
# the final map exists
mv "$map_img" "$finalmap_img"

exit 1
done
