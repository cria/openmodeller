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
#include <openmodeller/Settings.hh>

#include <vector>
#include <string.h>
#include <sstream>
#include <algorithm>

using namespace std;

#include <curl/curl.h>

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

      if ( pos == host.size() - accepted_sources[i].size() ) {

        // Configured source must match the end of the host
        return false;
      }
    }
  }

  return true;
}

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

  // Check if file is being downloaded (presence of lock file)
  std::string lock_file = cache_id;
  lock_file.append(".lock");

  if ( CacheManager::isCached( lock_file, OM_REMOTE_RASTER_SUBDIR ) ) {

    Log::instance()->debug( "Ongoing concurrent download\n" );
    throw RasterException( "Ongoing concurrent download", 1 );
  }

  if ( CacheManager::isCachedMd5( str, OM_REMOTE_RASTER_SUBDIR ) ) {

    Log::instance()->debug( "Layer %s already present in local cache (%s)\n", str.c_str(), cache_id.c_str() );

    GdalRaster::createRaster( cached_ref, categ );

    // TODO: send HEADER request to check if remote file has changed.
    //       (or always try to retrieve file anyway, but including If-Modified-Since and handling 304 code)
  }
  else {

    if ( isFromRejectedSource( str ) ) {

      std::string msg = "Untrusted source for remote raster. Aborting operation.\n";
      Log::instance()->error( msg.c_str() );
      throw RasterException( msg.c_str() );
    }

    std::string retries_file = cache_id;
    retries_file.append(".tries");

    std::string retries_fullpath = CacheManager::getContentLocation( retries_file, OM_REMOTE_RASTER_SUBDIR );
    int num_retries = 0;

    // Check number of previous attempts
    if ( CacheManager::isCached( retries_file, OM_REMOTE_RASTER_SUBDIR ) ) {

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

        CacheManager::eraseCache( retries_file, OM_REMOTE_RASTER_SUBDIR );
        throw RasterException( "Too many attempts to fetch raster. Aborting." );
      }
    }

    // Fetch file
    try {

      // Last minute double check
      if ( CacheManager::isCached( lock_file, OM_REMOTE_RASTER_SUBDIR ) ) {

        throw RasterException( "Ongoing concurrent download", 1 );
      }

      // Create lock file
      ostringstream oss (ostringstream::out);
      CacheManager::cache( lock_file, oss, OM_REMOTE_RASTER_SUBDIR );

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
      Log::instance()->debug( "Fetching remote raster %s (%s)...\n", str.c_str(), cache_id.c_str() );

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
        //curl_easy_setopt( curl, CURLOPT_VERBOSE, 1 );

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

      Log::instance()->debug( "Done!\n", str.c_str(), cache_id.c_str() );

      GdalRaster::createRaster( cached_ref, categ );

      // Erase lock and retries
      CacheManager::eraseCache( lock_file, OM_REMOTE_RASTER_SUBDIR );
      CacheManager::eraseCache( retries_file, OM_REMOTE_RASTER_SUBDIR );
    }
    catch ( RasterException& e ) {

      if ( e.getCode() != 1 ) {

        // Erase lock
        CacheManager::eraseCache( lock_file, OM_REMOTE_RASTER_SUBDIR );
      }

      throw;
    }
    catch (...) {

      // Erase lock
      CacheManager::eraseCache( lock_file, OM_REMOTE_RASTER_SUBDIR );
      throw;
    }
  }
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

/*******************/
/*** _write Data ***/
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
