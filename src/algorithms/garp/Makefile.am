# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

SUBDIRS = test

plugindir = ${pkglibdir}

plugin_LTLIBRARIES = libomgarp.la

libomgarp_la_SOURCES = \
	garp.cpp ruleset.cpp garp_sampler.cpp \
	rules_base.cpp rules_range.cpp rules_negrange.cpp \
	rules_atomic.cpp rules_logit.cpp 
	garp.hh  ruleset.hh  garp_sampler.hh \
	rules_base.hh rules_range.hh rules_negrange.hh \
	rules_atomic.hh rules_logit.hh 

libomgarp_la_LIBADD = ../../src/lib/libopenmodeller.la
libomgarp_la_CPPFLAGS = -I../../src/inc
