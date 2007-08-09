# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

SUBDIRS = garp_best_subsets test

OMSRCDIR = @top_srcdir@/src
OMLIB = $(OMSRCDIR)/openmodeller/libopenmodeller.la

plugindir = ${libdir}/openmodeller

# left out of build due to instabilities in algorithm...
plugin_LTLIBRARIES = libomgarp.la

# ...but include all sources in the distribution
EXTRA_DIST = \
	garp.cpp ruleset.cpp \
	bioclim_histogram.cpp regression.cpp \
	rules_base.cpp rules_range.cpp rules_negrange.cpp rules_logit.cpp \
	garp.hh ruleset.hh \
	bioclim_histogram.hh regression.hh \
	rules_base.hh rules_range.hh rules_negrange.hh rules_logit.hh

libomgarp_la_SOURCES = \
	garp.cpp ruleset.cpp \
	bioclim_histogram.cpp regression.cpp \
	rules_base.cpp rules_range.cpp rules_negrange.cpp rules_logit.cpp \
	garp.hh ruleset.hh \
	bioclim_histogram.hh regression.hh \
	rules_base.hh rules_range.hh rules_negrange.hh rules_logit.hh

libomgarp_la_LDFLAGS = -no-undefined
libomgarp_la_LIBADD = $(OMLIB)
libomgarp_la_CPPFLAGS = -I$(OMSRCDIR) -Ddllexp= -DOM_ALG_DLL_EXPORT=
