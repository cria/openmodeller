# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#TODO remove 'no-dependencies' and compile without terralib
AUTOMAKE_OPTIONS=subdir-objects no-dependencies

DPLUGIN_PATH=-DPLUGINPATH=\"$(libdir)/openmodeller/\"
DCONFIG_FILE=-DCONFIG_FILE=\"om_config\"

lib_LTLIBRARIES = libopenmodeller.la

if ISWINDOWS
SOURCEOS = os_specific_win.cpp
else
SOURCEOS = os_specific.cpp
endif

if BUILD_TERRALIB
TERRALIB_DATABASES = $(TERRALIB_HOME)/src/terralib/drivers/MySQL/TeMySQL.cpp \
	$(TERRALIB_HOME)/src/terralib/drivers/OracleSpatial/TeOCIConnect.cpp \
	$(TERRALIB_HOME)/src/terralib/drivers/OracleSpatial/TeOCICursor.cpp \
	$(TERRALIB_HOME)/src/terralib/drivers/OracleSpatial/TeOCIOracle.cpp \
	$(TERRALIB_HOME)/src/terralib/drivers/OracleSpatial/TeOCISDO.cpp \
	$(TERRALIB_HOME)/src/terralib/drivers/OracleSpatial/TeOracleSpatial.cpp \
	$(TERRALIB_HOME)/src/terralib/drivers/PostgreSQL/TePGInterface.cpp \
	$(TERRALIB_HOME)/src/terralib/drivers/PostgreSQL/TePGUtils.cpp \
	$(TERRALIB_HOME)/src/terralib/drivers/PostgreSQL/TePostGIS.cpp \
	$(TERRALIB_HOME)/src/terralib/drivers/PostgreSQL/TePostgreSQL.cpp

TERRALIB_CPP = TeUrlParser.cpp \
	TeDatabaseManager.cpp \
	env_io/TeOMRaster.cpp \
	occ_io/TeOccurrences.cpp

TERRALIB_HH = TeUrlParser.hh \
	TeDatabaseManager.hh \
	env_io/TeOMRaster.hh \
	occ_io/TeOccurrences.hh 

TERRALIB_FLAGS = -I$(TERRALIB_HOME)/src/terralib/kernel \
	-I$(TERRALIB_HOME)/src/terralib/application \
	-I$(TERRALIB_HOME)/src/terralib/drivers/MySQL \
	-I$(TERRALIB_HOME)/src/terralib/drivers/MySQL/include \
	-I$(TERRALIB_HOME)/src/terralib/drivers/PostgreSQL \
	-I$(TERRALIB_HOME)/src/terralib/drivers/PostgreSQL/includepg \
	-I$(TERRALIB_HOME)/src/terralib/drivers/OracleSpatial \
	-I$(TERRALIB_HOME)/src/terralib/drivers/OracleSpatial/OCI/include \
	-DBUILD_TERRALIB

TERRALIB_LIB = $(TERRALIB_HOME)/terralibx/terralib/libterralib.so \
	$(TERRALIB_HOME)/terralibx/tiff/libtiff.so \
	/usr/lib/libmysqlclient.so \
	$(TERRALIB_HOME)/terralibx/PostgreSQL/libpq.a \
	$(TERRALIB_HOME)/terralibx/Oracle/libclntsh.so \
	$(TERRALIB_HOME)/terralibx/Oracle/libnnz10.so
endif

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
	MeanVarianceNormalizer.cpp \
	Occurrence.cpp \
	Occurrences.cpp \
	OpenModeller.cpp \
	$(SOURCEOS) \
	Projector.cpp \
	Random.cpp \
	RocCurve.cpp \
	Sample.cpp \
	Sampler.cpp \
	ScaleNormalizer.cpp \
	env_io/GeoTransform.cpp \
	env_io/Header.cpp \
	env_io/Map.cpp \
	env_io/MapIterator.cpp \
	env_io/Raster.cpp \
	env_io/RasterGdal.cpp \
	env_io/RasterFactory.cpp \
	models/AlgoAdapterModel.cpp \
	models/AverageModel.cpp \
	models/ScaledModel.cpp \
	occ_io/OccurrencesFactory.cpp \
	occ_io/OccurrencesFile.cpp \
	occ_io/OccurrencesReader.cpp \
	$(TERRALIB_CPP) 

if BUILD_TERRALIB
nodist_libopenmodeller_la_SOURCES = $(TERRALIB_DATABASES)
endif

if ISAPPLE
APPLE_LDFLAGS=-F/System/Library/Frameworks/ -framework CoreFoundation -framework ApplicationServices -framework Carbon
endif

libopenmodeller_la_LDFLAGS = -no-undefined $(APPLE_LDFLAGS)
libopenmodeller_la_CPPFLAGS = -I@top_srcdir@/src $(DPLUGIN_PATH) $(DCONFIG_FILE) $(EXPAT_CFLAGS) $(TERRALIB_FLAGS) -Ddllexp=
libopenmodeller_la_LIBADD = $(GDAL_LIB) $(EXPAT_LDFLAGS) $(EXPAT_LIBS) $(TERRALIB_LIB) 

omincludedir = $(includedir)/openmodeller

ominclude_HEADERS = \
	AbortionCommand.hh \
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
	MeanVarianceNormalizer.hh \
	Model.hh \
	Normalizable.hh \
	Normalizer.hh \
	Occurrence.hh    \
	Occurrences.hh   \
	om.hh \
	om_defs.hh	    \
	OpenModeller.hh  \
	os_specific.hh   \
	Projector.hh \
	Random.hh\
	refcount.hh \
	RocCurve.hh \
	SampleExpr.hh \
	SampleExprVar.hh \
	Sample.hh \
	Sampler.hh \
	ScaleNormalizer.hh \
	$(TERRALIB_HH)

envioincludedir = $(omincludedir)/env_io

envioinclude_HEADERS = \
	env_io/GeoTransform.hh \
	env_io/Header.hh \
	env_io/Map.hh \
	env_io/MapIterator.hh \
	env_io/RasterFactory.hh \
	env_io/Raster.hh \
	env_io/RasterGdal.hh

modelsincludedir = $(omincludedir)/models

modelsinclude_HEADERS = \
	models/AlgoAdapterModel.hh \
	models/AverageModel.hh \
	models/ScaledModel.hh

occioincludedir = $(omincludedir)/occ_io

occioinclude_HEADERS = \
        occ_io/OccurrencesFactory.hh \
        occ_io/OccurrencesFile.hh \
        occ_io/OccurrencesReader.hh
