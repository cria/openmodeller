/**
 * openModeller SOAP interface.
 * 
 * @author Renato De Giovanni (renato [at] cria [dot] org [dot] br)
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2003 by CRIA -
 * Centro de Referencia em Informacao Ambiental
 *
 * http://www.cria.org.br
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details:
 * 
 * http://www.gnu.org/copyleft/gpl.html
 */

#include "om.nsmap"
#include "omwsH.h"
#include "omH.h"

#include <openmodeller/om.hh>
#include <openmodeller/FileParser.hh>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <algorithm>
using namespace std;

#include "omws_utils.hh"
#include "console/om_layer_utils.hh"

#define OMWS_VERSION "2.0"
#define OMWS_BACKLOG (100) // Max. request backlog 
#define OMWS_MIN *60
#define OMWS_H *3600
#define OMWS_TICKET_TEMPLATE "XXXXXX"
#define OMWS_CONFIG_FILE "../config/server.conf"
#define OMWS_WSDL_FILE "../config/openModeller.wsdl"
#define OMWS_LAYERS_CACHE_FILE "layers.xml"

/*****************************/
/***  Forward declarations ***/

static string   getServiceAddress();
static void*    processRequest( void* );
static string   getMapFile( string ticket );
static wchar_t* convertToWideChar( const char* p );
static void     logRequest( struct soap*, const char* operation, const char* params );
static void     addHeader( struct soap* );
static int      getStatus();
static void     createTicket( struct soap *soap, string requestPrefix, omws::xsd__string &ticket, string *requestFileName );
static void     scheduleJob( struct soap *soap, string requestPrefix, omws::XML xmlParameters, wchar_t* elementName, omws::xsd__string &ticket );
static void     scheduleJob( struct soap *soap, string requestPrefix, string xmlParameters, wchar_t* elementName, omws::xsd__string &ticket );
static map<string, string> scheduleExperiment( struct soap* soap, const om::_om__ExperimentParameters& ep, omws::xsd__string experiment_ticket );
static void     updateNextJobs( string next_id, string prev_id, map< string, vector<string> > *next_deps );
static string   collateTickets( vector<string>* ids, map<string, string> *jobs );
static string   collateTickets( map<string, string>* ids, map<string, string> *jobs );
static void     appendResult( string &ticket_dir, string &ticket, string &type, ostringstream &oss );

/****************/
/***  Globals ***/

FileParser gFileParser( OMWS_CONFIG_FILE ); // Config file parser

/***********************/
/*** main gSOAP code ***/

int main(int argc, char **argv)
{ 
  // Handle HTTP GET
  char * request_method = getenv( "REQUEST_METHOD" );

  if ( request_method != 0 ) {

    if ( strcmp(request_method, "HEAD") == 0 ) {

      printf("Content-type: text/xml\n\n");
      return 0;
    }
    else if ( strcmp(request_method, "GET") == 0 ) {

      char * query_string = getenv( "QUERY_STRING" );

      if ( query_string != 0 && strcmp(query_string, "wsdl") == 0 ) {

        // Display WSDL

        // read wsdl file
      
        fstream fin;
        fin.open( OMWS_WSDL_FILE, ios::in );

        if ( fin.is_open() ) {

          ostringstream oss;
          string line;

          while ( ! fin.eof() ) {
            getline( fin, line );
            oss << line << endl;
          }

          string wsdl = oss.str();

          // determine service address
          string service_address = getServiceAddress();

          // replace service address in wsdl
          string key("address location=");
          unsigned found = wsdl.rfind( key );
          unsigned found_closing_quote;
          if ( found != string::npos ) {

            found_closing_quote = wsdl.find( "\"", found+key.length()+1 );
            if ( found_closing_quote != string::npos ) {

              wsdl.replace( found+key.length()+1, found_closing_quote-(found+key.length())-1, service_address );
            }
          }

          printf("Content-type: text/xml\n\n");
          printf("%s", wsdl.c_str());
          fin.close();
        }
        else {

          printf("Status:404 Not found\n");
          printf("Content-Type: text/plain\n\n");
          printf("Resource not found\n");
        }

        return 0;
      } // endif wsdl request

      // Display HTML
      printf("Content-type: text/html\n\n");
      printf("<html><head><title>oM Server 2.0</title></head><body>\n");
      printf("<h1>oM Server</h1>\n");
      printf("<p>This is an <a href=\"http://openmodeller.sf.net\">openModeller</a> web service.</p>\n");
      printf("<p>You can find documentation about it <a href=\"http://openmodeller.sf.net/web_service_2.html\">here</a>.\n");
      printf("<p>To interact with the service you need a <a href=\"http://en.wikipedia.org/wiki/SOAP\">SOAP</a> client pointing to the service address.</p>\n");
      printf("</body></html>");
      return 0;
    } // endif GET
  } // endif REQUEST_METHOD check

  struct soap soap;
  soap_init(&soap);
  soap.encodingStyle = NULL;

  soap.accept_timeout = 0;  // always listening
  soap.send_timeout = 10 OMWS_H;
  soap.recv_timeout = 3 OMWS_MIN;

  // no args: assume this is a CGI application
  if ( argc < 2 ) { 

    omws::soap_serve( &soap );
    soap_destroy( &soap );
    soap_end( &soap );
    soap_done( &soap );
  }
  else { 

    int port = atoi( argv[1] ); // first command-line arg is port 

    int m, s, i; // master and slave sockets, and thread counter

    m = soap_bind( &soap, NULL, port, OMWS_BACKLOG );

    if ( m < 0 ) {

      soap_print_fault( &soap, stderr );
      exit(-1);
    }

    fprintf( stderr, "Socket connection successful: master socket = %d\n", m );

    int max_thr = 8; // max. number of threads to serve requests (default value)

    if ( argc > 2 ) {

      max_thr = atoi( argv[2] ); // second command-line arg is max. number of threads
    }

    // stand alone non-multi-threaded service
    if ( max_thr == 1 )	{

      for ( ; ; ) {

        s = soap_accept( &soap );
        fprintf( stderr, "Socket connection successful: slave socket = %d\n", s );

        if ( s < 0 ) { 

          soap_print_fault( &soap, stderr );
          exit(-1);
        } 

        omws::soap_serve( &soap );
        soap_destroy( &soap );
        soap_end( &soap );
      }
    }
    // stand alone multi-threaded service (pool of threads)
    else {

      struct soap *soap_thr[max_thr]; // each thread needs a runtime environment 
      pthread_t tid[max_thr]; 
	  
      for ( i = 0; i < max_thr; i++ ) {

        soap_thr[i] = NULL;
      }
	  
      for ( ; ; ) { 

        for ( i = 0; i < max_thr; i++ ) { 

          // Unix SIGPIPE, this is OS dependent:
          //soap.accept_flags = SO_NOSIGPIPE;    // some systems like this
          //soap.socket_flags = MSG_NOSIGNAL;    // others need this
          //signal(SIGPIPE, sigpipe_handle);     // or a sigpipe handler (portable)
		  
          s = soap_accept( &soap ); 
		  
          if ( s < 0 ) {

            if ( soap.errnum ) {

              soap_print_fault( &soap, stderr );
            }
            else {

              // Assume timeout is long enough for threads to complete serving requests 
              fprintf( stderr, "Server timed out\n" );
            }
		      
            break; 
          }
		  
          fprintf( stderr, "Thread %d accepts socket %d connection from IP %ld.%ld.%ld.%ld\n", i, s, (soap.ip >> 24)&0xFF, (soap.ip >> 16)&0xFF, (soap.ip >> 8)&0xFF, soap.ip&0xFF );
		  
          // first time around
          if ( !soap_thr[i] ) { 

            soap_thr[i] = soap_copy( &soap );
		      
            if ( ! soap_thr[i] ) {

              exit(1); // could not allocate 
            }
          }
          // recycle soap environment 
          else { 

            pthread_join( tid[i], NULL );
            fprintf( stderr, "Thread %d completed\n", i );
            soap_destroy( soap_thr[i] ); // deallocate C++ data of old thread 
            soap_end( soap_thr[i] ); // deallocate data of old thread 
          }
		  
          soap_thr[i]->socket = s; // new socket fd 
          //pthread_create(&tid[i], NULL, (void*(*)(void*))soap_serve, (void*)soap_thr[i]); 
          pthread_create( &tid[i], NULL, (void*(*)(void*))processRequest, (void*)soap_thr[i] ); 
        } 
      }
    }
  }

  return 0;
}

/***************************/
/**** getServiceAddress ****/
static string
getServiceAddress()
{ 
  string url("http");
  char * https = getenv( "HTTPS");
  if ( https != 0  ) {
    url.append("s");
  }
  url.append("://");
  char * server_name = getenv( "SERVER_NAME");
  url.append( server_name );
  url.append(":");
  char * port = getenv( "SERVER_PORT");
  url.append( port );
  char * script_name = getenv( "SCRIPT_NAME");
  if ( script_name != 0  ) {
    url.append(script_name);
  }
  char * path_info = getenv( "PATH_INFO");
  if ( path_info != 0  ) {
    url.append(path_info);
  }
  return url;
}

