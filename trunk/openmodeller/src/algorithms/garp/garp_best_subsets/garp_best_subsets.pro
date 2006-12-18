#################################################################
#
#         QMAKE Project File for openModeller Gui
# 
#                      Tim Sutton 2005
#
#################################################################

# include global settings from the top level dir
include (../../../../settings.pro)

TARGET = garp_bs
LIBS += $${OMLIBADD}
TEMPLATE = lib
DESTDIR=$${DESTDIR}/$${ALGDIR}
CONFIG(debug, debug|release){
    TARGET = $$member(TARGET, 0)-debug
}
message("Installing to $${DESTDIR}")

#################################################################

DEFINES += DONT_EXPORT_GARP_FACTORY

INCLUDEPATH += ../

HEADERS += best_subsets.hh \
           garp_best_subsets.hh \
           garp_run.hh \ 
           garp_run_thread.hh
            
SOURCES += best_subsets.cpp \
           bs_algorithm_factory.cpp \
           garp_best_subsets.cpp \
           garp_run.cpp  

HEADERS += ../garp.hh \
           ../ruleset.hh \
           ../rules_range.hh \
           ../rules_negrange.hh \
           ../rules_logit.hh \
           ../rules_base.hh \
           ../regression.hh \
           ../bioclim_histogram.hh
            
SOURCES += ../garp.cpp \
           ../ruleset.cpp \
           ../rules_range.cpp \
           ../rules_negrange.cpp \
           ../rules_logit.cpp \
           ../rules_base.cpp \
           ../regression.cpp \
           ../bioclim_histogram.cpp
