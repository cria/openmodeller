 Notes for using NSIS installer scripts.

             Tim Sutton 2006
---------------------------------------

The installer script has an optional data
download part. This requires an NSIS 
plugin available here:

http://nsis.sourceforge.net/ZipDLL
or
http://nsis.sourceforge.net/ZipDLL_plug-in

Follow the instructions on that page before
attempting to run the installer.

NOTE: the zipdll.nsh at the download was 
broken at time of writing this. Use the 
one provided in this directory instead
(still copy it to your nsis include dir 
though).

The NSIS installer now expects the openModeller 
binaries to be packaged to exist in 
c:\Program Files\openModeller<version> 

Set the PRODUCT_VERSION_NUMBER in openModeller.nsis 
before running. This will allow having multiple 
development versions on the same machine and 
being able to package them without issue.

Unattended Install:
===================

The installer supports silent installation now. 
Run from the command line like this

openModellerSetup.exe /S

There are a few things to note:
-  after running the above command the installer will 
   fork to the background and  immediately return 
   you to the prompt.
- the installer uses all default options that means 
  that it will NOT try to go onto the internet and 
  retrieve the two sample datasets - since these are
  disabled options by default.
- if you wish to customise the datasets etc that are 
  installed, you can quite easily modify the nsis 
  installer - its in svn under openmodeller/install_win/
