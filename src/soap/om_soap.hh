//gsoap om service name: openModeller
//gsoap om service port: http://localhost:8085
//gsoap om service namespace: http://openmodeller.sf.net/ns/1.0

/** gSOAP data type mappings */

typedef char *xsd__string;
typedef double xsd__double;
typedef int xsd__int; 

/** SOAP header structure.
 */
struct SOAP_ENV__Header 
{ 
  xsd__string om__version; 
}; 

/** SOAP struct to store an algorithm parameter metadata.*/
typedef struct soap_AlgorithmParameter
{
  @xsd__string Id;
  @xsd__string Name;
  @xsd__string Type;
  xsd__string  Overview;
  xsd__string  Description;

  @xsd__int     HasMin;
  @xsd__double  Min;
  @xsd__int     HasMax;
  @xsd__double  Max;
  @xsd__string  Typical;

} om__AlgorithmParameter;

/** SOAP struct to store algorithm parameters metadata */
typedef struct soap_AlgorithmParameters
{
  int __size;
  om__AlgorithmParameter *__ptrParam;

} om__AlgorithmParameters;

/** SOAP struct to store algorithm metadata. */
typedef struct soap_AlgorithmMetadata
{
  @xsd__string Id;
  @xsd__string Name;
  @xsd__string Version;
  xsd__string  Overview;
  xsd__string  Description;

  @xsd__string Author;
  xsd__string  Bibliography;

  @xsd__string CodeAuthor;
  @xsd__string Contact;

  @xsd__int Categorical;
  @xsd__int Absence;

} om__AlgorithmMetadata;

/** SOAP struct to store an available algorithm */
typedef struct soap_AvailableAlgorithm
{
  om__AlgorithmMetadata   *__ptrAlgorithmMetadata;
  om__AlgorithmParameters *__ptrAlgorithmParameters;

} om__AvailableAlgorithm;

/** SOAP struct to store a collection of available algorithms */
typedef struct soap_AvailableAlgorithms
{
  int __size;
  om__AvailableAlgorithm *__ptrAlgorithm;

} om__AvailableAlgorithms;

/** This struct exists because getAlgorithms may return more than one output parameter.
 *  See item 8.1.9 from gSOAP documentation ("How to specify multiple output parameters").
 */
struct om__getAlgorithmsResponse {om__AvailableAlgorithms _AvailableAlgorithms;};

/** Get all available algorithms and return their metadata in a SOAP structure.
 * @note This method is implemented in om_soap.cpp, where it receives a soap structure as
 *       an additional parameter. The definition here is used only to generate the SOAP 
 *       stub/skeleton.
 * @param _ A void* input parameter so that some compilers (notably vc++) will not complain 
 *          about empty structs. This SOAP method should be called without any parameters.
 * @param r Reference to the SOAP structure that will be returned.
 * @return standard gSOAP integer code
 */
//gsoap om service method-action: Return metadata about all available algorithms
//gsoap om service method-documentation: Return metadata about all available algorithms
int om__getAlgorithms(void *_, struct om__getAlgorithmsResponse &r); 

/** SOAP struct to store data about a point (presence or absence).
 */
typedef struct soap_Point
{
  @xsd__double latitude;
  @xsd__double longitude;

} om__Point;

/** SOAP struct to store a collection of presence points  */
typedef struct soap_PresencePoints
{
  int __size;
  om__Point *__ptrpoint;

} om__PresencePoints;

/** SOAP struct to store a collection of absence points  */
typedef struct soap_AbsencePoints
{
  int __size;
  om__Point *__ptrpoint;

} om__AbsencePoints;

/** SOAP struct to store points data */
typedef struct soap_Points
{
  xsd__string         coordsystem;
  om__PresencePoints *__ptrpresences;
  om__AbsencePoints  *__ptrabsences;

} om__Points;

/** SOAP struct to store data about a map (environmental layer).
 */
typedef struct soap_Map
{
  @xsd__string location;
  @xsd__int    categorical;

} om__Map;

/** SOAP struct to store a collection of maps (environmental layers)  */
typedef struct soap_Maps
{
  int __size;
  om__Map *__ptrmap;

} om__Maps;

/** SOAP struct to store data about a mask (region of interest).
 */
typedef struct soap_Mask
{
  @xsd__string  location;

} om__Mask;

/** SOAP struct to store a real algorithm parameter in a createModel request.
 */
typedef struct soap_Parameter
{
  @xsd__string Id;
  @xsd__string Value;

} om__Parameter;

/** SOAP struct to store algorithm data in createModel requests.
 */
typedef struct soap_Algorithm
{
  @xsd__string Id;

  int __size;
  om__Parameter *__ptrparameter;

} om__Algorithm;

/** SOAP struct to store specification of the distribution map.
 */
typedef struct soap_Output
{
  xsd__string header;
  xsd__int    scale;
  xsd__string format;

} om__Output;

/** Create a distribution model using all input parameters.
 * @note This method is implemented in om_soap.cpp, where it receives a soap structure as
 *       an additional parameter. The definition here is used only to generate the SOAP 
 *       stub/skeleton.
 * @param points Pointer to a structure with presence points, absence points and coordinate system 
 * @param maps Pointer to a structure with the environmental layers 
 * @param algorithm Pointer to an algorithm data structure 
 * @param absences Pointer to a structure with the output specification
 * @param ticket Job identification.
 * @return standard gSOAP integer code
 */
//gsoap om service method-action: Create a spatial distribution model
//gsoap om service method-documentation: Create a spatial distribution model
int om__createModel(om__Points *points, om__Maps *maps, om__Mask *mask, om__Algorithm *algorithm, om__Output *output, xsd__string *ticket); 

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
//gsoap om service method-action: Return the distribution map as an attachment
//gsoap om service method-documentation: Given a ticket, return the corresponding distribution map.
int om__getDistributionMap(xsd__string ticket, xsd__base64Binary &file);

/** Simple method just to monitor the service.
 * @param _ A void* input parameter so that some compilers (notably vc++) will not complain 
 *          about empty structs. This SOAP method should be called without any parameters.
 * @return status Integer value (1 - OK)
 */
//gsoap om service method-action: Return 1 to indicate availability of the service
//gsoap om service method-documentation: Simple method that enables monitoring of service availability.
//int om__ping(void *_, xsd__int *status);

int om__ping(void *_, xsd__int *status); 
