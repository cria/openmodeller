#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

# include global settings from the top level dir
include (../../settings.pro)
TEMPLATE=app
TARGET = om_project
LIBS += $${OMLIBADD}
INCLUDEPATH += ../lib
unix:DESTDIR=$${DESTDIR}/bin
CONFIG(debug, debug|release){
    TARGET = $$member(TARGET, 0)-debug
}
message("Installing to $${DESTDIR}")

#################################################################

contains(OMG_STATIC,true){
  #This option is used when creating a statically linked
  #lib to prevent double linked libraries. 
  CONFIG += create_prl        
win32{
  LIBS += c:/MinGW/lib/libexpat.a
}
  LIBS += $${PREFIX}/omgui1/libopenmodeller.a
}else{
}

#################################################################

HEADERS += consolexml.h
            
SOURCES += consolexml.cpp \
           om_project.cpp

