/**
 * Definition of OccurrencesFactory class.
 * 
 * @author Alexandre Copertino Jardim <alexcj@dpi.inpe.br>
 * @date 2006-03-21
 * $Id$ 
 *
 * LICENSE INFORMATION
 *
 * Copyright © 2006 INPE
 * Copyright(c) 2003 by CRIA - Centro de Referencia em Informacao Ambiental
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


#ifndef _OCCURRENCES_FACTORY_HH_
#define _OCCURRENCES_FACTORY_HH_

#include <openmodeller/occ_io/OccurrencesReader.hh>
#include <openmodeller/occ_io/DelimitedTextOccurrences.hh>
#include <openmodeller/occ_io/SerializedXmlOccurrences.hh>

#ifdef TERRALIB_FOUND
#include <openmodeller/occ_io/TeOccurrences.hh>
#endif

#ifdef CURL_FOUND
#include <openmodeller/occ_io/TapirOccurrences.hh>
#include <openmodeller/occ_io/GbifOccurrences.hh>
#endif

#include <string>
using std::string;

#include <map>
using std::map;

#include <vector>
using std::vector;

/**
* OccurrencesReader factory.
*/
class dllexp OccurrencesFactory
{

private:

  // Function pointer to builder function.
  typedef OccurrencesReader * (*CreateOccurrencesReaderCallback)( const char * source, const char * coordSystem );

  // Map of OccurrencesReader and identifiers.
  typedef map<string, CreateOccurrencesReaderCallback> DriversMap;

public:

  /** Returns the unique instance of OccurrencesFactory (singleton pattern).
  */
  static OccurrencesFactory& instance();
  
  /** Register an OccurrencesReader driver.
   * 
   * @param driverId Driver identifier.
   * @param builder Function pointer to builder function.
   */
  bool registerDriver( const string& driverId, CreateOccurrencesReaderCallback builder );
  
  /** Unregister an OccurrencesReader driver.
   * 
   * @param driverId Driver identifier.
   */
  bool unregisterDriver( const string& driverId );
  
  /** Return registered drivers (vector of driver ids).
   * 
   */
  vector<string> getRegisteredDrivers();

  /** Return an OccurrencesReader given a source string.
   *
   * @param source URL used to locate the occurrences.
   * @param coordSystem coord system.
   */
  OccurrencesReader * create( const char * source, const char * coordSystem );

private:

  // Indicates if the factory was initiated (i.e., drivers were registered).
  static bool _initiated;

  // Map of OccurrencesReader and identifiers.
  DriversMap _drivers;
  
  // No constructor allowed (singleton pattern).
  OccurrencesFactory(){};

  // No copy allowed.
  OccurrencesFactory( const OccurrencesFactory& );

  // No copy allowed.
  OccurrencesFactory& operator=( const OccurrencesFactory& );

  // No destructor allowed.
  ~OccurrencesFactory(){};
};

#endif
