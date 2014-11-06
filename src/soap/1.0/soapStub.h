/* soapStub.h
   Generated by gSOAP 2.8.15 from om_soap.hh

Copyright(C) 2000-2013, Robert van Engelen, Genivia Inc. All Rights Reserved.
The generated code is released under ONE of the following licenses:
GPL or Genivia's license for commercial use.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
*/

#ifndef soapStub_H
#define soapStub_H
#include "stdsoap2.h"
#if GSOAP_VERSION != 20815
# error "GSOAP VERSION MISMATCH IN GENERATED CODE: PLEASE REINSTALL PACKAGE"
#endif


/******************************************************************************\
 *                                                                            *
 * Enumerations                                                               *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Types with Custom Serializers                                              *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Classes and Structs                                                        *
 *                                                                            *
\******************************************************************************/


#if 0 /* volatile type: do not declare here, declared elsewhere */

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (14)
/* SOAP Header: */
struct SOAP_ENV__Header
{
public:
	char *omws__version;	/* optional element of type xsd:string */
public:
	int soap_type() const { return 14; } /* = unique id SOAP_TYPE_SOAP_ENV__Header */
};
#endif

#endif

#ifndef SOAP_TYPE_omws__pingResponse
#define SOAP_TYPE_omws__pingResponse (19)
/* omws:pingResponse */
struct omws__pingResponse
{
public:
	int status;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:int */
public:
	int soap_type() const { return 19; } /* = unique id SOAP_TYPE_omws__pingResponse */
};
#endif

#ifndef SOAP_TYPE_omws__ping
#define SOAP_TYPE_omws__ping (20)
/* omws:ping */
struct omws__ping
{
public:
	void *_;	/* transient */
public:
	int soap_type() const { return 20; } /* = unique id SOAP_TYPE_omws__ping */
};
#endif

#ifndef SOAP_TYPE_omws__getAlgorithmsResponse
#define SOAP_TYPE_omws__getAlgorithmsResponse (21)
/* omws:getAlgorithmsResponse */
struct omws__getAlgorithmsResponse
{
public:
	wchar_t *om__AvailableAlgorithms;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 21; } /* = unique id SOAP_TYPE_omws__getAlgorithmsResponse */
};
#endif

#ifndef SOAP_TYPE_omws__getAlgorithms
#define SOAP_TYPE_omws__getAlgorithms (24)
/* omws:getAlgorithms */
struct omws__getAlgorithms
{
public:
	void *_;	/* transient */
public:
	int soap_type() const { return 24; } /* = unique id SOAP_TYPE_omws__getAlgorithms */
};
#endif

#ifndef SOAP_TYPE_omws__getLayersResponse
#define SOAP_TYPE_omws__getLayersResponse (25)
/* omws:getLayersResponse */
struct omws__getLayersResponse
{
public:
	wchar_t *om__AvailableLayers;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 25; } /* = unique id SOAP_TYPE_omws__getLayersResponse */
};
#endif

#ifndef SOAP_TYPE_omws__getLayers
#define SOAP_TYPE_omws__getLayers (28)
/* omws:getLayers */
struct omws__getLayers
{
public:
	void *_;	/* transient */
public:
	int soap_type() const { return 28; } /* = unique id SOAP_TYPE_omws__getLayers */
};
#endif

#ifndef SOAP_TYPE_omws__createModelResponse
#define SOAP_TYPE_omws__createModelResponse (31)
/* omws:createModelResponse */
struct omws__createModelResponse
{
public:
	char *ticket;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 31; } /* = unique id SOAP_TYPE_omws__createModelResponse */
};
#endif

#ifndef SOAP_TYPE_omws__createModel
#define SOAP_TYPE_omws__createModel (32)
/* omws:createModel */
struct omws__createModel
{
public:
	wchar_t *om__ModelParameters;	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 32; } /* = unique id SOAP_TYPE_omws__createModel */
};
#endif

#ifndef SOAP_TYPE_omws__getModelResponse
#define SOAP_TYPE_omws__getModelResponse (33)
/* omws:getModelResponse */
struct omws__getModelResponse
{
public:
	wchar_t *om__ModelEnvelope;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 33; } /* = unique id SOAP_TYPE_omws__getModelResponse */
};
#endif

#ifndef SOAP_TYPE_omws__getModel
#define SOAP_TYPE_omws__getModel (36)
/* omws:getModel */
struct omws__getModel
{
public:
	char *ticket;	/* optional element of type xsd:string */
public:
	int soap_type() const { return 36; } /* = unique id SOAP_TYPE_omws__getModel */
};
#endif