/*************************/
/**** Process request ****/
void *processRequest( void *soap )
{
  pthread_detach( pthread_self() );
  ((struct soap*)soap)->recv_timeout = 300; // Timeout after 5 minutes stall on recv
  ((struct soap*)soap)->send_timeout = 60; // Timeout after 1 minute stall on send
  omws::soap_serve( (struct soap*)soap );
  soap_destroy( (struct soap*)soap );
  soap_end( (struct soap*)soap );
  soap_done( (struct soap*)soap );
  free( soap );

  return NULL;
}

/**************/
/**** Ping ****/
int
omws::omws__ping( struct soap *soap, void *_, omws::xsd__int &status )
{
  logRequest( soap, "ping", "" );

  soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip

  addHeader( soap );

  // Check essential configuration
  string layersDir( gFileParser.get( "LAYERS_DIRECTORY" ) );

  if ( layersDir.empty() ) {

    return soap_receiver_fault( soap, "Missing configuration (1)", NULL );
  }

  string ticketDir( gFileParser.get( "TICKET_DIRECTORY" ) );

  if ( ticketDir.empty() ) {

    return soap_receiver_fault( soap, "Missing configuration (2)", NULL );
  }

  string baseUrl( gFileParser.get( "BASE_URL" ) );

  if ( baseUrl.empty() ) {

    return soap_receiver_fault( soap, "Missing configuration (3)", NULL );
  }

  string distMapDir( gFileParser.get( "DISTRIBUTION_MAP_DIRECTORY" ) );

  if ( distMapDir.empty() ) {

    return soap_receiver_fault( soap, "Missing configuration (4)", NULL );
  }

  // Check algorithm availability
  Log::instance()->setLevel( Log::Error ); // supress debug/info/warning
  AlgorithmFactory::searchDefaultDirs();   // load DLL algorithms
  OpenModeller om;
  AlgMetadata const **algorithms = om.availableAlgorithms();

  if ( ! *algorithms ) {

    delete[] algorithms;
    return soap_receiver_fault( soap, "No available algorithms", NULL );
  }
  delete[] algorithms;

  // TODO: test layer cache

  // TODO: at least one layer available

  status = getStatus();

  return SOAP_OK;
}

/**********************/
/*** get Algorithms ***/
int 
omws::omws__getAlgorithms( struct soap *soap, void *_, struct omws::omws__getAlgorithmsResponse *out )
{
  logRequest( soap, "getAlgorithms", "" );

  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  string enableCompression( gFileParser.get( "ENABLE_COMPRESSION" ) );

  if ( enableCompression == "yes" ) {
#ifdef WITH_GZIP
    // client supports gzip?
    if (soap->zlib_out == SOAP_ZLIB_GZIP) { 

      // compress response
      soap_set_omode(soap, SOAP_ENC_ZLIB);
    }
    else {

      soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip 
    }
#else
    soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip 
#endif
  }

  addHeader( soap );

  // Load algorithms
  Log::instance()->setLevel( Log::Error ); // supress debug/info/warning
  AlgorithmFactory::searchDefaultDirs();

  // Instantiate openModeller controller
  OpenModeller om;

  AlgMetadata const **algorithms = om.availableAlgorithms();

  if ( ! *algorithms ) {

    delete[] algorithms;
    return soap_receiver_fault( soap, "No available algorithms", NULL );
  }

  ConfigurationPtr cfg = AlgorithmFactory::getConfiguration();
  ostringstream oss;
  Configuration::writeXml( cfg, oss );

  out->om__AvailableAlgorithms = convertToWideChar( oss.str().c_str() ) ;

  delete[] algorithms;

  return SOAP_OK;
}

/**********************/
/*** get Layers ***/
int 
omws::omws__getLayers( struct soap *soap, void *_, struct omws::omws__getLayersResponse *out )
{
  logRequest( soap, "getLayers", "" );

  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  string enableCompression( gFileParser.get( "ENABLE_COMPRESSION" ) );

  if ( enableCompression == "yes" ) {

#ifdef WITH_GZIP
    // client supports gzip?
    if (soap->zlib_out == SOAP_ZLIB_GZIP) { 

      // compress response
      soap_set_omode(soap, SOAP_ENC_ZLIB);
    }
    else {

      soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip 
    }
#else
    soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip 
#endif
  }

  addHeader( soap );

  string cacheDir( gFileParser.get( "CACHE_DIRECTORY" ) );

  string layersFile( "" );

  // If cache directory is configured
  if ( ! cacheDir.empty() ) {

    // Append slash if necessary
    if ( cacheDir.find_last_of( "/" ) != cacheDir.size() - 1 ) {

      cacheDir.append( "/" );
    }

    // Cache file name with path
    layersFile.append( cacheDir );
    layersFile.append( OMWS_LAYERS_CACHE_FILE );

    if ( fileExists( layersFile.c_str() ) ) {

      size_t bufSize = 1024;

      // Note: working with wide charcters was causing problems (wchar_t* cast), 
      //       so all code was disabled.

      //wchar_t *buf = (wchar_t*)soap_malloc( soap, bufSize * sizeof( wchar_t ) );
      char *buf = (char*)soap_malloc( soap, bufSize * sizeof( char ) );

      //wstring cachedXml( L"" );
      string cachedXml( "" );

      // There's something cached, so return its content
      FILE *file = fopen( layersFile.c_str(), "r" );

      if ( file == NULL ) {

        // TODO: Does the client need to know about this?
        return soap_receiver_fault( soap, "Cache manipulation error (1)", NULL );
      }

      //while ( fgetws( buf, bufSize, file ) != (wchar_t *)NULL ) {
      while ( fgets( buf, bufSize, file ) != (char *)NULL ) {

        cachedXml.append( buf );
      }

      if ( ! feof( file ) ) {

        // TODO: Does the client need to know about this?
        return soap_receiver_fault( soap, "Cache manipulation error (2)", NULL );
      }

      //out->om__AvailableLayers = (wchar_t*)cachedXml.c_str();
      out->om__AvailableLayers = convertToWideChar( cachedXml.c_str() );

      fclose( file );

      return SOAP_OK;
    }
  }

  // Recurse on all sub directories searching for GDAL compatible layers
  ostringstream oss;

  int seq = 1;

  if ( ! readDirectory( gFileParser.get( "LAYERS_DIRECTORY" ).c_str(), 
                        gFileParser.get( "LAYERS_LABEL" ).c_str(), oss, 0, &seq ) ) {

    return soap_receiver_fault( soap, "Could not read available layers", NULL );
  }

  out->om__AvailableLayers = convertToWideChar( oss.str().c_str() ) ;

  // If cache is configured
  if ( ! cacheDir.empty() ) {

    // Cache result for the next requests
    FILE *file = fopen( layersFile.c_str(), "w" );

    if ( file == NULL ) {

      return soap_receiver_fault( soap, "Cache manipulation error (3)", NULL );
    }

    //if ( fputws( out->om__AvailableLayers, file ) < 0 ) {
    if ( fputs( oss.str().c_str(), file ) < 0 ) {

      // TODO: Does the client need to know about this?
      return soap_receiver_fault( soap, "Cache manipulation error (4)", NULL );
    }

    fclose( file );
  }

  return SOAP_OK;
}

/**********************/
/**** create Model ****/
int 
omws::omws__createModel( struct soap *soap, omws::XML om__ModelParameters, omws::xsd__string &ticket )
{
  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip

  try {

    wchar_t elementName[] = L"ModelParameters";
    scheduleJob( soap, OMWS_MODEL _REQUEST, om__ModelParameters, elementName, ticket );
    logRequest( soap, "createModel", ticket.c_str() );
  }
  catch (OmwsException& e) {

    return soap_receiver_fault( soap, e.what(), NULL );
  }
  catch (...) {

    return soap_receiver_fault( soap, "Failed to process request", NULL );
  }

  return SOAP_OK;
}

/********************/
/**** test Model ****/
int 
omws::omws__testModel( struct soap *soap, omws::XML om__TestParameters, omws::xsd__string &ticket )
{
  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip

  try {

    wchar_t elementName[] = L"TestParameters";
    scheduleJob( soap, OMWS_TEST _REQUEST, om__TestParameters, elementName, ticket );
    logRequest( soap, "testModel", ticket.c_str() );
  }
  catch (OmwsException& e) {

    return soap_receiver_fault( soap, e.what(), NULL );
  }
  catch (...) {

    return soap_receiver_fault( soap, "Failed to process request", NULL );
  }

  return SOAP_OK;
}

/***********************/
/**** project Model ****/
int 
omws::omws__projectModel( struct soap *soap, omws::XML om__ProjectionParameters, omws::xsd__string &ticket )
{
  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip

  try {

    wchar_t elementName[] = L"ProjectionParameters";
    scheduleJob( soap, OMWS_PROJECTION _REQUEST, om__ProjectionParameters, elementName, ticket );
    logRequest( soap, "projectModel", ticket.c_str() );
  }
  catch (OmwsException& e) {

    return soap_receiver_fault( soap, e.what(), NULL );
  }
  catch (...) {

    return soap_receiver_fault( soap, "Failed to process request", NULL );
  }

  return SOAP_OK;
}

