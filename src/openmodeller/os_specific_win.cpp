/**
 * Definitions of Windows operating system's specific functions.
 * 
 * @author Ricardo Scachetti Pereira (rpereira at ku.edu)
 * @date   2004-04-20
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 * Copyright(c), David R.B. Stockwell of Symbiotik Pty. Ltd.
 * Copyright(c), CRIA - Centro de Referencia em Informacao Ambiental
 *
 * http://www.nhm.ku.edu
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

#ifdef WIN32
// avoid warnings caused by problems in VC headers
#define _SCL_SECURE_NO_DEPRECATE
#endif

#include <os_specific.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/AlgorithmFactory.hh>

#include <stdio.h>
#include <fstream>
#include <windows.h>
#include <cpl_conv.h> // for setting gdal options
#include <proj_api.h> // for setting proj options

using std::vector;
using std::string;

/****************************************************************/
/********************* Dynamic Linking Loader *******************/

/****************/
/*** dll Open ***/
DLLHandle
dllOpen( char const *dll_file_name )
{
#ifdef MINGW_QT 
  //Added by Tim because under mingw wchar is expected
  return LoadLibraryA( dll_file_name );
#else
  return LoadLibraryA( dll_file_name );
#endif
}


/********************/
/*** dll Function ***/
void *
dllFunction( DLLHandle handle, char const *function_name )
{
  return (void *) GetProcAddress( handle, function_name );
}


/*****************/
/*** dll Close ***/
int
dllClose( DLLHandle handle )
{
  return FreeLibrary( handle );
}


/*****************/
/*** dll Error ***/
const char *
dllError( DLLHandle )
{
  char * szBuf = NULL;
  long error = GetLastError();

  if (error)
  {
    szBuf = new char[1024]; 
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	               NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &szBuf, 0, NULL );
  }
  return szBuf;
}

/*********************************/
/*** set up External Resources ***/
void setupExternalResources()
{
  // Get directory where the .exe resides
  char initial_path[FILENAME_MAX];

  int bytes = GetModuleFileName( NULL, initial_path, FILENAME_MAX );

  if ( bytes == 0 ) {
        
      return;
  }

  // Remove file name from path
  string initial_path_str( initial_path );
        
  size_t last_sep;
        
  last_sep = initial_path_str.find_last_of("/\\");
        
  if ( ! last_sep ) {
        
      return;
  }

  string path = initial_path_str.substr( 0, last_sep+1 );
        
  // Set algorithms path
  string alg_path = path;
  alg_path.append( "algs" );
  AlgorithmFactory::setDefaultAlgDir( alg_path );

  // Set data path
  string data_path = path;
  data_path.append( "data" );
  omDataPath( data_path );

  // Set GDAL_DATA for openModeller lib
  string gdal_data_path = path;
  gdal_data_path.append( "gdal" );
  CPLSetConfigOption( "GDAL_DATA", gdal_data_path.c_str() );

  // Set PROJ_LIB for openModeller lib
  string proj_data_path = path;
  proj_data_path.append( "nad" );
  const char * char_path = proj_data_path.c_str();
  const char ** paths;
  paths = &char_path;
  pj_set_searchpath( 1, paths );
}

/********************/
/*** om Data Path ***/
std::string omDataPath( std::string dir )
{
  static string data_path;

  // Set default directory, if specified through parameter
  if ( ! dir.empty() ) {

    data_path = dir;
  }

  // Priority to env variable
  char *env = getenv( "OM_DATA_PATH" );

  if ( env != 0 ) {

    string om_data_path = (char const *)env;

    if ( ! om_data_path.empty() ) {

      return om_data_path;
    }
  }

  // Return static content if it's not empty
  if ( ! data_path.empty() ) {

    return data_path;
  }

  string pkg_data_path = PKGDATAPATH;
  
  // Try compile constant
  if ( ! pkg_data_path.empty() ) {

    return pkg_data_path;
  }

  return data_path;
}

/***************************/
/*** initial Plugin Path ***/
vector<string>
initialPluginPath()
{
  Log::instance()->debug( "Determining algorithm paths\n" );

  vector<string> entries;

  char *env = getenv( "OM_ALG_PATH" );

  // Check if the environment variable is set
  if ( env != 0 ) {

    string envpath( (char const *)env );

    // Ignore empty string
    if ( ! envpath.empty() ) {

      Log::instance()->debug( "Found not empty environment variable OM_ALG_PATH: %s\n", envpath.c_str() );

      // Parse the OM_ALG_PATH with semi-colon (';') delimiters just like all other 
      // Windows path structures.

      // string::size_type start marks the beginning of the substring.
      // initial value is beginning of string, iterate value is one past the ';'
      for ( string::size_type start = 0; start < envpath.length() ; ) {
      
        // Find the next ';' after start
        string::size_type it = envpath.find( ';', start );

        // If no ';' is found..
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
  }

  // Default location that can be set programatically
  std::string default_dir = AlgorithmFactory::getDefaultAlgDir();
    
  if ( ! default_dir.empty() ) {

    Log::instance()->debug( "Default location is set\n" );

    entries.push_back( default_dir );
    return entries;
  }

  Log::instance()->debug( "Checking CONFIG_FILE constant: " CONFIG_FILE "\n" );

  std::ifstream conf_file( CONFIG_FILE, std::ios::in );

  if ( ! conf_file ) {

    Log::instance()->debug( "Using PLUGINPATH constant: " PLUGINPATH "\n" );

    entries.reserve(1);
    entries.push_back( PLUGINPATH );
    return entries;
  }

  while ( conf_file ) {

    Log::instance()->debug( "Found config file\n" );

    string line;
    getline( conf_file, line );
    entries.push_back( line );
  }

  return entries;
}

/****************************************************************/
/********************* Scan Directory Entries *******************/

#include <io.h>
#include <string.h>
#include <stdlib.h>


/****************/
/*** scan Dir ***/

vector<string> 
scanDirectory( string dir )
{
  string filepattern(dir);
  long dirhandle;
  struct _finddata_t fileinfo;

  vector<string> entries;

  // check for empty string
  if (!dir.length())
  {
	  return entries;
  }

  // check for slashes at the end of directory name
  if ( (filepattern.find_last_of("/")  != filepattern.length() - 1) && 
	   (filepattern.find_last_of("\\") != filepattern.length() - 1) )
	  filepattern.append("\\");

	  // Windows findfirst and findnext calls.
  filepattern.append("*.dll");
  dirhandle = _findfirst(filepattern.c_str(), &fileinfo); 

  if (dirhandle == -1L)
    return entries;

  int nent = 1;
  while (!_findnext(dirhandle, &fileinfo))
  { nent++; }

  // Directory path size - not used so remove this!
  //int dir_size = dir.size();

  // Windows findfirst and findnext calls.
  dirhandle = _findfirst(filepattern.c_str(), &fileinfo); 

  if (dirhandle == -1L)
    return entries;

  // Copy from windows structure to the return structure.
  for ( int i = 0; i < nent; i++ )
    {
      char *found = fileinfo.name;

      string name = dir;
      name += "\\";
      name += found;

      entries.push_back(name);

      _findnext(dirhandle, &fileinfo);
    }

  _findclose(dirhandle);

  return entries;
}




/****************************************************************/
/*********************** Random Generation **********************/

#include <time.h>


/*******************/
/*** init Random ***/
dllexp int initRandom()
{
  unsigned int seed;
  seed = (unsigned int) time( NULL );
  srand( seed );
  return 1;
}

