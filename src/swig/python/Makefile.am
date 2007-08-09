# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

OMSRCDIR = @top_srcdir@/src
OMLIB = $(OMSRCDIR)/openmodeller/libopenmodeller.la

pythonSitePackagesdir = $(pythondir)
pythonSitePackages_LTLIBRARIES = _om.la
pythonSitePackages_SCRIPTS = om.py

_om_la_SOURCES = om_wrap.cxx
_om_la_LDFLAGS = -module -no-undefined
_om_la_CPPFLAGS = -I$(OMSRCDIR) $(PYTHON_INCLUDES) -Ddllexp=
_om_la_LIBADD = $(GDAL_LIB) $(OMLIB)

EXTRA_DIST = om_python.i om.py

PY_WRAPPER_DEPS = om_python.i \
		$(OMSRCDIR)/openmodeller/AlgorithmFactory.hh \
		$(OMSRCDIR)/openmodeller/Configuration.hh \
		$(OMSRCDIR)/openmodeller/env_io/Map.hh \
		$(OMSRCDIR)/openmodeller/Environment.hh \
		$(OMSRCDIR)/openmodeller/MapFormat.hh \
		$(OMSRCDIR)/openmodeller/Model.hh \
		$(OMSRCDIR)/openmodeller/models/AverageModel.hh \
		$(OMSRCDIR)/openmodeller/AlgParameter.hh \
		$(OMSRCDIR)/openmodeller/AlgMetadata.hh \
		$(OMSRCDIR)/openmodeller/AreaStats.hh \
		$(OMSRCDIR)/openmodeller/ConfusionMatrix.hh \
		$(OMSRCDIR)/openmodeller/OpenModeller.hh \
		$(OMSRCDIR)/openmodeller/om_defs.hh \
		$(OMSRCDIR)/openmodeller/Log.hh \
		$(OMSRCDIR)/openmodeller/Occurrences.hh \
		$(OMSRCDIR)/openmodeller/Projector.hh \
		$(OMSRCDIR)/openmodeller/Algorithm.hh \
		$(OMSRCDIR)/openmodeller/Sampler.hh \
		$(OMSRCDIR)/openmodeller/refcount.hh

om_wrap.cxx: $(PY_WRAPPER_DEPS)
	swig -c++ -I$(OMSRCDIR) -python -o om_wrap.cxx om_python.i 

om.py: $(PY_WRAPPER_DEPS)
	swig -c++ -I$(OMSRCDIR) -python -o om_wrap.cxx om_python.i 