/************************/
/**** evaluate Model ****/
int 
omws::omws__evaluateModel( struct soap *soap, omws::XML om__ModelEvaluationParameters, omws::xsd__string &ticket )
{
  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip

  try {

    wchar_t elementName[] = L"ModelEvaluationParameters";
    scheduleJob( soap, OMWS_EVALUATE _REQUEST, om__ModelEvaluationParameters, elementName, ticket );
    logRequest( soap, "evaluateModel", ticket.c_str() );
  }
  catch (OmwsException& e) {

    return soap_receiver_fault( soap, e.what(), NULL );
  }
  catch (...) {

    return soap_receiver_fault( soap, "Failed to process request", NULL );
  }

  return SOAP_OK;
}

/***********************/
/**** sample Points ****/
int 
omws::omws__samplePoints( struct soap *soap, omws::XML om__SamplingParameters, omws::xsd__string &ticket )
{
  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip

  try {

    wchar_t elementName[] = L"SamplingParameters";
    scheduleJob( soap, OMWS_SAMPLING _REQUEST, om__SamplingParameters, elementName, ticket );
    logRequest( soap, "samplePoints", ticket.c_str() );
  }
  catch (OmwsException& e) {

    return soap_receiver_fault( soap, e.what(), NULL );
  }
  catch (...) {

    return soap_receiver_fault( soap, "Failed to process request", NULL );
  }

  return SOAP_OK;
}

/************************/
/**** run Experiment ****/
int
omws::omws__runExperiment( struct soap *soap, omws::XML_ om__ExperimentParameters, struct omws::omws__runExperimentResponse *out )
{
  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip

  // New soap context to parse experiment parameters manually
  struct soap *ctx = soap_new1(SOAP_XML_STRICT);
  soap_init(ctx);
  soap_imode(ctx, SOAP_ENC_XML); // Set input mode
  soap_imode(ctx, SOAP_XML_IGNORENS);
  soap_begin(ctx); // start new (de)serialization phase

  try {

    // Parse content
    string params( "<ExperimentParameteres>" );
    params.append(om__ExperimentParameters).append("</ExperimentParameters>");
    istringstream iss( params );
    ctx->is = &iss;

    om::_om__ExperimentParameters ep;

    if ( soap_read_om__ExperimentParametersType( ctx, &ep ) != SOAP_OK ) {

      throw OmwsException("Failed to parse experiment parameters");
    }
    else {

      // Store the request and create a ticket anyway, regardless using Condor DAGMan or not
      omws::xsd__string ticket;
      wchar_t elementName[] = L"ExperimentParameters";
      scheduleJob( soap, OMWS_EXPERIMENT _PENDING_REQUEST, params, elementName, ticket );
      logRequest( soap, "runExperiment", ticket.c_str() );

      string result("");

      // There's no experiment id anymore (removed from protocol)
      //string exp_id = string( ep.Jobs.id ); // experiment id provided by client
      //string result( "<Job Id=\"" );
      //result.append( exp_id ).append("\" Ticket=\"" ).append( ticket ).append( "\"/>" );

      // Create individual jobs and return all of them
      map<string, string> jobs = scheduleExperiment( ctx, ep, ticket );

      for ( map<string, string>::const_iterator it = jobs.begin(); it != jobs.end(); ++it ) {

        result.append( "<Job Id=\"" ).append( (*it).first ).append("\" Ticket=\"" ).append( (*it).second ).append( "\"/>" );
      }

      out->om__ExperimentTickets = convertToWideChar( result.c_str() ); 
    }

    soap_destroy(ctx); // remove deserialized class instances (C++ objects)
    soap_end(ctx);  // clean up and remove deserialized data
    soap_done(ctx); // detach context (last use and no longer in scope)
  }
  catch (OmwsException& e) {

    soap_destroy(ctx);
    soap_end(ctx);
    soap_done(ctx);
    return soap_receiver_fault( soap, e.what(), NULL );
  }
  catch (...) {

    soap_destroy(ctx);
    soap_end(ctx);
    soap_done(ctx);
    return soap_receiver_fault( soap, "Failed to process request", NULL );
  }

  return SOAP_OK;
}

/**********************/
/**** get Progress ****/
int 
omws::omws__getProgress( struct soap *soap, omws::xsd__string tickets, omws::xsd__string &progress )
{ 
  logRequest( soap, "getProgress", tickets.c_str() );

  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip

  if ( tickets.length() == 0 ) {

    return soap_sender_fault( soap, "Missing ticket(s) in request", NULL );
  }

  // Split tokens and process each one
  string ticket_dir = gFileParser.get( "TICKET_DIRECTORY" );
  stringstream ss( tickets );
  stringstream res( "" );
  string ticket;
  bool notFirst = false;
  while ( getline( ss, ticket, ',' ) ) {

    try {

      if ( notFirst ) {

        res << ",";
      }
      else {

        notFirst = true;
      }

      res << getProgress( ticket_dir, ticket );
    }
    catch (OmwsException& e) {

      return soap_receiver_fault( soap, e.what(), NULL );
    }
    catch (...) {

      return soap_receiver_fault( soap, "Failed to process request", NULL );
    }
  }

  progress = res.str();

  return SOAP_OK;
}

/*****************/
/**** get Log ****/
int 
omws::omws__getLog( struct soap *soap, omws::xsd__string ticket, omws::xsd__string &log )
{ 
  logRequest( soap, "getLog", ticket.c_str() );

  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip

  if ( ticket.length() == 0 ) {

    return soap_sender_fault( soap, "Missing ticket in request", NULL );
  }

  string fileName = getTicketFilePath( gFileParser.get( "TICKET_DIRECTORY" ), "", ticket );

  fstream fin;
  fin.open( fileName.c_str(), ios::in );

  if ( fin.is_open() ) {

    ostringstream oss;
    string line;

    // If file exists, read the log and return it
    while ( ! fin.eof() )
    {
      getline( fin, line );
      oss << line << endl;
    }

    log = oss.str();

    fin.close();
  }
  else {

    return soap_receiver_fault( soap, "Log unavailable", NULL );
  }

  return SOAP_OK;
}

/****************/
/**** cancel ****/
int
omws::omws__cancel( struct soap *soap, omws::xsd__string tickets, omws::xsd__string &cancelledTickets )
{
  logRequest( soap, "cancel", tickets.c_str() );

  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip

  if ( tickets.length() == 0 ) {

    return soap_sender_fault( soap, "Missing ticket(s) in request", NULL );
  }

  // Split tokens and process each one
  string ticket_dir = gFileParser.get( "TICKET_DIRECTORY" );
  stringstream ss( tickets );
  stringstream res( "" );
  string ticket;
  bool not_first = false;
  while ( getline( ss, ticket, ',' ) ) {

    try {

     if ( cancelJob( ticket_dir, ticket ) ) {

        if ( not_first ) {

          res << ",";
        }
        else {

          not_first = true;
        }

        res << ticket;
      }
    }
    catch (OmwsException& e) {

      return soap_receiver_fault( soap, e.what(), NULL );
    }
    catch (...) {

      return soap_receiver_fault( soap, "Failed to process request", NULL );
    }
  }

  cancelledTickets = res.str();

  return SOAP_OK;
}

/*******************/
/**** get Model ****/
int 
omws::omws__getModel( struct soap *soap, omws::xsd__string ticket, struct omws::omws__getModelResponse *out )
{ 
  logRequest( soap, "getModel", ticket.c_str() );

  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  string enableCompression( gFileParser.get( "ENABLE_COMPRESSION" ) );

  if ( enableCompression == "yes" ) {

#ifdef WITH_GZIP
    // client supports gzip?
    if (soap->zlib_out == SOAP_ZLIB_GZIP) { 

      // compress response
      soap_set_omode(soap, SOAP_ENC_ZLIB);
    }
    else {

      soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip 
    }
#else
    soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip 
#endif
  }

  if ( ticket.length() == 0 ) {

    return soap_sender_fault( soap, "Missing ticket in request", NULL );
  }

  string fileName = getTicketFilePath( gFileParser.get( "TICKET_DIRECTORY" ), OMWS_MODEL _RESPONSE, ticket );

  fstream fin;
  fin.open( fileName.c_str(), ios::in );

  if ( fin.is_open() ) {

    ostringstream oss;
    string line;

    while ( ! fin.eof() )
    {
      getline( fin, line );
      oss << line << endl;
    }

    out->om__ModelEnvelope = convertToWideChar( oss.str().c_str() );

    fin.close();
  }
  else {

    return soap_receiver_fault( soap, "Model unavailable", NULL );
  }

  return SOAP_OK;
}

