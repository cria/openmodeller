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


# TODO: Change system status to 2 (service unavailable). 
# Note: better not to recreate the configuration file to avoid
#       permission problems.


# TODO: Wait for all jobs to finish (check number of files under $PID_DIRECTORY)


# Synchronize layers
rsync -a rsync://$RSYNC_LAYERS_REPOSITORY $LAYERS_DIRECTORY

# TODO: Remove getLayers cached response (layers.xml under $CACHE_DIRECTORY)


# TODO: Switch back system status to 1 (normal)
# Note: better not to recreate the configuration file to avoid
#       permission problems.
