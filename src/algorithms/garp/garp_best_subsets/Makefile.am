# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

SUBDIRS = 

OMSRCDIR = @top_srcdir@/src
OMLIB = $(OMSRCDIR)/openmodeller/libopenmodeller.la

plugindir = ${libdir}/openmodeller

# do not use -lpthread on windows
if ISWINDOWS
LIB_PTHREAD = 
else
LIB_PTHREAD = -lpthread
endif

#plugin_LTLIBRARIES = libomgarp_bs.la libomdg_garp_bs.la
plugin_LTLIBRARIES = libomgarp_bs.la

libomgarp_bs_la_SOURCES = \
	best_subsets.cpp garp_best_subsets.cpp garp_run.cpp \
	best_subsets.hh bs_algorithm_factory.hh garp_best_subsets.hh garp_run.hh \
	garp_run_thread.hh \
	../garp.cpp ../ruleset.cpp \
	../bioclim_histogram.cpp ../regression.cpp \
	../rules_base.cpp ../rules_logit.cpp \
	../rules_range.cpp ../rules_negrange.cpp \
	../garp.hh  ../ruleset.hh \
	../bioclim_histogram.hh ../regression.hh \
	../rules_base.hh ../rules_logit.hh \
	../rules_range.hh ../rules_negrange.hh

libomgarp_bs_la_LDFLAGS = -no-undefined
libomgarp_bs_la_LIBADD = $(OMLIB) $(LIB_PTHREAD)
libomgarp_bs_la_CPPFLAGS = -I.. -I$(OMSRCDIR) -DDONT_EXPORT_GARP_FACTORY -Ddllexp= -DOM_ALG_DLL_EXPORT=

#libomdg_garp_bs_la_SOURCES = \
#	best_subsets.cpp dg_garp_bs.cpp garp_run.cpp \
#	best_subsets.hh dg_garp_bs.hh bs_algorithm_factory.hh garp_run.hh \
#	garp_run_thread.hh\
#	../../dg_garp/GarpAlgorithm.cpp \
#	../../dg_garp/RuleSet.cpp \
#	../../dg_garp/Rule.cpp \
#	../../dg_garp/EnvCell.cpp \
#	../../dg_garp/EnvCellSet.cpp \
#	../../dg_garp/Utilities.cpp \
#	../../dg_garp/GarpAlgorithm.h \
#	../../dg_garp/RuleSet.h \
#	../../dg_garp/Rule.h \
#	../../dg_garp/EnvCell.h \
#	../../dg_garp/EnvCellSet.h \
#	../../dg_garp/Utilities.h \
#	../../dg_garp/Global.h
#
#libomdg_garp_bs_la_LDFLAGS  = -no-undefined
#libomdg_garp_bs_la_LIBADD   = $(OMLIB) $(LIB_PTHREAD)
#libomdg_garp_bs_la_CPPFLAGS = -I.. -I$(OMSRCDIR) -I../../dg_garp \
#                              -DDONT_EXPORT_GARP_FACTORY -Ddllexp= -DOM_ALG_DLL_EXPORT=

