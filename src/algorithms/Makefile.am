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

if HAVE_GSL

endif

plugin_LTLIBRARIES = \
	libombioclim.la \
	libomcsm.la \
	libomdist_min.la \
	libomdistance.la

libombioclim_la_SOURCES = \
	bioclim.cpp \
	bioclim.hh

libomcsm_la_SOURCES = \
	csm.cpp \
	csm.hh

libomdist_min_la_SOURCES = \
	dist_min.cpp \
	dist_min.hh

libomdistance_la_SOURCES = \
	distance.cpp \
	distance.hh

libombioclim_la_CPPFLAGS = -I../src/inc
libomcsm_la_CPPFLAGS = -I../src/inc
libomdist_min_la_CPPFLAGS = -I../src/inc
libomdistance_la_CPPFLAGS = $(GSL_CFLAGS) -I../src/inc
