#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

DEFINES += PLUGINPATH="'./algs/'"
OMG_DEBUGMODE=true
## Dont link to any qt libs!
CONFIG -= qt
win32{
  OMG_STATIC=false
  DEFINES += CONFIG_FILE="'./pluginpath.cfg'"
  DEFINES += VERSION='0.4snapshot' 
  DEFINES += WIN32
  message(Installing for windows!)
  DEFINES += MINGW_QT
  LIBS+=-L"C:\Mingw\lib"
  INCLUDEPATH += . 
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
  OMG_STATIC=true
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
LIBS += -L$${PREFIX}/lib/
####################################################


# Where binary exes and libs should be placed when built
DESTDIR = $${PREFIX}
message(Install prefix set to $$PREFIX)
message(The compiled binaries will be installed in $$DESTDIR)

####################################################

contains(OMG_DEBUGMODE,true){
  message("Building with debugging support")
  CONFIG+=debug
  win32{
    CONFIG+=console
  }
}else{
  message("Debugging support disabled")
}

####################################################


contains(OMG_STATIC,true){
  message("Building statically")
  CONFIG+=static
}else{
  message("Building dynamically")
}

####################################################


