#!/bin/bash
#set -x
if [ -d `pwd`/src ]
then
  #src exists so we are prolly in the right dir
  echo "Good we are in openModeller checkout dir!"
else
  echo "You must run this from the top level openModeller checkout dir!"
  exit 1
fi

if [ -d `pwd`/debian ]
then
  echo "Good we have a debian dir!"
else
  echo "Debian directory is missing!"
  exit 1
fi

export DEBFULLNAME="Tim Sutton"
export DEBEMAIL=tim@linfiniti.com

# you may need to run make once to freshen 
# the manifest before building the package
cp install_manifest.txt debian/libopenmodeller.txt
#dch -v 0.5.+svn`date +%Y%m%d`
dch -v 0.5.2
fakeroot dpkg-buildpackage

