/**
 * Definition of CacheManager class.
 * 
 * @author Renato De Giovanni (renato (at) cria . org . br)
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

#include <openmodeller/CacheManager.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/os_specific.hh>
extern "C" {
#include <openmodeller/ext/md5/md5.h>
}

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>

using namespace std;

/**************************************************************************
 *
 * Implementation of CacheManager class
 *
 *************************************************************************/

/*******************/
/*** constructor ***/

CacheManager::CacheManager() :
  _cacheDir()
{
}

/******************/
/*** destructor ***/

CacheManager::~CacheManager()
{
}

/**************************/
/*** Singleton accessor ***/
CacheManager&
CacheManager::_getInstance()
{
  static CacheManager theInstance;
  return theInstance;
}

/******************/
/*** initialize ***/
void
CacheManager::initialize( const std::string dir )
{
  CacheManager& cm = _getInstance();

  bool use_default = true;

  if ( ! dir.empty() ) {

    cm._cacheDir = dir;
    use_default = false;
  }
  else {

    char *env = getenv( "OM_CACHE_PATH" );

    if ( env != 0 ) {

      string om_cache_path = (char const *)env;

      if ( ! om_cache_path.empty() ) {

        cm._cacheDir = om_cache_path;
        use_default = false;
      }
    }
  }

  if ( use_default ) {

    cm._cacheDir = getWorkingPath();
    cm._cacheDir.append("/cache");
  }

  if ( ! pathExists( cm._cacheDir ) ) {

    createPath( cm._cacheDir );
  }
}

/*****************/
/*** is cached ***/
void
CacheManager::_ensureInitialized()
{
  CacheManager& cm = _getInstance();

  if ( cm._cacheDir.empty() ) {

    cm.initialize();
  }
}

/*****************/
/*** is cached ***/
bool
CacheManager::isCached( const std::string id, const std::string subdir )
{
  _ensureInitialized();

  CacheManager& cm = _getInstance();

  std::string path = cm._cacheDir;

  if ( ! subdir.empty() ) {

    path.append("/");
    path.append(subdir);
  }

  createPath( path );

  path.append("/");
  path.append(id);

  ifstream ifile(path.c_str(), std::ios::in);

  if ( ifile ) {

    return true;
  }

  return false;
}

/*********************/
/*** is cached md5 ***/
bool
CacheManager::isCachedMd5( const std::string id, const std::string subdir )
{
  return isCached( _md5Encode( id ), subdir );
}

/*************/
/*** cache ***/
void
CacheManager::cache( const std::string id, const std::ostringstream& content, const std::string subdir )
{
  _ensureInitialized();

  CacheManager& cm = _getInstance();

  std::string path = cm._cacheDir;

  if ( ! subdir.empty() ) {

    path.append("/");
    path.append(subdir);
  }

  createPath( path );

  path.append("/");
  path.append(id);

  ofstream ofile( path.c_str() );

  if ( ofile ) {

    ofile << content.str();
  }
  else {

    Log::instance()->error( "Could not cache content for %s.\n", id.c_str() );
    throw FileIOException( "Could not cache content", id );
  }
}

/*****************/
/*** cache md5 ***/
void
CacheManager::cacheMd5( const std::string id, const std::ostringstream& content, const std::string subdir )
{
  cache( _md5Encode( id ), content, subdir );
}

/****************************/
/*** get Content Location ***/
std::string
CacheManager::getContentLocation( const std::string id, const std::string subdir )
{
  _ensureInitialized();

  CacheManager& cm = _getInstance();

  std::string path = cm._cacheDir;

  if ( ! subdir.empty() ) {

    path.append("/");
    path.append(subdir);
  }

  path.append("/");
  path.append(id);

  return path;
}

/****************************/
/*** get Content Location ***/
std::string
CacheManager::getContentLocationMd5( const std::string id, const std::string subdir )
{
  return getContentLocation( _md5Encode( id ), subdir );
}

/*************************/
/*** get Friendly Hash ***/
std::string
CacheManager::_md5Encode( const std::string id )
{
  Log::instance()->debug( "Original string: %s.\n", id.c_str() );

  size_t MYSIZE = 16;

  unsigned char digest[MYSIZE];

  MD5_CTX md5;
  MD5_Init( &md5 );
  MD5_Update( &md5, const_cast<char *>( id.c_str() ), id.size() );
  MD5_Final( digest, &md5 );

  char buffer[(MYSIZE*2)+1];

  for (unsigned int i = 0; i < MYSIZE; ++i ) {

    sprintf(&buffer[i*2], "%02x", (unsigned int)digest[i]);
  }

  std::string md5_id( buffer );

  Log::instance()->debug( "Hash: %s.\n", md5_id.c_str() );

  return md5_id;
}