/*************************/
/**** get Test Result ****/
int 
omws::omws__getTestResult( struct soap *soap, omws::xsd__string ticket, struct omws::omws__testResponse *out )
{ 
  logRequest( soap, "getTestResult", ticket.c_str() );

  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip

  if ( ticket.length() == 0 ) {

    return soap_sender_fault( soap, "Missing ticket in request", NULL );
  }

  string fileName = getTicketFilePath( gFileParser.get( "TICKET_DIRECTORY" ), OMWS_TEST _RESPONSE, ticket );

  fstream fin;
  fin.open( fileName.c_str(), ios::in );

  if ( fin.is_open() ) {

    ostringstream oss;
    string line;

    while ( ! fin.eof() )
    {
      getline( fin, line );
      oss << line << endl;
    }

    out->om__TestResultEnvelope = convertToWideChar( oss.str().c_str() );

    fin.close();
  }
  else {

    return soap_receiver_fault( soap, "Test result unavailable", NULL );
  }

  return SOAP_OK;
}

/**************************/
/**** get Layer As URL ****/
int 
omws::omws__getLayerAsUrl( struct soap *soap, omws::xsd__string id, omws::xsd__string &url )
{ 
  logRequest( soap, "getLayerAsUrl", id.c_str() );

  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip

  // This method is now working only for distribution maps!

  if ( id.length() == 0 ) {

    return soap_sender_fault(soap, "Missing id in request", NULL);
  }

  string fileName = getMapFile( id );

  if ( fileName.empty() ) {

    return soap_receiver_fault( soap, "Layer unavailable", NULL );
  }

  url = gFileParser.get( "BASE_URL" );

  if ( url.find_last_of( "/" ) != url.size() - 1 ) {

    url.append( "/" );
  }

  url.append( fileName );

  return SOAP_OK;
}

/*****************************/
/**** get Projection Data ****/
int 
omws::omws__getProjectionMetadata( struct soap *soap, omws::xsd__string ticket, struct omws::omws__getProjectionMetadataResponse *out )
{ 
  logRequest( soap, "getProjectionMetadata", ticket.c_str() );

  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip

  if ( ticket.length() == 0 ) {

    return soap_sender_fault( soap, "Missing ticket in request", NULL );
  }

  // Get map size
  string mapFileName = getMapFile( ticket );

  if ( mapFileName.empty() ) {

    return soap_receiver_fault( soap, "Projection unavailable", NULL );
  }

  string completeFileName( gFileParser.get( "DISTRIBUTION_MAP_DIRECTORY" ) );

  if ( completeFileName.find_last_of( "/" ) != completeFileName.size() - 1 ) {

    // Append slash if necessary
    completeFileName.append( "/" );
  }

  completeFileName.append( mapFileName );

  FILE *fd = fopen( completeFileName.c_str(), "rb" );

  if ( fd == NULL ) {

    return soap_receiver_fault( soap, "Projection unreadable", NULL );
  }

  // Get statistics
  string statsFileName = getTicketFilePath( gFileParser.get( "TICKET_DIRECTORY" ), OMWS_PROJECTION_STATISTICS_PREFIX, ticket );

  fstream fin;
  fin.open( statsFileName.c_str(), ios::in );

  if ( fin.is_open() ) {

    ostringstream oss;
    string line;

    while ( ! fin.eof() )
    {
      getline( fin, line );
      oss << line << endl;
    }

    out->om__ProjectionEnvelope = convertToWideChar( oss.str().c_str() );

    fin.close();
  }
  else {

    return soap_receiver_fault( soap, "Metadata unavailable", NULL );
  }

  return SOAP_OK;
}

/******************************/
/**** get Model Evaluation ****/
int 
omws::omws__getModelEvaluation( struct soap *soap, omws::xsd__string ticket, struct omws::omws__modelEvaluationResponse *out )
{ 
  logRequest( soap, "getModelEvaluation", ticket.c_str() );

  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  string enableCompression( gFileParser.get( "ENABLE_COMPRESSION" ) );

  if ( enableCompression == "yes" ) {

#ifdef WITH_GZIP
    // client supports gzip?
    if (soap->zlib_out == SOAP_ZLIB_GZIP) { 

      // compress response
      soap_set_omode(soap, SOAP_ENC_ZLIB);
    }
    else {

      soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip 
    }
#else
    soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip 
#endif
  }

  if ( ticket.length() == 0 ) {

    return soap_sender_fault( soap, "Missing ticket in request", NULL );
  }

  string fileName = getTicketFilePath( gFileParser.get( "TICKET_DIRECTORY" ), OMWS_EVALUATE _RESPONSE, ticket );

  fstream fin;
  fin.open( fileName.c_str(), ios::in );

  if ( fin.is_open() ) {

    ostringstream oss;
    string line;

    while ( ! fin.eof() )
    {
      getline( fin, line );
      oss << line << endl;
    }

    out->om__ModelEvaluation = convertToWideChar( oss.str().c_str() );

    fin.close();
  }
  else {

    return soap_receiver_fault( soap, "Result unavailable", NULL );
  }

  return SOAP_OK;
}

/*****************************/
/**** get Sampling Result ****/
int 
omws::omws__getSamplingResult( struct soap *soap, omws::xsd__string ticket, struct omws::omws__getSamplingResultResponse *out )
{ 
  logRequest( soap, "getSamplingResult", ticket.c_str() );

  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  string enableCompression( gFileParser.get( "ENABLE_COMPRESSION" ) );

  if ( enableCompression == "yes" ) {

#ifdef WITH_GZIP
    // client supports gzip?
    if (soap->zlib_out == SOAP_ZLIB_GZIP) { 

      // compress response
      soap_set_omode(soap, SOAP_ENC_ZLIB);
    }
    else {

      soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip 
    }
#else
    soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip 
#endif
  }

  if ( ticket.length() == 0 ) {

    return soap_sender_fault( soap, "Missing ticket in request", NULL );
  }

  string fileName = getTicketFilePath( gFileParser.get( "TICKET_DIRECTORY" ), OMWS_SAMPLING _RESPONSE, ticket );

  fstream fin;
  fin.open( fileName.c_str(), ios::in );

  if ( fin.is_open() ) {

    ostringstream oss;
    string line;

    while ( ! fin.eof() )
    {
      getline( fin, line );
      oss << line << endl;
    }

    out->om__Sampler = convertToWideChar( oss.str().c_str() );

    fin.close();
  }
  else {

    return soap_receiver_fault( soap, "Result unavailable", NULL );
  }

  return SOAP_OK;
}
/*********************/
/**** get Results ****/
int 
omws::omws__getResults( struct soap *soap, omws::xsd__string tickets, struct omws::omws__getResultsResponse *out )
{ 
  logRequest( soap, "getResults", tickets.c_str() );

  if ( getStatus() == 2 ) {

    return soap_receiver_fault( soap, "Service unavailable", NULL );
  }

  soap_clr_omode(soap, SOAP_ENC_ZLIB); // disable Zlib's gzip

  if ( tickets.length() == 0 ) {

    return soap_sender_fault( soap, "Missing ticket(s) in request", NULL );
  }

  // Split tokens and process each one
  string ticket_dir = gFileParser.get( "TICKET_DIRECTORY" );
  stringstream ss( tickets );
  ostringstream oss;

  string ticket;
  bool has_result = false;
  while ( getline( ss, ticket, ',' ) ) {

    try {

      string metadata_file = ticket_dir + OMWS_JOB_METADATA_PREFIX + ticket;

      FileParser data( metadata_file );

      string type = data.get("TYPE");

      if ( type.compare(0, 3, OMWS_EXPERIMENT) == 0 ) {

        FileParser exp_data( ticket_dir + OMWS_JOB_METADATA_PREFIX + ticket );

        string all_jobs = exp_data.get( "JOBS" );

        vector<string> all_tickets = getTickets( all_jobs );

        for ( vector<string>::iterator at = all_tickets.begin(); at != all_tickets.end(); ++at ) {

          if ( getProgress( ticket_dir, (*at) ) == 100 ) {

            has_result = true;

            appendResult( ticket_dir, (*at), type, oss );
          }
        }
      }
      else {

        if ( getProgress( ticket_dir, ticket ) == 100 ) {

          has_result = true;

          appendResult( ticket_dir, ticket, type, oss );
        }
      }
    }
    catch (OmwsException& e) {

      return soap_receiver_fault( soap, e.what(), NULL );
    }
    catch (...) {

      return soap_receiver_fault( soap, "Failed to process request", NULL );
    }
  }

  if ( ! has_result ) {

    return soap_receiver_fault( soap, "No results", NULL );
  }

  out->om__ResultSet = convertToWideChar( oss.str().c_str() );

  return SOAP_OK;
}

////////////////////////////////////////////////////
//                                                //
//                Helper functions                //
//                                                //
////////////////////////////////////////////////////