#ifndef SOAP_TYPE_omws__testModelResponse
#define SOAP_TYPE_omws__testModelResponse (38)
/* omws:testModelResponse */
struct omws__testModelResponse
{
public:
	char *ticket;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 38; } /* = unique id SOAP_TYPE_omws__testModelResponse */
};
#endif

#ifndef SOAP_TYPE_omws__testModel
#define SOAP_TYPE_omws__testModel (39)
/* omws:testModel */
struct omws__testModel
{
public:
	wchar_t *om__TestParameters;	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 39; } /* = unique id SOAP_TYPE_omws__testModel */
};
#endif

#ifndef SOAP_TYPE_omws__testResponse
#define SOAP_TYPE_omws__testResponse (40)
/* omws:testResponse */
struct omws__testResponse
{
public:
	wchar_t *om__TestResultEnvelope;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 40; } /* = unique id SOAP_TYPE_omws__testResponse */
};
#endif

#ifndef SOAP_TYPE_omws__getTestResult
#define SOAP_TYPE_omws__getTestResult (43)
/* omws:getTestResult */
struct omws__getTestResult
{
public:
	char *ticket;	/* optional element of type xsd:string */
public:
	int soap_type() const { return 43; } /* = unique id SOAP_TYPE_omws__getTestResult */
};
#endif

#ifndef SOAP_TYPE_omws__projectModelResponse
#define SOAP_TYPE_omws__projectModelResponse (45)
/* omws:projectModelResponse */
struct omws__projectModelResponse
{
public:
	char *ticket;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 45; } /* = unique id SOAP_TYPE_omws__projectModelResponse */
};
#endif

#ifndef SOAP_TYPE_omws__projectModel
#define SOAP_TYPE_omws__projectModel (46)
/* omws:projectModel */
struct omws__projectModel
{
public:
	wchar_t *om__ProjectionParameters;	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 46; } /* = unique id SOAP_TYPE_omws__projectModel */
};
#endif

#ifndef SOAP_TYPE_omws__getProgressResponse
#define SOAP_TYPE_omws__getProgressResponse (48)
/* omws:getProgressResponse */
struct omws__getProgressResponse
{
public:
	int progress;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:int */
public:
	int soap_type() const { return 48; } /* = unique id SOAP_TYPE_omws__getProgressResponse */
};
#endif

#ifndef SOAP_TYPE_omws__getProgress
#define SOAP_TYPE_omws__getProgress (49)
/* omws:getProgress */
struct omws__getProgress
{
public:
	char *ticket;	/* optional element of type xsd:string */
public:
	int soap_type() const { return 49; } /* = unique id SOAP_TYPE_omws__getProgress */
};
#endif

#ifndef SOAP_TYPE_omws__getLogResponse
#define SOAP_TYPE_omws__getLogResponse (51)
/* omws:getLogResponse */
struct omws__getLogResponse
{
public:
	char *log;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 51; } /* = unique id SOAP_TYPE_omws__getLogResponse */
};
#endif

#ifndef SOAP_TYPE_omws__getLog
#define SOAP_TYPE_omws__getLog (52)
/* omws:getLog */
struct omws__getLog
{
public:
	char *ticket;	/* optional element of type xsd:string */
public:
	int soap_type() const { return 52; } /* = unique id SOAP_TYPE_omws__getLog */
};
#endif

#ifndef SOAP_TYPE_xsd__base64Binary
#define SOAP_TYPE_xsd__base64Binary (53)
/* Base64 schema type: */
class SOAP_CMAC xsd__base64Binary
{
public:
	unsigned char *__ptr;
	int __size;
	char *id;	/* optional element of type xsd:string */
	char *type;	/* optional element of type xsd:string */
	char *options;	/* optional element of type xsd:string */
	xsd__base64Binary();	/* transient */
	struct soap *soap;	/* transient */
public:
	virtual int soap_type() const { return 53; } /* = unique id SOAP_TYPE_xsd__base64Binary */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	virtual ~xsd__base64Binary() { }
};
#endif

#ifndef SOAP_TYPE_omws__getLayerAsAttachmentResponse
#define SOAP_TYPE_omws__getLayerAsAttachmentResponse (62)
/* omws:getLayerAsAttachmentResponse */
struct omws__getLayerAsAttachmentResponse
{
public:
	xsd__base64Binary file;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:base64Binary */
public:
	int soap_type() const { return 62; } /* = unique id SOAP_TYPE_omws__getLayerAsAttachmentResponse */
};
#endif

