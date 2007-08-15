# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

EXTRA_DIST = \
	AUTHORS \
	README.txt \
	INSTALL \
	NEWS \
	ChangeLog \
	COPYING.txt \
	doc/openModeller.doxygen \
	doc/openModeller.xsd \
	examples/furcata_boliviana.txt \
	examples/rain_coolest.tif \
	examples/temp_avg.tif \
	examples/request.txt \
	examples/request_te.txt \
	examples/model_request.xml \
	examples/projection_request.xml \
	libopenmodeller.spec \
	CMakeLists.txt \
	src/CMakeLists.txt \
	src/algorithms/CMakeLists.txt \
	src/algorithms/best_subsets/CMakeLists.txt \
	src/algorithms/dg_garp/CMakeLists.txt \
	src/algorithms/garp/CMakeLists.txt \
	src/algorithms/garp/garp_best_subsets/CMakeLists.txt \
	src/console/CMakeLists.txt \
	src/openmodeller/CMakeLists.txt \
	src/soap/CMakeLists.txt \
	test/CMakeLists.txt \
	test/CppUnitLite/CMakeLists.txt \
	test/library/CMakeLists.txt \
	cmake/FindExpat.cmake \
	cmake/FindGDAL.cmake \
	cmake/FindGSL.cmake \
	cmake/FindProj.cmake \
	cmake/FindSqlite3.cmake \
	cmake/Python.cmake \
	ctest/CMakeLists.txt \
	ctest/cxxtestgen.pl \
	ctest/om_hello_world.cpp \
	ctest/om_load_algs.cpp \
	ctest/om_test_areastats.cpp \
	ctest/om_test_areastats.h \
	ctest/om_test_configuration.cpp \
	ctest/om_test_configuration.h \
	ctest/om_test_exceptions.cpp \
	ctest/om_test_exceptions.h \
	ctest/om_test_icstring.cpp \
	ctest/om_test_icstring.h \
	ctest/om_test_log.cpp \
	ctest/om_test_mapformat.cpp \
	ctest/om_test_mapformat.h \
	ctest/om_test_random.cpp \
	ctest/om_test_random.h \
	ctest/om_test_refcount.cpp \
	ctest/om_test_refcount.h \
	ctest/om_test_sample.cpp \
	ctest/om_test_sampleexpr.cpp \
	ctest/om_test_sampleexpr.h \
	ctest/om_test_sample.h \
	ctest/om_test_scalenormalizer.cpp \
	ctest/om_test_scalenormalizer.h \
	ctest/om_test_template.h \
	ctest/README.txt \
	ctest/test_data/model_request.xml \
	ctest/test_data/projection_request.xml

#SUBDIRS = test src data
SUBDIRS = test src

release: distdir
	-chmod -R a+r $(distdir)
	GZIP=$(GZIP_ENV) $(AMTAR)$(TAR) chozf $(distdir).tar.gz $(distdir)
	rm -f $(distdir)/libopenmodeller.spec
	sed '/^Source:/s/\.tar\.gz$$/\.tar\.bz2/' libopenmodeller.spec \
			> $(distdir)/libopenmodeller.spec
	BZIP2=$(BZIP2_ENV) $(AMTAR)$(TAR) --bzip2 -chof $(distdir).tar.bz2 $(distdir)
	-rm -rf $(distdir)

