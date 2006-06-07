#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################


message("**** Building om_create ****")

# include global settings from the top level dir
include (../../settings.pro)

TARGET = om_create
win32{
  #on windows build libs into bin dir!
  DESTDIR=$${PREFIX}/omgui1
  LIBS += c:/MinGW/lib/libexpat.a
  LIBS += -L$${PREFIX}/omgui1 -lopenmodeller
}else{
  DESTDIR=$${PREFIX}/lib
  LIBS += /usr/local/lib/libexpat.a
  LIBS += -L$${PREFIX}/lib -lopenmodeller
}
message("Installing to $${PREFIX}")
TEMPLATE = app

#################################################################

contains(OMG_STATIC,true){
  #This option is used when creating a statically linked
  #lib to prevent double linked libraries. 
  CONFIG += create_prl        
win32{
  LIBS += c:/MinGW/lib/libexpat.a
  LIBS += $${PREFIX}/omgui1/libopenmodeller.a
}else{
  LIBS += /usr/local/lib/libexpat.a #hard coded for mac! and linking statically for mac
  LIBS += $${PREFIX}/lib/libopenmodeller.a
}
}

#################################################################

HEADERS +=  consolexml.hh
            
SOURCES += consolexml.cpp \
           om_create.cpp

