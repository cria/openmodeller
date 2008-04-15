#!/bin/bash

# Script to prepare a mac package root
# Tim Sutton 2007

# You must run make install from in the build dir
# first and I assume that you are installing to
# /usr/local

#clear away any old stuff
rm -rf fakeroot
#make a new fake root
mkdir fakeroot
cd fakeroot
#get the name of every installed file as listed in the manifest
for FILE in `cat ../../build_universal/install_manifest.txt`; do 
  # get just the dir part of the name
  DIR=`dirname $FILE`
  # strip off the leading slash
  DIR=`echo $DIR|sed 's/^\///g'`
  # make sure we have a similar directory in our fake root
  mkdir -p $DIR 
  # copy the original file into our fake root
  cp $FILE $DIR
done
#copy over a few extra needed libs
mkdir -p Library/Frameworks
cp -r /Library/Frameworks/GDAL.framework Library/Frameworks
cp -r /Library/Frameworks/UnixImageIO.framework Library/Frameworks
cp -r /Library/Frameworks/Xerces.framework Library/Frameworks
cp -r /Library/Frameworks/GEOS.framework Library/Frameworks
cp -r /Library/Frameworks/PROJ.framework Library/Frameworks
cp -r /Library/Frameworks/SQLite3.framework Library/Frameworks
#using -d to preserve symlinks etc AAArgh...not supported on mac!
cp -p /usr/local/lib/libexpat* usr/local/lib/
cp -p /usr/local/lib/libgsl* usr/local/lib/

# All done!
