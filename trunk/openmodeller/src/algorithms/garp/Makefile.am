# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

plugindir = ${pkglibdir}

plugin_LTLIBRARIES = libomgarp.la

libomgarp_la_SOURCES = \
	garp.cpp garp_rules.cpp garp_sampler.cpp  \
	garp.hh  garp_rules.hh  garp_sampler.hh

libomgarp_la_LIBADD = ../../src/lib/libopenmodeller.la
libomgarp_la_CPPFLAGS = -I../../src/inc
