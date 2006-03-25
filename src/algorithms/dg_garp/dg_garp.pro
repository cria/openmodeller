#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

# include global settings from the top level dir
include (../../../settings.pro)

TARGET = dg_garp
win32{
  #on windows build libs into bin dir!
  DESTDIR=$${PREFIX}/omgui1/algs
}else{
  DESTDIR=$${PREFIX}/lib
}
message("Installing to $${PREFIX}")
TEMPLATE = lib
LIBS += -L$${PREFIX}/omgui1 -lopenmodeller

#################################################################

contains(OMG_STATIC,true){
  #This option is used when creating a statically linked
  #lib to prevent double linked libraries. 
  CONFIG += create_prl        
}else{
}

#################################################################

HEADERS += 	GarpAlgorithm.h \
		EnvCell.h \
		EnvCellSet.h \
		Rule.h \
		RuleSet.h \
		Utilities.h
            
SOURCES += 	GarpAlgorithm.cpp \
		EnvCell.cpp \
		EnvCellSet.cpp \
		Rule.cpp \
		RuleSet.cpp \
		Utilities.cpp 
