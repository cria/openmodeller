# Copyright (C) 2004 Jens Oberender <j.obi at troja.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

SUBDIRS = env_io

lib_LTLIBRARIES = libopenmodeller.la

libopenmodeller_la_SOURCES = \
	algorithm.cpp \
	control.cpp \
	environment.cpp \
	log.cpp \
	occurrences.cpp \
	occurrences_file.cpp \
	random.cpp \
	sampler.cpp \
	sampled_data.cpp \
	algorithm_factory.cpp \
	os_specific.cpp

libopenmodeller_la_CPPFLAGS = -I../inc -I../..
