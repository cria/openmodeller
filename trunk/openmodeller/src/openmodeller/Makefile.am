# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AUTOMAKE_OPTIONS=subdir-objects

DPLUGIN_PATH=-DPLUGINPATH=\"$(libdir)/openmodeller/\"
DCONFIG_FILE=-DCONFIG_FILE=\"om_config\"

lib_LTLIBRARIES = libopenmodeller.la

libopenmodeller_la_SOURCES = \
	algorithm.cpp \
	algorithm_factory.cpp \
	alg_parameter.cpp \
	configuration.cpp \
	configuration_xmlio.cpp \
	control.cpp \
	environment.cpp \
	file_parser.cpp \
	icstring.cpp \
	log.cpp \
	map_format.cpp \
	Model.cpp \
	occurrences.cpp \
	occurrence.cpp \
	os_specific.cpp \
	random.cpp \
	sampler.cpp \
	Sample.cpp \
	conf_matrix.cpp \
	area_stats.cpp \
	projector.cpp \
	env_io/geo_transform.cpp \
	env_io/header.cpp \
	env_io/map.cpp \
	env_io/map_iter.cpp \
	env_io/raster.cpp \
	env_io/raster_gdal.cpp \
	models/AlgoAdapterModel.cpp \
	models/AverageModel.cpp \
	models/ScaledModel.cpp

libopenmodeller_la_LDFLAGS = -no-undefined
libopenmodeller_la_CPPFLAGS = -I../inc -I../.. $(DPLUGIN_PATH) $(DCONFIG_FILE) $(EXPAT_CFLAGS)
libopenmodeller_la_LIBADD = $(GDAL_LIB) $(EXPAT_LDFLAGS) $(EXPAT_LIBS)
