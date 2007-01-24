/**
 * openModeller SOAP interface.
 * 
 * @file
 * @author Renato De Giovanni (renato [at] cria [dot] org [dot] br)
 * @date   2004-04-06
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

#include "openModeller.nsmap"
#include <openmodeller/om.hh>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <dirent.h>

#include <sstream>
#include <fstream>
using namespace std;

#include "gdal_priv.h"

#include "file_parser.hh"

#define OMWS_BACKLOG (100) // Max. request backlog 
#define OMWS_MIN *60
#define OMWS_H *3600
#define OMWS_TICKET_TEMPLATE "XXXXXX"
#define OMWS_MODEL_CREATION_REQUEST_PREFIX "model_req."
#define OMWS_MODEL_CREATION_RESPONSE_PREFIX "model_resp."
#define OMWS_MODEL_PROJECTION_REQUEST_PREFIX "proj_req."
#define OMWS_PROJECTION_STATISTICS_PREFIX "stats."
#define OMWS_CONFIG_FILE "../config/server.conf"
#define OMWS_LAYERS_CACHE_FILE "layers.xml"

/*****************************/
/***  Forward declarations ***/

static void*    process_request( void* );
static bool     fileExists( const char* fileName );
static string   getMapFile( const char* ticket );
static wchar_t* convertToWideChar( const char* p );
static bool     readDirectory( const char* dir, const char* label, ostream &xml, int depth );
static bool     isValidGdalFile( const char* fileName );
static bool     hasValidGdalProjection( const char* fileName );
static int      getSize( FILE *fd );
static bool     getData( struct soap*, const xsd__string, xsd__base64Binary& );


/****************/
/***  Globals ***/

FileParser gFileParser( OMWS_CONFIG_FILE ); // Config file parser

/***********************/
/*** main gSOAP code ***/

int main(int argc, char **argv)
{ 
  struct soap soap;
  soap_init(&soap);
  soap.encodingStyle = NULL;

  // Load algorithms
  AlgorithmFactory::searchDefaultDirs();

  // Instantiate a shared openModeller controller
  OpenModeller *om;

  soap.user = (void*)om;   

  soap.accept_timeout = 0;  // always listening
  soap.send_timeout = 10 OMWS_H;
  soap.recv_timeout = 3 OMWS_MIN;

  // no args: assume this is a CGI application
  if ( argc < 2 ) { 

    soap_serve( &soap );
    soap_destroy( &soap );
    soap_end( &soap );
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

        soap_serve( &soap );
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
		  
          fprintf( stderr, "Thread %d accepts socket %d connection from IP %d.%d.%d.%d\n", i, s, (soap.ip >> 24)&0xFF, (soap.ip >> 16)&0xFF, (soap.ip >> 8)&0xFF, soap.ip&0xFF );
		  
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
          pthread_create( &tid[i], NULL, (void*(*)(void*))process_request, (void*)soap_thr[i] ); 
        } 
      }
    }
  }

  return 0;
}

/*************************/
/**** Process request ****/
void *process_request( void *soap )
{
  pthread_detach( pthread_self() );
  ((struct soap*)soap)->recv_timeout = 300; // Timeout after 5 minutes stall on recv
  ((struct soap*)soap)->send_timeout = 60; // Timeout after 1 minute stall on send
  soap_serve( (struct soap*)soap );
  soap_destroy( (struct soap*)soap );
  soap_end( (struct soap*)soap );
  soap_done( (struct soap*)soap );
  free( soap );

  return NULL;
}

/**************/
/**** Ping ****/
int
omws__ping( struct soap *soap, void *_, xsd__int &status )
{
  status = 1;

  return SOAP_OK;
}

