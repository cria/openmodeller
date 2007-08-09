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
	file_parser.hh \
	omws.xsd \
	openModeller.nsmap \
	openModeller.wsdl \
	openModeller.ping.req.xml \
	openModeller.ping.res.xml \
	openModeller.getAlgorithms.req.xml \
	openModeller.getAlgorithms.res.xml \
	openModeller.getLayers.req.xml \
	openModeller.getLayers.res.xml \
	openModeller.getProgress.req.xml \
	openModeller.getProgress.res.xml \
	openModeller.getLog.req.xml \
	openModeller.getLog.res.xml \
	openModeller.createModel.req.xml \
	openModeller.createModel.res.xml \
	openModeller.getModel.req.xml \
	openModeller.getModel.res.xml \
	openModeller.projectModel.req.xml \
	openModeller.projectModel.res.xml \
	openModeller.getProjectionMetadata.req.xml \
	openModeller.getProjectionMetadata.res.xml \
	openModeller.getLayerAsAttachment.req.xml \
	openModeller.getLayerAsAttachment.res.xml \
	openModeller.getLayerAsUrl.req.xml \
	openModeller.getLayerAsUrl.res.xml \
	sampleClient.pl \
	scheduler.sh


SOAPHEADER = om_soap.hh

om_soap_server_SOURCES = \
	om_soap_server.cpp \
	$(SOAPHEADER) \
	stdsoap2.cpp \
	soapServer.cpp \
	soapC.cpp \
	file_parser.cpp

om_soap_server_CPPFLAGS = -I$(OMSRCDIR) -Ddllexp=

om_soap_server_LDADD = $(GDAL_LIB) $(OMLIB) -lpthread
