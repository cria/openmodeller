

         Windows Installer scripts for openModeller and omgui
                         Tim Sutton 2005

To Use:

* Install the Nullsoft Scriptable Install System (available at http://nsis.sourceforge.net) - hereafter referred to as 'NSIS'

* Create binary versions of om and gui and place them in the same directory (the 'build' directory) along with all deps such as qt, proj, gdal etc. 

* For a full list of required deps, consult the nsi file included in this dir. 

* Copy this om_installer directory into the build directory.

* Copy any sample data you wish to distribute into [build dir]\om_sample_data\
  (or just make the above dir but leave it blank if you wish)

* open the NSIS app

* Choose MakeNSISW (compiler interface) option

* In the compiler, open the omgui.nsi folder in this directory

* If the gods are smiling on you, the gui will be compiled and you will have a nice gui install wizard you can run.

TODO:

* Create options in installer to choose 1 or more installation options:
  - om libs only
  - console apps
  - omgui standalone
  - omgui qgis plugin
  

$version: $
$log: $
