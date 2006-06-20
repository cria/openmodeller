/**
 * Definition of RasterFactory class.
 * 
 * @file
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

/**
* Singleton pattern
*/
RasterFactory& 
RasterFactory::instance()
{
	static RasterFactory unique_;
	return unique_;
}

/**
* Register a Raster Lib.
*/
bool 
RasterFactory::registerRaster(const string& decId, CreateRasterCallback creator)
{
	return mapRasters_.insert(CreatorMap::value_type(decId,creator)).second;
}

/**
* Unregister a Raster Lib.
*/
bool 
RasterFactory::unregisterRaster(const string& decId)
{
	return mapRasters_.erase(decId) != 0;
}


/**
* Build GDAL, TerraLib or another Raster lib.
*/
Raster*
RasterFactory::create( const string& url, int categ )
{
	// Another Raster Lib
	int i = url.find( ">" );
	if( i != -1)
	{
		string libName = url.substr( 0, i );
		CreatorMap::const_iterator i = mapRasters_.find( libName );
		if ( i != mapRasters_.end())
		{
			Raster* r = (i->second)();
			r->createRaster( url, categ );
			return r;
		}
	}
	
	// Default: GDAL Raster Lib
	return new RasterGdal( url, categ );
}

Raster* 
RasterFactory::create( const string& url, const MapFormat& format )
{
	// Another Raster Lib
	int i = url.find( ">" );
	if( i != -1)
	{
		string libName = url.substr( 0, i );
		CreatorMap::const_iterator i = mapRasters_.find( libName );
		if ( i != mapRasters_.end())
		{
			Raster* r = (i->second)();
			r->createRaster( url, format );
			return r;
		}
	}
	
	// Default: GDAL Raster Lib
	return new RasterGdal( url, format );
}
