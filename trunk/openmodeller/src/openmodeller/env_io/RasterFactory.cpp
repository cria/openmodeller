/**
 * Definition of RasterFactory class.
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

#include <openmodeller/env_io/RasterFactory.hh>
#include <openmodeller/MapFormat.hh>

#include <string>
using std::string;

bool RasterFactory::_initiated;

/****************/
/*** instance ***/
RasterFactory& 
RasterFactory::instance()
{
  static RasterFactory _instance;

  if ( ! _initiated ) {

#ifdef TERRALIB_FOUND
_instance.registerDriver( "terralib", &TerralibRaster::CreateRasterCallback );
#endif

    _instance.registerDriver( "wcs", &WcsProxyRaster::CreateRasterCallback );

    _initiated = true;
  }

  return _instance;
}

/***********************/
/*** register driver ***/
bool 
RasterFactory::registerDriver( const string& driverId, CreateRasterCallback builder )
{
  return _drivers.insert( DriversMap::value_type( driverId, builder ) ).second;
}

/*************************/
/*** unregister driver ***/
bool 
RasterFactory::unregisterDriver( const string& driverId )
{
  return _drivers.erase( driverId ) != 0;
}

/**************/
/*** create ***/
Raster*
RasterFactory::create( const string& source, int categ )
{
  // Another Raster Lib
  int i = source.find( ">" );

  if ( i != -1 ) {

    string driver_id = source.substr( 0, i );

    DriversMap::const_iterator i = _drivers.find( driver_id );

    if ( i != _drivers.end() ) {

      Raster* r = (i->second)();
      r->createRaster( source, categ );
      return r;
    }
  }
  
  // Default: GDAL Raster Lib
  return new GdalRaster( source, categ );
}


#ifdef MPI_FOUND
/**************/
/*** create ***/
Raster*
RasterFactory::create( const string& output_file_source, const string& source, const MapFormat& format )
{
  // Another Raster Lib
  int i = source.find( ">" );

  if ( i != -1 ) {

    string driver_id = source.substr( 0, i );

    DriversMap::const_iterator i = _drivers.find( driver_id );

    if ( i != _drivers.end() ) {

      Raster* r = (i->second)();
      r->createRaster( source, format );
      return r;
    }
  }

  // Default: GDAL Raster Lib
  return new GdalRaster( output_file_source, source, format );
}
#else
/**************/
/*** create ***/
Raster* 
RasterFactory::create( const string& source, const MapFormat& format )
{
  // Another Raster Lib
  int i = source.find( ">" );

  if ( i != -1 ) {

    string driver_id = source.substr( 0, i );

    DriversMap::const_iterator i = _drivers.find( driver_id );

    if ( i != _drivers.end() ) {

      Raster* r = (i->second)();
      r->createRaster( source, format );
      return r;
    }
  }
  
  // Default: GDAL Raster Lib
  return new GdalRaster( source, format );
}
#endif
