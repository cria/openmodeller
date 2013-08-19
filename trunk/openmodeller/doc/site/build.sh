#!/bin/sh

# generate algorithms pages
echo "===> generating algorithm pages using openModeller"
./gen_alg_pages.py

# generate command-line pages
echo "===> generating command line pages"
if [ ! -d "man1" ]; then
  mkdir man1
fi

ls ../../src/console/man/*.1 2> /dev/null | while read path; do
  filename=$(basename "$path")
  filename="${filename%.*}"
  echo $filename
  man2html -r "$path" | tail -n +3 > man1/"$filename".1.html
done

sed -i 's/<\/HEAD>/<LINK REL="stylesheet" TYPE="text\/css" HREF="..\/om.css"><\/HEAD>/g' man1/*.1.html
sed -i 's/<A HREF="\/cgi-bin\/man\/man2html">man2html<\/A>/man2html/g' man1/*.1.html
sed -i 's/<A HREF="..\/index.html">Return to Main Contents<\/A>//g' man1/*.1.html

# generate regular t2t pages
echo "===> generating HTML pages from .t2t files"
files="alg_manual
       developers 
       documentation 
       index
       news 
       om_desktop
       overview 
       publications"

for f in $files; do
    txt2tags $f.t2t
done

# Installation guide
cp -u ../INSTALL.t2t ./INSTALL.t2t
txt2tags -o INSTALL.html INSTALL.t2t

# Web service documentation
txt2tags -o web_service.html ../../src/soap/1.0/web_service.t2t
txt2tags -o om_server.html ../../src/soap/1.0/om_server.t2t
txt2tags -o web_service_2.html ../../src/soap/2.0/web_service.t2t
txt2tags -o om_server_2.html ../../src/soap/2.0/om_server.t2t

# Change log
cp -u ../../ChangeLog ChangeLog
echo "\n\n\n%!target: html\n%! style: om.css\n%! encoding: utf-8\n%!include: menu.t2t\n==openModeller framework change log==" > changelog.t2t
cat ChangeLog >> changelog.t2t
sed -i 's/\t\*/-/g' changelog.t2t
sed -i 's/^\(Release.*\)/**\1**/g' changelog.t2t
txt2tags -o changelog.html changelog.t2t
