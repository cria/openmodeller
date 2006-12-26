#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################
#set this to where you want om built to
#it will create an omgui1-debug and an omgui1-release dir
#under this location and build the debug and release versions 
#into there
PREFIX=c:\dev\cpp\omgui1

#################################################################
# Normally you should not need to change anything from here on
#################################################################
DEFINES += PLUGINPATH="'./algs/'"
## Dont link to any qt libs!
CONFIG -= qt
CONFIG += debug_and_release
CONFIG += build_all
win32{
  DEFINES += CONFIG_FILE="'./pluginpath.cfg'"
  DEFINES += VERSION='0.4snapshot' 
  DEFINES += WIN32
  message(Installing for windows!)
  DEFINES += MINGW_QT
  LIBS+=-L"C:\msys\local\lib"
  INCLUDEPATH += . 
  INCLUDEPATH +="c:\mingw\include" #hard coded for now! 
  INCLUDEPATH +="c:\dev\cpp\om\src" #hard coded for now!
  INCLUDEPATH +="c:\dev\cpp\om\src\openmodeller" #hard coded for now!
  INCLUDEPATH +="c:\dev\cpp\om\src\openmodeller\env_io" #hard coded for now!
  INCLUDEPATH +="c:\dev\cpp\om\src\openmodeller\models" #hard coded for now!
  LIBS += -lgdal -lexpat 
}

#
# Unix platform specific directives
#

unix{
}

#
# MacOSX platform specific directives
#

macx{
  DEFINES += CONFIG_FILE="'./pluginpath.cfg'"
  DEFINES += VERSION="'0.4'"
  DEFINES += MACOSX
  message(Installing for mac!)
  LIBS+=-L"/usr/local/lib"
  INCLUDEPATH += . 
  INCLUDEPATH +="/Users/timsutton/dev/cpp/om/src" #hard coded for now!
  INCLUDEPATH +="/Users/timsutton/dev/cpp/om/src/openmodeller" #hard coded for now!
  INCLUDEPATH +="/Users/timsutton/dev/cpp/om/src/openmodeller/env_io" #hard coded for now!
  INCLUDEPATH +="/Users/timsutton/dev/cpp/om/src/openmodeller/models" #hard coded for now!
  LIBS += /usr/local/lib/libgdal.a  # for mac we link to the static versions of gdal and expat
  LIBS += /usr/local/lib/libexpat.a  # for mac we link to the static versions of gdal and expat
}

#
#    all platforms
#

CONFIG += warn_off 
CONFIG -= core network gui
LANGUAGE  = C++
####################################################


# Where binary exes and libs should be placed when built
DESTDIR = $${PREFIX}
ALGDIR=algs
CONFIG(debug, debug|release){
  message("Building with debugging support")
  DESTDIR=$${PREFIX}/omgui1-debug
  OMLIBADD=-lopenmodeller-debug
  CONFIG+=console
}else{
  message("Debugging support disabled")
  DESTDIR=$${PREFIX}/omgui1-release
  OMLIBADD=-lopenmodeller
}
unix:LIBS += -L$${DESTDIR}/lib/
win32:LIBS += -L$${DESTDIR}
win32:LIBS += -L$${DESTDIR}/algs
SQLITELIBADD = -lsqlite3
message(Install prefix set to $$PREFIX)
message(The compiled binaries will be installed in $$DESTDIR)

####################################################




