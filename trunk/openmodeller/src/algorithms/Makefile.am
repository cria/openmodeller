# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

SUBDIRS = garp dg_garp

plugindir = ${pkglibdir}

plugin_LTLIBRARIES = \
		libomdistance_to_average.la \
		libomminimum_distance.la \
		libombioclim.la \
		libombioclim_distance.la

libomdistance_to_average_la_SOURCES = \
	distance_to_average.cpp \
	distance_to_average.hh

libombioclim_la_SOURCES = \
	bioclim.cpp \
	bioclim.hh

libombioclim_distance_la_SOURCES = \
	bioclim_distance.cpp \
	bioclim_distance.hh

libomminimum_distance_la_SOURCES = \
	minimum_distance.cpp \
	minimum_distance.hh

libomdistance_to_average_la_LDFLAGS = -no-undefined
libomdistance_to_average_la_LIBADD = ../src/lib/libopenmodeller.la
libomdistance_to_average_la_CPPFLAGS = -I../src/inc -I../src/inc/serialization

libombioclim_la_LDFLAGS = -no-undefined
libombioclim_la_LIBADD = ../src/lib/libopenmodeller.la
libombioclim_la_CPPFLAGS = -I../src/inc -I../src/inc/serialization

libombioclim_distance_la_LDFLAGS = -no-undefined
libombioclim_distance_la_LIBADD = ../src/lib/libopenmodeller.la
libombioclim_distance_la_CPPFLAGS = -I../src/inc -I../src/inc/serialization

libomminimum_distance_la_LDFLAGS = -no-undefined
libomminimum_distance_la_LIBADD = ../src/lib/libopenmodeller.la
libomminimum_distance_la_CPPFLAGS = -I../src/inc -I../src/inc/serialization

#
# Build csm - kaiser-gutman only when gsl is available
#
#if HAVE_GSL
#plugin_LTLIBRARIES += libomcsmkg.la
#libomcsmkg_la_SOURCES = \
#	csm.cpp \
#	csm.hh \
#	csmkg.cpp \
#	csmkg.hh
#libomcsmkg_la_LDFLAGS = -no-undefined
#libomcsmkg_la_LIBADD = $(GSL_LIBS) ../src/lib/libopenmodeller.la
#libomcsmkg_la_CPPFLAGS = $(GSL_CFLAGS) -I../src/inc -I../src/inc/serialization
#endif
#
# Build csm - broken stick only when gsl is available
#
if HAVE_GSL
plugin_LTLIBRARIES += libomcsmbs.la
libomcsmbs_la_SOURCES = \
	csm.cpp \
	csm.hh \
	csmbs.cpp \
	csmbs.hh
libomcsmbs_la_LDFLAGS = -no-undefined
libomcsmbs_la_LIBADD = $(GSL_LIBS) ../src/lib/libopenmodeller.la
libomcsmbs_la_CPPFLAGS = $(GSL_CFLAGS) -I../src/inc -I../src/inc/serialization
endif
