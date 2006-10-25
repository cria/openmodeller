//gsoap omws service name: openModeller
//gsoap omws service port: http://localhost:8085
//gsoap omws service namespace: http://openmodeller.cria.org.br/ws/1.0
//gsoap omws service documentation: Web Services interface for remote invocation of openModeller
//gsoap omws service encoding: literal
//gsoap om schema namespace: http://openmodeller.cria.org.br/xml/1.0
//gsoap om schema import: http://openmodeller.cria.org.br/xml/1.0/openModeller.xsd

/** gSOAP data type mappings */

typedef char *xsd__string;
typedef double xsd__double;
typedef int xsd__int; 
typedef wchar_t *XML;

/** SOAP header structure.
 */
struct SOAP_ENV__Header 
{ 
  xsd__string omws__version; 
}; 

/** Simple method just to monitor the service.
 *
 * @param _ A void* input parameter so that some compilers (notably vc++) will not complain 
 *          about empty structs. This SOAP method should be called without any parameters.
 * @return status Integer value (1 - OK)
 */
//gsoap omws service method-documentation: ping Simple method to monitor service availability.
int omws__ping(void *_, xsd__int &status); 

/** Get all available algorithms and return their metadata.
 *
 * @param _ A void* input parameter so that some compilers (notably vc++) will not complain 
 *          about empty structs. This SOAP method should be called without any parameters.
 * @param om__AvailableAlgorithms Reference to the literal XML that will be returned.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: getAlgorithms Returns metadata about all available algorithms.
int omws__getAlgorithms(void *_, XML &om__AvailableAlgorithms);

/** Get all available layers on the server side.
 *
 * @param _ A void* input parameter so that some compilers (notably vc++) will not complain 
 *          about empty structs. This SOAP method should be called without any parameters.
 * @param om__AvailableLayers Reference to the literal XML that will be returned.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: getLayers Returns all available layers on the server side.
int omws__getLayers(void *_, XML &om__AvailableLayers);

/** Create a distribution model (not a map!) using all input parameters.
 *
 * @param omws__createModel XML with model creation parameters 
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-documentation: createModel Requests the creation of a spatial distribution model
int omws__createModel(XML om__ModelParameters, xsd__string &ticket); 

/** Return a serialized model given a ticket.
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-documentation: getModel Retrieves a serialized model given a ticket.
int omws__getModel(xsd__string ticket, XML &om__ModelEnvelope);

/** Project a distribution model (creating a map) using all input parameters.
 *
 * @param omws__projectModel XML with model projection parameters 
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-documentation: projectModel Requests the projection of a spatial distribution model
int omws__projectModel(XML om__ProjectionParameters, xsd__string &ticket); 

/** Return the progress of a job given a ticket.
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-documentation: getProgress Returns the progress of a job.
int omws__getProgress(xsd__string ticket, xsd__int &progress);

/** Return the log of a finished job given a ticket.
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-documentation: getLog Returns the log of a job.
int omws__getLog(xsd__string ticket, xsd__string &log);

class xsd__base64Binary 
{ 
   unsigned char *__ptr; 
   xsd__int __size; 
   xsd__string id; 
   xsd__string type; 
   xsd__string options; 
   xsd__base64Binary();
   struct soap *soap;	/* soap context that created this instance */
}; 

/** Return the distribution map as an attachment.
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-documentation: getMapAsAttachment Returns the corresponding distribution map as a DIME attachment.
int omws__getMapAsAttachment(xsd__string ticket, xsd__base64Binary &file);

/** Return the distribution map as an URL.
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-documentation: getMapAsUrl Returns the corresponding distribution map URL.
int omws__getMapAsUrl(xsd__string ticket, xsd__string &url);

