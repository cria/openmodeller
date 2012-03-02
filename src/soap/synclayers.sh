#!/bin/bash

#######################################################################
# Script that can be included in the cronjob to update environmental 
# layers. Requires rsync.
# Parameter 1: path to configuration file.
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

# Check configuration
if [[ "$RSYNC_LAYERS_REPOSITORY" ]]; then
  if [[ "$LAYERS_DIRECTORY" ]]; then
    if [ ! -d "$LAYERS_DIRECTORY" ]; then

      mkdir "$LAYERS_DIRECTORY"
    fi
  else
    exit 0
  fi  
else
  exit 0
fi  

# Use cases:
# 1. a long running job
# 2. a fast job

# Check to see if there are changes in repository
# TODO: Check the status of rsync, in the case server is down
if [ ! -e REPOSITORY_CHANGED ]; then
    # check the status of rsync, to certify that server is running
    rsync -ani --delete --no-motd rsync://$RSYNC_LAYERS_REPOSITORY \
        $LAYERS_DIRECTORY > rsync.out.tmp
    $SERVER_STATUS=$?

    grep -E '^(\*|>)' rsync.out.tmp
    $FOUND_CHANGES=$?

    if [ $SERVER_STATUS -a $FOUND_CHANGES ]; then
    # Change the status of the server accordingly
        cat server.conf | \
            sed 's/SYSTEM_STATUS=1/SYSTEM_STATUS=2/' > server.conf.tmp
        mv server.conf.tmp server.conf

        touch REPOSITORY_CHANGED
    else
        # there server is down or there are no changes
        exit 0
    fi
fi

# Wait for all jobs to finish (check number of files under $PID_DIRECTORY)
#
# Two problems arise here:
# 1. If we check for files under $PID_DIRECTORY, the script will exit
# early and a new request to the $RSYNC_LAYERS_REPOSITORY will be
# generated next time cron runs this 
#
# 2. If we wait here long enough, cron will be called _before_ we leave
# this script, so the correct time must be set
# 
# An alternative is to use a local file to bypass the rsync check if it
# was already checked that the repository has changed, which is what we
# do before the first rsync.

if [ ]; then
    # TODO: Check the format of the files under $PID_DIRECTORY
else
    # Synchronize layers
    rsync -a --delete --no-motd rsync://$RSYNC_LAYERS_REPOSITORY $LAYERS_DIRECTORY
    rm REPOSITORY_CHANGED

    # Switch back system status to 1 (normal)
    cat server.conf | \
        sed 's/SYSTEM_STATUS=2/SYSTEM_STATUS=1/' > server.conf.tmp
    mv server.conf.tmp server.conf
fi

# TODO: Remove getLayers cached response (layers.xml under $CACHE_DIRECTORY)