/**********************/
/*** get Algorithms ***/
int 
omws__getAlgorithms( struct soap *soap, void *_, struct omws__getAlgorithmsResponse *out )
{
  // Get controller object previously instantiated
  OpenModeller *om = (OpenModeller*)soap->user; 

  // alloc new header
  soap->header = (struct SOAP_ENV__Header*)soap_malloc( soap, sizeof(struct SOAP_ENV__Header) ); 
  soap->header->omws__version = om->getVersion();

  AlgMetadata const **algorithms = om->availableAlgorithms();

  int nalg = om->numAvailableAlgorithms();

  if ( ! *algorithms ) {

    return soap_receiver_fault( soap, "No available algorithms", NULL );
  }

  ConfigurationPtr cfg = AlgorithmFactory::getConfiguration();
  ostringstream oss;
  Configuration::writeXml( cfg, oss );

  out->om__AvailableAlgorithms = convertToWideChar( oss.str().c_str() ) ;

  return SOAP_OK;
}

/**********************/
/*** get Layers ***/
int 
omws__getLayers( struct soap *soap, void *_, struct omws__getLayersResponse *out )
{
  // Get controller object previously instantiated
  OpenModeller *om = (OpenModeller*)soap->user; 

  // alloc new header
  soap->header = (struct SOAP_ENV__Header*)soap_malloc( soap, sizeof(struct SOAP_ENV__Header) ); 
  soap->header->omws__version = om->getVersion();

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

        return soap_receiver_fault( soap, "Could not open cache", NULL );
      }

      //while ( fgetws( buf, bufSize, file ) != (wchar_t *)NULL ) {
      while ( fgets( buf, bufSize, file ) != (char *)NULL ) {

        cachedXml.append( buf );
      }

      if ( ! feof( file ) ) {

        return soap_receiver_fault( soap, "Cache reading error", NULL );
      }

      //out->om__AvailableLayers = (wchar_t*)cachedXml.c_str();
      out->om__AvailableLayers = convertToWideChar( cachedXml.c_str() );

      fclose( file );

      return SOAP_OK;
    }
  }

  // Recurse on all sub directories searching for GDAL compatible layers
  ostringstream oss;

  if ( ! readDirectory( gFileParser.get( "LAYERS_DIRECTORY" ).c_str(), 
                        gFileParser.get( "LAYERS_LABEL" ).c_str(), oss, 0 ) ) {

    return soap_receiver_fault( soap, "Could not read available layers", NULL );
  }

  out->om__AvailableLayers = convertToWideChar( oss.str().c_str() ) ;

  // If cache is configured
  if ( ! cacheDir.empty() ) {

    // Cache result for the next requests
    FILE *file = fopen( layersFile.c_str(), "w" );

    if ( file == NULL ) {

      return soap_receiver_fault( soap, "Could not open cache", NULL );
    }

    //if ( fputws( out->om__AvailableLayers, file ) < 0 ) {
    if ( fputs( oss.str().c_str(), file ) < 0 ) {

      return soap_receiver_fault( soap, "Could not write to cache", NULL );
    }

    fclose( file );
  }

  return SOAP_OK;
}

/**********************/
/**** create Model ****/
int 
omws__createModel( struct soap *soap, XML om__ModelParameters, xsd__string &ticket )
{
  string ticketFileName( gFileParser.get( "TICKET_DIRECTORY" ) );

  // Append slash if necessary
  if ( ticketFileName.find_last_of( "/" ) != ticketFileName.size() - 1 ) {

    ticketFileName.append( "/" );
  }

  // Copy name to future request file name
  string requestFileName( ticketFileName );

  // Append ticket template
  ticketFileName.append( OMWS_TICKET_TEMPLATE );

  // Temporary variable to generate ticket and copy its value
  char *tempFileName = (char*)soap_malloc( soap, ticketFileName.length() +1 );

  strcpy( tempFileName, ticketFileName.c_str() );

  // Generate unique ticket file
  int fd = mkstemp( tempFileName );

  if ( fd == -1 ) {

    return soap_receiver_fault( soap, "Could not create ticket", NULL );
  }

  // Get ticket value
  ticket = strrchr( tempFileName, '/' ) + 1;

  // Append prefix to request file
  requestFileName.append( OMWS_MODEL_CREATION_REQUEST_PREFIX );

  // Append ticket to request file
  requestFileName.append( ticket );

  // Create and open request file - at this point there should be no file with the same name!
  FILE *file = fopen( requestFileName.c_str(), "w" );

  if ( file == NULL ) {

     return soap_receiver_fault( soap, "Could not open ticket", NULL );
  }

  // Add wrapper element
  wchar_t openRoot[] = L"<ModelParameters>";

  if ( fputws( openRoot, file ) < 0 ) {

    return soap_receiver_fault( soap, "Could not start processing ticket", NULL );
  }

  // Put content of model request there
  if ( fputws( om__ModelParameters, file ) < 0 ) {

    return soap_receiver_fault( soap, "Could not process request", NULL );
  }

  // Close wrapper element
  wchar_t closeRoot[] = L"</ModelParameters>";

  if ( fputws( closeRoot, file ) < 0 ) {

    return soap_receiver_fault( soap, "Could not finish processing ticket", NULL );
  }

  fclose( file );

  return SOAP_OK;
}

