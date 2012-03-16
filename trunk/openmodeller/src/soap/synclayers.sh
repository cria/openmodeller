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

# by default, we'll use 2 repositories in each server, at least when
# this script syncs, which are (1) the effective repository, which is
# $LAYERS_DIRECTORY and (2) the new repository, which is
# $UPDATED_LAYERS_DIRECTORY
#
# Status codes:
# 0: NORMAL_STAND_BY
# 1: RUNNING (rsync initiated)
# 2: READY
# 3: ABORTED

check_om_processes() {
    # oM instances running?
    if [ $(pgrep om_console) ]; then
        exit 1
    else
        # lock server
        cat server.conf | \
            sed 's/SYSTEM_STATUS=1/SYSTEM_STATUS=2/' > \
            server.conf.tmp
        mv server.conf.tmp server.conf

        # swap directories
        rm -fr $LAYERS_DIRECTORY/
        mv $UPDATED_LAYERS_DIRECTORY/ $LAYERS_DIRECTORY/
        mkdir $UPDATED_LAYERS_DIRECTORY/

        # remove flag
        rm $STATUS_DIR_PREFIX/COPY_READY
    fi
}

if [ $(pgrep rsync) ]; then
    exit 1
else
    # check flag
    if [ -e $STATUS_DIR_PREFIX/COPY_READY ]; then
        check_om_processes

        exit 0
    else
        rsync -ani --delete --no-motd rsync://$RSYNC_LAYERS_DIRECTORY \
            $LAYERS_DIRECTORY > rsync.out.tmp
        $RSYNC_LAYERS_REPOSITORY_STATUS=$?

        grep -E '^(\*|>)' rsync.out.tmp
        $HAVE_UPDATES=$?

        if [ $RSYNC_LAYERS_REPOSITORY_STATUS -and $HAVE_UPDATES ]; then
            rsync -a --delete --no-motd rsync://$RSYNC_LAYERS_DIRECTORY \
                $LAYERS_DIRECTORY

            $RSYNC_LAYERS_REPOSITORY_STATUS=$?
            if [ $RSYNC_LAYERS_DIRECTORY_STATUS ]; then
                touch $STATUS_DIRECTORY_PREFIX/COPY_READY

                check_om_processes
            else
                exit 0
            fi
        fi
    fi
fi
