#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################
DEFINES += VERSION='0.3.5' 
DEFINES += CONFIG_FILE="'./pluginpath.cfg'"
DEFINES += PLUGINPATH="'./'"
DEFINES += WIN32
OMG_DEBUGMODE=false
OMG_STATIC=false

win32{
  message(Installing for windows!)
  DEFINES += MINGW_QT
  LIBS+=-L"C:\Mingw\lib"
  INCLUDEPATH += . 
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
}

#
#    all platforms
#

CONFIG += warn_off 
CONFIG -= core network gui
LANGUAGE  = C++
LIBS += -L$${PREFIX}/lib/
LIBS += -lgdal -lexpat 
INCLUDEPATH +="c:\dev\cpp\om\src" #hard coded for now!
INCLUDEPATH +="c:\dev\cpp\om\src\openmodeller" #hard coded for now!
INCLUDEPATH +="c:\dev\cpp\om\src\openmodeller\env_io" #hard coded for now!
INCLUDEPATH +="c:\dev\cpp\om\src\openmodeller\models" #hard coded for now!
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


