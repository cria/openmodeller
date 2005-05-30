

         Windows Installer scripts for openModeller and omgui
                         Tim Sutton 2005

To Use:

* Install the Nullsoft Scriptable Install System (available at http://nsis.sourceforge.net) - hereafter referred to as 'NSIS'
* Create a directory named 'build' on the root of openModeller modeller module (on the same level as algorithms, src, examples, etc)
* Create binary versions of om and gui and place them into the 'build' directory along with all deps such as qt, proj, gdal etc. 
* For a full list of required deps, consult the nsi file included in this dir. 
* Copy any sample data you wish to distribute into build\om_sample_data\
  (or just make the above dir but leave it blank if you wish)
* open the NSIS app
* Choose MakeNSISW (compiler interface) option
* In the compiler, open the omgui_setup.nsi folder in this directory
* If the gods are smiling on you, the gui will be compiled and you will have a nice gui install wizard you can run.
$version: $
$log: $
