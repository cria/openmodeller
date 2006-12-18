#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

# include global settings from the top level dir
include (../../../settings.pro)

TARGET = garp
LIBS += $${OMLIBADD}
TEMPLATE = lib
DESTDIR=$${DESTDIR}/$${ALGDIR}
CONFIG(debug, debug|release){
    TARGET = $$member(TARGET, 0)-debug
}
message("Installing to $${DESTDIR}")


#################################################################

HEADERS += garp.hh \
           ruleset.hh \
           rules_range.hh \
           rules_negrange.hh \
           rules_logit.hh \
           rules_base.hh \
           regression.hh \
           bioclim_histogram.hh
            
SOURCES += garp.cpp \
           ruleset.cpp \
           rules_range.cpp \
           rules_negrange.cpp \
           rules_logit.cpp \
           rules_base.cpp \
           regression.cpp \
           bioclim_histogram.cpp
