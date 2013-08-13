#!/bin/sh

# generate algorithms pages
echo "===> generating HTML pages directly from openModeller"
./genpage.py

# generate pages
echo "===> generating HTML pages from .t2t files"
files="alg_manual
       developers 
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

txt2tags -o INSTALL.html ../INSTALL.t2t
txt2tags -o web_service_1-0.html ../../src/soap/1.0/web_service.t2t
txt2tags -o om_server_1-0.html ../../src/soap/1.0/om_server.t2t
txt2tags -o web_service_2-0.html ../../src/soap/2.0/web_service.t2t
txt2tags -o om_server_2-0.html ../../src/soap/2.0/om_server.t2t

