#!/bin/bash
#set -x
if [ -d `pwd`/src ]
then
  #src exists so we are prolly in the right dir
  echo "good we are in openModeller checkout dir!"
else
  echo "You must run this from the top level openModeller checkout dir!"
  exit 1
fi

if [ -d `pwd`/debian ]
  echo "Debian directory is missing!"
  exit 1
fi

export DEBFULLNAME="Tim Sutton"
export DEBEMAIL=tim@linfiniti.com
dch -v 0.5.1+svn`date +%Y%m%d`
fakeroot dpkg-buildpackage

