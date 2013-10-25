/*****************************************************************************
 * openModeller Web Service (OMWS) API 2.0
 * 
 * This file contains all gSOAP definitions that are necessary to build client 
 * and/or server implementations compliant with the OMWS API:
 * namespaces, configuration, data type mapppings, structures and operations.
 * It should be used as input to soapcpp2 to automatically generate WSDL and
 * SOAP libraries.
 *
 *****************************************************************************/

//gsoap omws service name: openModeller-web-service-2.0
//gsoap omws service port: http://modeller.cria.org.br/ws2/om
//gsoap omws service namespace: http://openmodeller.cria.org.br/ws/2.0
//gsoap omws service documentation: Second generation of a web wervice interface for remote invocation of openModeller
//gsoap omws service encoding: literal
//gsoap om schema namespace: http://openmodeller.cria.org.br/xml/2.0
//gsoap om schema import: http://openmodeller.cria.org.br/xml/2.0/openModeller.xsd

/** gSOAP data type mappings */

typedef std::string xsd__string;
typedef double xsd__double;
typedef int xsd__int; 
typedef wchar_t *XML;
typedef char *XML_;

/** Operation to monitor the service.
 *
 * @param _ A void* input parameter so that some compilers (notably vc++) will not complain 
 *          about empty structs. This SOAP method should be called without any parameters.
 * @return status Integer value (1 - OK, 2 - unavailable)
 */
//gsoap omws service method-documentation: ping Operation to monitor service availability.
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

/** Get all available layers on the server.
 *
 * @param _ A void* input parameter so that some compilers (notably vc++) will not complain 
 *          about empty structs. This SOAP method should be called without any parameters.
 * @param om__AvailableLayers Reference to the literal XML that will be returned.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: getLayers Returns all available layers on the server.
int omws__getLayers(void *_, struct omws__getLayersResponse *out);

/** Create a distribution model (not a map!) using all input parameters.
 *
 * @param om__ModelParameters XML with model creation parameters 
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: createModel Schedules the creation of an ecological niche model
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
//gsoap omws service method-documentation: getModel Returns an ecological niche model given a ticket.
int omws__getModel(xsd__string ticket, struct omws__getModelResponse *out);

/** Test a distribution model.
 *
 * @param om__TestParameters XML with parameters to test a model
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: testModel Schedules a model test with the specified points and environmental scenario.
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
//gsoap omws service method-documentation: getTestResult Returns a test result given a ticket.
int omws__getTestResult(xsd__string ticket, struct omws__testResponse *out);

/** Project a distribution model (creating a map) using all input parameters.
 *
 * @param om__ProjectionParameters XML with model projection parameters 
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: projectModel Schedules a model projection.
int omws__projectModel(XML om__ProjectionParameters, xsd__string &ticket); 

/** Return the progress given a list of tickets.
 * @param ticket Comma-separated job tickets.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: getProgress Returns the progress of one or more jobs.
int omws__getProgress(xsd__string tickets, xsd__string &progress);

/** Return the log of a finished job given a ticket.
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: getLog Returns the log of a job.
int omws__getLog(xsd__string ticket, xsd__string &log);

/** Return a layer as an URL.
 * @param id Layer identification.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: getLayerAsUrl Returns the URL of a projected model.
int omws__getLayerAsUrl(xsd__string id, xsd__string &url);

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
//gsoap omws service method-documentation: getProjectionMetadata Returns additional data about a model projection.
int omws__getProjectionMetadata(xsd__string ticket, struct omws__getProjectionMetadataResponse *out);

/** Evaluate Model.
 *
 * @param om__EvaluateModel XML with model evaluation parameters 
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: evaluateModel Schedules a model evaluation to get model values at the specified points and environmental scenario.
int omws__evaluateModel(XML om__ModelEvaluationParameters, xsd__string &ticket); 

/** Return type of evaluateModel */
struct omws__modelEvaluationResponse
{
   XML om__ModelEvaluation;
};

/** Return model evaluation given a ticket.
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: getModelEvaluation Returns model values given a ticket.
int omws__getModelEvaluation(xsd__string ticket, struct omws__modelEvaluationResponse *out);

/** Sample points.
 *
 * @param om__SamplingParameters XML with sampling parameters 
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: samplePoints Schedules a sampling operation to get a random set of points in the given environment and mask.
int omws__samplePoints(XML om__SamplingParameters, xsd__string &ticket); 

/** Return type of samplePoints */
struct omws__getSamplingResultResponse
{
   XML om__Sampler;
};

/** Return sampling result given a ticket.
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: getSamplingresult Returns a set of points given a ticket.
int omws__getSamplingResult(xsd__string ticket, struct omws__getSamplingResultResponse *out);

/** Return type of runExperiment */
struct omws__runExperimentResponse
{
   XML om__ExperimentTickets;
};

/** Run Experiment. Note: the first parameter uses the char * type so that its content can be
 *  deserialized by gsoap inside the method using an istream. 
 *
 * @param om__RunExperiment XML with experiment parameters 
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: runExperiment Schedules a modelling experiment, which may involve sampling points, model creation, test and/or projection.
int omws__runExperiment(XML_ om__ExperimentParameters, struct omws__runExperimentResponse *out); 

/** Return type of getResults */
struct omws__getResultsResponse
{
   XML om__ResultSet;
};

/** Return sampling result given a list of tickets.
 * @param tickets Comma-separated job tickets.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: getResults Returns job results given one or more tickets.
int omws__getResults(xsd__string tickets, struct omws__getResultsResponse *out);

/** Cancel the specified jobs.
 * @param ticket Comma-separated job tickets.
 * @return standard gSOAP integer code
 */
//gsoap omws service method-documentation: cancel Cancels one or more jobs, returning the list of cancelled jobs.
int omws__cancel(xsd__string tickets, xsd__string &cancelledTickets);
