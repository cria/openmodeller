# Script to be included in the cronjob. It checks if there are requests
# (create model or project model) in a specific directory and runs them

# TODO: check number of running processes and don't exceed a certain limit

# First parameter is the ticket repository directory
ticket_repository=$1

# Second parameter is the map repository directory
map_repository=$2

# model requests
for req in `find $ticket_repository/model_req.* -type f`; do
ticket="${req##*.}"
moved=$ticket_repository"/model_proc."$ticket
resp=$ticket_repository"/model_resp."$ticket
log=$ticket_repository"/"$ticket
# rename file
mv "$req" "$moved"
# execute om_create
om_create "$moved" "$resp" "$log"
done

# projection requests
for req in `find $ticket_repository/proj_req.* -type f`; do
ticket="${req##*.}"
moved=$ticket_repository"/proj_proc."$ticket
map=$map_repository"/"$ticket".tif"
log=$ticket_repository"/"$ticket
# rename file
mv "$req" "$moved"
# execute om_create
om_project "$moved" "$map" "$log"
done
