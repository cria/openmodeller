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
TEMPLATE=app
TARGET = om_create
LIBS += $${OMLIBADD}
unix:DESTDIR=$${DESTDIR}/bin
CONFIG(debug, debug|release){
    TARGET = $$member(TARGET, 0)-debug
}
message("Installing to $${DESTDIR}")


#################################################################

HEADERS +=  consolexml.hh
            
SOURCES += consolexml.cpp \
           om_create.cpp

