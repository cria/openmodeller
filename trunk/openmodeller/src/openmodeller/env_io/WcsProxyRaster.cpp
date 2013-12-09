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
#include <openmodeller/Settings.hh>

#include <gdal.h>

#include <string.h>
#include <sstream>

using namespace std;

#ifdef MPI_FOUND
#include "mpi.h"
#endif

/****************************************************************/
/*********************** WCS Proxy Raster ***********************/

/******************/
/*** Destructor ***/
WcsProxyRaster::~WcsProxyRaster()
{
}

/******************************/
/*** create Raster Callback ***/
Raster*
WcsProxyRaster::CreateRasterCallback()
{
  return new WcsProxyRaster();
}

/*********************/
/*** create Raster ***/
void 
WcsProxyRaster::createRaster( const string& str, int categ )
{
  string cache_id = CacheManager::getContentIdMd5( str );

  ///// Handle local proxy WCS file ///// 

  if ( CacheManager::isCachedMd5( str, OM_WCS_PROXY_SUBDIR ) ) {

    Log::instance()->debug( "WCS proxy raster %s already present in local cache (%s)\n", str.c_str(), cache_id.c_str() );
  }
  else {

    Log::instance()->debug( "Setting up WCS proxy for %s (%s)\n", str.c_str(), cache_id.c_str() );

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
    CacheManager::cacheMd5( str, oss, OM_WCS_PROXY_SUBDIR );
  }

  string cached_ref = CacheManager::getContentLocationMd5( str, OM_WCS_PROXY_SUBDIR );

  ///// Should we work with entire local copies ///// 
  if ( Settings::count( "FETCH_WCS" ) == 1 && Settings::get( "FETCH_WCS" ) == "true" ) {

    Log::instance()->debug( "openModeller configured to fetch WCS\n" );

    std::string clone_id( cache_id );
    clone_id.append(".img");

    std::string clone_ref( cached_ref );
    clone_ref.append(".img");

    if ( CacheManager::isCached( clone_id, OM_WCS_PROXY_SUBDIR ) ) {

      Log::instance()->debug( "Local WCS copy already present: %s\n", clone_ref.c_str() );
    }
    else {

      Log::instance()->debug( "Local WCS copy does not exist. Creating  %s\n", clone_ref.c_str() );

      // Open source raster 
      GDALDatasetH hDataset = GDALOpenShared( cached_ref.c_str(), GA_ReadOnly );
      if ( hDataset == NULL ) {

        std::string msg = "Failed to open WCS raster!\n";
        Log::instance()->error( msg.c_str() );
        throw RasterException( msg.c_str() );
      }

      GDALDriverH hDriver = GDALGetDriverByName( "HFA" ); // Use Erdas Imagine
      if ( hDriver == NULL ) {

        GDALClose( hDataset );
        GDALDestroyDriverManager();

        std::string msg = "Could not find GDAL HFA driver!\n";
        Log::instance()->error( msg.c_str() );
        throw RasterException( msg.c_str() );
      }

      // Copy WCS proxy to HFA
      GDALDatasetH hOutDS = GDALCreateCopy( hDriver, clone_ref.c_str(), hDataset, FALSE, NULL, NULL, NULL );

      if ( hOutDS == NULL ) {

        GDALClose( hDataset );
        GDALDestroyDriverManager();

        std::string msg = "Could not clone WCS raster!\n";
        Log::instance()->error( msg.c_str() );
        throw RasterException( msg.c_str() );
      }

      GDALClose( hOutDS );
      GDALClose( hDataset );

      Log::instance()->debug( "Finished creating local WCS copy %s\n", clone_ref.c_str() );
    }

    GdalRaster::createRaster( clone_ref, categ );
  }
  else {

    Log::instance()->debug( "openModeller configured to work with remote WCS\n" );

    GdalRaster::createRaster( cached_ref, categ );
  }
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
