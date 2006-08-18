#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

# include global settings from the top level dir
include (../../../settings.pro)

TARGET = garp_bs
win32{
  #on windows build libs into bin dir!
  CONFIG(debug, debug|release){
    DESTDIR=$${PREFIX}/omgui1-debug/algs
    TARGET = $$member(TARGET, 0)-debug
    LIBS += -L$${PREFIX}/omgui1-debug -lopenmodeller-debug
  }else{
    DESTDIR=$${PREFIX}/omgui1/algs
    LIBS += -L$${PREFIX}/omgui1 -lopenmodeller
  }
}else{
  DESTDIR=$${PREFIX}/lib
  LIBS += -L$${PREFIX}/omgui1 -lopenmodeller
}
message("Installing to $${PREFIX}")
TEMPLATE = lib

#################################################################

contains(OMG_STATIC,true){
  #This option is used when creating a statically linked
  #lib to prevent double linked libraries. 
  CONFIG += create_prl        
}else{
}

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
