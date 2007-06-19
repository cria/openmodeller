# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = LIBSVM_COPYRIGHT

OMSRCDIR = @top_srcdir@/src
OMLIB = $(OMSRCDIR)/openmodeller/libopenmodeller.la

plugindir = ${libdir}/openmodeller

plugin_LTLIBRARIES = libomsvm.la

libomsvm_la_SOURCES = \
	svm.cpp \
	svm.hh \
	svm_alg.cpp \
	svm_alg.hh

libomsvm_la_LDFLAGS = -no-undefined
libomsvm_la_LIBADD = $(OMLIB)
libomsvm_la_CPPFLAGS = -I$(OMSRCDIR)

