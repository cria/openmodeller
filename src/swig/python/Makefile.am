# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

pythonSitePackagesdir = $(prefix)/python2.2/site-packages
pythonSitePackages_LTLIBRARIES = _om.la
pythonSitePackages_SCRIPTS = om.py

_om_la_SOURCES = om_wrap.cxx ../../console/occurrences_file.cpp
_om_la_LDFLAGS = -module
_om_la_CPPFLAGS = -I../../inc -I ../../console -I/usr/include/python2.2
_om_la_LIBADD = $(GDAL_LIB) ../../lib/libopenmodeller.la ../../lib/env_io/libenv_io.la

om_wrap.cxx: ../om.i
	swig -c++ -python -o om_wrap.cxx ../om.i 

# TODO: remove hardcoded reference to Python directories
# But AM_CHECK_PYTHON in modeller/configure.ac doesn't work 
# When it works, those two lines below should replace the ones above
#pythonSitePackagesdir = $(PYTHON_SITE_PACKAGE)
#_om_la_CPPFLAGS = -I../../inc -I$(includedir)/python$(PYTHON_VERSION)
