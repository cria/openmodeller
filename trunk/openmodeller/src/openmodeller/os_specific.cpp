/**
 * Definitions of operating system's specific functions.
 * 
 * @file
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

#include <stdlib.h>

using std::vector;
using std::string;

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
char *
dllError( DLLHandle )
{
  return dlerror();
}

/*************************/
/*** initial Plugin Path ***/
vector<string>
initialPluginPath()
{

  vector<string> entries;

  //
  // Order of initialization:
  //
  // 1) environment variable: OM_LIB_PATH
  // 2) read from CONFIG_FILE
  // 3) PLUGINPATH compiled constant.

  char *env = getenv( "OM_LIB_PATH" );

  //
  // Check if the environment variable is set
  if ( env != 0 ) {

    string envpath( (char const *)env );

    // If it's set to "" then we return.  Don't know what this means since the
    // path is emtpy.  Maybe we need to have this drop to using CONFIG_FILE
    if( envpath.empty() ) {
      return entries;
    }

    // Parse the OM_LIB_PATH with colon (':') delimiters just like all other 
    // unix path structures.

    // string::size_type start marks the beginning of the substring.
    // initial value is beginning of string, iterate value is one past the ':'
    for( string::size_type start = 0; start < envpath.length() ; ) {
      
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

  std::ifstream conf_file( CONFIG_FILE, std::ios::in );

  if ( conf_file ) {

    while( conf_file ) {
      string line;
      getline( conf_file, line );
      entries.push_back( line );
    }

    return entries;

  }

  entries.push_back( PLUGINPATH );

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
filter( const TDirent *dir )
{
#ifndef __CYGWIN__
  char *ext = ".so";
#else
  // under cygwin, libraries can be loaded using dlopen
  // but their extension is .dll
  char *ext = ".dll";
#endif

  char *found = strstr( dir->d_name, ext );

  return found ? ! strcmp( found, ext ) : 0;
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
  unsigned int seed;

#ifndef WIN32
  struct timeval time;
  gettimeofday( &time, (struct timezone *)NULL );
  seed = time.tv_usec;

#else
  seed = (unsigned int) time( NULL );

#endif

  //printf("Random seed: %d\n", seed);

  srand( seed );
  return 1;
}
