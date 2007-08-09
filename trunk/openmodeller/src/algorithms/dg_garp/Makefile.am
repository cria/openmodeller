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

plugin_LTLIBRARIES = libomoldgarp.la

libomoldgarp_la_SOURCES = \
	GarpAlgorithm.cpp GarpAlgorithm.h \
	EnvCell.cpp EnvCell.h EnvCellSet.cpp EnvCellSet.h \
	Rule.cpp Rule.h RuleSet.cpp RuleSet.h \
	Utilities.cpp Utilities.h

libomoldgarp_la_LDFLAGS  = -no-undefined
libomoldgarp_la_LIBADD   = $(OMLIB)
libomoldgarp_la_CPPFLAGS = -I$(OMSRCDIR) -Ddllexp= -DOM_ALG_DLL_EXPORT=