/***********************/
/**** project Model ****/
int 
omws__projectModel( struct soap *soap, XML om__ProjectionParameters, xsd__string &ticket )
{
  string ticketFileName( gFileParser.get( "TICKET_DIRECTORY" ) );

  // Append slash if necessary
  if ( ticketFileName.find_last_of( "/" ) != ticketFileName.size() - 1 ) {

    ticketFileName.append( "/" );
  }

  // Copy name to future request file name
  string requestFileName( ticketFileName );

  // Append ticket template
  ticketFileName.append( OMWS_TICKET_TEMPLATE );

  // Temporary variable to generate ticket and copy its value
  char *tempFileName = (char*)soap_malloc( soap, ticketFileName.length() +1 );

  strcpy( tempFileName, ticketFileName.c_str() );

  // Generate unique ticket file
  int fd = mkstemp( tempFileName );

  if ( fd == -1 ) {

    return soap_receiver_fault( soap, "Could not create ticket", NULL );
  }

  // Get ticket value
  ticket = strrchr( tempFileName, '/' ) + 1;

  // Append prefix to request file
  requestFileName.append( OMWS_MODEL_PROJECTION_REQUEST_PREFIX );

  // Append ticket to request file
  requestFileName.append( ticket );

  // Create and open request file - at this point there should be no file with the same name!
  FILE *file = fopen( requestFileName.c_str(), "w" );

  if ( file == NULL ) {

     return soap_receiver_fault( soap, "Could not open ticket", NULL );
  }

  // Add wrapper element
  wchar_t openRoot[] = L"<ProjectionParameters>";

  if ( fputws( openRoot, file ) < 0 ) {

    return soap_receiver_fault( soap, "Could not start processing ticket", NULL );
  }

  // Put content of model request there
  if ( fputws( om__ProjectionParameters, file ) < 0 ) {

    return soap_receiver_fault( soap, "Could not process request", NULL );
  }

  // Close wrapper element
  wchar_t closeRoot[] = L"</ProjectionParameters>";

  if ( fputws( closeRoot, file ) < 0 ) {

    return soap_receiver_fault( soap, "Could not finish processing ticket", NULL );
  }

  fclose( file );

  return SOAP_OK;
}

/**********************/
/**** get progress ****/
int 
omws__getProgress( struct soap *soap, xsd__string ticket, xsd__int &progress )
{ 
  if ( ! ticket ) {

    return soap_sender_fault( soap, "Missing ticket in request", NULL );
  }

  string fileName( gFileParser.get( "TICKET_DIRECTORY" ) );

  // Append slash if necessary
  if ( fileName.find_last_of( "/" ) != fileName.size() - 1 ) {

    fileName.append( "/" );
  }

  // First try searching on model creation jobs
  string modelRespFile( fileName );

  modelRespFile.append( OMWS_MODEL_CREATION_RESPONSE_PREFIX );
  modelRespFile.append( ticket );

  if ( fileExists( modelRespFile.c_str() ) ) {

    // If file exists, then just report 100%
    progress = 100;
    return SOAP_OK;
  }

  // Now try searching on model projection jobs, for each possible file extension

  fileName = getMapFile( ticket );

  if ( ! fileName.empty() ) {

    progress = 100;
  }
  else {

    progress = 0;
  }

  return SOAP_OK;
}

