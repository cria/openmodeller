# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#EXTRA_DIST = doc

SUBDIRS = lib inc

bin_PROGRAMS = om_console om_viewer

om_console_SOURCES = om_console.cpp
om_console_CPPFLAGS = -Iinc
om_console_LDADD = $(GSL_LIB) $(GDAL_LIB) ./lib/libopenmodeller.la ./lib/env_io/libenv_io.la

om_viewer_SOURCES = om_viewer.cpp
om_viewer_CPPFLAGS = -Iinc
om_viewer_LDADD = $(GSL_LIB) $(GDAL_LIB) $(X11_LIB) ./lib/libopenmodeller.la ./lib/env_io/libenv_io.la ./lib/graph/libopenmodellergraph.la
