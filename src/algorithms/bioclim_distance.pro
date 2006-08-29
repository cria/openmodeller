#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

# include global settings from the top level dir
include (../../settings.pro)
TARGET = bioclim_distance
TEMPLATE = lib
LIBS += $${OMLIBADD}
DESTDIR=$${DESTDIR}/$${ALGDIR}
CONFIG(debug, debug|release){
    TARGET = $$member(TARGET, 0)-debug
}
message("Installing to $${DESTDIR}")

#################################################################

HEADERS += bioclim_distance.hh 
            
SOURCES += bioclim_distance.cpp

