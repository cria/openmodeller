#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

# include global settings from the top level dir
include (../../settings.pro)
TARGET = aquamaps
LIBS += $${OMLIBADD} $${SQLITELIBADD}
TEMPLATE = lib
DESTDIR=$${DESTDIR}/$${ALGDIR}
CONFIG(debug, debug|release){
    TARGET = $$member(TARGET, 0)-debug
}
message("Installing to $${DESTDIR}")
DEFINES += OMDATAPATH="'data'"
#################################################################


HEADERS += aquamaps.hh 
            
SOURCES += aquamaps.cpp 

