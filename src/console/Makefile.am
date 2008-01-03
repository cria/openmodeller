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

if BUILD_TERRALIB
TERRALIB_LIB = $(TERRALIB_HOME)/terralibx/terralib/libterralib.so \
	/usr/lib/libmysqlclient.so \
	$(TERRALIB_HOME)/terralibx/PostgreSQL/libpq.a \
	$(TERRALIB_HOME)/terralibx/Oracle/libclntsh.so \
	$(TERRALIB_HOME)/terralibx/Oracle/libnnz10.so

TERRALIB_FLAGS = -DBUILD_TERRALIB
endif

EXTRA_DIST = \
	file_parser.hh \
	request_file.hh \
	graph/color.hh \
	graph/graphic.hh \
	graph/graphic_x11.hh \
	graph/list.cpp \
	graph/list.hh \
	graph/view_model.hh \
	getopts/getopts.c \
	getopts/getopts.h \
	getopts/LICENSE \
	getopts/README \
	getopts/main.c

bin_PROGRAMS = om_console $(OM_VIEWER) $(OM_NICHE) om_dump om_sampledump om_pseudo om_project om_create om_metadump om_testmodel
lib_LTLIBRARIES = libopenmodellerxml.la

dist_man_MANS = man/om_console.1 man/om_pseudo.1

libopenmodellerxml_la_SOURCES = consolexml.cpp consolexml.hh
libopenmodellerxml_la_LDFLAGS = -no-undefined
libopenmodellerxml_la_CPPFLAGS = -I$(OMSRCDIR) $(TERRALIB_FLAGS) -Ddllexp= -DOM_XML_DLL_EXPORT=
libopenmodellerxml_la_LIBADD = $(GDAL_LIB) $(EXPAT_LDFLAGS) $(EXPAT_LIBS) $(OMLIB)

OM_XML_LIB = $(OMSRCDIR)/console/libopenmodellerxml.la

om_console_SOURCES = om_console.cpp request_file.cpp file_parser.cpp
om_console_CPPFLAGS = -DCORE_DLL_IMPORT -I$(OMSRCDIR) $(TERRALIB_FLAGS) -Ddllexp=
om_console_LDADD = $(GDAL_LIB) $(OMLIB) $(EXPAT_LIBS) $(TERRALIB_LIB)

om_dump_SOURCES = om_dump.cpp request_file.cpp file_parser.cpp
om_dump_CPPFLAGS = -DCORE_DLL_IMPORT -I$(OMSRCDIR) $(TERRALIB_FLAGS) -Ddllexp=
om_dump_LDADD = $(GDAL_LIB) $(OMLIB) $(EXPAT_LIBS) $(TERRALIB_LIB)

om_sampledump_SOURCES = om_sampledump.cpp request_file.cpp file_parser.cpp
om_sampledump_CPPFLAGS = -DCORE_DLL_IMPORT -I$(OMSRCDIR) $(TERRALIB_FLAGS) -Ddllexp=
om_sampledump_LDADD = $(GDAL_LIB) $(OMLIB) $(EXPAT_LIBS) $(TERRALIB_LIB)

om_pseudo_SOURCES = om_pseudo.cpp getopts/getopts.c
om_pseudo_CPPFLAGS = -DCORE_DLL_IMPORT -I$(OMSRCDIR) -I getopts $(TERRALIB_FLAGS) -Ddllexp=
om_pseudo_LDADD = $(GDAL_LIB) $(OMLIB) $(EXPAT_LIBS) $(TERRALIB_LIB)

om_project_SOURCES = om_project.cpp  
om_project_CPPFLAGS = -DCORE_DLL_IMPORT -I$(OMSRCDIR) $(TERRALIB_FLAGS) -Ddllexp= -DOM_XML_DLL_EXPORT=
om_project_LDADD = $(GDAL_LIB) $(OMLIB) $(EXPAT_LIBS) $(OM_XML_LIB) $(TERRALIB_LIB)

om_create_SOURCES = om_create.cpp 
om_create_CPPFLAGS = -DCORE_DLL_IMPORT -I$(OMSRCDIR) $(TERRALIB_FLAGS) -Ddllexp= -DOM_XML_DLL_EXPORT=
om_create_LDADD = $(GDAL_LIB) $(OMLIB) $(EXPAT_LIBS) $(OM_XML_LIB) $(TERRALIB_LIB)

om_metadump_SOURCES = om_metadump.cpp 
om_metadump_CPPFLAGS = -DCORE_DLL_IMPORT -I$(OMSRCDIR) $(TERRALIB_FLAGS) -Ddllexp= -DOM_XML_DLL_EXPORT=
om_metadump_LDADD = $(GDAL_LIB) $(OMLIB) $(EXPAT_LIBS) $(OM_XML_LIB) $(TERRALIB_LIB)

om_testmodel_SOURCES = om_testmodel.cpp request_file.cpp file_parser.cpp
om_testmodel_CPPFLAGS = -DCORE_DLL_IMPORT -I$(OMSRCDIR) -Ddllexp=
om_testmodel_LDADD = $(GDAL_LIB) $(OMLIB) $(EXPAT_LIBS)

om_viewer_SOURCES = om_viewer.cpp file_parser.cpp graph/color.cpp graph/graphic.cpp graph/graphic_x11.cpp graph/view_model.cpp
om_viewer_CPPFLAGS = -I$(OMSRCDIR) -I graph $(TERRALIB_FLAGS) -Ddllexp=
om_viewer_LDADD = $(GDAL_LIB) $(X11_LIB) $(OMLIB) $(TERRALIB_LIB)

om_niche_SOURCES = om_niche.cpp request_file.cpp file_parser.cpp graph/color.cpp graph/graphic.cpp graph/graphic_x11.cpp
om_niche_CPPFLAGS = -I$(OMSRCDIR) -I graph $(TERRALIB_FLAGS) -Ddllexp=
om_niche_LDADD = $(GDAL_LIB) $(X11_LIB) $(OMLIB) $(TERRALIB_LIB)