/********************/
/**** getMapFile ****/
static string
getMapFile( string ticket )
{ 
  string path( gFileParser.get( "DISTRIBUTION_MAP_DIRECTORY" ) );

  // Append slash if necessary
  if ( path.find_last_of( "/" ) != path.size() - 1 ) {

    path.append( "/" );
  }

  // IMG
  string fileName( ticket );
  fileName.append( ".img" );

  string filePath( path );
  filePath.append( fileName );

  if ( fileExists( filePath.c_str() ) ) {

    return fileName;
  }

  // TIF
  fileName = ticket;
  fileName.append( ".tif" );

  filePath = path;
  filePath.append( fileName );

  if ( fileExists( filePath.c_str() ) ) {

    return fileName;
  }

  // ASC
  fileName = ticket;
  fileName.append( ".asc" );

  filePath = path;
  filePath.append( fileName );

  if ( fileExists( filePath.c_str() ) ) {

    return fileName;
  }

  // BMP
  fileName = ticket;
  fileName.append( ".bmp" );

  filePath = path;
  filePath.append( fileName );

  if ( fileExists( filePath.c_str() ) ) {

    return fileName;
  }

  return "";
}

/***************************/
/**** convertToWideChar ****/
static wchar_t* 
convertToWideChar( const char* p )
{
  wchar_t *r;

  r = new wchar_t[strlen(p)+1];

  const char *tempsource = p;

  wchar_t *tempdest = r;

  while (( *tempdest++ = (unsigned char)*tempsource++ ));

  return r;
}

/********************/
/**** logRequest ****/
static void
logRequest( struct soap* soap, const char* operation, const char* params )
{
  string logFile( gFileParser.get( "LOG_DIRECTORY" ) );

  if ( logFile.find_last_of( "/" ) != logFile.size() - 1 ) {

    logFile.append( "/" );
  }

  time_t mytime = time((time_t *)NULL);
  struct tm * mytm = localtime(&mytime);

  int year = mytm->tm_year + 1900;

  ostringstream name;

  name << year << "-";

  int month = mytm->tm_mon + 1;

  if ( month < 10 ) {

    name << "0";
  }

  name << month;

  logFile.append( name.str() );
  logFile.append( ".log" );

  FILE *file = fopen( logFile.c_str(), "a" );

  if ( file == NULL ) {

    return;
  }

  string ip("");
  char * remote_addr = getenv( "REMOTE_ADDR" );

  // Check if the environment variable is set
  if ( remote_addr != 0 ) {

    ip = (char const *)remote_addr;
  }

  char strtime[30];
  strftime( strtime, 30, "%Y-%m-%d %X %Z", mytm );

  ostringstream log;

  // IP TAB datetime TAB operation
  log << ip.c_str() << "\t" << strtime << "\t" << operation << "\t" << params << endl;
 
  if ( fputs( log.str().c_str(), file ) < 0 ) {

    return;
  }

  fclose( file );
}

/*******************/
/**** addHeader ****/
static void
addHeader( struct soap* soap )
{
  // Instantiate openModeller controller
  OpenModeller om;

  // alloc new header
  soap->header = (struct SOAP_ENV__Header*)soap_malloc( soap, sizeof(struct SOAP_ENV__Header) );
  string version( "oM Server " ); // Default server implementation name
  version.append( OMWS_VERSION ); // Control version on top of this file
  version.append( " (openModeller " ); // Add om version
  version.append( om.getVersion() );
  version.append( ")" );
  char *version_buf = (char*)soap_malloc( soap, version.size() * sizeof( char ) );
  strcpy( version_buf, version.c_str() );
  soap->header->omws__version = version_buf; 
}

/*******************/
/**** getStatus ****/
static int
getStatus()
{ 
  string sysStatus( gFileParser.get( "SYSTEM_STATUS" ) );

  if ( sysStatus.empty() ) {

    return 1; // Default value
  }
  else {

    return atoi( sysStatus.c_str() );
  }
}

/**********************/
/**** createTicket ****/
static void
createTicket( struct soap *soap, string requestPrefix, omws::xsd__string &ticket, string *requestFileName )
{
  string ticketFileName( gFileParser.get( "TICKET_DIRECTORY" ) );

  // Append slash if necessary
  if ( ticketFileName.find_last_of( "/" ) != ticketFileName.size() - 1 ) {

    ticketFileName.append( "/" );
  }

  // Job metadata file
  string mfile_name( ticketFileName );

  // Copy name to future request file name
  requestFileName->append( ticketFileName );

  // Append ticket template
  ticketFileName.append( OMWS_TICKET_TEMPLATE );

  // Temporary variable to generate ticket and copy its value
  char *tempFileName = (char*)soap_malloc( soap, ticketFileName.length() +1 );

  strcpy( tempFileName, ticketFileName.c_str() );

  // Generate unique ticket file
  int fd = mkstemp( tempFileName );

  if ( fd == -1 ) {

    throw OmwsException("Failed to create ticket (1)");
  }

  char * cticket = strrchr( tempFileName, '/' ) + 1;

  ticket = string( cticket );

  // Append prefix to request file
  requestFileName->append( requestPrefix );

  // Append ticket to request file
  requestFileName->append( ticket );

  // Create metadata file
  mfile_name.append( OMWS_JOB_METADATA_PREFIX ).append( ticket );

  FILE *mfile = fopen( mfile_name.c_str(), "w" );

  string job_type_line = "TYPE=";

  if ( requestPrefix.compare(0, 5, OMWS_MODEL) == 0 ) {
    job_type_line.append(OMWS_MODEL);
  }
  else if ( requestPrefix.compare(0, 4, OMWS_TEST) == 0 ) {
    job_type_line.append(OMWS_TEST);
  }
  else if ( requestPrefix.compare(0, 4, OMWS_PROJECTION) == 0 ) {
    job_type_line.append(OMWS_PROJECTION);
  }
  else if ( requestPrefix.compare(0, 4, OMWS_SAMPLING) == 0 ) {
    job_type_line.append(OMWS_SAMPLING);
  }
  else if ( requestPrefix.compare(0, 4, OMWS_EVALUATE) == 0 ) {
    job_type_line.append(OMWS_EVALUATE);
  }
  else if ( requestPrefix.compare(0, 3, OMWS_EXPERIMENT) == 0 ) {
    job_type_line.append(OMWS_EXPERIMENT);
  }
  else {
    fclose( mfile );
    throw OmwsException("Unknown job prefix");
  }

  job_type_line.append("\n");

  if ( mfile == NULL || fputs( job_type_line.c_str(), mfile ) < 0 ) {

    fclose( mfile );
    throw OmwsException("Failed to create metadata file");
  }

  fclose( mfile );
}

/*********************/
/**** scheduleJob ****/
static void
scheduleJob( struct soap *soap, string requestPrefix, omws::XML xmlParameters, wchar_t* elementName, omws::xsd__string &ticket )
{
  string requestFileName = "";

  createTicket( soap, requestPrefix, ticket, &requestFileName );

  // Create and open request file - at this point there should be no file with the same name!
  FILE *file = fopen( requestFileName.c_str(), "w" );

  if ( file == NULL ) {

    throw OmwsException("Failed to create ticket (2)");
  }

  // Add wrapper element
  if ( fputws(L"<", file) < 0 || fputws(elementName, file) < 0 || fputws(L">", file) < 0 ) {

    fclose( file );
    throw OmwsException("Failed to create ticket (3)");
  }

  // Put content of model request there
  if ( fputws( xmlParameters, file ) < 0 ) {

    fclose( file );
    throw OmwsException("Failed to create ticket (4)");
  }

  // Close wrapper element
  if ( fputws(L"</", file) < 0 || fputws(elementName, file) < 0 || fputws(L">", file) < 0 ) {

    fclose( file );
    throw OmwsException("Failed to create ticket (5)");
  }

  fclose( file );
}

/*********************/
/**** scheduleJob ****/
static void
scheduleJob( struct soap *soap, string requestPrefix, string xmlParameters, wchar_t* elementName, omws::xsd__string &ticket )
{
  omws::XML wideXmlParameters = convertToWideChar( xmlParameters.c_str() );
  scheduleJob( soap, requestPrefix, wideXmlParameters, elementName, ticket );
  delete[] wideXmlParameters;
}

