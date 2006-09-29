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

/** Get all available algorithms and return their metadata in a SOAP structure.
 * @note This method is implemented in om_soap.cpp, where it receives a soap structure as
 *       an additional parameter. The definition here is used only to generate the SOAP 
 *       stub/skeleton.
 * @param _ A void* input parameter so that some compilers (notably vc++) will not complain 
 *          about empty structs. This SOAP method should be called without any parameters.
 * @param r Reference to the SOAP structure that will be returned.
 * @return standard gSOAP integer code
 */
//gsoap om service method-action: getAlgorithms ""
//gsoap om service method-documentation: getAlgorithms returns metadata about all available algorithms
int omws__getAlgorithms(XML &om__AvailableAlgorithms);



/** SOAP struct to store data about a point (presence or absence).
 */
typedef struct soap_Point
{
  @xsd__double latitude;
  @xsd__double longitude;

} omws__Point;

/** SOAP struct to store a collection of presence points  */
typedef struct soap_PresencePoints
{
  int __size;
  omws__Point *__ptrpoint;

} omws__PresencePoints;

/** SOAP struct to store a collection of absence points  */
typedef struct soap_AbsencePoints
{
  int __size;
  omws__Point *__ptrpoint;

} omws__AbsencePoints;

/** SOAP struct to store points data */
typedef struct soap_Points
{
  xsd__string         coordsystem;
  omws__PresencePoints *__ptrpresences;
  omws__AbsencePoints  *__ptrabsences;

} omws__Points;

/** SOAP struct to store data about a map (environmental layer).
 */
typedef struct soap_Map
{
  @xsd__string location;
  @xsd__int    categorical;

} omws__Map;

/** SOAP struct to store a collection of maps (environmental layers)  */
typedef struct soap_Maps
{
  int __size;
  omws__Map *__ptrmap;

} omws__Maps;

/** SOAP struct to store data about a mask (region of interest).
 */
typedef struct soap_Mask
{
  @xsd__string  location;

} omws__Mask;

/** SOAP struct to store a real algorithm parameter in a createModel request.
 */
typedef struct soap_Parameter
{
  @xsd__string Id;
  @xsd__string Value;

} omws__Parameter;

/** SOAP struct to store algorithm data in createModel requests.
 */
typedef struct soap_Algorithm
{
  @xsd__string Id;

  int __size;
  omws__Parameter *__ptrparameter;

} omws__Algorithm;

/** SOAP struct to store specification of the distribution map.
 */
typedef struct soap_Output
{
  xsd__string header;
  xsd__int    scale;
  xsd__string format;

} omws__Output;

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
int omws__createModel(omws__Points *points, omws__Maps *maps, omws__Mask *mask, omws__Algorithm *algorithm, omws__Output *output, xsd__string *ticket); 

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
int omws__getDistributionMap(xsd__string ticket, xsd__base64Binary &file);

/** Simple method just to monitor the service.
 * @param _ A void* input parameter so that some compilers (notably vc++) will not complain 
 *          about empty structs. This SOAP method should be called without any parameters.
 * @return status Integer value (1 - OK)
 */
//gsoap om service method-action: Return 1 to indicate availability of the service
//gsoap om service method-documentation: Simple method that enables monitoring of service availability.
//int omws__ping(void *_, xsd__int *status);

int omws__ping(void *_, xsd__int *status); 
