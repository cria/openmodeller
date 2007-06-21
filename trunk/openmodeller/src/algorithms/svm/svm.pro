#################################################################
#
#         QMAKE Project File for openModeller lib
# 
#                      Tim Sutton 2007
#
#################################################################

# include global settings from the top level dir
include (../../../settings.pro)
TARGET = svm
LIBS += $${OMLIBADD}
TEMPLATE = lib
DESTDIR=$${DESTDIR}/$${ALGDIR}
CONFIG(debug, debug|release){
    TARGET = $$member(TARGET, 0)-debug
}
message("Installing to $${DESTDIR}")
DEFINES += OMDATAPATH="'data'"
#################################################################


HEADERS += svm.hh \
  svm_alg.h
            
SOURCES += svm.cpp \
  svm_alg.cpp