/****************************/
/**** scheduleExperiment ****/
static map<string, string> 
scheduleExperiment(struct soap* soap, const om::_om__ExperimentParameters& ep, omws::xsd__string experiment_ticket) {

  soap_set_omode(soap, SOAP_XML_CANONICAL);
  soap_set_omode(soap, SOAP_XML_INDENT);
  soap_set_omode(soap, SOAP_XML_TREE);
  soap_set_omode(soap, SOAP_XML_NOTYPE);

  // Read Environments provided
  map<string, om::om__EnvironmentType> environments;

  vector<om::_om__ExperimentParametersType_Environment>::const_iterator it = ep.Environment.begin();
  for ( ; it != ep.Environment.end(); it++ ) {

    string env_id = string( (*it).id );

    om::om__EnvironmentType env;
    env.NumLayers = (*it).NumLayers;
    env.Map = (*it).Map;
    env.Mask = (*it).Mask;

    environments.insert( pair<string, om::om__EnvironmentType>( env_id, env ) );
  }

  // Read Presences provided
  map<string, om::om__OccurrencesType> presences;

  if ( ep.Presence != 0 ) {

    vector<om::_om__ExperimentParametersType_Presence>::const_iterator it = ep.Presence->begin();
    for ( ; it != ep.Presence->end(); it++ ) {

      string pre_id = string( (*it).id );

      om::om__OccurrencesType pre;
      pre.Count = (*it).Count;
      pre.Label = (*it).Label;
      pre.CoordinateSystem = (*it).CoordinateSystem;

      vector<om::_om__OccurrencesType_Point> points;
      vector<om::_om__ExperimentParametersType_Presence_Point>::const_iterator pit = (*it).Point.begin();
      for ( ; pit != (*it).Point.end(); pit++ ) {

        om::_om__OccurrencesType_Point point;
        point.Id = (*pit).Id;
        point.X = (*pit).X;
        point.Y = (*pit).Y;
        point.Sample = (*pit).Sample;
        points.push_back( point );
      }
      pre.Point = points;

      presences.insert( pair<string, om::om__OccurrencesType>( pre_id, pre ) );
    }
  }

  // Read Absences provided
  map<string, om::om__OccurrencesType> absences;

  if ( ep.Absence != 0 ) {

    vector<om::_om__ExperimentParametersType_Absence>::const_iterator it = ep.Absence->begin();
    for ( ; it != ep.Absence->end(); it++ ) {

      string abs_id = string( (*it).id );

      om::om__OccurrencesType abs;
      abs.Count = (*it).Count;
      abs.Label = (*it).Label;
      abs.CoordinateSystem = (*it).CoordinateSystem;

      vector<om::_om__OccurrencesType_Point> points;
      vector<om::_om__ExperimentParametersType_Absence_Point>::const_iterator pit = (*it).Point.begin();
      for ( ; pit != (*it).Point.end(); pit++ ) {

        om::_om__OccurrencesType_Point point;
        point.Id = (*pit).Id;
        point.X = (*pit).X;
        point.Y = (*pit).Y;
        point.Sample = (*pit).Sample;
        points.push_back( point );
      }
      abs.Point = points;

      absences.insert( pair<string, om::om__OccurrencesType>( abs_id, abs ) );
    }
  }

  // Read Algorithms provided
  map<string, om::om__BasicAlgorithmDefinitionType> algorithms;

  if ( ep.AlgorithmSettings != 0 ) {

    vector<om::_om__ExperimentParametersType_AlgorithmSettings>::const_iterator it = ep.AlgorithmSettings->begin();
    for ( ; it != ep.AlgorithmSettings->end(); it++ ) {

      string alg_id = string( (*it).id );

      om::om__BasicAlgorithmDefinitionType alg = *(*it).Algorithm;

      algorithms.insert( pair<string, om::om__BasicAlgorithmDefinitionType>( alg_id, alg ) );
    }
  }

  // Read Models provided
  map<string, om::om__SerializedAlgorithmType> models;

  if ( ep.SerializedAlgorithm != 0 ) {

    vector<om::_om__ExperimentParametersType_SerializedAlgorithm>::const_iterator it = ep.SerializedAlgorithm->begin();
    for ( ; it != ep.SerializedAlgorithm->end(); it++ ) {

      string model_id = string( (*it).id );

      om::om__SerializedAlgorithmType model = *(*it).Algorithm;

      models.insert( pair<string, om::om__SerializedAlgorithmType>( model_id, model ) );
    }
  }

  // Process jobs
  int num_jobs = ep.Jobs.__sizeAbstractJob;

  if ( num_jobs == 0 ) {

    throw OmwsException( "Could not find any jobs after parsing request" );
  } 

  map<string, string> jobs; // job id -> job ticket
  map< string, map<string, string> > prev_deps; // job id -> map of previous job ids/usage (dependencies)
  map< string, vector<string> > next_deps; // job id -> vector of next job ids (dependencies)
  vector<string> files_to_be_renamed;
  map<string, string> created_jobs; // job ticket -> job type (used to delete files in case of exception)

  string ticket_dir( gFileParser.get( "TICKET_DIRECTORY" ) );
  if ( ticket_dir.find_last_of( "/" ) != ticket_dir.size() - 1 ) {

    ticket_dir.append( "/" );
  }

  try {

    string job_id;
    omws::xsd__string ticket;

    for ( int i=0; i < num_jobs; i++ ) {

      om::__om__union_ExperimentParametersType_Jobs job = ep.Jobs.__union_ExperimentParametersType_Jobs[i];

      job_id = "";
      map<string, string> depends_on; // job id -> result usage for it (model, presence, absence, lpt)

      // SamplingJobs
      if ( job.__unionAbstractJob == 1 ) {

        om::om__SamplingJobType * sampling_job = job.__union_ExperimentParametersType_Jobs.SamplingJob;

        job_id = string( sampling_job->id );

        om::_om__SamplingParameters sp;

        string env_ref = string( sampling_job->EnvironmentRef->idref );
        if ( environments.count( env_ref ) > 0 ) {

          sp.Environment = &environments[env_ref];
        }
        else {

          string msg = "No Environment found for reference ";
          msg.append( env_ref ).append( " on job " ).append( job_id );
          throw OmwsException( msg.c_str() );
        }

        sp.Options = sampling_job->Options;

        string requestFileName = "";
        createTicket( soap, OMWS_SAMPLING _PENDING_REQUEST, ticket, &requestFileName );
        created_jobs.insert( pair<string, string>( string( ticket ), OMWS_SAMPLING ) );
        ofstream fstreamOUT( requestFileName.c_str() );
        soap->os = &fstreamOUT;
        files_to_be_renamed.push_back( requestFileName );
 
        // The following line reproduces the same encapsulated call used by 
        // soap_write_om__SamplingParametersType, but here we need a different element name, 
        // that's why soap_write is not used directly
        if ( ( sp.soap_serialize(soap), soap_begin_send(soap) || sp.soap_put(soap, "om:SamplingParameters", NULL) || soap_end_send(soap), soap->error ) != SOAP_OK ) {

          string msg = "Failed to write sampling points job with id ";
          msg.append( job_id );
          throw OmwsException( msg.c_str() );
        }
      }
      // Create model job
      else if ( job.__unionAbstractJob == 2 ) {

        om::om__CreateModelJobType * model_job = job.__union_ExperimentParametersType_Jobs.CreateModelJob;

        job_id = string( model_job->id );

        om::_om__ModelParameters mp;

        om::_om__Sampler sampler;

        string env_ref = string( model_job->EnvironmentRef->idref );
        if ( environments.count( env_ref ) > 0 ) {

          sampler.Environment = &environments[env_ref];
        }
        else {

          string msg = "No Environment found for reference ";
          msg.append( env_ref ).append( " on job " ).append( job_id );
          throw OmwsException( msg.c_str() );
        }

        string pre_ref = string( model_job->PresenceRef->idref );
        if ( presences.count( pre_ref ) > 0 ) {

          sampler.Presence = &presences[pre_ref];
        }
        else {

          depends_on.insert( pair<string, string>( pre_ref, "presence" ) );
          updateNextJobs( job_id, pre_ref, &next_deps );
        }

        if ( model_job->AbsenceRef != 0 ) {

          string abs_ref = string( model_job->AbsenceRef->idref );
          if ( absences.count( abs_ref ) > 0 ) {

            sampler.Absence = &absences[abs_ref];
          }
          else {

            depends_on.insert( pair<string, string>( abs_ref, "absence" ) );
            updateNextJobs( job_id, abs_ref, &next_deps );
          }
        }

        mp.Sampler = &sampler;

        string alg_ref = string( model_job->AlgorithmRef->idref );
        if ( algorithms.count( alg_ref ) > 0 ) {

          mp.Algorithm = &algorithms[alg_ref];
        }
        else {

          string msg = "No AlgorithmSettings found for reference ";
          msg.append( alg_ref ).append( " on job " ).append( job_id );
          throw OmwsException( msg.c_str() );
        }

        mp.Options = model_job->Options;

        string requestFileName = "";
        createTicket( soap, OMWS_MODEL _PENDING_REQUEST, ticket, &requestFileName );
        created_jobs.insert( pair<string, string>( string( ticket ), OMWS_MODEL ) );
        ofstream fstreamOUT( requestFileName.c_str() );
        soap->os = &fstreamOUT;

        if ( depends_on.size() == 0 ) {

          files_to_be_renamed.push_back( requestFileName );
        }
 
        // The following line reproduces the same encapsulated call used by 
        // soap_write_om__ModelParametersType, but here we need a different element name, 
        // that's why soap_write is not used directly.
        if ( ( mp.soap_serialize(soap), soap_begin_send(soap) || mp.soap_put(soap, "om:ModelParameters", NULL) || soap_end_send(soap), soap->error ) != SOAP_OK ) {

          string msg = "Failed to write create model job with id ";
          msg.append( job_id );
          throw OmwsException( msg.c_str() );
        }
      }
      // Test model job
      else if ( job.__unionAbstractJob == 3 ) {

        om::om__TestModelJobType * test_job = job.__union_ExperimentParametersType_Jobs.TestModelJob;

        job_id = string( test_job->id );

        om::_om__TestParameters tp;

        om::_om__Sampler sampler;

        string env_ref = string( test_job->EnvironmentRef->idref );
        if ( environments.count( env_ref ) > 0 ) {

          sampler.Environment = &environments[env_ref];
        }
        else {

          string msg = "No Environment found for reference ";
          msg.append( env_ref ).append( " on job " ).append( job_id );
          throw OmwsException( msg.c_str() );
        }

        string pre_ref = string( test_job->PresenceRef->idref );
        if ( presences.count( pre_ref ) > 0 ) {

          sampler.Presence = &presences[pre_ref];
        }
        else {

          depends_on.insert( pair<string, string>( pre_ref, "presence" ) );
          updateNextJobs( job_id, pre_ref, &next_deps );
        }

        if (  test_job->AbsenceRef != 0 ) {

          string abs_ref = string( test_job->AbsenceRef->idref );
          if ( absences.count( abs_ref ) > 0 ) {

            sampler.Absence = &absences[abs_ref];
          }
          else {

            depends_on.insert( pair<string, string>( abs_ref, "absence" ) );
            updateNextJobs( job_id, abs_ref, &next_deps );
          }
        }

        tp.Sampler = &sampler;

        // Check if LPT is used
        bool use_lpt = false;

        if ( test_job->Statistics->ConfusionMatrix != 0 && test_job->Statistics->ConfusionMatrix->Threshold != 0 ) {

          string threshold = string( test_job->Statistics->ConfusionMatrix->Threshold->c_str() );

          if ( threshold.compare("lpt") == 0 ) {

            use_lpt = true;
          }
        }

        string model_ref = string( test_job->ModelRef->idref );
        if ( models.count( model_ref ) > 0 ) {

          if ( use_lpt ) {

            string msg = "LPT threshold cannot be used on job ";
            msg.append( job_id ).append(" when there is no previous model creation");
            throw OmwsException( msg.c_str() );
          }

          tp.Algorithm = &models[model_ref];
        }
        else {

          // Insert model dependency regardless LPT to facilitate model extraction
          depends_on.insert( pair<string, string>( model_ref, "model" ) );

          if ( use_lpt ) {

            string lpt_id = model_ref + "_lpt";

            depends_on.insert( pair<string, string>( lpt_id, "lpt" ) );
            updateNextJobs( job_id, lpt_id, &next_deps );
            updateNextJobs( lpt_id, model_ref, &next_deps );

            // manually include prev dependency for implicit job
            map<string, string> lpt_dep;
            lpt_dep.insert( pair<string, string>( model_ref, "model" ) );
            prev_deps.insert( pair< string, map<string, string> >( lpt_id, lpt_dep ) );

            if ( jobs.count( lpt_id ) == 0 ) {

              // Create model evaluation job
              omws::xsd__string lpt_ticket;
              string lptRequestFileName = "";
              createTicket( soap, OMWS_EVALUATE _PENDING_REQUEST, lpt_ticket, &lptRequestFileName );
              created_jobs.insert( pair<string, string>( string( lpt_ticket ), OMWS_EVALUATE ) );
              ofstream fstreamOUT( lptRequestFileName.c_str() );
              soap->os = &fstreamOUT;

              // The following line reproduces the same encapsulated call used by 
              // soap_write_om__ModelEvaluationParametersType, but here we need a different element name, 
              // that's why soap_write is not used directly.
              om::_om__ModelEvaluationParameters me;

              if ( ( me.soap_serialize(soap), soap_begin_send(soap) || me.soap_put(soap, "om:ModelEvaluationParameters", NULL) || soap_end_send(soap), soap->error ) != SOAP_OK ) {

                string msg = "Failed to write LPT model evaluation job for ";
                msg.append( model_ref );
                throw OmwsException( msg.c_str() );
              }

              jobs.insert( pair<string, string>( lpt_id, string( lpt_ticket ) ) );
            }
          }
          else {

            updateNextJobs( job_id, model_ref, &next_deps );
          }
        }

        tp.Statistics = test_job->Statistics;

        string requestFileName = "";
        createTicket( soap, OMWS_TEST _PENDING_REQUEST, ticket, &requestFileName );
        created_jobs.insert( pair<string, string>( string( ticket ), OMWS_TEST ) );
        ofstream fstreamOUT( requestFileName.c_str() );
        soap->os = &fstreamOUT;

        if ( depends_on.size() == 0 ) {

          files_to_be_renamed.push_back( requestFileName );
        }
 
        // The following line reproduces the same encapsulated call used by 
        // soap_write_om__TestParametersType, but here we need a different element name, 
        // that's why soap_write is not used directly.
        if ( ( tp.soap_serialize(soap), soap_begin_send(soap) || tp.soap_put(soap, "om:TestParameters", NULL) || soap_end_send(soap), soap->error ) != SOAP_OK ) {

          string msg = "Failed to write test model job with id ";
          msg.append( job_id );
          throw OmwsException( msg.c_str() );
        }
      }
      // Project model job
      else if ( job.__unionAbstractJob == 4 ) {

        om::om__ProjectModelJobType * proj_job = job.__union_ExperimentParametersType_Jobs.ProjectModelJob;

        job_id = string( proj_job->id );

        om::_om__ProjectionParameters pp;

        // Check if LPT is used
        bool use_lpt = false;

        if ( proj_job->Statistics->AreaStatistics != 0 ) {

          string threshold = string( proj_job->Statistics->AreaStatistics->PredictionThreshold.c_str() );

          if ( threshold.compare("lpt") == 0 ) {

            use_lpt = true;
          }
        }

        string model_ref = string( proj_job->ModelRef->idref );
        if ( models.count( model_ref ) > 0 ) {

          if ( use_lpt ) {

            string msg = "LPT threshold cannot be used on job ";
            msg.append( job_id ).append(" when there is no previous model creation");
            throw OmwsException( msg.c_str() );
          }

          pp.Algorithm = &models[model_ref];
        }
        else {

          // Insert model dependency regardless LPT to facilitate model extraction
          depends_on.insert( pair<string, string>( model_ref, "model" ) );

          if ( use_lpt ) {

            string lpt_id = model_ref + "_lpt";

            depends_on.insert( pair<string, string>( lpt_id, "lpt" ) );
            updateNextJobs( job_id, lpt_id, &next_deps );
            updateNextJobs( lpt_id, model_ref, &next_deps );

            // manually include prev dependency for implicit job
            map<string, string> lpt_dep;
            lpt_dep.insert( pair<string, string>( model_ref, "model" ) );
            prev_deps.insert( pair< string, map<string, string> >( lpt_id, lpt_dep ) );

            if ( jobs.count( lpt_id ) == 0 ) {

              // Create model evaluation job
              omws::xsd__string lpt_ticket;
              string lptRequestFileName = "";
              createTicket( soap, OMWS_EVALUATE _PENDING_REQUEST, lpt_ticket, &lptRequestFileName );
              created_jobs.insert( pair<string, string>( string( lpt_ticket ), OMWS_EVALUATE ) );
              ofstream fstreamOUT( lptRequestFileName.c_str() );
              soap->os = &fstreamOUT;

              // The following line reproduces the same encapsulated call used by 
              // soap_write_om__ModelEvaluationParametersType, but here we need a different element name, 
              // that's why soap_write is not used directly.
              om::_om__ModelEvaluationParameters me;

              if ( ( me.soap_serialize(soap), soap_begin_send(soap) || me.soap_put(soap, "om:ModelEvaluationParameters", NULL) || soap_end_send(soap), soap->error ) != SOAP_OK ) {

                string msg = "Failed to write LPT model evaluation job for ";
                msg.append( model_ref );
                throw OmwsException( msg.c_str() );
              }

              jobs.insert( pair<string, string>( lpt_id, string( lpt_ticket ) ) );
            }
          }
          else {

            updateNextJobs( job_id, model_ref, &next_deps );
          }
        }

        string env_ref = string( proj_job->EnvironmentRef->idref );
        if ( environments.count( env_ref ) > 0 ) {

          pp.Environment = &environments[env_ref];
        }
        else {

          string msg = "No Environment found for reference ";
          msg.append( env_ref ).append( " on job " ).append( job_id );
          throw OmwsException( msg.c_str() );
        }

        pp.OutputParameters = proj_job->OutputParameters;
        pp.Statistics = proj_job->Statistics;

        string requestFileName = "";
        createTicket( soap, OMWS_PROJECTION _PENDING_REQUEST, ticket, &requestFileName );
        created_jobs.insert( pair<string, string>( string( ticket ), OMWS_PROJECTION ) );
        ofstream fstreamOUT( requestFileName.c_str() );
        soap->os = &fstreamOUT;

        if ( depends_on.size() == 0 ) {

          files_to_be_renamed.push_back( requestFileName );
        }
 
        // The following line reproduces the same encapsulated call used by 
        // soap_write_om__TestParametersType, but here we need a different element name, 
        // that's why soap_write is not used directly.
        if ( ( pp.soap_serialize(soap), soap_begin_send(soap) || pp.soap_put(soap, "om:ProjectionParameters", NULL) || soap_end_send(soap), soap->error ) != SOAP_OK ) {

          string msg = "Failed to write project model job with id ";
          msg.append( job_id );
          throw OmwsException( msg.c_str() );
        }
      }
      else {

        throw OmwsException( "Unknown job type" );
      }

      jobs.insert( pair<string, string>( job_id, string( ticket ) ) );

      if ( depends_on.size() > 0 ) {

        prev_deps.insert( pair< string, map<string, string> >( job_id, depends_on ) );
      }
    }

    // Insert dependencies in job metadata, also gathering the tickets to use after the loop
    string all_tickets;
    string all_ids;
    for ( map<string, string>::const_iterator jit = jobs.begin(); jit != jobs.end(); ++jit ) {

      string job_id = (*jit).first;
      string job_ticket = (*jit).second;

      // Concatenate all tickets to use later
      if ( jit != jobs.begin() ) {

        all_tickets += ",";
        all_ids += ",";
      }
      all_tickets += job_ticket;
      all_ids += job_id;

      // Build metadata file name
      string mfile_name( ticket_dir );
      mfile_name.append( OMWS_JOB_METADATA_PREFIX ).append( job_ticket );

      // Open metadata file
      FILE *mfile = fopen( mfile_name.c_str(), "a" );

      // Add reference to experiment
      string exp_line("EXP=");
      exp_line.append( experiment_ticket ).append("\n");

      if ( mfile == NULL || fputs( exp_line.c_str(), mfile ) < 0 ) {

        fclose( mfile );
        throw OmwsException("Failed to update jobs metadata (1)");
      }

      // Add dependencies (previous jobs)
      if ( prev_deps.count( job_id ) > 0 ) {

        string prev_line("PREV=");
        prev_line.append( collateTickets( &prev_deps[job_id], &jobs ) ).append("\n");

        if ( fputs( prev_line.c_str(), mfile ) < 0 ) {

          fclose( mfile );
          throw OmwsException("Failed to update jobs metadata (2)");
        }
      }

      // Add next jobs
      if ( next_deps.count( job_id ) > 0 ) {

        string next_line("NEXT=");
        next_line.append( collateTickets( &next_deps[job_id], &jobs ) ).append("\n");

        if ( fputs( next_line.c_str(), mfile ) < 0 ) {

          fclose( mfile );
          throw OmwsException("Failed to update jobs metadata (3)");
        }
      }

      fclose( mfile );
    }

    // Include job tickets in experiment metadata
    string exp_ticket_file( ticket_dir );
    exp_ticket_file.append( OMWS_JOB_METADATA_PREFIX ).append( experiment_ticket );

    FILE *efile = fopen( exp_ticket_file.c_str(), "a" );

    string jobs_line("JOBS=");
    jobs_line.append( all_tickets ).append("\n");

    // This is only expected to be used by Condor/DAGMan backends
    string ids_line("IDS=");
    ids_line.append( all_ids ).append("\n");

    if ( efile == NULL || fputs( jobs_line.c_str(), efile ) < 0 || fputs( ids_line.c_str(), efile ) < 0 ) {

      fclose( efile );
      throw OmwsException("Failed to update experiment metadata");
    }
    fclose( efile );
  }
  catch (...) {

    // Delete files and re raise exception
    for ( map<string, string>::const_iterator fit = created_jobs.begin(); fit != created_jobs.end(); ++fit ) {
      // Delete job request
      string j_req( ticket_dir );
      j_req.append( (*fit).second );
      j_req.append( _PENDING_REQUEST );
      j_req.append( (*fit).first );
      remove( j_req.c_str() );

      // Delete metadata file
      string j_meta( ticket_dir );
      j_meta.append( OMWS_JOB_METADATA_PREFIX ).append( (*fit).first );
      remove( j_meta.c_str() );

      // Delete ticket
      string j_ticket( ticket_dir );
      j_ticket.append( (*fit).first );
      remove( j_ticket.c_str() );
    }

    throw;
  }

  string condorIntegration( gFileParser.get( "CONDOR_INTEGRATION" ) );
  string dagmanEnabled( gFileParser.get( "DAGMAN_ENABLED" ) );

  if ( condorIntegration == "yes" && dagmanEnabled == "yes" ) {

    // Rename experiment job so that Condor can process it
    string exp_job_file = ticket_dir + OMWS_EXPERIMENT _PENDING_REQUEST + experiment_ticket;

    renameJobFile( exp_job_file, _PENDING_REQUEST, _REQUEST );
  }
  else {

    // Rename jobs that can be executed
    for ( vector<string>::const_iterator fit = files_to_be_renamed.begin(); fit != files_to_be_renamed.end(); ++fit ) {

      string job_file_name = (*fit);

      renameJobFile( job_file_name, _PENDING_REQUEST, _REQUEST );
    }

    // Rename experiment job, indicating that it was already processed
    string exp_job_file = ticket_dir + OMWS_EXPERIMENT _PENDING_REQUEST + experiment_ticket;

    renameJobFile( exp_job_file, _PENDING_REQUEST, _PROCESSED_REQUEST );
  }

  return jobs;
}

