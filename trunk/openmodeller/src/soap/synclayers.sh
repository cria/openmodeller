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

debug "\$REFERENCE_LAYERS_REPOSITORY=$REFERENCE_LAYERS_REPOSITORY"

# workaround to follow documentation
LAYERS_DIRECTORY="/var/local/om/${LAYERS_DIRECTORY}"
UPDATED_LAYERS_DIRECTORY="/var/local/om/${UPDATED_LAYERS_DIRECTORY}"
debug "\$LAYERS_DIRECTORY=$LAYERS_DIRECTORY"
debug "\$UPDATED_LAYERS_DIRECTORY=$UPDATED_LAYERS_DIRECTORY"

# by default, we'll use 2 repositories in each server, at least when
# this script syncs, which are (1) the effective repository, which is
# $LAYERS_DIRECTORY and (2) the new repository, which is
# $UPDATED_LAYERS_DIRECTORY

check_om_processes() {
    # checking for running oM instances
    logger -t "$TAG" "checking for running oM instances."
    if [ $(pgrep om_model) -o  $(pgrep om_test) -o $(pgrep om_project) ]; then
        # y: some job is still running
        logger -t "$TAG" "jobs are still running. aborting."
        exit 0
    else
        # n
        # lock server
        logger -t "$TAG" "no jobs running, locking server."
        cat $CONFIG | \
            sed 's/SYSTEM_STATUS=1/SYSTEM_STATUS=2/' > \
            ${CONFIG}.tmp
        mv ${CONFIG}.tmp $CONFIG
        logger -t "$TAG" "service locked."

        # update work dir
        logger -t "$TAG" "updating working copy."
        rsync -aL --copy-unsafe-links --delete \
            $UPDATED_LAYERS_DIRECTORY $LAYERS_DIRECTORY

        # remove flag
        debug "removing flag"
        rm -f /tmp/COPY_READY

        # erase cache
        rm -f $CACHE_DIRECTORY/layers.xml

        # unlock server
        cat $CONFIG | \
            sed 's/SYSTEM_STATUS=2/SYSTEM_STATUS=1/' > \
            ${CONFIG}.tmp
        mv ${CONFIG}.tmp $CONFIG
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
        logger -t "$TAG" "checking updates."
        OLD_DIR=`pwd`
        cd $UPDATED_LAYERS_DIRECTORY
        debug "\$REFERENCE_LAYERS_REPOSITORY=$REFERENCE_LAYERS_REPOSITORY; \$UPDATED_LAYERS_DIRECTORY=$UPDATED_LAYERS_DIRECTORY"
        lftp -e'mirror -e --dry-run -L; quit' $REFERENCE_LAYERS_REPOSITORY \
            > /tmp/lftp.out.tmp
        cd $OLD_DIR

        grep -E '(New:|Modified:|Removed:)' /tmp/lftp.out.tmp
        HAVE_UPDATES=$?
        # 0: yes
        #rm -f /tmp/lftp.out.tmp

        debug "\$SYSTEM_STATUS=$SYSTEM_STATUS; \$HAVE_UPDATES=$HAVE_UPDATES"
        if [ "$SYSTEM_STATUS" -eq 1 -a "$HAVE_UPDATES" -eq 0 ]; then
            # y
            logger -t "$TAG" "updates found, updating local copy."
            # run lftp
            debug "\$REFERENCE_LAYERS_REPOSITORY=$REFERENCE_LAYERS_REPOSITORY; \$UPDATED_LAYERS_DIRECTORY=$UPDATED_LAYERS_DIRECTORY"
            OLD_DIR=`pwd`
            cd $UPDATED_LAYERS_DIRECTORY
            lftp -e'mirror -e -L; quit' $REFERENCE_LAYERS_REPOSITORY 
            cd $OLD_DIR

            # set flag
            COPY_SUCCEEDED=$?
            debug "\$COPY_SUCCEEDED=$COPY_SUCCEEDED"
            logger -t "$TAG" "setting local copy as ready."
            if [ "$COPY_SUCCEEDED" -eq 0 ]; then
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
