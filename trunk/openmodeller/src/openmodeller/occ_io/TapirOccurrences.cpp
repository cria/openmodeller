/**
 * Definition of TapirOccurrences class.
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

#include <openmodeller/occ_io/TapirOccurrences.hh>

#include <openmodeller/Occurrences.hh>

#include <openmodeller/Exceptions.hh>

#include <stdlib.h>

#include <string.h>
using std::string;

#include <sstream>
using std::ostringstream;

#include <curl/curl.h>

#include <expat.h>

#define TP_TEMPLATE_LOCATION "http://openmodeller.cria.org.br/xml/tapir/1.0/st_v3.xml"
#define TP_TEMPLATE_STRSIZE 55
#define TP_OUTPUT_MODEL_LOCATION "http://openmodeller.cria.org.br/xml/tapir/1.0/om.xml"
#define TP_OUTPUT_MODEL_STRSIZE 52

/*****************************************/
/*** create OccurrencesReader callback ***/
OccurrencesReader * 
TapirOccurrences::CreateOccurrencesReaderCallback( const char *source, const char *coordSystem )
{
  return new TapirOccurrences( source, coordSystem );
}

/***************************/
/*** _curl header writer ***/
size_t 
TapirOccurrences::_curl_header_writer( void *ptr, size_t size, size_t nmemb, void *stream )
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
TapirOccurrences::_curl_body_writer( void *ptr, size_t size, size_t nmemb, void *stream )
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
TapirOccurrences::TapirOccurrences( const char *source, const char *coordSystem )
{
  if ( curl_global_init( CURL_GLOBAL_ALL ) != CURLE_OK ) {

    throw OccurrencesReaderException( "Could not initialize libcurl" );
  }

  _loaded = false;

  _source = (char *) source; // TAPIR endpoint

  _coord_system = (char *) coordSystem;

  _default_limit = 100;
}


/******************/
/*** Destructor ***/
TapirOccurrences::~TapirOccurrences()
{
  curl_global_cleanup();
}


