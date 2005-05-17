#!/bin/bash

##
## A simple bash script to perform a checkinstall
##

# Accept default answers to all questions
# Set name
# Set version
# Set software group
# The package maintainer (.deb)

checkinstall --default --exclude ~/.ccache/stats --pkgname=libopenmodeller --pkgversion=0.3preview --pkggroup=GIS --maintainer=tim@reading.ac.uk        

