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

OMSRCDIR = @top_srcdir@/src
OMLIB = $(OMSRCDIR)/openmodeller/libopenmodeller.la

if HAVE_X11
GRAPH = graph
OM_VIEWER = om_viewer
OM_NICHE  = om_niche
endif

EXTRA_DIST = \
	file_parser.hh \
	occurrences_file.hh \
	request_file.hh \
	graph/color.hh \
	graph/graphic.hh \
	graph/graphic_x11.hh \
	graph/list.cpp \
	graph/list.hh \
	graph/view_model.hh


bin_PROGRAMS = om_console $(OM_VIEWER) $(OM_NICHE) om_dump om_sampledump om_project om_create
lib_LTLIBRARIES = libopenmodellerxml.la

libopenmodellerxml_la_SOURCES = consolexml.cpp
# Dont for no-undefined it fails on mac
# Tim to investigate and resolve issues with unresolved symbols
#libopenmodellerxml_la_LDFLAGS = -no-undefined
libopenmodellerxml_la_CPPFLAGS = -I$(OMSRCDIR)
libopenmodellerxml_la_LIBADD = $(GDAL_LIB) $(EXPAT_LDFLAGS) $(EXPAT_LIBS) $(OMLIB)

			     
om_console_SOURCES = om_console.cpp occurrences_file.cpp request_file.cpp file_parser.cpp
om_console_CPPFLAGS = -DCORE_DLL_IMPORT -I$(OMSRCDIR)
om_console_LDADD = $(GDAL_LIB) $(OMLIB) $(EXPAT_LIBS)

om_dump_SOURCES = om_dump.cpp occurrences_file.cpp request_file.cpp file_parser.cpp
om_dump_CPPFLAGS = -DCORE_DLL_IMPORT -I$(OMSRCDIR)
om_dump_LDADD = $(GDAL_LIB) $(OMLIB) $(EXPAT_LIBS)

om_sampledump_SOURCES = om_sampledump.cpp occurrences_file.cpp request_file.cpp file_parser.cpp
om_sampledump_CPPFLAGS = -DCORE_DLL_IMPORT -I$(OMSRCDIR)
om_sampledump_LDADD = $(GDAL_LIB) $(OMLIB) $(EXPAT_LIBS)

om_project_SOURCES = om_project.cpp  
om_project_CPPFLAGS = -DCORE_DLL_IMPORT -I$(OMSRCDIR)
om_project_LDADD = $(GDAL_LIB) $(OMLIB) $(EXPAT_LIBS) -lopenmodellerxml

om_create_SOURCES = om_create.cpp 
om_create_CPPFLAGS = -DCORE_DLL_IMPORT -I$(OMSRCDIR)
om_create_LDADD = $(GDAL_LIB) $(OMLIB) $(EXPAT_LIBS) -lopenmodellerxml

#om_test_SOURCES = om_test.cpp occurrences_file.cpp
#om_test_CPPFLAGS = -I../inc
#om_test_LDADD = $(GDAL_LIB) ../lib/libopenmodeller.la

om_viewer_SOURCES = om_viewer.cpp occurrences_file.cpp file_parser.cpp graph/color.cpp graph/graphic.cpp graph/graphic_x11.cpp graph/view_model.cpp
om_viewer_CPPFLAGS = -I$(OMSRCDIR) -I graph
om_viewer_LDADD = $(GDAL_LIB) $(X11_LIB) $(OMLIB)

om_niche_SOURCES = om_niche.cpp occurrences_file.cpp request_file.cpp file_parser.cpp graph/color.cpp graph/graphic.cpp graph/graphic_x11.cpp
om_niche_CPPFLAGS = -I$(OMSRCDIR) -I graph
om_niche_LDADD = $(GDAL_LIB) $(X11_LIB) $(OMLIB)
