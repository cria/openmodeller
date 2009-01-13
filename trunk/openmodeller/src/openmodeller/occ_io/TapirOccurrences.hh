/**
 * Declaration of TapirOccurrences class.
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

#ifndef _TAPIR_OCCURRENCESHH_
#define _TAPIR_OCCURRENCESHH_

#include <openmodeller/occ_io/OccurrencesReader.hh>

#include <openmodeller/om_defs.hh>
#include <openmodeller/Occurrences.hh>

#include <string>
using std::string;

#include <expat.h>

// Data structure to store relevant information about a TAPIR capabilities response
struct CapabilitiesInfo {

  CapabilitiesInfo(): _is_tapir(false), _has_guid(false), _has_name(false),_has_long(false),_has_lat(false),_accepts_any_model(false),_accepts_om_model(false),_accepts_om_template(false),_max_records("") { }

  XML_Parser _parser;
  bool _is_tapir; // indicates if it's a TAPIR response
  bool _has_guid; // indicates if provider mapped the DarwinCore global unique identifier concept
  bool _has_name; // indicates if provider mapped the DarwinCore scientific name concept
  bool _has_long; // indicates if provider mapped the DarwinCore longitude concept
  bool _has_lat;  // indicates if provider mapped the DarwinCore latitude concept
  bool _accepts_any_model; // indicates if provider accepts any output model
  bool _accepts_om_model;  // indicates if provider accepts the openModeller output model
  bool _accepts_om_template;  // indicates if provider accepts the openModeller query template
  std::string _max_records; // maximum number of records that can be returned by the provider
};

// Data structure to be used by the expat handlers during searches
struct TapirRecordData {

  OccurrencesPtr _occurrences;
  int _next; // index of next record to be retrieved
};

/**  
 * Read occurrences data from a TAPIR service that uses DarwinCore 1.4 
 * and the corresponding geospatial extension.
 *
 */
class dllexp TapirOccurrences : public OccurrencesReader
{
public:

  /** 
   * Return a new instance of TapirOccurrences.
   */
  static OccurrencesReader * CreateOccurrencesReaderCallback( const char * source, const char * coordSystem );

  /**
   * Constructor.
   * @param url Service URL.
   * @param coordSystem Default coordinate system in WKT.
   */
  TapirOccurrences( const char * url, const char * coordSystem );

  /** Destructor.
   */
  ~TapirOccurrences();

  /**
   * Read occurrences from a TAPIR service.
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

  // Parse capabilities response
  bool _parseCapabilities( const std::string * xml, CapabilitiesInfo * info );

  // Start element handler for capabilities parser
  static void _startCapabilitiesElement( void *data, const char *el, const char **attr );

  // Character data handler for maxElementRepetitions element
  static void _maxRecordsDataHandler( void *data, const char *value, int len );

  // End element handler for capabilities parser
  static void _endCapabilitiesElement( void *data, const char *el );

  // Retrieve records
  bool _retrieveRecords( TapirRecordData *data, int limit );

  // Start element handler for search parser
  static void _startSearchElement( void *data, const char *el, const char **attr );

};

#endif

