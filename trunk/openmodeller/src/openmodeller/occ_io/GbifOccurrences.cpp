/**
 * Definition of GbifOccurrences class.
 * 
 * @author Renato De Giovanni (renato [at] cria dot org dot br)
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2008 by CRIA -
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

#include <openmodeller/occ_io/GbifOccurrences.hh>

#include <openmodeller/Occurrences.hh>

#include <openmodeller/Exceptions.hh>

#include <stdlib.h>

//c style include needed for strcmp etc
#include <string.h>
using std::string;

#include <sstream>
using std::ostringstream;

#include <curl/curl.h>

#include <expat.h>

/*****************************************/
/*** create OccurrencesReader callback ***/
OccurrencesReader * 
GbifOccurrences::CreateOccurrencesReaderCallback( const char *source, const char *coordSystem )
{
  return new GbifOccurrences( source, coordSystem );
}

/***************************/
/*** _curl header writer ***/
size_t 
GbifOccurrences::_curl_header_writer( void *ptr, size_t size, size_t nmemb, void *stream )
{
   if ( stream == NULL ) {

     return 0;
   }

  // *stream is actually a string object
  std::string& str = *( reinterpret_cast<std::string*>( stream ) );

  str.append( reinterpret_cast<const char*>(ptr), size*nmemb );

  return size*nmemb;
}

/*************************/
/*** _curl body writer ***/
size_t 
GbifOccurrences::_curl_body_writer( void *ptr, size_t size, size_t nmemb, void *stream )
{
   if ( stream == NULL ) {

     return 0;
   }

  // *stream is actually a string object
  std::string& str = *( reinterpret_cast<std::string*>( stream ) );

  str.append( reinterpret_cast<const char*>(ptr), size*nmemb );

  return size*nmemb;
}

/*******************/
/*** Constructor ***/
GbifOccurrences::GbifOccurrences( const char *source, const char *coordSystem )
{
  if ( curl_global_init( CURL_GLOBAL_ALL ) != CURLE_OK ) {

    throw OccurrencesReaderException( "Could not initialize libcurl" );
  }

  _loaded = false;

  _source = (char *) source; // endpoint

  _coord_system = (char *) coordSystem;

  _default_limit = 100;
}


/******************/
/*** Destructor ***/
GbifOccurrences::~GbifOccurrences()
{
  curl_global_cleanup();
}


