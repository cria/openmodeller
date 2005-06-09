# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

pythonSitePackagesdir = $(pythondir)
pythonSitePackages_LTLIBRARIES = _om.la
pythonSitePackages_SCRIPTS = om.py

_om_la_SOURCES = om_wrap.cxx
_om_la_LDFLAGS = -module -no-undefined
_om_la_CPPFLAGS = -I../../inc -I../../console $(PYTHON_INCLUDES)
_om_la_LIBADD = $(GDAL_LIB) ../../lib/libopenmodeller.la 

EXTRA_DIST = om_python.i om.py

PY_WRAPPER_DEPS = om_python.i \
		../../inc/algorithm_factory.hh \
		../../inc/configuration.hh \
		../../inc/env_io/map.hh \
		../../inc/environment.hh \
		../../inc/map_format.hh \
		../../inc/Model.hh \
		../../inc/models/AverageModel.hh \
		../../inc/om_alg_parameter.hh \
		../../inc/om_algorithm_metadata.hh \
		../../inc/om_area_stats.hh \
		../../inc/om_conf_matrix.hh \
		../../inc/om_control.hh \
		../../inc/om_defs.hh \
		../../inc/om_log.hh \
		../../inc/om_occurrences.hh \
		../../inc/om_projector.hh \
		../../inc/om_algorithm.hh \
		../../inc/om_sampler.hh \
		../../inc/refcount.hh

om_wrap.cxx: $(PY_WRAPPER_DEPS)
	swig -c++ -I../../inc -python -o om_wrap.cxx om_python.i 

om.py: $(PY_WRAPPER_DEPS)
	swig -c++ -I../../inc -python -o om_wrap.cxx om_python.i 