#ifndef SOAP_TYPE_omws__getLayerAsAttachment
#define SOAP_TYPE_omws__getLayerAsAttachment (63)
/* omws:getLayerAsAttachment */
struct omws__getLayerAsAttachment
{
public:
	char *id;	/* optional element of type xsd:string */
public:
	int soap_type() const { return 63; } /* = unique id SOAP_TYPE_omws__getLayerAsAttachment */
};
#endif

#ifndef SOAP_TYPE_omws__getLayerAsUrlResponse
#define SOAP_TYPE_omws__getLayerAsUrlResponse (65)
/* omws:getLayerAsUrlResponse */
struct omws__getLayerAsUrlResponse
{
public:
	char *url;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 65; } /* = unique id SOAP_TYPE_omws__getLayerAsUrlResponse */
};
#endif

#ifndef SOAP_TYPE_omws__getLayerAsUrl
#define SOAP_TYPE_omws__getLayerAsUrl (66)
/* omws:getLayerAsUrl */
struct omws__getLayerAsUrl
{
public:
	char *id;	/* optional element of type xsd:string */
public:
	int soap_type() const { return 66; } /* = unique id SOAP_TYPE_omws__getLayerAsUrl */
};
#endif

#ifndef SOAP_TYPE_omws__getLayerAsWcsResponse
#define SOAP_TYPE_omws__getLayerAsWcsResponse (68)
/* omws:getLayerAsWcsResponse */
struct omws__getLayerAsWcsResponse
{
public:
	char *url;	/* SOAP 1.2 RPC return element (when namespace qualified) */	/* required element of type xsd:string */
public:
	int soap_type() const { return 68; } /* = unique id SOAP_TYPE_omws__getLayerAsWcsResponse */
};
#endif

#ifndef SOAP_TYPE_omws__getLayerAsWcs
#define SOAP_TYPE_omws__getLayerAsWcs (69)
/* omws:getLayerAsWcs */
struct omws__getLayerAsWcs
{
public:
	char *id;	/* optional element of type xsd:string */
public:
	int soap_type() const { return 69; } /* = unique id SOAP_TYPE_omws__getLayerAsWcs */
};
#endif

#ifndef SOAP_TYPE_omws__getProjectionMetadataResponse
#define SOAP_TYPE_omws__getProjectionMetadataResponse (70)
/* omws:getProjectionMetadataResponse */
struct omws__getProjectionMetadataResponse
{
public:
	int FileSize;	/* optional attribute of type xsd:int */
	wchar_t *om__ProjectionEnvelope;	/* optional element of type xsd:anyType */
public:
	int soap_type() const { return 70; } /* = unique id SOAP_TYPE_omws__getProjectionMetadataResponse */
};
#endif