/************************/
/**** updateNextJobs ****/
static void
updateNextJobs( string next_id, string prev_id, map< string, vector<string> > *next_deps )
{
  if ( next_deps->count( prev_id ) > 0 ) {

    vector<string> * vec = &next_deps->at( prev_id );

    // Avoid including the same dependency
    if ( find( vec->begin(), vec->end(), next_id ) == vec->end() ) {

      vec->push_back( next_id );
    }
  }
  else {

    vector<string> next;
    next.push_back( next_id );
    next_deps->insert( pair< string, vector<string> >( prev_id, next ) );
  }
}

/************************/
/**** collateTickets ****/
static string
collateTickets( vector<string>* ids, map<string, string> *jobs )
{
  string result;
  for ( vector<string>::iterator it = ids->begin(); it != ids->end(); ++it ) {

    if ( it != ids->begin() ) {

      result += ",";
    }

    if ( jobs->count( *it ) > 0 ) {

      result += jobs->at( *it );
    }
    else {

      string msg( "Broken dependency (job referenced by id " );
      msg.append( *it ).append(" does not exist)");
      throw OmwsException( msg.c_str() );
    }
  }
  return result;
}

/************************/
/**** collateTickets ****/
static string
collateTickets( map<string, string>* ids, map<string, string> *jobs )
{
  string result;
  for ( map<string, string>::iterator it = ids->begin(); it != ids->end(); ++it ) {

    string job_id = (*it).first;
    string job_usage = (*it).second;

    if ( it != ids->begin() ) {

      result += ",";
    }

    if ( jobs->count( job_id ) > 0 ) {

      result += job_usage + "_" + jobs->at( job_id );
    }
    else {

      string msg( "Broken dependency (job referenced by id " );
      msg.append( job_id ).append(" does not exist)");
      throw OmwsException( msg.c_str() );
    }
  }
  return result;
}

