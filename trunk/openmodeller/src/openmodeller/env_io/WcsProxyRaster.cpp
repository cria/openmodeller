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
#include <algorithm>

using namespace std;

#ifdef MPI_FOUND
#include "mpi.h"
#endif

/*********************************************************/
/*********************** Functions ***********************/

/*******************************/
/*** is From Rejected Source ***/
bool
isFromRejectedSource( const std::string& str ) {

  // Remote sources are untrusted by default

  if ( Settings::count( "ALLOW_RASTER_SOURCE" ) > 0 ) {

    // get host from url
    string host;
    string lower_url;
    transform( str.begin(), str.end(), std::back_inserter(lower_url), ::tolower );

    if ( str.size() < 9 ) { // ftp://x.x

      std::string msg = "Invalid identifier for remote raster (1).\n";
      Log::instance()->error( msg.c_str() );
      throw RasterException( msg.c_str() );
    }

    size_t prot_i = str.find("://");

    if ( prot_i == string::npos ) {

      std::string msg = "Missing protocol in remote raster identifier.\n";
      Log::instance()->error( msg.c_str() );
      throw RasterException( msg.c_str() );
    }

    size_t path_i = str.find("/", prot_i+3);

    if ( path_i == string::npos ) {

      // There must be at least one slash in the identifier! 
      std::string msg = "Invalid identifier for remote raster (2).\n";
      Log::instance()->error( msg.c_str() );
      throw RasterException( msg.c_str() );
    }

    host = lower_url.substr( prot_i+3, path_i - (prot_i+3) );

    size_t port_i = host.find(":");

    if ( port_i != string::npos ) {

      // Ignore port
      host = host.substr( 0, port_i );
    }

    vector<string> accepted_sources = Settings::getAll( "ALLOW_RASTER_SOURCE" );

    for( unsigned int i = 0; i < accepted_sources.size(); i++ ) {

      // This is how you can accept any source (don't do that)
      if ( accepted_sources[i].compare("*") == 0 ) {

        return false;
      }

      size_t pos = host.find( accepted_sources[i] );

      if ( pos == string::npos ) {

        continue;
      }

      if ( pos == host.size() - accepted_sources[i].size() ) {

        // Configured source must match the end of the host
        return false;
      }
    }
  }

  return true;
}

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

    if ( isFromRejectedSource( tokens[1] ) ) {

      std::string msg = "Untrusted source for WCS raster. Aborting operation.\n";
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

    Log::instance()->debug( "openModeller configured to work with local copies of WCS rasters\n" );

    std::string clone_id( cache_id );
    clone_id.append(".img");

    std::string clone_ref( cached_ref );
    clone_ref.append(".img");

    // Check if file is being downloaded (presence of lock file)
    std::string lock_file = cache_id;
    lock_file.append(".lock");

    if ( CacheManager::isCached( lock_file, OM_WCS_PROXY_SUBDIR ) ) {

      Log::instance()->debug( "Ongoing concurrent download\n" );
      throw RasterException( "Ongoing concurrent download", 1 );
    }

    if ( CacheManager::isCached( clone_id, OM_WCS_PROXY_SUBDIR ) ) {

      Log::instance()->debug( "Local WCS copy already present: %s\n", clone_ref.c_str() );

      GdalRaster::createRaster( clone_ref, categ );
    }
    else {

      Log::instance()->debug( "Local WCS copy does not exist\n" );

      std::string retries_file = cache_id;
      retries_file.append(".tries");

      std::string retries_fullpath = CacheManager::getContentLocation( retries_file, OM_WCS_PROXY_SUBDIR );
      int num_retries = 0;

      // Check number of previous attempts
      if ( CacheManager::isCached( retries_file, OM_WCS_PROXY_SUBDIR ) ) {

        fstream fin;
        fin.open( retries_fullpath.c_str(), ios::in );

        if ( fin.is_open() ) {

          ostringstream oss;
          string line;

          getline( fin, line );
          oss << line << endl;

          // Note: if the content is empty, atoi returns 0
          num_retries = atoi( oss.str().c_str() );

          fin.close();
        }
        else {

          throw RasterException( "Could not determine number of previous download retries." );
        }

        if ( num_retries > 3 ) {

          // Without removing the file, it will need to be manually removed before trying again!
          CacheManager::eraseCache( retries_file, OM_WCS_PROXY_SUBDIR );
          throw RasterException( "Too many attempts to fetch raster. Aborting." );
        }
      }

      // Fetch file
      try {

        // Last minute double check
        if ( CacheManager::isCached( lock_file, OM_WCS_PROXY_SUBDIR ) ) {

          throw RasterException( "Ongoing concurrent download", 1 );
        }

        // Create lock file
        ostringstream oss (ostringstream::out);
        CacheManager::cache( lock_file, oss, OM_WCS_PROXY_SUBDIR );

        // Increase number of retries
        FILE *p_file = NULL;
        p_file = fopen( retries_fullpath.c_str(), "w" );

        if ( p_file == NULL ) {

          // Could not open file
          throw RasterException( "Could not store number of download retries." );
        }
        else {

          ++num_retries;
          char buffer[2];
          sprintf( buffer, "%d", num_retries );
          fputs( buffer, p_file );
          fclose( p_file );
        }

        // Finally fetch raster
        Log::instance()->debug( "Fetching WCS raster...\n" );

        GDALAllRegister();

        // Open source raster 
        GDALDatasetH hDataset = GDALOpen( cached_ref.c_str(), GA_ReadOnly );
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

        Log::instance()->debug( "Done!\n" );
    
        GdalRaster::createRaster( clone_ref, categ );

        // Erase lock and retries
        CacheManager::eraseCache( lock_file, OM_WCS_PROXY_SUBDIR );
        CacheManager::eraseCache( retries_file, OM_WCS_PROXY_SUBDIR );
      }
      catch ( RasterException& e ) {

        if ( e.getCode() != 1 ) {

          // Erase lock
          CacheManager::eraseCache( lock_file, OM_WCS_PROXY_SUBDIR );
        }

        throw;
      }
      catch (...) {

        // Erase lock
        CacheManager::eraseCache( lock_file, OM_WCS_PROXY_SUBDIR );
        throw;
      }
    }
  }
  else {

    Log::instance()->debug( "openModeller configured to work with remote WCS rasters\n" );

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
