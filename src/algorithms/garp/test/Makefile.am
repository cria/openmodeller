# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

TESTS = test_ruleset

check_PROGRAMS = test_ruleset

test_ruleset_SOURCES  = test_main.cpp test_ruleset.cpp \
			../ruleset.cpp ../../../src/lib/log.cpp
test_ruleset_LDADD    = ../../../test/CppUnitLite/libcppunitlite.la
test_ruleset_CPPFLAGS = -I../../../test -I.. -I../../../src/inc

