# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

GARP_UNIT_TESTS = 	test_ruleset \
			test_rules_base \
			test_rules_range \
			test_rules_negrange \
			test_rules_logit \
			test_rules_evaluate

TESTS = $(GARP_UNIT_TESTS)

check_PROGRAMS = $(GARP_UNIT_TESTS)

# ============ test_ruleset ==================================

test_ruleset_SOURCES = test_main.cpp test_ruleset.cpp \
			../ruleset.cpp \
			../../../src/lib/log.cpp \
			../../../src/lib/Sample.cpp

test_ruleset_LDADD = ../../../test/CppUnitLite/libcppunitlite.la

test_ruleset_CPPFLAGS = -I../../../test -I.. \
			-I../../../src/inc


# ============ test_rules_base ===============================

test_rules_base_SOURCES  = test_main.cpp test_rules_base.cpp \
			../rules_base.cpp \
			../../../src/lib/Sample.cpp \
			../../../src/lib/random.cpp \
			../../../src/lib/log.cpp \
			../../../src/lib/os_specific.cpp

test_rules_base_LDADD = ../../../test/CppUnitLite/libcppunitlite.la -ldl

test_rules_base_CPPFLAGS = -I../../../test -I.. \
			-I../../../src/inc


# ============ test_rules_range ==============================

test_rules_range_SOURCES  = test_main.cpp test_rules_range.cpp \
			../bioclim_histogram.cpp \
			../rules_base.cpp \
			../rules_range.cpp \
			../../../src/lib/Sample.cpp \
			../../../src/lib/random.cpp \
			../../../src/lib/log.cpp \
			../../../src/lib/os_specific.cpp

test_rules_range_LDADD = ../../../test/CppUnitLite/libcppunitlite.la -ldl

test_rules_range_CPPFLAGS = -I../../../test -I.. \
				-I../../../src/inc



# ============ test_rules_negrange ===========================

test_rules_negrange_SOURCES  = test_main.cpp test_rules_negrange.cpp \
				../bioclim_histogram.cpp \
				../rules_base.cpp \
				../rules_range.cpp \
				../rules_negrange.cpp \
				../../../src/lib/Sample.cpp \
				../../../src/lib/random.cpp \
				../../../src/lib/log.cpp \
				../../../src/lib/os_specific.cpp

test_rules_negrange_LDADD = ../../../test/CppUnitLite/libcppunitlite.la -ldl

test_rules_negrange_CPPFLAGS = -I../../../test -I.. \
				-I../../../src/inc



# ============ test_rules_logit ===============================

test_rules_logit_SOURCES  = test_main.cpp test_rules_logit.cpp \
			../regression.cpp \
			../rules_base.cpp \
			../rules_logit.cpp \
			../../../src/lib/Sample.cpp \
			../../../src/lib/random.cpp \
			../../../src/lib/log.cpp \
			../../../src/lib/os_specific.cpp

test_rules_logit_LDADD = ../../../test/CppUnitLite/libcppunitlite.la -ldl

test_rules_logit_CPPFLAGS = -I../../../test -I.. \
			-I../../../src/inc


# ============ test_rules_evaluate ===========================

test_rules_evaluate_SOURCES  = test_main.cpp test_rules_evaluate.cpp \
				../rules_base.cpp \
				../rules_logit.cpp \
				../rules_range.cpp \
				../rules_negrange.cpp \
				../bioclim_histogram.cpp \
				../regression.cpp \
				../../../src/lib/occurrence.cpp \
				../../../src/lib/occurrences.cpp \
				../../../src/lib/Sample.cpp \
				../../../src/lib/random.cpp \
				../../../src/lib/log.cpp \
				../../../src/lib/os_specific.cpp \
				../../../src/lib/configuration.cpp \
				../../../src/lib/env_io/geo_transform.cpp

test_rules_evaluate_LDADD = ../../../test/CppUnitLite/libcppunitlite.la -ldl -lgdal

test_rules_evaluate_CPPFLAGS = -I../../../test -I.. \
			-I../../../src/inc


# ============================================================
