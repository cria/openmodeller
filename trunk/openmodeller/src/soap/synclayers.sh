#!/bin/bash

#######################################################################
# Script that can be included in the cronjob to update environmental 
# layers. Requires rsync.
# Parameter 1: path to configuration file.
#######################################################################

debug() {
    [ "$DEBUG" = 1 ] && echo -e "\033[31m$*\033[m"
}

TAG="synclayers.sh" 

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

# by default, we'll use 2 repositories in each server, at least when
# this script syncs, which are (1) the effective repository, which is
# $LAYERS_DIRECTORY and (2) the new repository, which is
# $UPDATED_LAYERS_DIRECTORY

check_om_processes() {
    # check for running oM instances
    debug "check for running oM instances"
    if [ $(pgrep om_model) -o  $(pgrep om_test) -o $(pgrep om_project) ]; then
        # y: some job is still running
        debug "jobs are still running. aborting."
        logger -t "$TAG" "jobs are still running. aborting."
        exit 0
    else
        # n
        # lock server
        logger -t "$TAG" "no jobs running."
        debug "lock server"
        cat $CONFIG | \
            sed 's/SYSTEM_STATUS=1/SYSTEM_STATUS=2/' > \
            ${CONFIG}.tmp
        mv ${CONFIG}.tmp $CONFIG
        logger -t "$TAG" "service locked."

        # setting dirs
        debug "setting dirs"
        logger -t "$TAG" "updating working copy."
        rsync -aL --copy-unsafe-links --delete \
            $UPDATED_LAYERS_DIRECTORY $LAYERS_DIRECTORY

        # remove flag
        debug "remove flag"
        rm -f /tmp/COPY_READY

        # erase cache
        rm -f $CACHE_DIRECTORY/layers.xml

        # unlock server
        debug "unlock server"
        cat $CONFIG | \
            sed 's/SYSTEM_STATUS=2/SYSTEM_STATUS=1/' > \
            ${CONFIG}.tmp
        mv ${CONFIG}.tmp $CONFIG
        logger -t "$TAG" "service unlocked."

        logger -t "$TAG" "working copy synced."
        exit 0
    fi
}

# check rsync
# NOTE: Here we assume that rsync is only used by this script!
debug "checking rsync"
if [ $(pgrep rsync) ]; then
    # y
    logger -t "$TAG" "rsync already running. Exiting."
    exit 0
else
    # n
    # check flag
    debug "check flag"
    if [ -e /tmp/COPY_READY ]; then
        # y
        logger -t "$TAG" "local copy is ready."
        check_om_processes

        exit 0
    else
        # n
        # check for repository updates
        debug "check for repository updates"
        logger -t "$TAG" "checking updates."
        rsync -aniL --copy-unsafe-links --delete \
            rsync://$RSYNC_LAYERS_REPOSITORY \
            $LAYERS_DIRECTORY > rsync.out.tmp
        RSYNC_LAYERS_REPOSITORY_STATUS=$?

        grep -E '^(\*|>)' /tmp/rsync.out.tmp
        HAVE_UPDATES=$?
        rm -f /tmp/rsync.out.tmp

        if [ "$RSYNC_LAYERS_REPOSITORY_STATUS" -eq 0 -a \
            "$HAVE_UPDATES" -eq 0 ]; then
            # y
            logger -t "$TAG" "updates found."
            # run rsync
            debug "updating local copy"
            rsync -aL --copy-unsafe-links --delete \
                rsync://$RSYNC_LAYERS_REPOSITORY \
                $UPDATED_LAYERS_DIRECTORY

            # set flag
            debug "setting flag"
            logger -t "$TAG" "setting local copy as ready."
            RSYNC_LAYERS_REPOSITORY_STATUS=$?
            if [ "$RSYNC_LAYERS_REPOSITORY_STATUS" -eq 0 ]; then
                touch /tmp/COPY_READY
            fi

            check_om_processes
        else
            # n
            logger -t "$TAG" "no updates."
            exit 0
        fi
    fi
fi
