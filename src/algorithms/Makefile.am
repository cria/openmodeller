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
	libomdistance_to_average.la \
	libombioclim.la \
	libomminimum_distance.la \
	libomcsm.la

libomdistance_to_average_la_SOURCES = \
	distance_to_average.cpp \
	distance_to_average.hh

libombioclim_la_SOURCES = \
	bioclim.cpp \
	bioclim.hh

libomminimum_distance_la_SOURCES = \
	minimum_distance.cpp \
	minimum_distance.hh

libomcsm_la_SOURCES = \
	csm.cpp \
	csm.hh

libomdistance_to_average_la_CPPFLAGS = $(GSL_CFLAGS) -I../src/inc
libombioclim_la_CPPFLAGS = -I../src/inc
libomminimum_distance_la_CPPFLAGS = -I../src/inc
libomcsm_la_CPPFLAGS = -I../src/inc


