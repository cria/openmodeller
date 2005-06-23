# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

TESTLIBDIR = @top_srcdir@/test/CppUnitLite

OMSRCDIR = @top_srcdir@/src
OMLIB = $(OMSRCDIR)/openmodeller/libopenmodeller.la

GARP_UNIT_TESTS = 	test_ruleset \
			test_rules_base \
			test_rules_range \
			test_rules_negrange \
			test_rules_logit \
			test_rules_evaluate

TESTS = $(GARP_UNIT_TESTS)

check_PROGRAMS = $(GARP_UNIT_TESTS)

# ============ test_ruleset ==================================

test_ruleset_SOURCES = test_ruleset.cpp \
			../ruleset.cpp

test_ruleset_LDADD = $(TESTLIBDIR)/libcppunitlite.la $(OMLIB)

test_ruleset_CPPFLAGS = -I.. -I$(OMSRCDIR) -I$(TESTLIBDIR)


# ============ test_rules_base ===============================

test_rules_base_SOURCES  = test_rules_base.cpp \
			../rules_base.cpp

test_rules_base_LDADD = $(TESTLIBDIR)/libcppunitlite.la $(OMLIB) -ldl

test_rules_base_CPPFLAGS = -I.. -I$(OMSRCDIR) -I$(TESTLIBDIR)


# ============ test_rules_range ==============================

test_rules_range_SOURCES  = test_rules_range.cpp \
			../bioclim_histogram.cpp \
			../rules_base.cpp \
			../rules_range.cpp

test_rules_range_LDADD = $(TESTLIBDIR)/libcppunitlite.la $(OMLIB) -ldl

test_rules_range_CPPFLAGS = -I.. -I$(OMSRCDIR) -I$(TESTLIBDIR)



# ============ test_rules_negrange ===========================

test_rules_negrange_SOURCES  = test_rules_negrange.cpp \
				../bioclim_histogram.cpp \
				../rules_base.cpp \
				../rules_range.cpp \
				../rules_negrange.cpp

test_rules_negrange_LDADD = $(TESTLIBDIR)/libcppunitlite.la $(OMLIB) -ldl

test_rules_negrange_CPPFLAGS = -I.. -I$(OMSRCDIR) -I$(TESTLIBDIR)



# ============ test_rules_logit ===============================

test_rules_logit_SOURCES  = test_rules_logit.cpp \
			../regression.cpp \
			../rules_base.cpp \
			../rules_logit.cpp

test_rules_logit_LDADD = $(TESTLIBDIR)/libcppunitlite.la $(OMLIB) -ldl

test_rules_logit_CPPFLAGS = -I.. -I$(OMSRCDIR) -I$(TESTLIBDIR)


# ============ test_rules_evaluate ===========================

test_rules_evaluate_SOURCES  = test_rules_evaluate.cpp \
				../rules_base.cpp \
				../rules_logit.cpp \
				../rules_range.cpp \
				../rules_negrange.cpp \
				../bioclim_histogram.cpp \
				../regression.cpp

test_rules_evaluate_LDADD = $(TESTLIBDIR)/libcppunitlite.la $(OMLIB) -ldl -lgdal

test_rules_evaluate_CPPFLAGS = -I.. -I$(OMSRCDIR) -I$(TESTLIBDIR)


# ============================================================
