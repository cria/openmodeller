/**
 * Definitions of operating system's specific functions.
 * 
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2004-03-19
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
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

#include <os_specific.hh>
#include <openmodeller/Log.hh>

#include <iostream>
#include <stdlib.h>

using std::vector;
using std::string;
#if defined(__APPLE__)
//for getting app bundle path
    
#include <CoreFoundation/CoreFoundation.h>
#include <ApplicationServices/ApplicationServices.h>
#endif

#include <fstream>

/****************************************************************/
/********************* Dynamic Linking Loader *******************/

#include <dlfcn.h>


/****************/
/*** dll Open ***/
DLLHandle
dllOpen( char const *dll_file_name )
{
  return dlopen( dll_file_name, RTLD_NOW );
}


/********************/
/*** dll Function ***/
void *
dllFunction( DLLHandle handle, char const *function_name )
{
  return dlsym( handle, function_name );
}


/*****************/
/*** dll Close ***/
int
dllClose( DLLHandle handle )
{
  return dlclose( handle );
}


/*****************/
/*** dll Error ***/
const char *
dllError( DLLHandle )
{
  return dlerror();
}

/*************************/
/*** initial Plugin Path ***/
vector<string>
initialPluginPath()
{
  Log::instance()->debug( "Determining algorithm paths\n" );

  vector<string> entries;

  //
  // Order of initialization:
  //
  // 1) environment variable: OM_ALG_PATH
  // 2) read from CONFIG_FILE compiled constant.
  // 3) PLUGINPATH compiled constant.
  // 4) on mac <application bundle>.app/Contents/MacOS/algs

  char *env = getenv( "OM_ALG_PATH" );

  //
  // Check if the environment variable is set
  if ( env != 0 ) {

    string envpath( (char const *)env );

    Log::instance()->debug( "Found environment variable OM_ALG_PATH: %s\n", envpath.c_str() );

    // If it's set to "" then we return.  Don't know what this means since the
    // path is emtpy.  Maybe we need to have this drop to using CONFIG_FILE
    if ( envpath.empty() ) {

      return entries;
    }

    // Parse the OM_ALG_PATH with colon (':') delimiters just like all other 
    // unix path structures.

    // string::size_type start marks the beginning of the substring.
    // initial value is beginning of string, iterate value is one past the ':'
    for ( string::size_type start = 0; start < envpath.length() ; ) {
      
      // Find the next ':' after start
      string::size_type it = envpath.find( ':', start );

      // If no ':' is found..
      if ( it == string::npos ) {

        // the substring is (start, end-of-string)
        entries.push_back( envpath.substr( start ) );
        break;
      }
      // Else, test that the substring is non empty.
      else if ( it > start ) {
        
        string::size_type len = it - start;
        entries.push_back( envpath.substr( start, len ) );
      }

      // move the start of the next substring to one after the ':'
      start = it+1;
    }

    return entries;
  }

  Log::instance()->debug( "Checking CONFIG_FILE constant: " CONFIG_FILE "\n" );

  std::ifstream conf_file( CONFIG_FILE, std::ios::in );
  
  if ( conf_file ) {

    Log::instance()->debug( "Found config file\n" );

    while ( conf_file ) {

      string line;
      getline( conf_file, line );
      entries.push_back( line );
    }

    return entries;
  }

  Log::instance()->debug( "Checking PLUGINPATH constant\n" );

#if defined(__APPLE__)
  CFURLRef myPluginRef = CFBundleCopyBundleURL(CFBundleGetMainBundle());
  CFStringRef myMacPath = CFURLCopyFileSystemPath(myPluginRef, kCFURLPOSIXPathStyle);
  const char *mypPathPtr = CFStringGetCStringPtr(myMacPath,CFStringGetSystemEncoding());
  CFRelease(myPluginRef);
  CFRelease(myMacPath);
  std::string myFullPath(mypPathPtr);
  string::size_type loc = myFullPath.find( ".app", 0 );
  if( loc != string::npos ) //found so we are in a mac application bundle
  {
    myFullPath += "/Contents/MacOS/lib/openmodeller";
    entries.push_back(myFullPath.c_str());
  } 
  else //not in a bundle! 
  {
    //otherwise use the normal search path
    entries.push_back( PLUGINPATH );
  }
#else
  entries.push_back( PLUGINPATH );
#endif

  return entries;
}

/****************************************************************/
/********************* Scan Directory Entries *******************/

#include <dirent.h>
#include <string.h>
#include <stdlib.h>

typedef struct dirent TDirent;


/**************/
/*** filter ***/
/**
 * Filter the file's names that refers to a dynamic shared
 * libraries.
 */
int
#ifdef __APPLE__
filter( TDirent *dir )
#elif __FreeBSD__
filter( TDirent *dir )
#else
filter( const TDirent *dir )
#endif
{
#if defined(__APPLE__)
  // constant version number should not be in filter but
  // symlinks ending in .0.dylib and .dylib also exist
  // and each library should be found only once
  std::string ext = ".so";
  //std::string ext = ".0.0.0.dylib";
#elif defined(__CYGWIN__)
  // under cygwin, libraries can be loaded using dlopen
  // but their extension is .dll
  std::string ext = ".dll";
#else
  std::string ext = ".so";
#endif

  char *found = strstr( dir->d_name, (const char *)ext.c_str() );

  return found ? ! strcmp( found, ext.c_str() ) : 0;
}


/****************/
/*** scan Dir ***/
vector<string>
scanDirectory( string dir )
{
  vector<string> entries;

  if ( dir.length() == 0 ) {
    return entries;
  }

  if ( dir[ dir.length() ] != '/' ) {
    dir += "/";
  }

  // Unix scandir call.
  TDirent **namelist;

  int nent = scandir( dir.c_str(), &namelist, filter, alphasort );

  if ( nent < 0 )
    return entries;

  // Copy from unix structure to the return structure.
  for ( int i = 0; i < nent; i++ ) {

    char *found = namelist[i]->d_name;

    string name = dir + found;
    entries.push_back( name );

    free( namelist[i] );
  }

  // Free unix structure.
  free( namelist );

  return entries;
}




/****************************************************************/
/*********************** Random Generation **********************/

#ifndef WIN32
#include <sys/time.h>
#include <sys/resource.h>

#else
#include <time.h>

#endif


/*******************/
/*** init Random ***/
int
initRandom()
{
  static unsigned int seed = 0;

  if ( seed ) {

    return 1; // reseeding rand can decrease the randomness, so avoid doing it
  }

#ifndef WIN32
  struct timeval time;
  gettimeofday( &time, (struct timezone *)NULL );
  seed = time.tv_usec;

#else
  seed = (unsigned int) time( NULL );

#endif

  Log::instance()->debug( "Setting random seed %u\n", seed );

  srand( seed );

// This is an workaround to get some things working with GCC. Unfortunately we don't
// know if the implementation of functions like random_shuffle use rand or lrand48
// internally so that we can seed it accordingly. Apparently, the test below is the
// same one used by the function. A more portable solution would be to develop our
// custom random number generator and pass it to random_shuffle or use it directly 
// in other parts of the code.
#ifdef _GLIBCPP_HAVE_DRAND48
  srand48( seed );
#endif

  return 1;
}
