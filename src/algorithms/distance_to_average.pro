#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

# include global settings from the top level dir
include (../../settings.pro)

TARGET =  distance_to_average
LIBS += $${OMLIBADD}
TEMPLATE = lib
DESTDIR=$${DESTDIR}/$${ALGDIR}
CONFIG(debug, debug|release){
    TARGET = $$member(TARGET, 0)-debug
}
message("Installing to $${DESTDIR}")
#################################################################

HEADERS += distance_to_average.hh 
            
SOURCES += distance_to_average.cpp 