/*****************/
/**** get log ****/
int 
omws__getLog( struct soap *soap, xsd__string ticket, xsd__string &log )
{ 
  if ( ! ticket ) {

    return soap_sender_fault( soap, "Missing ticket in request", NULL );
  }

  string fileName( gFileParser.get( "TICKET_DIRECTORY" ) );

  // Append slash if necessary
  if ( fileName.find_last_of( "/" ) != fileName.size() - 1 ) {

    fileName.append( "/" );
  }

  // Log should be in the ticket file
  fileName.append( ticket );

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

    log = (char*)oss.str().c_str();

    fin.close();
  }
  else {

    return soap_receiver_fault( soap, "Log unavailable", NULL );
  }

  return SOAP_OK;
}

/*******************/
/**** get model ****/
int 
omws__getModel( struct soap *soap, xsd__string ticket, struct omws__getModelResponse *out )
{ 
  if ( ! ticket ) {

    return soap_sender_fault( soap, "Missing ticket in request", NULL );
  }

  string fileName( gFileParser.get( "TICKET_DIRECTORY" ) );

  // Append slash if necessary
  if ( fileName.find_last_of( "/" ) != fileName.size() - 1 ) {

    fileName.append( "/" );
  }

  fileName.append( OMWS_MODEL_CREATION_RESPONSE_PREFIX );
  fileName.append( ticket );

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

/**********************************/
/**** get layer as attachment ****/
int 
omws__getLayerAsAttachment( struct soap *soap, xsd__string id, xsd__base64Binary &file )
{ 
  if ( ! id ) {

    return soap_sender_fault( soap, "Layer id required", NULL );
  }

  if ( ! getData( soap, id, file ) ) {

    return soap_sender_fault( soap, "Layer unavailable", NULL );
  }

  return SOAP_OK;
}

/**************************/
/**** get layer as URL ****/
int 
omws__getLayerAsUrl( struct soap *soap, xsd__string id, xsd__string &url )
{ 

  // This method is now working only for distribution maps!

  if ( ! id ) {

    return soap_sender_fault(soap, "Layer id required", NULL);
  }

  string fileName = getMapFile( id );

  if ( fileName.empty() ) {

    return soap_receiver_fault( soap, "Layer unavailable", NULL );
  }

  string urlString( gFileParser.get( "BASE_URL" ) );

  if ( urlString.find_last_of( "/" ) != urlString.size() - 1 ) {

    urlString.append( "/" );
  }

  urlString.append( fileName );
  
  url = (char*)urlString.c_str();

  return SOAP_OK;
}

/*****************************/
/**** get projection data ****/
int 
omws__getProjectionMetadata( struct soap *soap, xsd__string ticket, struct omws__getProjectionMetadataResponse *out )
{ 
  if ( ! ticket ) {

    return soap_sender_fault( soap, "Missing ticket in request", NULL );
  }

  // Get map size
  string mapFileName = getMapFile( ticket );

  if ( mapFileName.empty() ) {

    return soap_receiver_fault( soap, "Distribution map unavailable", NULL );
  }

  string completeFileName( gFileParser.get( "DISTRIBUTION_MAP_DIRECTORY" ) );

  if ( completeFileName.find_last_of( "/" ) != completeFileName.size() - 1 ) {

    // Append slash if necessary
    completeFileName.append( "/" );
  }

  completeFileName.append( mapFileName );

  FILE *fd = fopen( completeFileName.c_str(), "rb" );

  if ( fd == NULL ) {

    return soap_receiver_fault( soap, "Could not read distribution map", NULL );
  }

  int size = getSize( fd );

  // Get statistics
  string statsFileName( gFileParser.get( "TICKET_DIRECTORY" ) );

  // Append slash if necessary
  if ( statsFileName.find_last_of( "/" ) != statsFileName.size() - 1 ) {

    statsFileName.append( "/" );
  }

  statsFileName.append( OMWS_PROJECTION_STATISTICS_PREFIX );
  statsFileName.append( ticket );

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

    out->FileSize = size;
    out->om__AreaStatistics = convertToWideChar( oss.str().c_str() );

    fin.close();
  }
  else {

    return soap_receiver_fault( soap, "Projection data unavailable", NULL );
  }

  return SOAP_OK;
}


