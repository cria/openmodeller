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

EXTRA_DIST = \
	AlgorithmRun.hh \
	GenericBestSubsets.hh \
	AbstractBestSubsets.hh \
	threads.hh

plugin_LTLIBRARIES = libomdg_bs.la

libomdg_bs_la_SOURCES = \
	AlgorithmRun.cpp \
	AbstractBestSubsets.cpp \
	DgGarpBestSubsets.cpp \
	DgGarpBestSubsets.hh

libomdg_bs_la_LDFLAGS = -no-undefined
libomdg_bs_la_LIBADD = $(OMLIB) $(LIB_PTHREAD)
libomdg_bs_la_CPPFLAGS = -I.. -I$(OMSRCDIR) -I../dg_garp -Ddllexp= -DOM_ALG_DLL_EXPORT=

#libombs_la_SOURCES = \
#	AlgorithmRun.cpp \
#	AbstractBestSubsets.cpp \
#	GenericBestSubsets.cpp
#
#libombs_la_LDFLAGS = -no-undefined
#libombs_la_LIBADD = $(OMLIB) $(LIB_PTHREAD)
#libombs_la_CPPFLAGS = -I.. -I$(OMSRCDIR) -I../dg_garp -Ddllexp= -DOM_ALG_DLL_EXPORT=
