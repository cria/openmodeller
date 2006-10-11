# Copyright (C) 2004 Renato De Giovanni - CRIA <renato at cria.org.br>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

OMSRCDIR = @top_srcdir@/src
OMLIB = $(OMSRCDIR)/openmodeller/libopenmodeller.la

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
	openModeller.projectModel.req.xml \
	openModeller.projectModel.res.xml \
	openModeller.getProgress.req.xml \
	openModeller.getProgress.res.xml \
	openModeller.getLog.req.xml \
	openModeller.getLog.res.xml \
	openModeller.getMapAsAttachment.req.xml \
	openModeller.getMapAsAttachment.res.xml \
	openModeller.getMapAsUrl.req.xml \
	openModeller.getMapAsUrl.res.xml \
	openModeller.ping.req.xml \
	openModeller.ping.res.xml \
	openModeller.nsmap \
	openModeller.wsdl \
	sampleClient.pl


SOAPHEADER = om_soap.hh

om_soap_server_SOURCES = om_soap_server.cpp $(SOAPHEADER) stdsoap2.cpp soapServer.cpp soapC.cpp
om_soap_server_CPPFLAGS = -I$(OMSRCDIR)
om_soap_server_LDADD = $(GDAL_LIB) $(OMLIB) -lpthread
