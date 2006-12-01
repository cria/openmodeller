/* soapStub.h
   Generated by gSOAP 2.7.6d from om_soap.hh
   Copyright (C) 2000-2005, Robert van Engelen, Genivia Inc. All Rights Reserved.
   This part of the software is released under one of the following licenses:
   GPL, the gSOAP public license, or Genivia's license for commercial use.
*/
#ifndef soapStub_H
#define soapStub_H
#include "stdsoap2.h"

/******************************************************************************\
 *                                                                            *
 * Enumerations                                                               *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Classes and Structs                                                        *
 *                                                                            *
\******************************************************************************/


#ifndef SOAP_TYPE_SOAP_ENV__Header
#define SOAP_TYPE_SOAP_ENV__Header (13)
/* SOAP Header: */
struct SOAP_ENV__Header
{
	char *omws__version;	/* optional element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_omws__pingResponse
#define SOAP_TYPE_omws__pingResponse (18)
/* omws:pingResponse */
struct omws__pingResponse
{
	int status;	/* RPC return element */	/* required element of type xsd:int */
};
#endif

#ifndef SOAP_TYPE_omws__ping
#define SOAP_TYPE_omws__ping (19)
/* omws:ping */
struct omws__ping
{
	void *_;	/* transient */
};
#endif

#ifndef SOAP_TYPE_omws__getAlgorithms
#define SOAP_TYPE_omws__getAlgorithms (22)
/* omws:getAlgorithms */
struct omws__getAlgorithms
{
	void *_;	/* transient */
};
#endif

#ifndef SOAP_TYPE_omws__getLayers
#define SOAP_TYPE_omws__getLayers (24)
/* omws:getLayers */
struct omws__getLayers
{
	void *_;	/* transient */
};
#endif

#ifndef SOAP_TYPE_omws__createModelResponse
#define SOAP_TYPE_omws__createModelResponse (27)
/* omws:createModelResponse */
struct omws__createModelResponse
{
	char *ticket;	/* RPC return element */	/* required element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_omws__createModel
#define SOAP_TYPE_omws__createModel (28)
/* omws:createModel */
struct omws__createModel
{
	wchar_t *om__ModelParameters;	/* optional element of type xsd:anyType */
};
#endif

#ifndef SOAP_TYPE_omws__getModel
#define SOAP_TYPE_omws__getModel (30)
/* omws:getModel */
struct omws__getModel
{
	char *ticket;	/* optional element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_omws__projectModelResponse
#define SOAP_TYPE_omws__projectModelResponse (32)
/* omws:projectModelResponse */
struct omws__projectModelResponse
{
	char *ticket;	/* RPC return element */	/* required element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_omws__projectModel
#define SOAP_TYPE_omws__projectModel (33)
/* omws:projectModel */
struct omws__projectModel
{
	wchar_t *om__ProjectionParameters;	/* optional element of type xsd:anyType */
};
#endif

#ifndef SOAP_TYPE_omws__getProgressResponse
#define SOAP_TYPE_omws__getProgressResponse (35)
/* omws:getProgressResponse */
struct omws__getProgressResponse
{
	int progress;	/* RPC return element */	/* required element of type xsd:int */
};
#endif

#ifndef SOAP_TYPE_omws__getProgress
#define SOAP_TYPE_omws__getProgress (36)
/* omws:getProgress */
struct omws__getProgress
{
	char *ticket;	/* optional element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_omws__getLogResponse
#define SOAP_TYPE_omws__getLogResponse (38)
/* omws:getLogResponse */
struct omws__getLogResponse
{
	char *log;	/* RPC return element */	/* required element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_omws__getLog
#define SOAP_TYPE_omws__getLog (39)
/* omws:getLog */
struct omws__getLog
{
	char *ticket;	/* optional element of type xsd:string */
};
#endif



#ifndef SOAP_TYPE_xsd__base64Binary
#define SOAP_TYPE_xsd__base64Binary (40)
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
	virtual int soap_type() const { return 40; } /* = unique id SOAP_TYPE_xsd__base64Binary */
	virtual void soap_default(struct soap*);
	virtual void soap_serialize(struct soap*) const;
	virtual int soap_put(struct soap*, const char*, const char*) const;
	virtual int soap_out(struct soap*, const char*, int, const char*) const;
	virtual void *soap_get(struct soap*, const char*, const char*);
	virtual void *soap_in(struct soap*, const char*, const char*);
	virtual ~xsd__base64Binary() { }
};
#endif

#ifndef SOAP_TYPE_omws__getMapAsAttachmentResponse
#define SOAP_TYPE_omws__getMapAsAttachmentResponse (50)
/* omws:getMapAsAttachmentResponse */
struct omws__getMapAsAttachmentResponse
{
	xsd__base64Binary file;	/* RPC return element */	/* required element of type xsd:base64Binary */
};
#endif

#ifndef SOAP_TYPE_omws__getMapAsAttachment
#define SOAP_TYPE_omws__getMapAsAttachment (51)
/* omws:getMapAsAttachment */
struct omws__getMapAsAttachment
{
	char *ticket;	/* optional element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_omws__getMapAsUrlResponse
#define SOAP_TYPE_omws__getMapAsUrlResponse (53)
/* omws:getMapAsUrlResponse */
struct omws__getMapAsUrlResponse
{
	char *url;	/* RPC return element */	/* required element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_omws__getMapAsUrl
#define SOAP_TYPE_omws__getMapAsUrl (54)
/* omws:getMapAsUrl */
struct omws__getMapAsUrl
{
	char *ticket;	/* optional element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_omws__ProjectionData
#define SOAP_TYPE_omws__ProjectionData (55)
/* omws:ProjectionData */
struct omws__ProjectionData
{
	int fileSize;	/* RPC return element */	/* optional attribute of type xsd:int */
	wchar_t *om__AreaStatistics;	/* optional element of type xsd:anyType */
};
#endif

#ifndef SOAP_TYPE_omws__getProjectionData
#define SOAP_TYPE_omws__getProjectionData (58)
/* omws:getProjectionData */
struct omws__getProjectionData
{
	char *ticket;	/* optional element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Code
#define SOAP_TYPE_SOAP_ENV__Code (59)
/* SOAP Fault Code: */
struct SOAP_ENV__Code
{
	char *SOAP_ENV__Value;	/* optional element of type QName */
	struct SOAP_ENV__Code *SOAP_ENV__Subcode;	/* optional element of type SOAP-ENV:Code */
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Detail
#define SOAP_TYPE_SOAP_ENV__Detail (61)
/* SOAP-ENV:Detail */
struct SOAP_ENV__Detail
{
	int __type;	/* any type of element <fault> (defined below) */
	void *fault;	/* transient */
	char *__any;
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Reason
#define SOAP_TYPE_SOAP_ENV__Reason (62)
/* SOAP-ENV:Reason */
struct SOAP_ENV__Reason
{
	char *SOAP_ENV__Text;	/* optional element of type xsd:string */
};
#endif

#ifndef SOAP_TYPE_SOAP_ENV__Fault
#define SOAP_TYPE_SOAP_ENV__Fault (63)
/* SOAP Fault: */
struct SOAP_ENV__Fault
{
	char *faultcode;	/* optional element of type QName */
	char *faultstring;	/* optional element of type xsd:string */
	char *faultactor;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *detail;	/* optional element of type SOAP-ENV:Detail */
	struct SOAP_ENV__Code *SOAP_ENV__Code;	/* optional element of type SOAP-ENV:Code */
	struct SOAP_ENV__Reason *SOAP_ENV__Reason;	/* optional element of type SOAP-ENV:Reason */
	char *SOAP_ENV__Node;	/* optional element of type xsd:string */
	char *SOAP_ENV__Role;	/* optional element of type xsd:string */
	struct SOAP_ENV__Detail *SOAP_ENV__Detail;	/* optional element of type SOAP-ENV:Detail */
};
#endif

/******************************************************************************\
 *                                                                            *
 * Types with Custom Serializers                                              *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Typedefs                                                                   *
 *                                                                            *
\******************************************************************************/

#ifndef SOAP_TYPE__XML
#define SOAP_TYPE__XML (4)
typedef char *_XML;
#endif

#ifndef SOAP_TYPE__QName
#define SOAP_TYPE__QName (5)
typedef char *_QName;
#endif

#ifndef SOAP_TYPE_xsd__string
#define SOAP_TYPE_xsd__string (6)
typedef char *xsd__string;
#endif

#ifndef SOAP_TYPE_xsd__double
#define SOAP_TYPE_xsd__double (8)
typedef double xsd__double;
#endif

#ifndef SOAP_TYPE_xsd__int
#define SOAP_TYPE_xsd__int (9)
typedef int xsd__int;
#endif

#ifndef SOAP_TYPE_XML
#define SOAP_TYPE_XML (12)
typedef wchar_t *XML;
#endif


/******************************************************************************\
 *                                                                            *
 * Typedef Synonyms                                                           *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Externals                                                                  *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Service Operations                                                         *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 omws__ping(struct soap*, void *_, int &status);

SOAP_FMAC5 int SOAP_FMAC6 omws__getAlgorithms(struct soap*, void *_, wchar_t *&om__AvailableAlgorithms);

SOAP_FMAC5 int SOAP_FMAC6 omws__getLayers(struct soap*, void *_, wchar_t *&om__AvailableLayers);

SOAP_FMAC5 int SOAP_FMAC6 omws__createModel(struct soap*, wchar_t *om__ModelParameters, char *&ticket);

SOAP_FMAC5 int SOAP_FMAC6 omws__getModel(struct soap*, char *ticket, wchar_t *&om__ModelEnvelope);

SOAP_FMAC5 int SOAP_FMAC6 omws__projectModel(struct soap*, wchar_t *om__ProjectionParameters, char *&ticket);

SOAP_FMAC5 int SOAP_FMAC6 omws__getProgress(struct soap*, char *ticket, int &progress);

SOAP_FMAC5 int SOAP_FMAC6 omws__getLog(struct soap*, char *ticket, char *&log);

SOAP_FMAC5 int SOAP_FMAC6 omws__getMapAsAttachment(struct soap*, char *ticket, xsd__base64Binary &file);

SOAP_FMAC5 int SOAP_FMAC6 omws__getMapAsUrl(struct soap*, char *ticket, char *&url);

SOAP_FMAC5 int SOAP_FMAC6 omws__getProjectionData(struct soap*, char *ticket, struct omws__ProjectionData *out);

/******************************************************************************\
 *                                                                            *
 * Stubs                                                                      *
 *                                                                            *
\******************************************************************************/


SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__ping(struct soap *soap, const char *soap_endpoint, const char *soap_action, void *_, int &status);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getAlgorithms(struct soap *soap, const char *soap_endpoint, const char *soap_action, void *_, wchar_t *&om__AvailableAlgorithms);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getLayers(struct soap *soap, const char *soap_endpoint, const char *soap_action, void *_, wchar_t *&om__AvailableLayers);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__createModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, wchar_t *om__ModelParameters, char *&ticket);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, wchar_t *&om__ModelEnvelope);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__projectModel(struct soap *soap, const char *soap_endpoint, const char *soap_action, wchar_t *om__ProjectionParameters, char *&ticket);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getProgress(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, int &progress);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getLog(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, char *&log);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getMapAsAttachment(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, xsd__base64Binary &file);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getMapAsUrl(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, char *&url);

SOAP_FMAC5 int SOAP_FMAC6 soap_call_omws__getProjectionData(struct soap *soap, const char *soap_endpoint, const char *soap_action, char *ticket, struct omws__ProjectionData *out);

/******************************************************************************\
 *                                                                            *
 * Skeletons                                                                  *
 *                                                                            *
\******************************************************************************/

SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__ping(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getAlgorithms(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getLayers(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__createModel(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getModel(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__projectModel(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getProgress(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getLog(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getMapAsAttachment(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getMapAsUrl(struct soap*);

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_omws__getProjectionData(struct soap*);

#endif

/* End of soapStub.h */