/***********************/
/**** append Result ****/
static void
appendResult( string &ticket_dir, string &ticket, string &type, ostringstream &oss )
{
  string res_file;
  string container = "";
  string subcontainer = "";
  string attrs = "";

  if ( type.compare(0, 4, OMWS_PROJECTION) == 0 ) {

    res_file = ticket_dir + OMWS_PROJECTION_STATISTICS_PREFIX + ticket;

    container = "ProjectionEnvelope";
    subcontainer = "Statistics";

    attrs = " url=\"" + gFileParser.get( "BASE_URL" );

    if ( attrs.find_last_of( "/" ) != attrs.size() - 1 ) {

      attrs.append( "/" );
    }

    attrs.append( getMapFile( ticket ) ).append("\"");
  }
  else {

    res_file = ticket_dir + type + _RESPONSE + ticket;

    if ( type.compare(0, 5, OMWS_MODEL) == 0 ) {

      container = "ModelEnvelope";
    }
    else if ( type.compare(0, 4, OMWS_TEST) == 0 ) {

      container = "TestResultEnvelope";
    }
  }

  oss << "<Job Ticket=\"" << ticket << "\">" << endl;

  if ( container.size() > 0 ) {

    oss << "<" << container.c_str();

    if ( attrs.size() > 0 ) {

      oss << attrs.c_str();
    }

    oss << ">" << endl;

    if ( subcontainer.size() > 0 ) {

      ostringstream myoss;

      readFile( res_file.c_str(), myoss );

      if ( myoss.str().size() > 0 ) {

        oss << "<" << subcontainer.c_str() << ">" << endl;

        oss << myoss.str();

        oss << "</" << subcontainer.c_str() << ">" << endl;
      }
    }
    else {

      readFile( res_file.c_str(), oss );
    }

    oss << "</" << container.c_str() << ">" << endl;
  }
  else {

    readFile( res_file.c_str(), oss );
  }

  oss << "</Job>" << endl;
}
