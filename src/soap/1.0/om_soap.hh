//gsoap omws service name: openModeller-web-service-1.0
//gsoap omws service port: http://modeller.cria.org.br:80/cgi-bin/om_soap_server.cgi
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

/** Return type of getAlgorithms */
struct omws__getAlgorithmsResponse
{
   XML om__AvailableAlgorithms;
}; 

/** Get all available algorithms and return their metadata.
 *
 * @param _ A void* input parameter so that some compilers (notably vc++) will not complain 
 *          about empty structs. This SOAP method should be called without any parameters.
 * @param om__AvailableAlgorithms Reference to the literal XML that will be returned.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: getAlgorithms Returns metadata about all available algorithms.
int omws__getAlgorithms(void *_, struct omws__getAlgorithmsResponse *out);

/** Return type of getLayers */
struct omws__getLayersResponse
{
   XML om__AvailableLayers;
}; 

/** Get all available layers on the server side.
 *
 * @param _ A void* input parameter so that some compilers (notably vc++) will not complain 
 *          about empty structs. This SOAP method should be called without any parameters.
 * @param om__AvailableLayers Reference to the literal XML that will be returned.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: getLayers Returns all available layers on the server side.
int omws__getLayers(void *_, struct omws__getLayersResponse *out);

/** Create a distribution model (not a map!) using all input parameters.
 *
 * @param om__ModelParameters XML with model creation parameters 
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-documentation: createModel Requests the creation of a spatial distribution model
int omws__createModel(XML om__ModelParameters, xsd__string &ticket); 

/** Return type of getModel */
struct omws__getModelResponse
{
   XML om__ModelEnvelope;
}; 

/** Return a serialized model given a ticket.
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-documentation: getModel Retrieves a serialized model given a ticket.
int omws__getModel(xsd__string ticket, struct omws__getModelResponse *out);

/** Test a distribution model.
 *
 * @param om__TestParameters XML with parameters to test a model
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-documentation: testModel Test a given distribution model with the given points
int omws__testModel(XML om__TestParameters, xsd__string &ticket);

/** Return type of testModel */
struct omws__testResponse
{
   XML om__TestResultEnvelope;
}; 

/** Return the result of a test.
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-documentation: getTestResult Retrieves the result of a test.
int omws__getTestResult(xsd__string ticket, struct omws__testResponse *out);

/** Project a distribution model (creating a map) using all input parameters.
 *
 * @param om__ProjectionParameters XML with model projection parameters 
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

/** Return a layer as an attachment.
 * @param id Layer identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-documentation: getLayerAsAttachment Returns the corresponding distribution map as a DIME attachment.
int omws__getLayerAsAttachment(xsd__string id, xsd__base64Binary &file);

/** Return a layer as an URL.
 * @param id Layer identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-documentation: getLayerAsUrl Returns the corresponding distribution map URL.
int omws__getLayerAsUrl(xsd__string id, xsd__string &url);

/** Return a WCS (OCG Web Coverage Service) URL for a layer.
 * @param id Layer identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-documentation: getLayerAsWcs Returns a WCS URL for the corresponding distribution map.
int omws__getLayerAsWcs(xsd__string id, xsd__string &url);

/** Return type of getProjectionMetadata */
struct omws__getProjectionMetadataResponse
{
   @xsd__int FileSize; 
   XML om__ProjectionEnvelope;
}; 

/** Return additional data about the projection.
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-documentation: getProjectionMetadata Retrieves additional data about the projection (area statistics and file size) given a ticket.
int omws__getProjectionMetadata(xsd__string ticket, struct omws__getProjectionMetadataResponse *out);
