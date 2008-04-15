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

bool OccurrencesFactory::_initiated;

/****************/
/*** instance ***/
OccurrencesFactory& 
OccurrencesFactory::instance()
{
  static OccurrencesFactory _instance;

  if ( ! _initiated ) {

#ifdef TERRALIB_FOUND
  _instance.registerDriver( "terralib", &TeOccurrences::CreateOccurrencesReaderCallback );
#endif

#ifdef CURL_FOUND
  _instance.registerDriver( "tapir", &TapirOccurrences::CreateOccurrencesReaderCallback );
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

/**************/
/*** create ***/
OccurrencesReader*
OccurrencesFactory::create( const char * source, const char * coordSystem )
{
  string source_str( source );

  int i = source_str.find( "terralib>" );

  if ( i != -1 ) {

    DriversMap::const_iterator i = _drivers.find( "terralib" );

    if ( i != _drivers.end() ) {

      OccurrencesReader * te_driver = (i->second)( source, coordSystem );
      te_driver->load();

      return te_driver;
    }
  }

  i = source_str.find( "http://" );

  if ( i == 0 ) {

    DriversMap::const_iterator i = _drivers.find( "tapir" );

    if ( i != _drivers.end() ) {

      OccurrencesReader * tapir_driver = (i->second)( source, coordSystem );

      if ( tapir_driver->load() ) {

        return tapir_driver;
      }
    }
  }

  // Default driver
  OccurrencesFile * file_driver = new OccurrencesFile( source, coordSystem );

  file_driver->load();

  return file_driver;
}
