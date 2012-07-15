/**
 * Definition of WcsProxyRaster class.
 * 
 * @author Renato De Giovanni <renato (at) cria . org . br>
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2012 by CRIA -
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

#include <openmodeller/env_io/WcsProxyRaster.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/MapFormat.hh>
#include <openmodeller/CacheManager.hh>

#include <string.h>
#include <sstream>

using namespace std;

#ifdef MPI_FOUND
#include "mpi.h"
#endif

/****************************************************************/
/*********************** WCS Proxy Raster ***********************/

/******************************/
/*** create Raster Callback ***/
Raster*
WcsProxyRaster::CreateRasterCallback()
{
  return new WcsProxyRaster();
}

/*******************/
/*** constructor ***/
WcsProxyRaster::WcsProxyRaster()
{
  f_gdal = 0;
}

/*****************/
/*** destructor ***/
WcsProxyRaster::~WcsProxyRaster()
{
  if ( f_gdal ) {

    delete f_gdal;
  }
}

/*********************/
/*** create Raster ***/
void 
WcsProxyRaster::createRaster( const string& str, int categ )
{
  if ( !CacheManager::isCachedMd5( str, "wcs" ) ) {

    // Extract tokens from identifier
    vector<string> tokens;
    stringstream ss(str);
    string token;
    while ( getline(ss, token, '>') ) {

      tokens.push_back(token);
    }

    if ( tokens.size() != 3 ) {

      std::string msg = "Invalid WCS identifier. Make sure it has 3 parts separated by \">\".\n";
      Log::instance()->error( msg.c_str() );
      throw RasterException( msg.c_str() );
    }

    // Create GDAL WCS XML file
    ostringstream oss (ostringstream::out);
    oss<<"<WCS_GDAL>"<<std::endl;
    oss<<"<ServiceURL>"<<tokens[1]<<"</ServiceURL>"<<std::endl;
    oss<<"<CoverageName>"<<tokens[2]<<"</CoverageName>"<<std::endl;
    oss<<"</WCS_GDAL>";

    // Then cache it
    CacheManager::cacheMd5( str, oss, "wcs" );
  }

  string cached_ref = CacheManager::getContentLocationMd5( str, "wcs" );

  f_gdal = new GdalRaster( cached_ref, categ );
}

#ifdef MPI_FOUND
void
WcsProxyRaster::createRaster( const string& output_file, const string& file, const MapFormat& format) {

  std::string msg = "Method createRaster() not available to create writable WCS rasters.\n";
  Log::instance()->error( msg.c_str() );
  throw RasterException( msg.c_str() );
}

#else
void
WcsProxyRaster::createRaster( const string& file, const MapFormat& format) {

  std::string msg = "Method createRaster() not available to create writable WCS rasters.\n";
  Log::instance()->warn( msg.c_str() );
  throw RasterException( msg.c_str() );
}
#endif

/***********/
/*** get ***/
int
WcsProxyRaster::get( Coord px, Coord py, Scalar *val )
{
  return f_gdal->get(px, py, val);
}

/***********/
/*** put ***/
int
WcsProxyRaster::put( Coord px, Coord py, Scalar val )
{
  std::string msg = "Method put() not available for WCS rasters.\n";
  Log::instance()->warn( msg.c_str() );
  throw RasterException( msg.c_str() );
}

/***********/
/*** put ***/
int
WcsProxyRaster::put( Coord px, Coord py )
{
  std::string msg = "Method put() not available for WCS rasters.\n";
  Log::instance()->warn( msg.c_str() );
  throw RasterException( msg.c_str() );
}

/*******************/
/*** get Min Max ***/
int
WcsProxyRaster::getMinMax( Scalar *min, Scalar *max )
{
  return f_gdal->getMinMax(min, max);
}

/**************/
/*** finish ***/
void 
WcsProxyRaster::finish()
{
  std::string msg = "Method finish() not available for WCS rasters.\n";
  Log::instance()->warn( msg.c_str() );
  throw RasterException( msg.c_str() );
}

/*********************/
/*** delete Raster ***/
int
WcsProxyRaster::deleteRaster()
{
  std::string msg = "Method deleteRaster() not available for WCS rasters.\n";
  Log::instance()->warn( msg.c_str() );
  throw RasterException( msg.c_str() );
}
