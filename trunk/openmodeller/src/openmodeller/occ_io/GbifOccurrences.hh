/**
 * Declaration of GbifOccurrences class.
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

#ifndef _GBIF_OCCURRENCESHH_
#define _GBIF_OCCURRENCESHH_

#include <openmodeller/occ_io/OccurrencesReader.hh>

#include <openmodeller/om_defs.hh>
#include <openmodeller/Occurrences.hh>

#include <string>
using std::string;

#include <expat.h>

// Data structure to store relevant information about a TAPIR capabilities response
struct ServiceInfo {

  ServiceInfo(): _is_gbif(false), _max_records("") { }

  XML_Parser _parser;
  bool _is_gbif; // indicates if it's a GBIF WS REST response
  std::string _max_records; // maximum number of records that can be returned by the provider
};

// Data structure to be used by the expat handlers during searches
struct GbifRecordData {

  XML_Parser _parser;
  OccurrencesPtr _occurrences;
  std::string _last_guid; // last guid
  Coord _last_lg; // last longitude
  Coord _last_lt; // last latitude
  int _next; // index of next record to be retrieved
};

/**  
 * Read occurrences data from the GBIF REST service.
 *
 */
class dllexp GbifOccurrences : public OccurrencesReader
{
public:

  /** 
   * Return a new instance of GbifOccurrences.
   */
  static OccurrencesReader * CreateOccurrencesReaderCallback( const char * source, const char * coordSystem );

  /**
   * Constructor.
   * @param url Service URL.
   * @param coordSystem Default coordinate system in WKT.
   */
  GbifOccurrences( const char * url, const char * coordSystem );

  /** Destructor.
   */
  ~GbifOccurrences();

  /**
   * Read occurrences from the GBIF REST service.
   */
  bool load();

  /** Return the presences (abundance > 0) from a specific group.
   * @param groupId Identifier for a group of occurrences (usually a species name).
   * @return Pointer to occurrences of the specified group. If the group was not found
   *         or not specified, an empty OccurrencesPtr is returned.
   */
  OccurrencesPtr getPresences( const char *groupId );

  /** Return the absences (abundance == 0) from a specific group.
   * @param groupId Identifier for a group of occurrences (usually a species name).
   * @return Pointer to occurrences of the specified group. If the group was not found
   *         or not specified, an empty OccurrencesPtr is returned.
   */
  OccurrencesPtr getAbsences( const char *groupId ) { return new OccurrencesImpl( 0 ); }

private:

  // Indicates if occurrences were already loaded.
  bool _loaded;

  // Default record limit to be retrieved from the provider
  int _default_limit;

  // Maximum number of records that can be returned by the provider
  int _provider_limit;

  // callback function that will be called by CURL when retrieving header content
  static size_t _curl_header_writer( void * ptr, size_t size, size_t nmemb, void * stream );

  // callback function that will be called by CURL when retrieving body content
  static size_t _curl_body_writer( void * ptr, size_t size, size_t nmemb, void * stream );

  // Parse service info response
  bool _parseServiceInfo( const std::string * xml, ServiceInfo * info );

  // Start element handler for service info parser
  static void _startServiceInfoElement( void *data, const char *el, const char **attr );

  // Retrieve records
  bool _retrieveRecords( GbifRecordData *data, int limit );

  // Start element handler for search parser
  static void _startSearchElement( void *data, const char *el, const char **attr );

  // End element handler for search parser
  static void _endSearchElement( void *data, const char *el );

  // Latitude character data handler for TaxonOccurrence element
  static void _ltDataHandler( void *data, const char *value, int len );

  // Longitude character data handler for TaxonOccurrence element
  static void _lgDataHandler( void *data, const char *value, int len );

};

#endif

