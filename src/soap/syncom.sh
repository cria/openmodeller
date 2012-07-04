#!/bin/sh

#######################################################################
# Script that can be included in the cronjob to update openModeller. 
# Requires Subversion and curl.
# Parameter 1: path to configuration file.
#######################################################################

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

# If configuration file exists, read the configuration
if [ -f $CONFIG ]; then
  # read configuration
  readconf $CONFIG
else
  echo "No configuration file provided. Aborting."
  exit 1
fi

${CTRL_FILE:?"Missing entry CTRL_FILE in the configuration. Aborting."}

omrev=`curl -s ${CTRL_FILE}`

# validates integer
if [ "$omrev" -ne 0 -o "$omrev" -eq 0 2>/dev/null ]; then
    echo "Detected reference revision $omrev."
else
    echo "Could not retrieve reference revision. Aborting."
    exit 1
fi

if [ -d "$SRC_DIR" ]; then
    cd $SRC_DIR
    svn -r$omrev update
    #rm -fr cmake/FindGDAL* # workaround for Mac and BSD
    cd build
    make
    make install
else
    echo "$SRC_DIR directory does not exist, checking out code."
    svn co -r$omrev https://openmodeller.svn.sourceforge.net/svnroot/openmodeller/trunk/openmodeller $SRC_DIR
    cd $SRC_DIR
    #rm -fr cmake/FindGDAL* # workaround
    mkdir build
    cd build
    cmake -DOM_BUILD_SERVICE=ON ..
    make
    make install
fi