/************/
/*** load ***/
bool
GbifOccurrences::load()
{
  if ( _loaded ) {

    return true;
  }

  Log::instance()->info( "Checking endpoint using GBIF driver\n" );

  Log::instance()->info( "CURL version is %s\n", curl_version() );

  // Prepare CURL handle
  CURL * curl_handle = curl_easy_init(); 

  if ( curl_handle == NULL ) {

    Log::instance()->error( "GbifOccurrences::load - Could not initialize curl handle\n" );
    return false;
  }

  // Prepare request with no parameters just to check if it's the GBIF service


  // Set CURL options
  if ( curl_easy_setopt( curl_handle, CURLOPT_URL, _source ) != CURLE_OK ) {

    Log::instance()->error( "GbifOccurrences::load - Failed to set CURLOPT_URL\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  if ( curl_easy_setopt( curl_handle, CURLOPT_HEADERFUNCTION, &GbifOccurrences::_curl_header_writer ) != CURLE_OK ) {

    Log::instance()->error( "GbifOccurrences::load - Failed to set CURLOPT_HEADERFUNCTION\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  std::string header;

  if ( curl_easy_setopt( curl_handle, CURLOPT_WRITEHEADER, &header ) != CURLE_OK ) {

    Log::instance()->error( "GbifOccurrences::load - Failed to set CURLOPT_WRITEHEADER\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  if ( curl_easy_setopt( curl_handle, CURLOPT_WRITEFUNCTION, &GbifOccurrences::_curl_body_writer ) != CURLE_OK ) {

    Log::instance()->error( "GbifOccurrences::load - Failed to set CURLOPT_WRITEFUNCTION\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  std::string response;

  if ( curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, &response ) != CURLE_OK ) {

    Log::instance()->error( "GbifOccurrences::load - Failed to set CURLOPT_WRITEDATA\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  Log::instance()->info( "Getting service info using %s\n", _source );

  // Send request
  CURLcode curl_code = curl_easy_perform( curl_handle );

  if ( curl_code != CURLE_OK )
  {
    Log::instance()->error( "GbifOccurrences::load - Could not get service info from the specified endpoint (CURL code error: %d)\n", curl_code );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  curl_easy_cleanup( curl_handle );

  // Check content type returned
  if ( header.find( "Content-Type: text/xml" ) == string::npos )
  {
    Log::instance()->error( "GbifOccurrences::load - URL does not seem to be a GBIF REST WS endpoint (HTTP Content-Type header is not text/xml)\n" );

    Log::instance()->info( "HEADER: %s\n", header.c_str() );

    return false;
  }

  // Data structure to store relevant service information
  ServiceInfo info;

  // Parse response
  if ( ! _parseServiceInfo( &response, &info ) ) {

    return false;
  }

  // Check response
  if ( ! info._is_gbif ) {

    Log::instance()->error( "GbifOccurrences::load - URL does not seem to be a GBIF WS REST endpoint (root element is not gbifResponse)\n" );
    return false;
  }

  Log::instance()->info( "Finished checking service info\n" );

  _loaded = true;

  return true;
}


/**************************/
/*** parse service info ***/
bool
GbifOccurrences::_parseServiceInfo( const std::string *xml, ServiceInfo *info )
{
  XML_Parser parser = XML_ParserCreateNS( NULL, '/' );

  if ( ! parser ) {

    Log::instance()->error( "Unable to allocate memory for capabilities response parser" );
    return false;
  }

  info->_parser = parser;

  XML_SetElementHandler( parser, &GbifOccurrences::_startServiceInfoElement, NULL );

  XML_SetUserData( parser, info );

  if ( ! XML_Parse( parser, xml->c_str(), xml->size(), 1 ) ) {

    XML_Error error_code =  XML_GetErrorCode( parser );
    std::ostringstream errormsg;
    errormsg << XML_ErrorString( error_code )
             << " at Line "
             << XML_GetCurrentLineNumber( parser )
             << " column "
             << XML_GetCurrentColumnNumber( parser )
             << std::ends;

    Log::instance()->error( "XML Parser fatal error for capabilities response: %s\n", errormsg.str().c_str() );
    XML_ParserFree( parser );
    return false;
  }
  
  XML_ParserFree( parser );

  _provider_limit = atoi( info->_max_records.c_str() );

  return true;
}


/**********************************/
/*** start service info element ***/
void 
GbifOccurrences::_startServiceInfoElement( void *data, const char *el, const char **attr )
{
  ServiceInfo& info = *( reinterpret_cast<ServiceInfo*>( data ) );

  // gbifResponse element
  if ( strncmp( el, "http://portal.gbif.org/ws/response/gbif/gbifResponse", 52 ) == 0 ) {

    info._is_gbif = true;
  }
  // parameter element
  else if ( strncmp( el, "http://portal.gbif.org/ws/response/gbif/parameter", 49 ) == 0 ) {

    bool is_maxresults = false;

    std::string value("");

    for ( int i = 0; attr[i]; i += 2 ) {

      // name attribute
      if ( strncmp( attr[i], "name", 4 ) == 0 ) {

        if ( strncmp( attr[i+1], "maxresults", 10 ) == 0 ) {
          is_maxresults = true;
        }
      }
      else if ( strncmp( attr[i], "value", 5 ) == 0 ) {

        value = attr[i+1];
      }

      if ( is_maxresults ) {

        info._max_records = value;

        // No need to keep parsing
        XML_SetElementHandler( info._parser, NULL, NULL );
      }
    }
  }
}


/*********************/
/*** get Presences ***/
OccurrencesPtr
GbifOccurrences::getPresences( const char *groupId )
{
  // If group was not specified, return empty set
  if ( ! groupId ) {

    return new OccurrencesImpl( 1 );
  }

  LstOccurrences::iterator ocs = _presences.begin();
  LstOccurrences::iterator end = _presences.end();

  while ( ocs != end ) {

    OccurrencesPtr oc = *ocs;

    if ( ! strcasecmp( groupId, oc->label() ) ) {

      _presences.erase( ocs );

      return oc;
    }

    ++ocs;
  }

  // If not found, create new group and retrieve records from provider

  OccurrencesPtr occurrences( new OccurrencesImpl( groupId, _coord_system ) );

  GbifRecordData search_data;

  search_data._occurrences = occurrences;
  search_data._next = 0;

  int limit = _default_limit;

  if ( _provider_limit > 0 && _provider_limit < _default_limit ) {

    limit = _provider_limit;
  }

  while ( search_data._next >= 0 ) {

    Log::instance()->info( "Fetching records (start %d, limit %d)\n", search_data._next, limit );

    if ( ! _retrieveRecords( &search_data, limit ) ) {

      break;
    }
  }

  _presences.push_back( occurrences );

  return occurrences;
}


/************************/
/*** retrieve Records ***/
bool
GbifOccurrences::_retrieveRecords( GbifRecordData *data, int limit )
{
  // Prepare CURL handle
  CURL * curl_handle = curl_easy_init(); 

  if ( curl_handle == NULL ) {

    Log::instance()->error( "GbifOccurrences::_retrieveRecords - Could not initialize curl handle\n" );
    return false;
  }

  // Prepare search request
  std::string source( _source );

  std::ostringstream search_url;

  search_url << source.c_str();

  if ( source.find( "?" ) != string::npos ) {

    search_url <<  "&";
  }
  else {

    search_url << "?";
  } 

  search_url << "startindex=" << data->_next;

  search_url << "&maxresults=" << limit;

// curl_easy_escape was included in libcurl version 7.15.4
#if LIBCURL_VERSION_NUM >= 0x070f04
  search_url << "&scientificname=" << curl_easy_escape( curl_handle, data->_occurrences->label(), 0 );
#else
  search_url << "&scientificname=" << curl_escape( data->_occurrences->label(), 0 );
#endif

  search_url << "&format=brief&coordinatestatus=true&coordinateissues=false";

  // After using next to make the URL, set it to -1 to stop the process in case 
  // the response does not return the "next" attribute
  data->_next = -1;

  // Set CURL options
  if ( curl_easy_setopt( curl_handle, CURLOPT_URL, search_url.str().c_str() ) != CURLE_OK ) {

    Log::instance()->error( "GbifOccurrences::_retrieveRecords - Failed to set CURLOPT_URL\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  if ( curl_easy_setopt( curl_handle, CURLOPT_HEADERFUNCTION, &GbifOccurrences::_curl_header_writer ) != CURLE_OK ) {

    Log::instance()->error( "GbifOccurrences::_retrieveRecords - Failed to set CURLOPT_HEADERFUNCTION\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  std::string header;

  if ( curl_easy_setopt( curl_handle, CURLOPT_WRITEHEADER, &header ) != CURLE_OK ) {

    Log::instance()->error( "GbifOccurrences::_retrieveRecords - Failed to set CURLOPT_WRITEHEADER\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  if ( curl_easy_setopt( curl_handle, CURLOPT_WRITEFUNCTION, &GbifOccurrences::_curl_body_writer ) != CURLE_OK ) {

    Log::instance()->error( "GbifOccurrences::_retrieveRecords - Failed to set CURLOPT_WRITEFUNCTION\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  std::string search_response;

  if ( curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, &search_response ) != CURLE_OK ) {

    Log::instance()->error( "GbifOccurrences::_retrieveRecords - Failed to set CURLOPT_WRITEDATA\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  Log::instance()->info( "Searching records using %s\n", search_url.str().c_str() );

  // Send search request
  CURLcode curl_code = curl_easy_perform( curl_handle );

  if ( curl_code != CURLE_OK )
  {
    const char * error_msg = curl_easy_strerror( curl_code );

    Log::instance()->error( "GbifOccurrences::_retrieveRecords - Could not search records from the specified GBIF endpoint (CURL error: %s)\n", error_msg );

    curl_easy_cleanup( curl_handle );
    return false;
  }

  curl_easy_cleanup( curl_handle );

  // Check content type returned
  if ( header.find( "Content-Type: text/xml" ) == string::npos )
  {
    Log::instance()->error( "GbifOccurrences::_retrieveRecords - URL does not seem to be a GBIF WS REST endpoint (HTTP Content-Type header is not text/xml)\n" );
    return false;
  }

  // Parse search response
  XML_Parser parser = XML_ParserCreateNS( NULL, '/' );

  if ( ! parser ) {

    Log::instance()->error( "Unable to allocate memory for search response parser" );
    return false;
  }

  data->_parser = parser;

  XML_SetElementHandler( parser, &GbifOccurrences::_startSearchElement, &GbifOccurrences::_endSearchElement );

  XML_SetUserData( parser, data );

  if ( ! XML_Parse( parser, search_response.c_str(), search_response.size(), 1 ) ) {

    XML_Error error_code =  XML_GetErrorCode( parser );
    std::ostringstream errormsg;
    errormsg << XML_ErrorString( error_code )
             << " at Line "
             << XML_GetCurrentLineNumber( parser )
             << " column "
             << XML_GetCurrentColumnNumber( parser )
             << std::ends;

    Log::instance()->error( "XML Parser fatal error for search response: %s\n", errormsg.str().c_str() );
    XML_ParserFree( parser );
    return false;
  }
  
  XML_ParserFree( parser );

  return true;
}


/****************************/
/*** start search element ***/
void 
GbifOccurrences::_startSearchElement( void *data, const char *el, const char **attr )
{
  GbifRecordData& search_data = *( reinterpret_cast<GbifRecordData*>( data ) );

  // summary element
  if ( strncmp( el, "http://portal.gbif.org/ws/response/gbif/summary", 47 ) == 0 ) {

    for ( int i = 0; attr[i]; i += 2 ) {

      // next attribute
      if ( strncmp( attr[i], "next", 4 ) == 0 ) {

        search_data._next = atoi( attr[i+1] );
      }
    }
  }
  // TaxonOccurrence element
  else if ( strncmp( el, "http://rs.tdwg.org/ontology/voc/TaxonOccurrence#/TaxonOccurrence", 64 ) == 0 ) {

    for ( int i = 0; attr[i]; i += 2 ) {

      // rdf:about attribute
      if ( strncmp( attr[i], "http://www.w3.org/1999/02/22-rdf-syntax-ns#/about", 49 ) == 0 ) {

        search_data._last_guid = attr[i+1];
      }
    }
  }
  // decimalLatitude element
  else if ( strncmp( el, "http://rs.tdwg.org/ontology/voc/TaxonOccurrence#/decimalLatitude", 64 ) == 0 ) {

    XML_SetCharacterDataHandler( search_data._parser, &GbifOccurrences::_ltDataHandler );
  }
  // decimalLongitude element
  else if ( strncmp( el, "http://rs.tdwg.org/ontology/voc/TaxonOccurrence#/decimalLongitude", 65 ) == 0 ) {

    XML_SetCharacterDataHandler( search_data._parser, &GbifOccurrences::_lgDataHandler );
  }
}


/**************************/
/*** end search element ***/
void 
GbifOccurrences::_endSearchElement( void *data, const char *el )
{
  // TaxonOccurrence element
  if ( strncmp( el, "http://rs.tdwg.org/ontology/voc/TaxonOccurrence#/TaxonOccurrence", 64 ) == 0 ) {

    GbifRecordData& search_data = *( reinterpret_cast<GbifRecordData*>( data ) );

    search_data._occurrences->createOccurrence( search_data._last_guid.c_str(), search_data._last_lg, search_data._last_lt, 0.0, 1, 0, 0 );
  }
}


/***********************/
/*** lt data handler ***/
void 
GbifOccurrences::_ltDataHandler( void *data, const char *value, int len )
{
  GbifRecordData& search_data = *( reinterpret_cast<GbifRecordData*>( data ) );

  std::string lt("");
  lt.append( value, len );

  search_data._last_lt = Coord( atof( lt.c_str() ) );

  XML_SetCharacterDataHandler( search_data._parser, NULL );
}


/***********************/
/*** lg data handler ***/
void 
GbifOccurrences::_lgDataHandler( void *data, const char *value, int len )
{
  GbifRecordData& search_data = *( reinterpret_cast<GbifRecordData*>( data ) );

  std::string lg("");
  lg.append( value, len );

  search_data._last_lg = Coord( atof( lg.c_str() ) );

  XML_SetCharacterDataHandler( search_data._parser, NULL );
}
