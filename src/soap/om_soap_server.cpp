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

#include <stdlib.h>
#include <string.h>
#include <pthread.h> 
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <dirent.h>

#include <sstream>
using namespace std;

#include "gdal_priv.h"

#define OMWS_BACKLOG (100) // Max. request backlog 
#define OMWS_TEMPLATE_FILE_NAME "dmapXXXXXX"
#define OMWS_LAYERS_DIRECTORY "/home/renato/projects/openmodeller/examples/layers/"
#define OMWS_LAYERS_LABEL "Remote layers"
#define OMWS_MIN *60
#define OMWS_H *3600

/*****************************/
/***  Forward declarations ***/

static void *process_request( void* );
static int getData( struct soap*, const xsd__string, xsd__base64Binary& );
static wchar_t* convertToWideChar( const char* p );
static bool readDirectory( const char* dir, const char* label, ostream &xml, int depth );
static bool isValidGdalFile( const char* fileName );

/********************/
/***  Static data ***/

static char *OM_SOAP_TMPDIR = ".";


/***********************/
/*** main gSOAP code ***/

int main(int argc, char **argv)
{ 
  char *s = getenv("OM_SOAP_TMPDIR");

  if (s)
    {
      OM_SOAP_TMPDIR = s;
    }

  char log_file[256];
  strcpy(log_file, OM_SOAP_TMPDIR);
  strcat(log_file, "/om.log");

  FILE *flog = fopen(log_file, "w");
  if (!flog)
    {
      fprintf(stderr, "Could not open log file\n");
    }

  g_log.set(Log::Debug, flog);

  struct soap soap;
  soap_init(&soap);

  // Load algorithms
  AlgorithmFactory::searchDefaultDirs();

  // Instantiate a shared openModeller controller
  OpenModeller *om;

  soap.user = (void*)om;   

  soap.accept_timeout = 0;  // always listening
  soap.send_timeout = 10 OMWS_H;
  soap.recv_timeout = 3 OMWS_MIN;

  if (argc < 2) // no args: assume this is a CGI application
    { 
      soap_serve(&soap);
      soap_destroy(&soap);
      soap_end(&soap);
    }
  else
    { 
      int port = atoi(argv[1]); // first command-line arg is port 

      int m, s, i; // master and slave sockets, and thread counter

      m = soap_bind(&soap, NULL, port, OMWS_BACKLOG);

      if (m < 0)
	{ 
	  soap_print_fault(&soap, stderr);
	  exit(-1);
	}

      fprintf(stderr, "Socket connection successful: master socket = %d\n", m);

      int max_thr = 8; // max. number of threads to serve requests (default value)

      if (argc > 2)
	{
	  max_thr = atoi(argv[2]); // second command-line arg is max. number of threads
	}

      if (max_thr == 1) // stand alone non-multi-threaded service
	{
	  for ( ; ; )
	    {
	      s = soap_accept(&soap);
	      fprintf(stderr, "Socket connection successful: slave socket = %d\n", s);

	      if (s < 0)
		{ 
		  soap_print_fault(&soap, stderr);
		  exit(-1);
		} 

	      soap_serve(&soap);
	      soap_destroy(&soap);
	      soap_end(&soap);
	    }
	}
      else // stand alone multi-threaded service (pool of threads)
	{
	  struct soap *soap_thr[max_thr]; // each thread needs a runtime environment 
	  pthread_t tid[max_thr]; 
	  
	  for (i = 0; i < max_thr; i++) 
	    soap_thr[i] = NULL; 
	  
	  for ( ; ; )
	    { 
	      for (i = 0; i < max_thr; i++) 
		{ 
		  // Unix SIGPIPE, this is OS dependent:
		  //soap.accept_flags = SO_NOSIGPIPE;    // some systems like this
		  //soap.socket_flags = MSG_NOSIGNAL;    // others need this
		  //signal(SIGPIPE, sigpipe_handle);     // or a sigpipe handler (portable)
		  
		  s = soap_accept(&soap); 
		  
		  if (s < 0)
		    {
		      if (soap.errnum) 
			{
			  soap_print_fault(&soap, stderr);
			}
		      else
			{
			  fprintf(stderr, "Server timed out\n"); // Assume timeout is long enough for threads to complete serving requests 
			}
		      
		      break; 
		    }
		  
		  fprintf(stderr, "Thread %d accepts socket %d connection from IP %d.%d.%d.%d\n", i, s, (soap.ip >> 24)&0xFF, (soap.ip >> 16)&0xFF, (soap.ip >> 8)&0xFF, soap.ip&0xFF);
		  
		  if (!soap_thr[i]) // first time around 
		    { 
		      soap_thr[i] = soap_copy(&soap); 
		      
		      if (!soap_thr[i]) 
			exit(1); // could not allocate 
		    } 
		  else // recycle soap environment 
		    { 
		      pthread_join(tid[i], NULL); 
		      fprintf(stderr, "Thread %d completed\n", i); 
		      soap_destroy(soap_thr[i]); // deallocate C++ data of old thread 
		      soap_end(soap_thr[i]); // deallocate data of old thread 
		    }
		  
		  soap_thr[i]->socket = s; // new socket fd 
		  //pthread_create(&tid[i], NULL, (void*(*)(void*))soap_serve, (void*)soap_thr[i]); 
		  pthread_create(&tid[i], NULL, (void*(*)(void*))process_request, (void*)soap_thr[i]); 
		} 
	    }
	}
    }

  fclose(flog);
  
  return 0;
}


