#!/bin/bash

##
## A simple bash script to perform a checkinstall
##

# Accept default answers to all questions
# Set name
# Set version
# Set software group
# The package maintainer (.deb)

checkinstall --default --pkgname=openModeller --pkgversion=0.2.1 --pkggroup=GIS --maintainer=p.w.brewer@rdg.ac.uk        

