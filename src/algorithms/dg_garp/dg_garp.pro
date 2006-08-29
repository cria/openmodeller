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
LIBS += $${OMLIBADD}
TEMPLATE = lib
DESTDIR=$${DESTDIR}/$${ALGDIR}
CONFIG(debug, debug|release){
    TARGET = $$member(TARGET, 0)-debug
}
message("Installing to $${DESTDIR}")


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
