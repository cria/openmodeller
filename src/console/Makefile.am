# Copyright (C) 2004 Mauro Muñoz - CRIA <mauro at cria.org.br>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

if HAVE_X11
GRAPH = graph
OM_VIEWER = om_viewer
endif

EXTRA_DIST = \
	occurrences_file.hh \
	graph/color.hh \
	graph/graphic.hh \
	graph/graphic_x11.hh \
	graph/view_model.hh


SUBDIRS = $(GRAPH)

bin_PROGRAMS = om_console $(OM_VIEWER)

om_console_SOURCES = om_console.cpp occurrences_file.cpp
om_console_CPPFLAGS = -I../inc -g
om_console_LDADD = $(GDAL_LIB) ../lib/libopenmodeller.la

om_viewer_SOURCES = om_viewer.cpp occurrences_file.cpp
om_viewer_CPPFLAGS = -I../inc
om_viewer_LDADD = $(GDAL_LIB) $(X11_LIB) graph/libopenmodellergraph.la \
	../lib/libopenmodeller.la
