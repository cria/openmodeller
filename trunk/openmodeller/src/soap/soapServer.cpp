/* soapServer.cpp
   Generated by gSOAP 2.7.6d from om_soap.hh
   Copyright (C) 2000-2005, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/
#include "soapH.h"

SOAP_SOURCE_STAMP("@(#) soapServer.cpp ver 2.7.6d 2006-10-02 19:29:19 GMT")


SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap *soap)
{
#ifndef WITH_FASTCGI
	unsigned int k = soap->max_keep_alive;
#endif

	do
	{
#ifdef WITH_FASTCGI
		if (FCGI_Accept() < 0)
		{
			soap->error = SOAP_EOF;
			return soap_send_fault(soap);
		}
#endif

		soap_begin(soap);

#ifndef WITH_FASTCGI
		if (!--k)
			soap->keep_alive = 0;
#endif

		if (soap_begin_recv(soap))
		{	if (soap->error < SOAP_STOP)
			{
#ifdef WITH_FASTCGI
				soap_send_fault(soap);
#else 
				return soap_send_fault(soap);
#endif
			}
			soap_closesock(soap);

			continue;
		}

		if (soap_envelope_begin_in(soap)
		 || soap_recv_header(soap)
		 || soap_body_begin_in(soap)
		 || soap_serve_request(soap)
		 || (soap->fserveloop && soap->fserveloop(soap)))
		{
#ifdef WITH_FASTCGI
			soap_send_fault(soap);
#else
			return soap_send_fault(soap);
#endif
		}

#ifdef WITH_FASTCGI
	} while (1);
#else
	} while (soap->keep_alive);
#endif
	return SOAP_OK;
}

#ifndef WITH_NOSERVEREQUEST
SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap *soap)
{
	soap_peek_element(soap);
	if (!soap_match_tag(soap, soap->tag, "omws:ping"))
		return soap_serve_omws__ping(soap);
	if (!soap_match_tag(soap, soap->tag, "omws:getAlgorithms"))
		return soap_serve_omws__getAlgorithms(soap);
	if (!soap_match_tag(soap, soap->tag, "omws:getLayers"))
		return soap_serve_omws__getLayers(soap);
	if (!soap_match_tag(soap, soap->tag, "omws:createModel"))
		return soap_serve_omws__createModel(soap);
	if (!soap_match_tag(soap, soap->tag, "omws:getDistributionMap"))
		return soap_serve_omws__getDistributionMap(soap);
	return soap->error = SOAP_NO_METHOD;
}
#endif

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__ping(struct soap *soap)
{	struct omws__ping soap_tmp_omws__ping;
	struct omws__pingResponse soap_tmp_omws__pingResponse;
	int soap_tmp_xsd__int;
	soap_default_omws__pingResponse(soap, &soap_tmp_omws__pingResponse);
	soap_default_xsd__int(soap, &soap_tmp_xsd__int);
	soap_tmp_omws__pingResponse.status = &soap_tmp_xsd__int;
	soap_default_omws__ping(soap, &soap_tmp_omws__ping);
	soap->encodingStyle = NULL;
	if (!soap_get_omws__ping(soap, &soap_tmp_omws__ping, "omws:ping", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = omws__ping(soap, soap_tmp_omws__ping._, &soap_tmp_xsd__int);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	soap_serialize_omws__pingResponse(soap, &soap_tmp_omws__pingResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__pingResponse(soap, &soap_tmp_omws__pingResponse, "omws:pingResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__pingResponse(soap, &soap_tmp_omws__pingResponse, "omws:pingResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getAlgorithms(struct soap *soap)
{	struct omws__getAlgorithms soap_tmp_omws__getAlgorithms;
	wchar_t * soap_tmp_XML;
	soap_tmp_XML = NULL;
	soap_default_omws__getAlgorithms(soap, &soap_tmp_omws__getAlgorithms);
	soap->encodingStyle = NULL;
	if (!soap_get_omws__getAlgorithms(soap, &soap_tmp_omws__getAlgorithms, "omws:getAlgorithms", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = omws__getAlgorithms(soap, soap_tmp_omws__getAlgorithms._, soap_tmp_XML);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_outwliteral(soap, "om:AvailableAlgorithms", &soap_tmp_XML)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_outwliteral(soap, "om:AvailableAlgorithms", &soap_tmp_XML)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getLayers(struct soap *soap)
{	struct omws__getLayers soap_tmp_omws__getLayers;
	wchar_t * soap_tmp_XML;
	soap_tmp_XML = NULL;
	soap_default_omws__getLayers(soap, &soap_tmp_omws__getLayers);
	soap->encodingStyle = NULL;
	if (!soap_get_omws__getLayers(soap, &soap_tmp_omws__getLayers, "omws:getLayers", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = omws__getLayers(soap, soap_tmp_omws__getLayers._, soap_tmp_XML);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_outwliteral(soap, "om:AvailableLayers", &soap_tmp_XML)
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_outwliteral(soap, "om:AvailableLayers", &soap_tmp_XML)
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__createModel(struct soap *soap)
{	struct omws__createModel soap_tmp_omws__createModel;
	struct omws__createModelResponse soap_tmp_omws__createModelResponse;
	char * soap_tmp_xsd__string;
	soap_default_omws__createModelResponse(soap, &soap_tmp_omws__createModelResponse);
	soap_tmp_xsd__string = NULL;
	soap_tmp_omws__createModelResponse.ticket = &soap_tmp_xsd__string;
	soap_default_omws__createModel(soap, &soap_tmp_omws__createModel);
	soap->encodingStyle = NULL;
	if (!soap_get_omws__createModel(soap, &soap_tmp_omws__createModel, "omws:createModel", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = omws__createModel(soap, soap_tmp_omws__createModel.points, soap_tmp_omws__createModel.maps, soap_tmp_omws__createModel.mask, soap_tmp_omws__createModel.algorithm, soap_tmp_omws__createModel.output, &soap_tmp_xsd__string);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	soap_serialize_omws__createModelResponse(soap, &soap_tmp_omws__createModelResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__createModelResponse(soap, &soap_tmp_omws__createModelResponse, "omws:createModelResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__createModelResponse(soap, &soap_tmp_omws__createModelResponse, "omws:createModelResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getDistributionMap(struct soap *soap)
{	struct omws__getDistributionMap soap_tmp_omws__getDistributionMap;
	struct omws__getDistributionMapResponse soap_tmp_omws__getDistributionMapResponse;
	soap_default_omws__getDistributionMapResponse(soap, &soap_tmp_omws__getDistributionMapResponse);
	soap_default_omws__getDistributionMap(soap, &soap_tmp_omws__getDistributionMap);
	soap->encodingStyle = NULL;
	if (!soap_get_omws__getDistributionMap(soap, &soap_tmp_omws__getDistributionMap, "omws:getDistributionMap", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = omws__getDistributionMap(soap, soap_tmp_omws__getDistributionMap.ticket, soap_tmp_omws__getDistributionMapResponse.file);
	if (soap->error)
		return soap->error;
	soap_serializeheader(soap);
	soap_serialize_omws__getDistributionMapResponse(soap, &soap_tmp_omws__getDistributionMapResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__getDistributionMapResponse(soap, &soap_tmp_omws__getDistributionMapResponse, "omws:getDistributionMapResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__getDistributionMapResponse(soap, &soap_tmp_omws__getDistributionMapResponse, "omws:getDistributionMapResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

/* End of soapServer.cpp */
