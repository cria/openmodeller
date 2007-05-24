# Copyright (C) 2004 Ricardo Scachetti Pereira <rpereira at ku.edu>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

noinst_LTLIBRARIES = libcppunitlite.la

OMDIR = @top_srcdir@/src

EXTRA_DIST = \
	Test.h \
	TestRegistry.h \
	TestResult.h \
	Failure.h \
	SimpleString.h \
	TestHarness.h


libcppunitlite_la_SOURCES = \
	Test.cpp TestRegistry.cpp TestResult.cpp \
	Failure.cpp SimpleString.cpp

libcppunitlite_la_CPPFLAGS = -I$(OMDIR)
