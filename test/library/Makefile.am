#
# General unit tests for OpenModeller library
#

OMSRCDIR = @top_srcdir@/src
OMLIB = $(OMSRCDIR)/openmodeller/libopenmodeller.la

LIB_UNIT_TESTS =	test_configuration\
			test_icstring \
			test_sample\
			test_sampleexpr\
			test_sampleexprvar

TESTS = $(LIB_UNIT_TESTS)

check_PROGRAMS = $(LIB_UNIT_TESTS)

LDADD = ../CppUnitLite/libcppunitlite.la $(OMLIB)

AM_CPPFLAGS = -I.. -I$(OMSRCDIR)

test_configuration_SOURCES = test_configuration.cpp

test_icstring_SOURCES = test_icstring.cpp

test_sample_SOURCES = test_sample.cpp

test_sampleexpr_SOURCES = test_sampleexpr.cpp

test_sampleexprvar_SOURCES = test_sampleexprvar.cpp
