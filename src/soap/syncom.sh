#!/bin/sh
#
# this script assumes it's located on a path on level up the
# openmodeller directory fetched from openModeller SVN on
# Sourceforge.net

SRC_DIR='openmodeller'
CTRL_FILE='http://www.cria.org.br/~daniel/omrev.txt'

omrev=`curl -s ${CTRL_FILE}`
echo "$0 to update to openModeller to rev. $omrev"

if [ -d "$SRC_DIR" ]; then
    cd $SRC_DIR
    svn -r$omrev update
    #rm -fr cmake/FindGDAL* # workaround
    cd build
    make
    sudo make install
else
    svn co -r$omrev https://openmodeller.svn.sourceforge.net/svnroot/openmodeller/trunk/openmodeller
    echo "$SRC_DIR directory does not exists, checkout'ing"
    cd $SRC_DIR
    #rm -fr cmake/FindGDAL* # workaround
    mkdir build
    cd build
    cmake -DOM_BUILD_SERVICE=ON -DCMAKE_INSTALL_PREFIX=/usr/local-dev ..
    #cmake -DOM_BUILD_SERVICE=OFF -DCMAKE_INSTALL_PREFIX=/usr/local-dev ..
    make
    sudo make install
fi
