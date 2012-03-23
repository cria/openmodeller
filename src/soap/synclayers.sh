#!/bin/bash

#######################################################################
# Script that can be included in the cronjob to update environmental 
# layers. Requires rsync.
# Parameter 1: path to configuration file.
#######################################################################

debug() {
    [ "$DEBUG" = 1 ] && echo -e "\033[31m$*\033[m"
}

TAG="om_syncserver" 

# Function to read configuration file
function readconf {
  while read line; do
    # skip comments
    [ "${line:0:1}" = "#" ] && continue
 
    # skip empty lines
    [ -z "$line" ] && continue
 
    # got a config line eval it
    eval $line
 
  done < "$1"
}

# Configuration file can be passed as a parameter
if [ "$1" ]; then
  CONFIG="$1"
else
  CONFIG="server.conf"
fi 
debug "config file: $CONFIG"

# If configuration file exists, read the configuration
if [ -f $CONFIG ]; then
  # read configuration
  readconf $CONFIG
else
  exit 1
fi

debug "\$RSYNC_LAYERS_REPOSITORY = $RSYNC_LAYERS_REPOSITORY"

if [ -z "$RSYNC_LAYERS_REPOSITORY" ]; then
    exit 1
elif [ ! -d "$LAYERS_DIRECTORY" ]; then
    # initial copy
    debug "initial copy"
    logger -t "$TAG" "initial copy"
    rsync -a --delete --no-motd rsync://$RSYNC_LAYERS_REPOSITORY \
        $LAYERS_DIRECTORY
    exit 0
fi

# Use cases:
# 1. a long running job
# 2. a fast job

# by default, we'll use 2 repositories in each server, at least when
# this script syncs, which are (1) the effective repository, which is
# $LAYERS_DIRECTORY and (2) the new repository, which is
# $UPDATED_LAYERS_DIRECTORY

check_om_processes() {
    # check for running oM instances
    debug "check for running oM instances"
    if [ $(pgrep om_console) ]; then
        # y
        debug "local copy ready (but not synced yet)"
        exit 0
    else
        # n
        # lock server
        debug "lock server"
        cat $CONFIG | \
            sed 's/SYSTEM_STATUS=1/SYSTEM_STATUS=2/' > \
            ${CONFIG}.tmp
        mv ${CONFIG}.tmp $CONFIG

        # setting dirs
        debug "setting dirs"
        rm -fr $LAYERS_DIRECTORY
        #mv $UPDATED_LAYERS_DIRECTORY $LAYERS_DIRECTORY
        rsync -a --delete $UPDATED_LAYERS_DIRECTORY $LAYERS_DIRECTORY

        # remove flag
        debug "remove flag"
        rm COPY_READY

        # unlock server
        debug "unlock server"
        cat $CONFIG | \
            sed 's/SYSTEM_STATUS=2/SYSTEM_STATUS=1/' > \
            ${CONFIG}.tmp
        mv ${CONFIG}.tmp $CONFIG

        logger -t "$TAG" "local copy synced"
        exit 0
    fi
}

# check rsync
debug "check rsync"
if [ $(pgrep rsync) ]; then
    # y
    exit 0
else
    # n
    # check flag
    debug "check flag"
    if [ -e COPY_READY ]; then
        # y
        check_om_processes

        exit 0
    else
        # n
        # check for repository updates
        debug "check for repository updates"
        rsync -ani --delete --no-motd rsync://$RSYNC_LAYERS_REPOSITORY \
            $LAYERS_DIRECTORY > rsync.out.tmp
        RSYNC_LAYERS_REPOSITORY_STATUS=$?

        grep -E '^(\*|>)' rsync.out.tmp
        HAVE_UPDATES=$?
        rm rsync.out.tmp

        if [ "$RSYNC_LAYERS_REPOSITORY_STATUS" -eq 0 -a \
            "$HAVE_UPDATES" -eq 0 ]; then
            # y
            # copy "mirror local running layers"
            debug "mirror local running layers"
            #cp -R $LAYERS_DIRECTORY $UPDATED_LAYERS_DIRECTORY
            rsync -a --delete $LAYERS_DIRECTORY $UPDATED_LAYERS_DIRECTORY

            # run rsync
            debug "run rsync"
            rsync -a --delete --no-motd rsync://$RSYNC_LAYERS_REPOSITORY \
                $UPDATED_LAYERS_DIRECTORY

            # set flag
            debug "set flag"
            RSYNC_LAYERS_REPOSITORY_STATUS=$?
            if [ "$RSYNC_LAYERS_REPOSITORY_STATUS" -eq 0 ]; then
                touch COPY_READY
            fi

            check_om_processes
        else
            # n
            logger -t "$TAG" "no updates found"
            exit 0
        fi
    fi
fi