void *process_request(void *soap)
{
  pthread_detach(pthread_self());
  ((struct soap*)soap)->recv_timeout = 300; // Timeout after 5 minutes stall on recv
  ((struct soap*)soap)->send_timeout = 60; // Timeout after 1 minute stall on send
  soap_serve((struct soap*)soap);
  soap_destroy((struct soap*)soap);
  soap_end((struct soap*)soap);
  soap_done((struct soap*)soap);
  free(soap);
  return NULL;
}

/**************/
/**** Ping ****/
int
omws__ping( struct soap *soap, void *_, xsd__int *status )
{
  *status = 1;

  return SOAP_OK;
}

/**********************/
/*** get Algorithms ***/
int 
omws__getAlgorithms( struct soap *soap, void *_, XML &om__AvailableAlgorithms )
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

  om__AvailableAlgorithms = convertToWideChar( oss.str().c_str() ) ;

  return SOAP_OK;
}

/**********************/
/*** get Layers ***/
int 
omws__getLayers( struct soap *soap, void *_, XML &om__AvailableLayers )
{
  // Get controller object previously instantiated
  OpenModeller *om = (OpenModeller*)soap->user; 

  // alloc new header
  soap->header = (struct SOAP_ENV__Header*)soap_malloc( soap, sizeof(struct SOAP_ENV__Header) ); 
  soap->header->omws__version = om->getVersion();

  // Recurse on all sub directories searching for GDAL compatible layers
  ostringstream oss;

  if ( ! readDirectory( OMWS_LAYERS_DIRECTORY, OMWS_LAYERS_LABEL, oss, 0 ) ) {

    return soap_receiver_fault( soap, "Could not read available layers", NULL );
  }

  om__AvailableLayers = convertToWideChar( oss.str().c_str() ) ;

  return SOAP_OK;
}


/**********************/
/**** create Model ****/
int 
omws__createModel( struct soap *soap, omws__Points *points, omws__Maps *maps, omws__Mask *mask, omws__Algorithm *algorithm, omws__Output *output, xsd__string *ticket )
{
  // Will use a special instance to run the model
  OpenModeller myom;

  // alloc new header
  soap->header = (struct SOAP_ENV__Header*)soap_malloc( soap, sizeof(struct SOAP_ENV__Header) ); 
  soap->header->omws__version = myom.getVersion();

  // Get algorithm metadata
  AlgMetadata const *alg_metadata = myom.algorithmMetadata( algorithm->Id );

  if ( ! alg_metadata )
    {
      return soap_receiver_fault( soap, "Could not retrieve algorithm metadata", NULL );
    }

  // Points
  OccurrencesPtr presences = new OccurrencesImpl( "presences", points->coordsystem );
  OccurrencesPtr absences  = new OccurrencesImpl( "absences" , points->coordsystem );

  soap_Point *point = points->__ptrpresences->__ptrpoint;

  for ( int i = 0; i < points->__ptrpresences->__size; i++, point++)
    {
      presences->createOccurrence( (Coord)point->longitude, (Coord)point->latitude, 
                                   (Scalar)-1.0, (Scalar)1.0 );
    }

  if ( points->__ptrabsences && alg_metadata->absence )
    {
      point = points->__ptrabsences->__ptrpoint;

      for ( int i = 0; i < points->__ptrabsences->__size; i++, point++)
	{
          absences->createOccurrence( (Coord)point->longitude, (Coord)point->latitude, 
                                      (Scalar)-1.0, (Scalar)0.0 );
	}
      
      myom.setOccurrences( presences, absences );
    }
  else
    {
      myom.setOccurrences( presences );
    }


  // Environmental layers
  std::vector<std::string> categorical_layers;
  std::vector<std::string> continuous_layers;

  soap_Map *map = maps->__ptrmap;
  for ( int i = 0; i < maps->__size; i++, map++) {

    if ( map->categorical && alg_metadata->categorical ) {

      categorical_layers.push_back( map->location );
    }
    else {

      continuous_layers.push_back( map->location );
    }
  }

  myom.setEnvironment( categorical_layers, continuous_layers, mask->location );

  // Set the algorithm to be used
  if ( ! myom.setAlgorithm( algorithm->Id, algorithm->__size, 
                          (AlgParameter *)algorithm->__ptrparameter ) )
  {
      return soap_receiver_fault( soap, "Could not load the requested algorithm", NULL );
  }


  // Output map  (FIX ME: use a portable and better solution for unique names)
  char *template_file_name = (char*)soap_malloc( soap, strlen(OM_SOAP_TMPDIR) + 
                                                       strlen(OMWS_TEMPLATE_FILE_NAME) +2 );
  strcpy( template_file_name, OM_SOAP_TMPDIR );
  strcat( template_file_name, "/" );
  strcat( template_file_name, OMWS_TEMPLATE_FILE_NAME );
  mkstemp( template_file_name ); // generate file with unique name, and keep it

  char *projection_file_name = (char*)soap_malloc( soap, strlen(template_file_name) + 
                                            strlen(output->format) + 1 );
  strcpy( projection_file_name, template_file_name );
  strcat( projection_file_name, output->format ); // unique name + file format (should include dot)

  *ticket = strrchr( projection_file_name, '/' ) + 1; //ticket is actually the output map file name

  pid_t pid = fork();

  // The idea is to immediately return the ticket in one of the processes
  // and to generate the model in the other (which exits in the end).
  // If the parent overtake the model creation and exit, the child process
  // will always persist, and the PIDs will be changing over the time.
  // If the child overtake the model creation and exit, its process 
  // becomes "defunct" (?).
  if (pid > 0) // parent process
    {
      if ( ! myom.createModel() )
        {
	  g_log( myom.error() );
          return soap_receiver_fault( soap, "Could not create model", NULL );
        }

      // Serialize model
      char *model_file_name = (char*)soap_malloc( soap, strlen(template_file_name) + 5 );
      strcpy( model_file_name, template_file_name );
      strcat( model_file_name, ".xml" );

      ConfigurationPtr cfg = myom.getConfiguration();
      Configuration::writeXml( cfg, model_file_name );

      // Output map format

      //      myom.setOutputMap( (xsd__double)output->scale, projection_file_name, mask->location, output->header );
      //
      // TODO: confirm if "scale" parameter should be removed.
      MapFormat map_format = MapFormat( output->header );

      // hard coded for now: 8-bit grey tiffs
      map_format.setFormat( MapFormat::GreyTiff );

      // Make projection

      //      if ( ! myom.createMap( projection_file_name ) )
      //        {
      //	  g_log( myom.error() );
      //          return soap_receiver_fault( soap, "Could not create map", NULL );
      //        }
      myom.createMap( projection_file_name, map_format );

      // do we need this here?
      soap_destroy((struct soap*)soap);
      soap_end((struct soap*)soap);
      soap_done((struct soap*)soap);
      free(soap);

      exit(0); // kill process
    }
  else if (pid == 0) // child process
    {
      // return ticket and keep listening 
      return SOAP_OK;
    }
  else
    {
      return soap_receiver_fault(soap, "Server fork error", NULL);
    }
}