//////////////////////////////
//
//	Helper functions
//
/////////////////////////////

/********************/
/**** fileExists ****/
static bool 
fileExists( const char* fileName )
{ 
  bool exists = false;

  FILE * file = fopen( fileName, "r" );

  if ( file != NULL ) {

    exists = true;

    fclose( file );
  }

  return exists;
}

/********************/
/**** getMapFile ****/
static string 
getMapFile( const char* ticket )
{ 
  string fileName( gFileParser.get( "DISTRIBUTION_MAP_DIRECTORY" ) );

  // Append slash if necessary
  if ( fileName.find_last_of( "/" ) != fileName.size() - 1 ) {

    fileName.append( "/" );
  }

  // TIF
  string projTifFile( fileName );

  projTifFile.append( ticket );
  projTifFile.append( ".tif" );

  if ( fileExists( projTifFile.c_str() ) ) {

    return projTifFile.substr( projTifFile.find_last_of("/") + 1 );
  }

  // IMG
  string projImgFile( fileName );

  projImgFile.append( ticket );
  projImgFile.append( ".img" );

  if ( fileExists( projImgFile.c_str() ) ) {

    return projImgFile.substr( projImgFile.find_last_of("/") + 1 );
  }

  // BMP
  string projBmpFile( fileName );

  projBmpFile.append( ticket );
  projBmpFile.append( ".bmp" );

  if ( fileExists( projBmpFile.c_str() ) ) {

    return projBmpFile.substr( projBmpFile.find_last_of("/") + 1 );
  }

  string emptyString("");

  return emptyString;
}

/***************************/
/**** convertToWideChar ****/
static 
wchar_t* convertToWideChar( const char* p )
{
  wchar_t *r;

  r = new wchar_t[strlen(p)+1];

  const char *tempsource = p;

  wchar_t *tempdest = r;

  while ( *tempdest++ = *tempsource++ );

  return r;
}

/***********************/
/**** readDirectory ****/
static 
bool readDirectory( const char* dir, const char* label, ostream &xml, int depth )
{
  bool r = true;

  ++depth;

  // Maximum depth level to recurse on directories
  if ( depth > 20 )
  {
    return false;
  }

  // Create copy of dir and add slash to the end if necessary
  string myDir( dir );

  if ( myDir.find_last_of( "/" ) != myDir.size() - 1 ) {

    myDir.append( "/" );
  }

  // Open directory
  struct dirent **nameList;
  int n;
  struct stat buf;

  n = scandir( myDir.c_str(), &nameList, 0, alphasort );

  if ( n < 0 ) {

      // Problems opening directory
      return false;
  }
  else if ( n == 0 ) {
  
      // Empty directories are useless
      return true;
  }

  xml << "<LayersGroup Id=\"" << myDir << "\">";

  xml << "<Label>" << label << "</Label>";

  string xmlFiles;

  for ( int i = 0; i < n; i++ ) {

    // Skip "." and ".." directories
    if ( strcmp( nameList[i]->d_name, "." ) == 0 || strcmp( nameList[i]->d_name, ".." ) == 0 ) {

      continue;
    }

    // Need the full name to "stat"
    string fullName( myDir );
    fullName.append( nameList[i]->d_name );

    // Get file attributes (skip on failure)
    if ( stat( fullName.c_str(), &buf ) == -1 ) {

      continue;
    }

    // Directory
    if ( S_ISDIR( buf.st_mode ) ) {

      if ( isValidGdalFile( fullName.c_str() ) ) {

        string hasProj = ( hasValidGdalProjection( fullName.c_str() ) ) ? "1" : "0";

        xmlFiles.append( "<Layer Id=\"" );
        xmlFiles.append( fullName );
        xmlFiles.append( "\" HasProjection=\"");
        xmlFiles.append( hasProj );
        xmlFiles.append( "\">" );
        xmlFiles.append( "<Label>" );
        xmlFiles.append( nameList[i]->d_name );
        xmlFiles.append( "</Label>" );
        xmlFiles.append( "</Layer>" );
      }
      else {

        r = readDirectory( fullName.c_str(), nameList[i]->d_name, xml, depth );
      }
    }
    // Regular file
    else if ( S_ISREG( buf.st_mode ) ) {

      if ( isValidGdalFile( fullName.c_str() ) ) {

        string hasProj = ( hasValidGdalProjection( fullName.c_str() ) ) ? "1" : "0";

        xmlFiles.append( "<Layer Id=\"" );
        xmlFiles.append( fullName );
        xmlFiles.append( "\" HasProjection=\"");
        xmlFiles.append( hasProj );
        xmlFiles.append( "\">" );
        xmlFiles.append( "<Label>" );
        xmlFiles.append( nameList[i]->d_name );
        xmlFiles.append( "</Label>" );
        xmlFiles.append( "</Layer>" );
      }
    }
    // Symbolic link
    else if ( S_ISLNK( buf.st_mode ) ) {

      // What should we do with symlinks?
      //readDirectory( fullName.c_str(), nameList[i]->d_name, xml, depth );
    }
  }

  // openModeller.xsd mandates that files should always come after directories
  xml << xmlFiles;

  xml << "</LayersGroup>";

  return r;
}

