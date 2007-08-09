# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

SUBDIRS = garp dg_garp best_subsets svm

OMSRCDIR = @top_srcdir@/src
OMLIB = $(OMSRCDIR)/openmodeller/libopenmodeller.la

plugindir = ${libdir}/openmodeller

plugin_LTLIBRARIES = \
		libomdistance_to_average.la \
		libomminimum_distance.la \
		libombioclim.la

libomdistance_to_average_la_SOURCES = \
	distance_to_average.cpp \
	distance_to_average.hh

libombioclim_la_SOURCES = \
	bioclim.cpp \
	bioclim.hh

libomminimum_distance_la_SOURCES = \
	minimum_distance.cpp \
	minimum_distance.hh

libomdistance_to_average_la_LDFLAGS = -no-undefined
libomdistance_to_average_la_LIBADD = $(OMLIB)
libomdistance_to_average_la_CPPFLAGS = -I$(OMSRCDIR) -Ddllexp= -DOM_ALG_DLL_EXPORT=

libombioclim_la_LDFLAGS = -no-undefined
libombioclim_la_LIBADD = $(OMLIB)
libombioclim_la_CPPFLAGS = -I$(OMSRCDIR) -Ddllexp= -DOM_ALG_DLL_EXPORT=

libomminimum_distance_la_LDFLAGS = -no-undefined
libomminimum_distance_la_LIBADD = $(OMLIB)
libomminimum_distance_la_CPPFLAGS = -I$(OMSRCDIR) -Ddllexp= -DOM_ALG_DLL_EXPORT=

#
# Build csm - kaiser-gutman only when gsl is available
#
#if HAVE_GSL
#plugin_LTLIBRARIES += libomcsmkg.la
#libomcsmkg_la_SOURCES = \
#	csm.cpp \
#  csm.hh \
#	csmkg.cpp \
#	csmkg.hh
#libomcsmkg_la_LDFLAGS = -no-undefined
#libomcsmkg_la_LIBADD = $(GSL_LIBS) $(OMLIB)
#libomcsmkg_la_CPPFLAGS = $(GSL_CFLAGS) -I../src/inc -I../src/inc/serialization -Ddllexp= -DOM_ALG_DLL_EXPORT=
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
libomcsmbs_la_LIBADD = $(GSL_LIBS) $(OMLIB)
libomcsmbs_la_CPPFLAGS = $(GSL_CFLAGS) -I$(OMSRCDIR) -Ddllexp= -DOM_ALG_DLL_EXPORT=
endif

#
# Build AquaMaps
#
#if HAVE_SQLITE3
#
#OMDATAPATH=-DOMDATAPATH=\"$(pkgdatadir)\"
#
#plugin_LTLIBRARIES += libomaquamaps.la
#libomaquamaps_la_SOURCES = \
#	aquamaps.cpp \
#	aquamaps.hh
#libomaquamaps_la_LDFLAGS = -no-undefined
#libomaquamaps_la_LIBADD = $(SQLITE3_LIB) $(OMLIB)
#libomaquamaps_la_CPPFLAGS = $(OMDATAPATH) $(SQLITE3_INC) -I$(OMSRCDIR) -Ddllexp= -DOM_ALG_DLL_EXPORT=
#endif

#
# Build Environmental Distance
#
plugin_LTLIBRARIES += libomenvironmental_distance.la
libomenvironmental_distance_la_SOURCES = environmental_distance.cpp environmental_distance.hh matrix.hh
libomenvironmental_distance_la_LDFLAGS = -no-undefined
libomenvironmental_distance_la_LIBADD = $(OMLIB)
libomenvironmental_distance_la_CPPFLAGS = -I$(OMSRCDIR) -Ddllexp= -DOM_ALG_DLL_EXPORT=