/******************************/
/**** get distribution map ****/
int 
omws__getDistributionMap( struct soap *soap, xsd__string ticket, xsd__base64Binary &file )
{ 
  if (!ticket)
    return soap_sender_fault(soap, "Ticket required", NULL);

  if (getData(soap, ticket, file))
    return soap_sender_fault(soap, "Access denied", NULL);

  file.type = "image/tif";
  file.options = soap_dime_option(soap, 0, "Distribution map");

  return SOAP_OK;
}


//////////////////////////////
//
//	Helper functions
//
/////////////////////////////

/******************/
/**** getData ****/
static int 
getData( struct soap *soap, const xsd__string ticket, xsd__base64Binary &file )
{ 
  struct stat sb;
  FILE *fd = NULL;
  if (!strchr(ticket, '/') && !strchr(ticket, '\\') && !strchr(ticket, ':'))
  { 
    char *s = (char*)soap_malloc(soap, strlen(OM_SOAP_TMPDIR) + strlen(ticket) + 2);
    strcpy(s, OM_SOAP_TMPDIR);
    strcat(s, "/");
    strcat(s, ticket);
    fd = fopen(s, "rb");
  }
  if (!fd)
    return SOAP_EOF;

  if ((!fstat(fileno(fd), &sb) && sb.st_size > 0))
  { 
    // don't use HTTP chunking - buffer the content
    int i;
    file.__size = sb.st_size;
    file.__ptr = (unsigned char*)soap_malloc(soap, sb.st_size);
    for (i = 0; i < sb.st_size; i++)
    { 
      int c;
      if ((c = fgetc(fd)) == EOF)
        break;

      file.__ptr[i] = c;
    }
    fclose(fd);
  }
  else
  { 
    return SOAP_EOF;
  }

  file.type = ""; // specify non-NULL id or type to enable DIME
  file.options = soap_dime_option(soap, 0, ticket);

  return SOAP_OK;
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

        xmlFiles.append( "<Layer Id=\"" );
        xmlFiles.append( fullName );
        xmlFiles.append( "\" IsCategorical=\"0\">" );
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

        xmlFiles.append( "<Layer Id=\"" );
        xmlFiles.append( fullName );
        xmlFiles.append( "\" IsCategorical=\"0\">" );
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
  //test whether the file is GDAL compatible
  GDALAllRegister();
  GDALDataset * myTestFile = (GDALDataset *)GDALOpen( fileName, GA_ReadOnly );

  if ( myTestFile == NULL ) {

    // not GDAL compatible
    GDALClose( myTestFile );
    return false;
  }
  else {

    // is GDAL compatible
    GDALClose( myTestFile );
    return true;  
  }
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
