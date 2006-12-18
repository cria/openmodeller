#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

# include global settings from the top level dir
include (../../../settings.pro)

TARGET = dg_garp_bs
LIBS += $${OMLIBADD}
TEMPLATE = lib
DESTDIR=$${DESTDIR}/$${ALGDIR}
CONFIG(debug, debug|release){
    TARGET = $$member(TARGET, 0)-debug
}
message("Installing to $${DESTDIR}")

#################################################################

INCLUDEPATH += ../dg_garp

HEADERS += 	AbstractBestSubsets.hh \
		AlgorithmRun.hh \
		DgGarpBestSubsets.hh \
		threads.hh \
		GenericBestSubsets.hh 
            
SOURCES += 	AbstractBestSubsets.cpp \
		AlgorithmRun.cpp \
		DgGarpBestSubsets.cpp 
