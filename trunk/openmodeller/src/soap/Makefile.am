# Copyright (C) 2004 Renato De Giovanni - CRIA <renato at cria.org.br>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

noinst_PROGRAMS = om_soap_server

EXTRA_DIST = \
	soapH.h \
	soapStub.h \
	stdsoap2.h \
	openModeller.nsmap \
	openModeller.createModel.req.xml \
	openModeller.createModel.res.xml \
	openModeller.getAlgorithms.req.xml \
	openModeller.getAlgorithms.res.xml \
	openModeller.getDistributionMap.req.xml \
	openModeller.getDistributionMap.res.xml \
	openModeller.nsmap \
	openModeller.ping.req.xml \
	openModeller.ping.res.xml \
	openModeller.wsdl \
	sampleClient.pl


SOAPHEADER = om_soap.hh

om_soap_server_SOURCES = om_soap_server.cpp $(SOAPHEADER) stdsoap2.cpp soapServer.cpp soapC.cpp
om_soap_server_CPPFLAGS = -I../inc -I../inc/serialization
om_soap_server_LDADD = $(GDAL_LIB) ../lib/libopenmodeller.la -lpthread
