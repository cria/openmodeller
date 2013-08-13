#!/bin/sh

# generate algorithms pages
echo "===> generating HTML pages directly from openModeller"
./genpage.py

# generate pages
echo "===> generating HTML pages from .t2t files"
files="developers 
       documentation 
       environmental_data 
       main
       news 
       om_desktop
       overview 
       publications
       screenshots
       title"

for f in $files; do
    txt2tags $f.t2t
done