/************/
/*** load ***/
bool
TapirOccurrences::load()
{
  if ( _loaded ) {

    return true;
  }

  Log::instance()->info( "Checking endpoint using TAPIR driver\n" );

  Log::instance()->info( "CURL version is %s\n", curl_version() );

  // Prepare CURL handle
  CURL * curl_handle = curl_easy_init(); 

  if ( curl_handle == NULL ) {

    Log::instance()->error( "TapirOccurrences::load - Could not initialize curl handle\n" );
    return false;
  }

  // Prepare TAPIR capabilities request
  std::string capabilities_url( _source );

  if ( capabilities_url.find( "?" ) != string::npos ) {

    capabilities_url.append( "&" );
  }
  else {

    capabilities_url.append( "?" );
  } 

  capabilities_url.append( "op=c" );

  // Set CURL options
  if ( curl_easy_setopt( curl_handle, CURLOPT_URL, capabilities_url.c_str() ) != CURLE_OK ) {

    Log::instance()->error( "TapirOccurrences::load - Failed to set CURLOPT_URL\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  if ( curl_easy_setopt( curl_handle, CURLOPT_HEADERFUNCTION, &TapirOccurrences::_curl_header_writer ) != CURLE_OK ) {

    Log::instance()->error( "TapirOccurrences::load - Failed to set CURLOPT_HEADERFUNCTION\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  std::string header;

  if ( curl_easy_setopt( curl_handle, CURLOPT_WRITEHEADER, &header ) != CURLE_OK ) {

    Log::instance()->error( "TapirOccurrences::load - Failed to set CURLOPT_WRITEHEADER\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  if ( curl_easy_setopt( curl_handle, CURLOPT_WRITEFUNCTION, &TapirOccurrences::_curl_body_writer ) != CURLE_OK ) {

    Log::instance()->error( "TapirOccurrences::load - Failed to set CURLOPT_WRITEFUNCTION\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  std::string capabilities_response;

  if ( curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, &capabilities_response ) != CURLE_OK ) {

    Log::instance()->error( "TapirOccurrences::load - Failed to set CURLOPT_WRITEDATA\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  Log::instance()->info( "Getting capabilities using %s\n", capabilities_url.c_str() );

  // Send capabilities request
  CURLcode curl_code = curl_easy_perform( curl_handle );

  if ( curl_code != CURLE_OK )
  {
    Log::instance()->error( "TapirOccurrences::load - Could not get service capabilities from the specified endpoint (CURL code error: %d)\n", curl_code );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  curl_easy_cleanup( curl_handle );

  // Check content type returned
  if ( header.find( "Content-Type: text/xml" ) == string::npos )
  {
    Log::instance()->warn( "TapirOccurrences::load - URL does not seem to be a TAPIR endpoint (HTTP Content-Type header is not text/xml)\n" );

    return false;
  }

  // Data structure to store relevant capabilities information
  CapabilitiesInfo info;

  // Parse capabilities response
  if ( ! _parseCapabilities( &capabilities_response, &info ) ) {

    return false;
  }

  // Check capabilitites response
  if ( ! info._is_tapir ) {

    Log::instance()->warn( "TapirOccurrences::load - URL does not seem to be a TAPIR endpoint (no TAPIR response element detected)\n" );
    return false;
  }
  if ( ! info._has_guid ) {

    Log::instance()->error( "TapirOccurrences::load - Provider did not map the DarwinCore GlobalUniqueIdentifier concept\n" );
    return false;
  }
  if ( ! info._has_name ) {

    Log::instance()->error( "TapirOccurrences::load - Provider did not map the DarwinCore ScientificName concept\n" );
    return false;
  }
  if ( ! info._has_long ) {

    Log::instance()->error( "TapirOccurrences::load - Provider did not map the DarwinCore DecimalLongitude concept from the geospatial extension\n" );
    return false;
  }
  if ( ! info._has_lat ) {

    Log::instance()->error( "TapirOccurrences::load - Provider did not map the DarwinCore DecimalLatitude concept from the geospatial extension\n" );
    return false;
  }
  if ( ( ! info._accepts_om_template ) && ( ! info._accepts_any_model ) && ! info._accepts_om_model ) {

    Log::instance()->error( "TapirOccurrences::load - Provider must accept searches with the openModeller query template or searches with the openModeller output model or searches with any output model\n" );
    return false;
  }

  Log::instance()->info( "Finished reading capabilities\n" );

  _loaded = true;

  return true;
}


/**************************/
/*** parse capabilities ***/
bool
TapirOccurrences::_parseCapabilities( const std::string *xml, CapabilitiesInfo *info )
{
  XML_Parser parser = XML_ParserCreateNS( NULL, '/' );

  if ( ! parser ) {

    Log::instance()->error( "Unable to allocate memory for capabilities response parser" );
    return false;
  }

  info->_parser = parser;

  XML_SetElementHandler( parser, &TapirOccurrences::_startCapabilitiesElement, NULL );

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
/*** start capabilities element ***/
void 
TapirOccurrences::_startCapabilitiesElement( void *data, const char *el, const char **attr )
{
  CapabilitiesInfo& info = *( reinterpret_cast<CapabilitiesInfo*>( data ) );

  // response element
  if ( strncmp( el, "http://rs.tdwg.org/tapir/1.0/response", 37 ) == 0 ) {

    info._is_tapir = true;
  }
  // mappedConcept element
  else if ( strncmp( el, "http://rs.tdwg.org/tapir/1.0/mappedConcept", 42 ) == 0 ) {

    for ( int i = 0; attr[i]; i += 2 ) {

      // id attribute  (TODO: check "searchable" attribute too)
      if ( strncmp( attr[i], "id", 2 ) == 0 ) {

        if ( strncmp( attr[i+1], "http://rs.tdwg.org/dwc/dwcore/GlobalUniqueIdentifier", 52 ) == 0 ) {
          info._has_guid = true;
        }
        else if ( strncmp( attr[i+1], "http://rs.tdwg.org/dwc/dwcore/ScientificName", 44 ) == 0 ) {

          info._has_name = true;
        }
        else if ( strncmp( attr[i+1], "http://rs.tdwg.org/dwc/geospatial/DecimalLongitude", 50 ) == 0 ) {

          info._has_long = true;
        }
        else if ( strncmp( attr[i+1], "http://rs.tdwg.org/dwc/geospatial/DecimalLatitude/", 49 ) == 0 ) {

          info._has_lat = true;
        }
      }
    }
  }
  // template element
  else if ( strncmp( el, "http://rs.tdwg.org/tapir/1.0/template", 37 ) == 0 ) {

    for ( int i = 0; attr[i]; i += 2 ) {

      // location attribute
      if ( strncmp( attr[i], "location", 8 ) == 0 && 
           strncmp( attr[i+1], TP_TEMPLATE_LOCATION, TP_TEMPLATE_STRSIZE ) == 0 ) {

        info._accepts_om_template = true;
      }
    }
  }
  // outputModel element
  else if ( strncmp( el, "http://rs.tdwg.org/tapir/1.0/outputModel", 40 ) == 0 ) {

    for ( int i = 0; attr[i]; i += 2 ) {

      // location attribute
      if ( strncmp( attr[i], "location", 8 ) == 0 && 
           strncmp( attr[i+1], TP_OUTPUT_MODEL_LOCATION, TP_OUTPUT_MODEL_STRSIZE ) == 0 ) {

        info._accepts_om_model = true;
      }
    }
  }
  // anyOutputModels element
  else if ( strncmp( el, "http://rs.tdwg.org/tapir/1.0/anyOutputModels", 44 ) == 0 ) {

    info._accepts_any_model = true;
  }
  // maxElementRepetitions element
  else if ( strncmp( el, "http://rs.tdwg.org/tapir/1.0/maxElementRepetitions", 50 ) == 0 ) {
    XML_SetCharacterDataHandler( info._parser, &TapirOccurrences::_maxRecordsDataHandler );

    XML_SetElementHandler( info._parser, &TapirOccurrences::_startCapabilitiesElement, 
                                         &TapirOccurrences::_endCapabilitiesElement );
  }
}


/********************************/
/*** max records data handler ***/
void 
TapirOccurrences::_maxRecordsDataHandler( void *data, const char *value, int len )
{
  CapabilitiesInfo& info = *( reinterpret_cast<CapabilitiesInfo*>( data ) );

  info._max_records.append( value, len );
}


/********************************/
/*** end capabilities element ***/
void 
TapirOccurrences::_endCapabilitiesElement( void *data, const char *el )
{
  // maxElementRepetitions element
  if ( strncmp( el, "http://rs.tdwg.org/tapir/1.0/maxElementRepetitions", 50 ) == 0 ) {

    CapabilitiesInfo& info = *( reinterpret_cast<CapabilitiesInfo*>( data ) );

    XML_SetCharacterDataHandler( info._parser, NULL );
    XML_SetElementHandler( info._parser, &TapirOccurrences::_startCapabilitiesElement, NULL );
  }
}


/*********************/
/*** get Presences ***/
OccurrencesPtr
TapirOccurrences::getPresences( const char *groupId )
{
  // If group was not specified, return empty set
  if ( ! groupId ) {

    return new OccurrencesImpl( 1 );
  }

  LstOccurrences::iterator ocs = _presences.begin();
  LstOccurrences::iterator end = _presences.end();

  while ( ocs != end ) {

    OccurrencesPtr oc = *ocs;

    if ( ! strcasecmp( groupId, oc->name() ) ) {

      _presences.erase( ocs );

      return oc;
    }

    ++ocs;
  }

  // If not found, create new group and retrieve records from provider

  OccurrencesPtr occurrences( new OccurrencesImpl( groupId, _coord_system ) );

  TapirRecordData search_data;

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
TapirOccurrences::_retrieveRecords( TapirRecordData *data, int limit )
{
  // Prepare CURL handle
  CURL * curl_handle = curl_easy_init(); 

  if ( curl_handle == NULL ) {

    Log::instance()->error( "TapirOccurrences::_retrieveRecords - Could not initialize curl handle\n" );
    return false;
  }

  // Prepare TAPIR search request
  std::string source( _source );

  std::ostringstream search_url;

  search_url << source.c_str();

  if ( source.find( "?" ) != string::npos ) {

    search_url <<  "&";
  }
  else {

    search_url << "?";
  } 

  search_url << "op=s&s=" << data->_next;

  search_url << "&l=" << limit;

// curl_easy_escape was included in libcurl version 7.15.4
#if LIBCURL_VERSION_NUM >= 0x070f04
  search_url << "&sciname=" << curl_easy_escape( curl_handle, data->_occurrences->name(), 0 );
  search_url << "&t=" << curl_easy_escape( curl_handle, TP_TEMPLATE_LOCATION, 0 );
#else
  search_url << "&sciname=" << curl_escape( data->_occurrences->name(), 0 );
  search_url << "&t=" << curl_escape( TP_TEMPLATE_LOCATION, 0 );
#endif

  // After using next to make the URL, set it to -1 to stop the process in case 
  // the response does not return the "next" attribute
  data->_next = -1;

  // Set CURL options
  if ( curl_easy_setopt( curl_handle, CURLOPT_URL, search_url.str().c_str() ) != CURLE_OK ) {

    Log::instance()->error( "TapirOccurrences::_retrieveRecords - Failed to set CURLOPT_URL\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  if ( curl_easy_setopt( curl_handle, CURLOPT_HEADERFUNCTION, &TapirOccurrences::_curl_header_writer ) != CURLE_OK ) {

    Log::instance()->error( "TapirOccurrences::_retrieveRecords - Failed to set CURLOPT_HEADERFUNCTION\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  std::string header;

  if ( curl_easy_setopt( curl_handle, CURLOPT_WRITEHEADER, &header ) != CURLE_OK ) {

    Log::instance()->error( "TapirOccurrences::_retrieveRecords - Failed to set CURLOPT_WRITEHEADER\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  if ( curl_easy_setopt( curl_handle, CURLOPT_WRITEFUNCTION, &TapirOccurrences::_curl_body_writer ) != CURLE_OK ) {

    Log::instance()->error( "TapirOccurrences::_retrieveRecords - Failed to set CURLOPT_WRITEFUNCTION\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  std::string search_response;

  if ( curl_easy_setopt( curl_handle, CURLOPT_WRITEDATA, &search_response ) != CURLE_OK ) {

    Log::instance()->error( "TapirOccurrences::_retrieveRecords - Failed to set CURLOPT_WRITEDATA\n" );
    curl_easy_cleanup( curl_handle );
    return false;
  }

  Log::instance()->info( "Searching records using %s\n", search_url.str().c_str() );

  // Send search request
  CURLcode curl_code = curl_easy_perform( curl_handle );

  if ( curl_code != CURLE_OK )
  {
    const char * error_msg = curl_easy_strerror( curl_code );

    Log::instance()->error( "TapirOccurrences::_retrieveRecords - Could not search records from the specified TAPIR endpoint (CURL error: %s)\n", error_msg );

    curl_easy_cleanup( curl_handle );
    return false;
  }

  curl_easy_cleanup( curl_handle );

  // Check content type returned
  if ( header.find( "Content-Type: text/xml" ) == string::npos )
  {
    Log::instance()->error( "TapirOccurrences::_retrieveRecords - URL does not seem to be a TAPIR endpoint (HTTP Content-Type header is not text/xml)\n" );
    return false;
  }

  // Parse search response
  XML_Parser parser = XML_ParserCreateNS( NULL, '/' );

  if ( ! parser ) {

    Log::instance()->error( "Unable to allocate memory for search response parser" );
    return false;
  }

  XML_SetElementHandler( parser, &TapirOccurrences::_startSearchElement, NULL );

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
TapirOccurrences::_startSearchElement( void *data, const char *el, const char **attr )
{
  TapirRecordData& search_data = *( reinterpret_cast<TapirRecordData*>( data ) );

  // occ element
  if ( strlen( el ) == 49 && 
       strncmp( el, "http://openmodeller.cria.org.br/xml/tapir/1.0/occ", 49 ) == 0 ) {

    std::string guid("");

    Coord lg = 0;
    Coord lt = 0;

    for ( int i = 0; attr[i]; i += 2 ) {

      // guid attribute
      if ( strncmp( attr[i], "guid", 4 ) == 0 ) {

        guid = attr[i+1];
      }
      // long attribute
      else if ( strncmp( attr[i], "long", 4 ) == 0 ) {

        lg = Coord( atof( attr[i+1] ) );
      }
      // lat attribute
      else if ( strncmp( attr[i], "lat", 3 ) == 0 ) {

        lt = Coord( atof( attr[i+1] ) );
      }
      // TODO: get datum and convert coordinates when necessary
    }

    search_data._occurrences->createOccurrence( guid.c_str(), lg, lt, 0.0, 1, 0, 0 );
  }
  // summary element
  if ( strncmp( el, "http://rs.tdwg.org/tapir/1.0/summary", 36 ) == 0 ) {

    for ( int i = 0; attr[i]; i += 2 ) {

      // next attribute
      if ( strncmp( attr[i], "next", 4 ) == 0 ) {

        search_data._next = atoi( attr[i+1] );
      }
    }
  }
}
