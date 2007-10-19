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
# execute om_create
om_project "$moved" "$map_img" "$stats" "$log" "$proj_prog"
finalmap_img=$map_repository"/"$ticket".img"
finalmap_ige=$map_repository"/"$ticket".ige"
mv "$map_img" "$finalmap_img"
if [ -e $map_ige ]; then
mv "$map_ige" "$finalmap_ige"
fi
exit 1
done
