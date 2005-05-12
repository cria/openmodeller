# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

#lib_LTLIBRARIES = libomjava.la

#libomjava_la_SOURCES = om_wrap.cxx ../../console/occurrences_file.cpp
#libomjava_la_LDFLAGS = -module
#libomjava_la_CPPFLAGS = -I../../inc -I ../../console
#libomjava_la_LIBADD = $(GDAL_LIB) ../../lib/libopenmodeller.la ../../lib/env_io/libenv_io.la

#om_wrap.cxx: om_java.i
#	-mkdir br
#	-mkdir br/org
#	-mkdir br/org/cria
#	-mkdir br/org/cria/OpenModeller
#	swig -c++ -java -package br.org.cria.OpenModeller \
#                        -o om_wrap.cxx om_java.i

EXTRA_DIST = om_java.i \
	OMTest1.java \
	OMTest2.java \
	OMTest3.java \
	Test.java

