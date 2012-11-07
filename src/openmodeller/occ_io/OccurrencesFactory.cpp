/**
 * Definition of OccurrencesFactory class.
 * 
 * @author Alexandre Copertino Jardim <alexcj@dpi.inpe.br>
 * @date 2006-03-21
 * $Id$ 
 * 
 *
 * LICENSE INFORMATION
 * 
 * Copyright © 2006 INPE
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

#include <openmodeller/occ_io/OccurrencesFactory.hh>

#include <openmodeller/Log.hh>
#include <openmodeller/Exceptions.hh>

bool OccurrencesFactory::_initiated;

/****************/
/*** instance ***/
OccurrencesFactory& 
OccurrencesFactory::instance()
{
  static OccurrencesFactory _instance;

  if ( ! _initiated ) {

  _instance.registerDriver( "TXT", &DelimitedTextOccurrences::CreateOccurrencesReaderCallback );
  _instance.registerDriver( "XML", &SerializedXmlOccurrences::CreateOccurrencesReaderCallback );

#ifdef TERRALIB_FOUND
  _instance.registerDriver( "TerraLib", &TeOccurrences::CreateOccurrencesReaderCallback );
#endif

#ifdef CURL_FOUND
  _instance.registerDriver( "TAPIR", &TapirOccurrences::CreateOccurrencesReaderCallback );
  _instance.registerDriver( "GBIF" , &GbifOccurrences::CreateOccurrencesReaderCallback );
#endif

    _initiated = true;
  }

  return _instance;
}

/***********************/
/*** register driver ***/
bool 
OccurrencesFactory::registerDriver( const string& driverId, CreateOccurrencesReaderCallback builder )
{
  return _drivers.insert( DriversMap::value_type( driverId, builder ) ).second;
}

/*************************/
/*** unregister driver ***/
bool 
OccurrencesFactory::unregisterDriver( const string& driverId )
{
  return _drivers.erase( driverId ) != 0;
}

/******************************/
/*** get registered drivers ***/
vector<string>
OccurrencesFactory::getRegisteredDrivers()
{
  vector<string> driver_ids;

  DriversMap::const_iterator d = _drivers.begin();
  DriversMap::const_iterator end = _drivers.end();

  while ( d != end ) {

    driver_ids.push_back( d->first );

    ++d;
  }

  return driver_ids;
}

/**************/
/*** create ***/
OccurrencesReader*
OccurrencesFactory::create( const char * source, const char * coordSystem )
{
  string source_str( source );

  // It must be TerraLib point data if source starts with "terralib>"
  int i = source_str.find( "terralib>" );

  DriversMap::const_iterator di;

  if ( i == 0 ) {

    di = _drivers.find( "TerraLib" );

    if ( di != _drivers.end() ) {

      OccurrencesReader * te_driver = (di->second)( source, coordSystem );
      te_driver->load();

      return te_driver;
    }
    else {

	throw OccurrencesReaderException( "TerraLib driver not found" );
    }
  }

  // It must be GBIF REST if source is the GBIF REST URL for occurrences
  if ( source_str == "http://data.gbif.org/ws/rest/occurrence/list" ) {

    di = _drivers.find( "GBIF" );

    if ( di != _drivers.end() ) {

      OccurrencesReader * gbif_driver = (di->second)( source, coordSystem );
      gbif_driver->load();

      return gbif_driver;
    }
    else {

	throw OccurrencesReaderException( "GBIF driver not found" );
    }
  }

  i = source_str.find( "http://" );

  if ( i == 0 ) {

    // Try TAPIR driver first
    di = _drivers.find( "TAPIR" );

    if ( di != _drivers.end() ) {

      OccurrencesReader * tapir_driver = (di->second)( source, coordSystem );

      if ( tapir_driver->load() ) {

        return tapir_driver;
      }
    }

    // Then try GBIF (in case they changed the URL or if someone else decided to 
    // implement a service using the GBIF REST protocol)
    di = _drivers.find( "GBIF" );

    if ( di != _drivers.end() ) {

      OccurrencesReader * gbif_driver = (di->second)( source, coordSystem );

      if ( gbif_driver->load() ) {

        return gbif_driver;
      }
    }
  }

  // Try to open as XML file
  try {

    Log::instance()->debug( "Trying to open occurrences source as XML file.\n" );

    OccurrencesReader * xml_driver = new SerializedXmlOccurrences( source, coordSystem );

    if ( xml_driver->load() ) {

      return xml_driver;
    }

    delete xml_driver;
  }
  catch ( OmException& e ) {

    Log::instance()->debug( "XML occurrences reader exception: %s\n", e.what() );
  }

  Log::instance()->debug( "Trying to open occurrences source as TXT file.\n" );

  // Default driver for delimited text
  OccurrencesReader * file_driver = new DelimitedTextOccurrences( source, coordSystem );

  file_driver->load();

  return file_driver;
}
