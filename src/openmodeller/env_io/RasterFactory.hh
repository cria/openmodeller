/**
 * Definition of RasterFactory class.
 * 
 * @file
 * @author Alexandre Copertino Jardim <alexcj@dpi.inpe.br>
 * @date 2006-03-21
 * $Id$ 
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

#ifndef _RASTER_FACTORYHH_
#define _RASTER_FACTORYHH_

#include <openmodeller/env_io/RasterGdal.hh>
#include <openmodeller/env_io/Raster.hh>

#include <string>
using std::string;

#include <map>
using std::map;

class MapFormat;

//! Build a Raster.
/**
* Its a Factory pattern to Build a Raster using a raster lib that is specified in a url.
* Register or Unregister a raster lib to use with openModeller.
* Default raster lib is GDAL http://www.remotesensing.org/gdal .
* Its a Singleton Pattern.
*/
class dllexp RasterFactory
{
private:
	//! Function pointer to builder function.
	typedef Raster* (*CreateRasterCallback)();
	//! Map of Rasters and identifiers.
	typedef std::map<string, CreateRasterCallback> CreatorMap;

public:
	//! Singleton pattern
	/**
	* Returns the unique instance of RasterFactory.
	*/
	static RasterFactory& instance();
	
	//! Register a Raster.
	/**
	* \param decId Raster Identifier.
	* \param creator Function pointer to builder function.
	*/
	bool registerRaster(const string& decId, CreateRasterCallback creator);
	
	//! Unregister a Raster.
	bool unregisterRaster(const string& decId);

	//! Build a Raster.
	/**
	* Open an existing raster (read only).
	*/
	Raster* create(const string& url, int categ = 0 );
	
	//! Build a Raster
	/**
	* Create a new file for projections.
	*/
	Raster* create(const string& url, const MapFormat& format );

private:
	//! Map of Rasters and identifiers.
	CreatorMap mapRasters_;
	
	//! No build allowed.
	/**
	* Singleton pattern doesn't allows a public constuctor.
	*/
	RasterFactory(){};
	//! No copy allowed.
	RasterFactory(const RasterFactory&);
	//! No copy allowed.
	RasterFactory& operator=(const RasterFactory&);
	//! No destruct allowed.
	~RasterFactory(){};
};

#endif