/*************************/
/**** isValidGdalFile ****/
static
bool isValidGdalFile( const char* fileName )
{
  // test whether the file is GDAL compatible
  GDALAllRegister();
  GDALDataset * testFile = (GDALDataset *)GDALOpen( fileName, GA_ReadOnly );

  if ( testFile == NULL ) {

    // not GDAL compatible
    GDALClose( testFile );
    return false;
  }
  else {

    // is GDAL compatible
    GDALClose( testFile );
    return true;  
  }
}

/********************************/
/**** hasValidGdalProjection ****/
static
bool hasValidGdalProjection( const char* fileName )
{
  // test whether the file has GDAL projection info
  GDALAllRegister();
  GDALDataset * testFile = (GDALDataset *)GDALOpen( fileName, GA_ReadOnly );
      
  const char *projectionString = testFile->GetProjectionRef();
      
  if ( projectionString ) {

    // does not have projection info
    GDALClose( testFile );
    return false;
  }
  else {

    // does have projection info
    GDALClose( testFile );
    return true;
  }
}

/******************/
/**** getSize ****/
static int 
getSize( FILE *fd )
{ 
  struct stat sb;

  if ( ! fstat( fileno( fd ), &sb ) ) { 

    return sb.st_size;
  }
  else { 

    return -1;
  }
}

/******************/
/**** getData ****/
static bool 
getData( struct soap *soap, const xsd__string ticket, xsd__base64Binary &file )
{ 
  string fileName = getMapFile( ticket );

  if ( fileName.empty() ) {

    return false;
  }

  string completeFileName( gFileParser.get( "DISTRIBUTION_MAP_DIRECTORY" ) );

  // Append slash if necessary
  if ( completeFileName.find_last_of( "/" ) != completeFileName.size() - 1 ) {

    completeFileName.append( "/" );
  }

  completeFileName.append( fileName );

  FILE *fd = fopen( completeFileName.c_str(), "rb" );

  if ( fd == NULL ) {

    return false;
  }

  int size = getSize( fd );

  if ( size <= 0 ) {

    fclose( fd );
    return false;
  }

  // don't use HTTP chunking - buffer the content
  int i;
  file.__size = size;
  file.__ptr = (unsigned char*)soap_malloc( soap, size );

  for ( i = 0; i < size; i++ ) {

    int c;

    if ( ( c = fgetc( fd ) ) == EOF ) {

      break;
    }

    file.__ptr[i] = c;
  }

  fclose( fd );

  string fileType( "image/" );

  fileType.append( fileName.substr( fileName.length() - 3 ) );

  file.type = (char*)fileType.c_str(); // specify non-NULL id or type to enable DIME
  file.options = soap_dime_option( soap, 0, "Distribution map" );

  return true;
}

///////////////////////
//
//	Classes
//
//////////////////////

xsd__base64Binary::xsd__base64Binary()
{
  __ptr = NULL;
  __size = 0;
  id = NULL;
  type = NULL;
  options = NULL;
  soap = NULL;
}
