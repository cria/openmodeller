
NOTE: This is a test version of openModeller library and command line tools.
Use entirely at your own risk!


This is an openModeller Mac application installer based on the code in Subversion 
Head (i.e. a non stable version of the code). Despite the aforementioned
disclaimer, we keep the library in a fairly stable state always and it should
be fine for some basic testing. It would be great if you could test the
installer and let me know how it works for you. The installer is rather large
since it contains frameworks for GDAL, GEOS, UnixImageIO, PROJ etc. If I (and
others) are happy with the quality of the package I will include it as part of
the next release of the openModeller library. Note the package only contains
openModeller library and the openModeller console tools (om_console, etc). If
you want the Graphical User Interface based openModeller Desktop, there is a
Max OSX application bundle already available for release 1.0.5 of the user
interface.

You can download the test installer here

http://www.cria.org.br/~timlinux/openModellerPackage.zip

I will appreciate any feedback those testing it might have to offer.

Tim Sutton
tim@linfiniti.com

Package contents:
----------------
Library/Frameworks:
GDAL.framework
GEOS.framework
PROJ.framework
SQLite3.framework
UnixImageIO.framework
Xerces.framework

usr/local/bin:
om_console
om_create
om_dump
om_niche
om_project
om_pseudo
om_sampledump
om_soap_server
om_testmodel
om_viewer
scheduler.sh

usr/local/include/openmodeller:
General Headers for openModeller lib

usr/local/include/openmodeller/env_io:
Headers for the env_io part of openModeller lib

usr/local/include/openmodeller/occ_io:
Headers for the occ_io part of openModeller lib

usr/local/lib:
libexpat.1.5.0.dylib
libexpat.1.dylib
libexpat.a
libexpat.dylib
libexpat.la
libgsl.0.9.0.dylib
libgsl.0.dylib
libgsl.a
libgsl.dylib
libgsl.la
libgslcblas.0.0.0.dylib
libgslcblas.0.dylib
libgslcblas.a
libgslcblas.dylib
libgslcblas.la
libopenmodeller.dylib
libopenmodellerxml.dylib

usr/local/lib/openmodeller:
libaquamaps.so
libbioclim.so
libcsm_bs.so
libdesktop_garp.so
libdesktop_garp_bs.so
libenvironmental_distance.so
libgarp.so
libgarp_best_subsets.so

usr/local/share/openmodeller/examples:
README
furcata_boliviana.txt
model_request.xml
projection_request.xml
rain_coolest.tif
request.txt
request_te.txt
temp_avg.tif
