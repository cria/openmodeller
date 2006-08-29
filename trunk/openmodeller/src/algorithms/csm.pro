#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

# include global settings from the top level dir
include (../../settings.pro)

TARGET = csm 
LIBS += -lgsl -lgslcblas
LIBS += $${OMLIBADD}
TEMPLATE = lib
DESTDIR=$${DESTDIR}/$${ALGDIR}
CONFIG(debug, debug|release){
    TARGET = $$member(TARGET, 0)-debug
}
message("Installing to $${DESTDIR}")

#################################################################


HEADERS += csm.hh \
           csmbs.hh 
            
SOURCES += csm.cpp \
           csmbs.cpp 

