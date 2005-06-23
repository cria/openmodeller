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
	Algorithm.cpp \
	AlgorithmFactory.cpp \
	AlgParameter.cpp \
	AreaStats.cpp \
	Configuration.cpp \
	Configuration_xmlio.cpp \
	ConfusionMatrix.cpp \
	Environment.cpp \
	ignorecase_traits.cpp \
	Log.cpp \
	MapFormat.cpp \
	Occurrence.cpp \
	Occurrences.cpp \
	OpenModeller.cpp \
	os_specific.cpp \
	Projector.cpp \
	Random.cpp \
	Sample.cpp \
	Sampler.cpp \
	env_io/GeoTransform.cpp \
	env_io/Header.cpp \
	env_io/Map.cpp \
	env_io/MapIterator.cpp \
	env_io/Raster.cpp \
	env_io/RasterGdal.cpp \
	models/AlgoAdapterModel.cpp \
	models/AverageModel.cpp \
	models/ScaledModel.cpp

libopenmodeller_la_LDFLAGS = -no-undefined
libopenmodeller_la_CPPFLAGS = -I@top_srcdir@/src $(DPLUGIN_PATH) $(DCONFIG_FILE) $(EXPAT_CFLAGS)
libopenmodeller_la_LIBADD = $(GDAL_LIB) $(EXPAT_LDFLAGS) $(EXPAT_LIBS)

omincludedir = $(includedir)/openmodeller

ominclude_HEADERS = \
	AlgMetadata.hh \
	AlgorithmFactory.hh  \
	Algorithm.hh	     \
	AlgParameter.hh      \
	AreaStats.hh	     \
	Configurable.hh      \
	Configuration.hh     \
	ConfusionMatrix.hh \
	Environment.hh	   \
	Exceptions.hh	   \
	ignorecase_traits.hh  \
	Log.hh		   \
	MapFormat.hh	   \
	Model.hh \
	Normalizable.hh \
	Occurrence.hh    \
	Occurrences.hh   \
	om.hh \
	om_defs.hh	    \
	OpenModeller.hh  \
	os_specific.hh   \
	Projector.hh \
	Random.hh\
	refcount.hh \
	SampleExpr.hh \
	SampleExprVar.hh \
	Sample.hh \
	Sampler.hh

envioincludedir = $(omincludedir)/env_io

envioinclude_HEADERS = \
	env_io/GeoTransform.hh \
	env_io/Header.hh \
	env_io/Map.hh \
	env_io/MapIterator.hh \
	env_io/Raster.hh \
	env_io/RasterGdal.hh

modelsincludedir = $(omincludedir)/models

modelsinclude_HEADERS = \
	models/AlgoAdapterModel.hh \
	models/AverageModel.hh \
	models/ScaledModel.hh
