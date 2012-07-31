#!/bin/bash

#######################################################################
# Script that can be included in the cronjob to update environmental 
# layers. Requires lftp AND rsync.
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

debug "\$RSYNC_LAYERS_REPOSITORY=$RSYNC_LAYERS_REPOSITORY"

# by default, we'll use 2 repositories in each server, at least when
# this script syncs, which are (1) the effective repository, which is
# $LAYERS_DIRECTORY and (2) the new repository, which is
# $UPDATED_LAYERS_DIRECTORY

check_om_processes() {
    # checking for running oM instances
    debug "checking for running oM instances"
    if [ $(pgrep om_model) -o  $(pgrep om_test) -o $(pgrep om_project) ]; then
        # y: some job is still running
        debug "jobs are still running. aborting."
        logger -t "$TAG" "jobs are still running. aborting."
        exit 0
    else
        # n
        # lock server
        logger -t "$TAG" "no jobs running."
        debug "locking server"
        cat $CONFIG | \
            sed 's/SYSTEM_STATUS=1/SYSTEM_STATUS=2/' > \
            server.conf.tmp
        mv server.conf.tmp $CONFIG
        logger -t "$TAG" "service locked."

        # setting dirs
        debug "setting dirs"
        logger -t "$TAG" "updating working copy."
        rsync -aL --copy-unsafe-links --delete \
            $UPDATED_LAYERS_DIRECTORY $LAYERS_DIRECTORY

        # remove flag
        debug "removing flag"
        rm -f /tmp/COPY_READY

        # erase cache
        rm -f $CACHE_DIRECTORY/layers.xml

        # unlock server
        debug "unlocking server"
        cat $CONFIG | \
            sed 's/SYSTEM_STATUS=2/SYSTEM_STATUS=1/' > \
            server.conf.tmp
        mv server.conf.tmp $CONFIG
        logger -t "$TAG" "service unlocked."

        logger -t "$TAG" "working copy synced."
        exit 0
    fi
}

# check lftp
# NOTE: Here we assume that lftp is only used by this script!
debug "checking lftp"
if [ $(pgrep lftp) ]; then
    # y
    logger -t "$TAG" "lftp already running. Exiting."
    exit 0
else
    # n
    # check flag
    debug "checking flag"
    if [ -e /tmp/COPY_READY ]; then
        # y
        logger -t "$TAG" "local copy is ready."
        check_om_processes

        exit 0
    else
        # n
        # check for repository updates
        debug "checking updates"
        logger -t "$TAG" "checking updates."
        OLD_DIR=`pwd`
        cd $UPDATED_LAYERS_DIRECTORY
        debug "\$RSYNC_LAYERS_REPOSITORY=$RSYNC_LAYERS_REPOSITORY; \$UPDATED_LAYERS_DIRECTORY=$UPDATED_LAYERS_DIRECTORY"
        lftp -e'mirror -e --dry-run -L; quit' $RSYNC_LAYERS_REPOSITORY \
            > /tmp/lftp.out.tmp
        cd $OLD_DIR

        grep -E '(New:|Modified:|Removed:)' /tmp/lftp.out.tmp
        HAVE_UPDATES=$?
        # 0: yes
        #rm -f /tmp/lftp.out.tmp

        debug "\$SYSTEM_STATUS=$SYSTEM_STATUS; \$HAVE_UPDATES=$HAVE_UPDATES"
        if [ "$SYSTEM_STATUS" -eq 1 -a "$HAVE_UPDATES" -eq 0 ]; then
            # y
            logger -t "$TAG" "updates found."
            # run lftp
            debug "updating local copy"
            debug "\$RSYNC_LAYERS_REPOSITORY=$RSYNC_LAYERS_REPOSITORY; \$UPDATED_LAYERS_DIRECTORY=$UPDATED_LAYERS_DIRECTORY"
            OLD_DIR=`pwd`
            cd $UPDATED_LAYERS_DIRECTORY
            lftp -e'mirror -e -L; quit' $RSYNC_LAYERS_REPOSITORY 
            cd $OLD_DIR

            # set flag
            debug "setting flag"
            logger -t "$TAG" "setting local copy as ready."
            SYSTEM_STATUS=$?
            debug "\$SYSTEM_STATUS=$SYSTEM_STATUS"
            if [ "$SYSTEM_STATUS" -eq 0 ]; then
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
