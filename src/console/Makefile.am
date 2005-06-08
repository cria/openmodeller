# Copyright (C) 2004 Mauro Muñoz - CRIA <mauro at cria.org.br>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AUTOMAKE_OPTIONS=subdir-objects

if HAVE_X11
GRAPH = graph
OM_VIEWER = om_viewer
OM_NICHE  = om_niche
endif

EXTRA_DIST = \
	occurrences_file.hh \
	request_file.hh \
	graph/color.hh \
	graph/graphic.hh \
	graph/graphic_x11.hh \
	graph/list.cpp \
	graph/list.hh \
	graph/view_model.hh


#bin_PROGRAMS = om_console om_test $(OM_VIEWER)
bin_PROGRAMS = om_console $(OM_VIEWER) $(OM_NICHE)

om_console_SOURCES = om_console.cpp occurrences_file.cpp request_file.cpp
om_console_CPPFLAGS = -I../inc 
om_console_LDADD = $(GDAL_LIB) ../lib/libopenmodeller.la $(EXPAT_LIBS)

#om_test_SOURCES = om_test.cpp occurrences_file.cpp
#om_test_CPPFLAGS = -I../inc
#om_test_LDADD = $(GDAL_LIB) ../lib/libopenmodeller.la

om_viewer_SOURCES = om_viewer.cpp occurrences_file.cpp graph/color.cpp graph/graphic.cpp graph/graphic_x11.cpp graph/view_model.cpp
om_viewer_CPPFLAGS = -I../inc -I graph
om_viewer_LDADD = $(GDAL_LIB) $(X11_LIB) ../lib/libopenmodeller.la

om_niche_SOURCES = om_niche.cpp occurrences_file.cpp request_file.cpp graph/color.cpp graph/graphic.cpp graph/graphic_x11.cpp
om_niche_CPPFLAGS = -I../inc -I graph
om_niche_LDADD = $(GDAL_LIB) $(X11_LIB) ../lib/libopenmodeller.la
