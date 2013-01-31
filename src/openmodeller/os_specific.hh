/**
 * Declaration of OS specific functions.
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

#ifndef _OS_SPECIFICHH_
#define _OS_SPECIFICHH_

#if (defined(__unix__) || defined(unix)) && !defined(USG)
#include <sys/param.h>
#define dllexp
#define OM_ALG_DLL_EXPORT
#endif

#ifdef BSD
#include <sys/sysctl.h>
#include <err.h>
#endif

#include <vector>
#include <string>

/****************************************************************/
/****** DEFINE MACRO TO NEATLY HANDLE UNUSED VARS ***************/
#ifdef _MSC_VER
  // fake use to make unused variable warnings go away in Visual C++
  #define UNUSED(symbol) symbol
#else
  #define UNUSED(symbol)
#endif

/****************************************************************/
/****** Replace finite with _finite when using MSVC *************/
#ifdef _MSC_VER
  #define finite _finite // function used by maxent
#endif

/****************************************************************/
/********************* Dynamic Linking Loader *******************/

#ifdef WIN32 
#include <windows.h>
typedef HMODULE DLLHandle;
#else
typedef void *DLLHandle;
#endif

extern DLLHandle   dllOpen    ( char const *dll_file_name );
extern void       *dllFunction( DLLHandle, char const *function_name );
extern int         dllClose   ( DLLHandle );
extern const char *dllError   ( DLLHandle );

/**
 * Set up external resources (GDAL & proj4 configuration, 
 * om algorithms & data paths).
 */
dllexp void setupExternalResources();

/**
 * Return the om data directory, optionally setting it through the parameter.
 * @param dir Default directory.
 * @return dir openModeller data directory.
 */
dllexp std::string omDataPath( std::string dir="" );

/**
 * Retrieve initial library path.
 * @return Vector of directories.
 */
std::vector<std::string> initialPluginPath();

/**
 * Scan directory entries.
 * @param dir Path of the directory to be scanned. (pass by value,
 * routine needs local copy).
 * @return A null terminated array of allocated strings with the
 * directory scanned entries. Return null if an error occurs.
 */
std::vector<std::string> scanDirectory( std::string dir );

/**
 * Generates a pseudo-random seed and initializes the system
 * random sequence generator. The seed is based in the
 * micro-seconds of the current machine time.
 * @param new_seed Optional seed that can be explicitly provided.
 * @return Not zero if the generator was initiated.
 */
dllexp int initRandom( unsigned int new_seed=0 );

#ifdef WIN32 
// rand_r implementation for Windows
dllexp int rand_r( unsigned * seed );
#endif

/**
 * Returns the current working path.
 * @return Working path.
 */
std::string getWorkingPath();

/**
 * Indicates if the specified path exists in the file system.
 * @path path Path to be tested
 * @return Yes or no.
 */
bool pathExists( const std::string path );

/**
 * Creates (recursively) the specified path.
 * @path path Path to be created
 * @return Succeeded or not.
 */
bool createPath( const std::string path );

#endif
