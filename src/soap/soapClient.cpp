/* soapClient.cpp
   Generated by gSOAP 2.7.6d from om_soap.hh
   Copyright (C) 2000-2005, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/
#include "soapH.h"

SOAP_SOURCE_STAMP("@(#) soapClient.cpp ver 2.7.6d 2008-09-16 19:53:58 GMT")


SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__ping(struct soap *soap, const char *soap_endpoint, const char *soap_action, void *_, int &status)
{	struct omws__ping soap_tmp_omws__ping;
	struct omws__pingResponse *soap_tmp_omws__pingResponse;
	if (!soap_endpoint)
		soap_endpoint = "http://modeller.cria.org.br:80/cgi-bin/om_soap_server.cgi";
	soap->encodingStyle = NULL;
	soap_tmp_omws__ping._ = _;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_omws__ping(soap, &soap_tmp_omws__ping);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__ping(soap, &soap_tmp_omws__ping, "omws:ping", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__ping(soap, &soap_tmp_omws__ping, "omws:ping", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	soap_default_xsd__int(soap, &status);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_tmp_omws__pingResponse = soap_get_omws__pingResponse(soap, NULL, "omws:pingResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	status = soap_tmp_omws__pingResponse->status;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getAlgorithms(struct soap *soap, const char *soap_endpoint, const char *soap_action, void *_, struct omws__getAlgorithmsResponse *out)
{	struct omws__getAlgorithms soap_tmp_omws__getAlgorithms;
	if (!soap_endpoint)
		soap_endpoint = "http://modeller.cria.org.br:80/cgi-bin/om_soap_server.cgi";
	soap->encodingStyle = NULL;
	soap_tmp_omws__getAlgorithms._ = _;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_omws__getAlgorithms(soap, &soap_tmp_omws__getAlgorithms);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__getAlgorithms(soap, &soap_tmp_omws__getAlgorithms, "omws:getAlgorithms", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__getAlgorithms(soap, &soap_tmp_omws__getAlgorithms, "omws:getAlgorithms", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	soap_default_omws__getAlgorithmsResponse(soap, out);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get_omws__getAlgorithmsResponse(soap, out, "omws:getAlgorithmsResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getLayers(struct soap *soap, const char *soap_endpoint, const char *soap_action, void *_, struct omws__getLayersResponse *out)
{	struct omws__getLayers soap_tmp_omws__getLayers;
	if (!soap_endpoint)
		soap_endpoint = "http://modeller.cria.org.br:80/cgi-bin/om_soap_server.cgi";
	soap->encodingStyle = NULL;
	soap_tmp_omws__getLayers._ = _;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_omws__getLayers(soap, &soap_tmp_omws__getLayers);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__getLayers(soap, &soap_tmp_omws__getLayers, "omws:getLayers", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__getLayers(soap, &soap_tmp_omws__getLayers, "omws:getLayers", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	soap_default_omws__getLayersResponse(soap, out);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get_omws__getLayersResponse(soap, out, "omws:getLayersResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__createModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, wchar_t *om__ModelParameters, char *&ticket)
{	struct omws__createModel soap_tmp_omws__createModel;
	struct omws__createModelResponse *soap_tmp_omws__createModelResponse;
	if (!soap_endpoint)
		soap_endpoint = "http://modeller.cria.org.br:80/cgi-bin/om_soap_server.cgi";
	soap->encodingStyle = NULL;
	soap_tmp_omws__createModel.om__ModelParameters = om__ModelParameters;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_omws__createModel(soap, &soap_tmp_omws__createModel);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__createModel(soap, &soap_tmp_omws__createModel, "omws:createModel", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__createModel(soap, &soap_tmp_omws__createModel, "omws:createModel", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	ticket = NULL;
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_tmp_omws__createModelResponse = soap_get_omws__createModelResponse(soap, NULL, "omws:createModelResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	ticket = soap_tmp_omws__createModelResponse->ticket;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, struct omws__getModelResponse *out)
{	struct omws__getModel soap_tmp_omws__getModel;
	if (!soap_endpoint)
		soap_endpoint = "http://modeller.cria.org.br:80/cgi-bin/om_soap_server.cgi";
	soap->encodingStyle = NULL;
	soap_tmp_omws__getModel.ticket = ticket;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_omws__getModel(soap, &soap_tmp_omws__getModel);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__getModel(soap, &soap_tmp_omws__getModel, "omws:getModel", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__getModel(soap, &soap_tmp_omws__getModel, "omws:getModel", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	soap_default_omws__getModelResponse(soap, out);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get_omws__getModelResponse(soap, out, "omws:getModelResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__testModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, wchar_t *om__TestParameters, char *&ticket)
{	struct omws__testModel soap_tmp_omws__testModel;
	struct omws__testModelResponse *soap_tmp_omws__testModelResponse;
	if (!soap_endpoint)
		soap_endpoint = "http://modeller.cria.org.br:80/cgi-bin/om_soap_server.cgi";
	soap->encodingStyle = NULL;
	soap_tmp_omws__testModel.om__TestParameters = om__TestParameters;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_omws__testModel(soap, &soap_tmp_omws__testModel);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__testModel(soap, &soap_tmp_omws__testModel, "omws:testModel", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__testModel(soap, &soap_tmp_omws__testModel, "omws:testModel", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	ticket = NULL;
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_tmp_omws__testModelResponse = soap_get_omws__testModelResponse(soap, NULL, "omws:testModelResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	ticket = soap_tmp_omws__testModelResponse->ticket;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getTestResult(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, struct omws__testResponse *out)
{	struct omws__getTestResult soap_tmp_omws__getTestResult;
	if (!soap_endpoint)
		soap_endpoint = "http://modeller.cria.org.br:80/cgi-bin/om_soap_server.cgi";
	soap->encodingStyle = NULL;
	soap_tmp_omws__getTestResult.ticket = ticket;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_omws__getTestResult(soap, &soap_tmp_omws__getTestResult);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__getTestResult(soap, &soap_tmp_omws__getTestResult, "omws:getTestResult", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__getTestResult(soap, &soap_tmp_omws__getTestResult, "omws:getTestResult", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	soap_default_omws__testResponse(soap, out);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get_omws__testResponse(soap, out, "omws:testResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__projectModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, wchar_t *om__ProjectionParameters, char *&ticket)
{	struct omws__projectModel soap_tmp_omws__projectModel;
	struct omws__projectModelResponse *soap_tmp_omws__projectModelResponse;
	if (!soap_endpoint)
		soap_endpoint = "http://modeller.cria.org.br:80/cgi-bin/om_soap_server.cgi";
	soap->encodingStyle = NULL;
	soap_tmp_omws__projectModel.om__ProjectionParameters = om__ProjectionParameters;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_omws__projectModel(soap, &soap_tmp_omws__projectModel);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__projectModel(soap, &soap_tmp_omws__projectModel, "omws:projectModel", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__projectModel(soap, &soap_tmp_omws__projectModel, "omws:projectModel", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	ticket = NULL;
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_tmp_omws__projectModelResponse = soap_get_omws__projectModelResponse(soap, NULL, "omws:projectModelResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	ticket = soap_tmp_omws__projectModelResponse->ticket;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getProgress(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, int &progress)
{	struct omws__getProgress soap_tmp_omws__getProgress;
	struct omws__getProgressResponse *soap_tmp_omws__getProgressResponse;
	if (!soap_endpoint)
		soap_endpoint = "http://modeller.cria.org.br:80/cgi-bin/om_soap_server.cgi";
	soap->encodingStyle = NULL;
	soap_tmp_omws__getProgress.ticket = ticket;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_omws__getProgress(soap, &soap_tmp_omws__getProgress);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__getProgress(soap, &soap_tmp_omws__getProgress, "omws:getProgress", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__getProgress(soap, &soap_tmp_omws__getProgress, "omws:getProgress", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	soap_default_xsd__int(soap, &progress);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_tmp_omws__getProgressResponse = soap_get_omws__getProgressResponse(soap, NULL, "omws:getProgressResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	progress = soap_tmp_omws__getProgressResponse->progress;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getLog(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, char *&log)
{	struct omws__getLog soap_tmp_omws__getLog;
	struct omws__getLogResponse *soap_tmp_omws__getLogResponse;
	if (!soap_endpoint)
		soap_endpoint = "http://modeller.cria.org.br:80/cgi-bin/om_soap_server.cgi";
	soap->encodingStyle = NULL;
	soap_tmp_omws__getLog.ticket = ticket;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_omws__getLog(soap, &soap_tmp_omws__getLog);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__getLog(soap, &soap_tmp_omws__getLog, "omws:getLog", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__getLog(soap, &soap_tmp_omws__getLog, "omws:getLog", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	log = NULL;
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_tmp_omws__getLogResponse = soap_get_omws__getLogResponse(soap, NULL, "omws:getLogResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	log = soap_tmp_omws__getLogResponse->log;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getLayerAsAttachment(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *id, xsd__base64Binary &file)
{	struct omws__getLayerAsAttachment soap_tmp_omws__getLayerAsAttachment;
	struct omws__getLayerAsAttachmentResponse *soap_tmp_omws__getLayerAsAttachmentResponse;
	if (!soap_endpoint)
		soap_endpoint = "http://modeller.cria.org.br:80/cgi-bin/om_soap_server.cgi";
	soap->encodingStyle = NULL;
	soap_tmp_omws__getLayerAsAttachment.id = id;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_omws__getLayerAsAttachment(soap, &soap_tmp_omws__getLayerAsAttachment);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__getLayerAsAttachment(soap, &soap_tmp_omws__getLayerAsAttachment, "omws:getLayerAsAttachment", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__getLayerAsAttachment(soap, &soap_tmp_omws__getLayerAsAttachment, "omws:getLayerAsAttachment", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	if (!&file)
		return soap_closesock(soap);
	file.soap_default(soap);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_tmp_omws__getLayerAsAttachmentResponse = soap_get_omws__getLayerAsAttachmentResponse(soap, NULL, "omws:getLayerAsAttachmentResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	file = soap_tmp_omws__getLayerAsAttachmentResponse->file;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getLayerAsUrl(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *id, char *&url)
{	struct omws__getLayerAsUrl soap_tmp_omws__getLayerAsUrl;
	struct omws__getLayerAsUrlResponse *soap_tmp_omws__getLayerAsUrlResponse;
	if (!soap_endpoint)
		soap_endpoint = "http://modeller.cria.org.br:80/cgi-bin/om_soap_server.cgi";
	soap->encodingStyle = NULL;
	soap_tmp_omws__getLayerAsUrl.id = id;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_omws__getLayerAsUrl(soap, &soap_tmp_omws__getLayerAsUrl);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__getLayerAsUrl(soap, &soap_tmp_omws__getLayerAsUrl, "omws:getLayerAsUrl", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__getLayerAsUrl(soap, &soap_tmp_omws__getLayerAsUrl, "omws:getLayerAsUrl", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	url = NULL;
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_tmp_omws__getLayerAsUrlResponse = soap_get_omws__getLayerAsUrlResponse(soap, NULL, "omws:getLayerAsUrlResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	url = soap_tmp_omws__getLayerAsUrlResponse->url;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getLayerAsWcs(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *id, char *&url)
{	struct omws__getLayerAsWcs soap_tmp_omws__getLayerAsWcs;
	struct omws__getLayerAsWcsResponse *soap_tmp_omws__getLayerAsWcsResponse;
	if (!soap_endpoint)
		soap_endpoint = "http://modeller.cria.org.br:80/cgi-bin/om_soap_server.cgi";
	soap->encodingStyle = NULL;
	soap_tmp_omws__getLayerAsWcs.id = id;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_omws__getLayerAsWcs(soap, &soap_tmp_omws__getLayerAsWcs);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__getLayerAsWcs(soap, &soap_tmp_omws__getLayerAsWcs, "omws:getLayerAsWcs", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__getLayerAsWcs(soap, &soap_tmp_omws__getLayerAsWcs, "omws:getLayerAsWcs", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	url = NULL;
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_tmp_omws__getLayerAsWcsResponse = soap_get_omws__getLayerAsWcsResponse(soap, NULL, "omws:getLayerAsWcsResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	url = soap_tmp_omws__getLayerAsWcsResponse->url;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getProjectionMetadata(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, struct omws__getProjectionMetadataResponse *out)
{	struct omws__getProjectionMetadata soap_tmp_omws__getProjectionMetadata;
	if (!soap_endpoint)
		soap_endpoint = "http://modeller.cria.org.br:80/cgi-bin/om_soap_server.cgi";
	soap->encodingStyle = NULL;
	soap_tmp_omws__getProjectionMetadata.ticket = ticket;
	soap_begin(soap);
	soap_serializeheader(soap);
	soap_serialize_omws__getProjectionMetadata(soap, &soap_tmp_omws__getProjectionMetadata);
	if (soap_begin_count(soap))
		return soap->error;
	if (soap->mode & SOAP_IO_LENGTH)
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_omws__getProjectionMetadata(soap, &soap_tmp_omws__getProjectionMetadata, "omws:getProjectionMetadata", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_omws__getProjectionMetadata(soap, &soap_tmp_omws__getProjectionMetadata, "omws:getProjectionMetadata", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	soap_default_omws__getProjectionMetadataResponse(soap, out);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	soap_get_omws__getProjectionMetadataResponse(soap, out, "omws:getProjectionMetadataResponse", "");
	if (soap->error)
	{	if (soap->error == SOAP_TAG_MISMATCH && soap->level == 2)
			return soap_recv_fault(soap);
		return soap_closesock(soap);
	}
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	return soap_closesock(soap);
}

/* End of soapClient.cpp */
