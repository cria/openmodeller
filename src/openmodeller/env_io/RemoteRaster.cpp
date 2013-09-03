/**
 * Definition of RemoteRaster class.
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

#include <openmodeller/env_io/RemoteRaster.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/MapFormat.hh>
#include <openmodeller/CacheManager.hh>

#include <string.h>
#include <sstream>

using namespace std;

#include <curl/curl.h>

#ifdef MPI_FOUND
#include "mpi.h"
#endif

/*************************************************************/
/*********************** Remote Raster ***********************/

/******************/
/*** Destructor ***/
RemoteRaster::~RemoteRaster()
{
}

/******************************/
/*** create Raster Callback ***/
Raster*
RemoteRaster::CreateRasterCallback()
{
  return new RemoteRaster();
}

/*********************/
/*** create Raster ***/
void 
RemoteRaster::createRaster( const string& str, int categ )
{
  string cached_ref = CacheManager::getContentLocationMd5( str, OM_REMOTE_RASTER_SUBDIR );
  string cache_id = CacheManager::getContentIdMd5( str );

  if ( CacheManager::isCachedMd5( str, OM_REMOTE_RASTER_SUBDIR ) ) {

    Log::instance()->debug( "Layer %s already present in local cache (%s)\n", str.c_str(), cache_id.c_str() );
  }
  else {

    Log::instance()->debug( "Fetching remote raster %s (%s)...\r", str.c_str(), cache_id.c_str() );

    CURL *curl;

    static CacheFile file_data;
    file_data.fileName = cached_ref.c_str();
    file_data.stream = NULL;

    curl_global_init( CURL_GLOBAL_DEFAULT );

    curl = curl_easy_init();

    if ( curl ) {

      if ( CURLE_OK != curl_easy_setopt( curl, CURLOPT_URL, str.c_str() ) ) {

        std::string msg = "Could not configure remote raster fetcher.\n";
        Log::instance()->error( msg.c_str() );
        throw RasterException( msg.c_str() );
      }

      // Enable following redirections
      curl_easy_setopt( curl, CURLOPT_FOLLOWLOCATION, 1 );
      curl_easy_setopt( curl, CURLOPT_MAXREDIRS, 5 );

      // Timeout (30s)
      curl_easy_setopt( curl, CURLOPT_TIMEOUT, 30 );

      /* NOSIGNAL should be set to true for timeout to work in multithread
       * environments on Unix, requires libcurl 7.10 or more recent.
       * (this force avoiding the use of signal handlers)
       */
#ifdef CURLOPT_NOSIGNAL
      curl_easy_setopt( curl, CURLOPT_NOSIGNAL, 1 );
#endif

      curl_easy_setopt( curl, CURLOPT_WRITEFUNCTION, &RemoteRaster::_writeData );
      curl_easy_setopt( curl, CURLOPT_WRITEDATA, &file_data );

      CURLcode res = curl_easy_perform( curl );

      curl_easy_cleanup( curl );

      if ( file_data.stream ) {

        fclose( file_data.stream );
      }

      curl_global_cleanup();

      if ( CURLE_OK != res ) {

        std::string msg = "Could not fetch remote raster.\n";
        Log::instance()->error( msg.c_str() );
        throw RasterException( msg.c_str() );
      }
    }
    else {

      std::string msg = "Could not initialize remote raster fetcher.\n";
      Log::instance()->error( msg.c_str() );
      throw RasterException( msg.c_str() );
    }

    Log::instance()->debug( "Fetching remote raster %s (%s)...done\n", str.c_str(), cache_id.c_str() );
  }

  GdalRaster::createRaster( cached_ref, categ );
}

#ifdef MPI_FOUND
void
RemoteRaster::createRaster( const string& output_file, const string& file, const MapFormat& format) {

  std::string msg = "Method createRaster() not available to create writable remote rasters.\n";
  Log::instance()->error( msg.c_str() );
  throw RasterException( msg.c_str() );
}

#else
void
RemoteRaster::createRaster( const string& file, const MapFormat& format) {

  std::string msg = "Method createRaster() not available to create writable remote rasters.\n";
  Log::instance()->warn( msg.c_str() );
  throw RasterException( msg.c_str() );
}
#endif

/***********/
/*** put ***/
int
RemoteRaster::put( Coord px, Coord py, Scalar val )
{
  std::string msg = "Method put() not available for remote rasters.\n";
  Log::instance()->warn( msg.c_str() );
  throw RasterException( msg.c_str() );
}

/***********/
/*** put ***/
int
RemoteRaster::put( Coord px, Coord py )
{
  std::string msg = "Method put() not available for remote rasters.\n";
  Log::instance()->warn( msg.c_str() );
  throw RasterException( msg.c_str() );
}

/**************/
/*** finish ***/
void 
RemoteRaster::finish()
{
  std::string msg = "Method finish() not available for remote rasters.\n";
  Log::instance()->warn( msg.c_str() );
  throw RasterException( msg.c_str() );
}

/*********************/
/*** delete Raster ***/
int
RemoteRaster::deleteRaster()
{
  std::string msg = "Method deleteRaster() not available for remote rasters.\n";
  Log::instance()->warn( msg.c_str() );
  throw RasterException( msg.c_str() );
}

/*************************/
/*** Callback for curl ***/
size_t
RemoteRaster::_writeData( void *buffer, size_t size, size_t nmemb, void *stream ) {

  struct CacheFile *out = reinterpret_cast<CacheFile *>(stream);

  if ( out && !out->stream ) {

    out->stream = fopen( out->fileName, "wb" );

    if ( ! out->stream ) {

      return -1;
    }
  }

  return fwrite( buffer, size, nmemb, out->stream );
}