#ifndef SOAP_TYPE_omws__getProjectionMetadata
#define SOAP_TYPE_omws__getProjectionMetadata (73)
/* omws:getProjectionMetadata */
struct omws__getProjectionMetadata
{
public:
	char *ticket;	/* optional element of type xsd:string */
public:
	int soap_type() const { return 73; } /* = unique id SOAP_TYPE_omws__getProjectionMetadata */
};
#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (74)
/* SOAP Fault Code: */
struct SOAP_ENV__Code
{
public:
	char *SOAP_ENV__Value;	/* optional element of type xsd:QName */
	struct SOAP_ENV__Code *SOAP_ENV__Subcode;	/* optional element of type SOAP-ENV:Code */
public:
	int soap_type() const { return 74; } /* = unique id SOAP_TYPE_SOAP_ENV__Code */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (76)
/* SOAP-ENV:Detail */
struct SOAP_ENV__Detail
{
public:
	char *__any;
	int __type;	/* any type of element <fault> (defined below) */
	void *fault;	/* transient */
public:
	int soap_type() const { return 76; } /* = unique id SOAP_TYPE_SOAP_ENV__Detail */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (77)
/* SOAP-ENV:Reason */
struct SOAP_ENV__Reason
{
public:
	char *SOAP_ENV__Text;	/* optional element of type xsd:string */
public:
	int soap_type() const { return 77; } /* = unique id SOAP_TYPE_SOAP_ENV__Reason */
};
#endif

#endif

#ifndef WITH_NOGLOBAL

#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (78)
/* SOAP Fault: */
struct SOAP_ENV__Fault
{
public:
	char *faultcode;	/* optional element of type xsd:QName */
	char *faultstring;	/* optional element of type xsd:string */
	char *faultactor;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *detail;	/* optional element of type SOAP-ENV:Detail */
	struct SOAP_ENV__Code *SOAP_ENV__Code;	/* optional element of type SOAP-ENV:Code */
	struct SOAP_ENV__Reason *SOAP_ENV__Reason;	/* optional element of type SOAP-ENV:Reason */
	char *SOAP_ENV__Node;	/* optional element of type xsd:string */
	char *SOAP_ENV__Role;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *SOAP_ENV__Detail;	/* optional element of type SOAP-ENV:Detail */
public:
	int soap_type() const { return 78; } /* = unique id SOAP_TYPE_SOAP_ENV__Fault */
};
#endif

#endif

/******************************************************************************\
 *                                                                            *
 * Typedefs                                                                   *
 *                                                                            *
\******************************************************************************/

#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (5)
typedef char *_QName;
#endif

#ifndef SOAP_TYPE__XML
#define SOAP_TYPE__XML (6)
typedef char *_XML;
#endif

#ifndef SOAP_TYPE_xsd__string
#define SOAP_TYPE_xsd__string (7)
typedef char *xsd__string;
#endif

#ifndef SOAP_TYPE_xsd__double
#define SOAP_TYPE_xsd__double (9)
typedef double xsd__double;
#endif

#ifndef SOAP_TYPE_xsd__int
#define SOAP_TYPE_xsd__int (10)
typedef int xsd__int;
#endif

#ifndef SOAP_TYPE_XML
#define SOAP_TYPE_XML (13)
typedef wchar_t *XML;
#endif


/******************************************************************************\
 *                                                                            *
 * Externals                                                                  *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Server-Side Operations                                                     *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 omws__ping(struct soap*, void *_, int &status);

SOAP_FMAC5 int SOAP_FMAC6 omws__getAlgorithms(struct soap*, void *_, struct omws__getAlgorithmsResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 omws__getLayers(struct soap*, void *_, struct omws__getLayersResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 omws__createModel(struct soap*, wchar_t *om__ModelParameters, char *&ticket);

SOAP_FMAC5 int SOAP_FMAC6 omws__getModel(struct soap*, char *ticket, struct omws__getModelResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 omws__testModel(struct soap*, wchar_t *om__TestParameters, char *&ticket);

SOAP_FMAC5 int SOAP_FMAC6 omws__getTestResult(struct soap*, char *ticket, struct omws__testResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 omws__projectModel(struct soap*, wchar_t *om__ProjectionParameters, char *&ticket);

SOAP_FMAC5 int SOAP_FMAC6 omws__getProgress(struct soap*, char *ticket, int &progress);

SOAP_FMAC5 int SOAP_FMAC6 omws__getLog(struct soap*, char *ticket, char *&log);

SOAP_FMAC5 int SOAP_FMAC6 omws__getLayerAsAttachment(struct soap*, char *id, xsd__base64Binary &file);

SOAP_FMAC5 int SOAP_FMAC6 omws__getLayerAsUrl(struct soap*, char *id, char *&url);

SOAP_FMAC5 int SOAP_FMAC6 omws__getLayerAsWcs(struct soap*, char *id, char *&url);

SOAP_FMAC5 int SOAP_FMAC6 omws__getProjectionMetadata(struct soap*, char *ticket, struct omws__getProjectionMetadataResponse *out);

/******************************************************************************\
 *                                                                            *
 * Server-Side Skeletons to Invoke Service Operations                         *
 *                                                                            *
\******************************************************************************/

extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap*);

extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__ping(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getAlgorithms(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getLayers(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__createModel(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getModel(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__testModel(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getTestResult(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__projectModel(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getProgress(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getLog(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getLayerAsAttachment(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getLayerAsUrl(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getLayerAsWcs(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getProjectionMetadata(struct soap*);

/******************************************************************************\
 *                                                                            *
 * Client-Side Call Stubs                                                     *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__ping(struct soap *soap, const char *soap_endpoint, const char *soap_action, void *_, int &status);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getAlgorithms(struct soap *soap, const char *soap_endpoint, const char *soap_action, void *_, struct omws__getAlgorithmsResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getLayers(struct soap *soap, const char *soap_endpoint, const char *soap_action, void *_, struct omws__getLayersResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__createModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, wchar_t *om__ModelParameters, char *&ticket);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, struct omws__getModelResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__testModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, wchar_t *om__TestParameters, char *&ticket);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getTestResult(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, struct omws__testResponse *out);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__projectModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, wchar_t *om__ProjectionParameters, char *&ticket);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getProgress(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, int &progress);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getLog(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, char *&log);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getLayerAsAttachment(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *id, xsd__base64Binary &file);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getLayerAsUrl(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *id, char *&url);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getLayerAsWcs(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *id, char *&url);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getProjectionMetadata(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, struct omws__getProjectionMetadataResponse *out);

#endif

/* End of soapStub.h */