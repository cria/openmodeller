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
#include <openmodeller/Settings.hh>

#include <stdio.h>
#include <fstream>
#include <windows.h>
#include <direct.h>
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

    return data_path;
  }

  // Check configuration
  if ( Settings::count( "DATA_DIRECTORY" ) == 1 ) {

    return Settings::get( "DATA_DIRECTORY" );
  }

  // Check env variable
  char *env = getenv( "OM_DATA_DIR" );

  if ( env != 0 ) {

    string om_data_path = (char const *)env;

    if ( ! om_data_path.empty() ) {

      return om_data_path;
    }
  }

  // Finally compiler constant
  return OM_DATA_DIR;
}

/***************************/
/*** initial Plugin Path ***/
vector<string>
initialPluginPath()
{
  Log::instance()->debug( "Determining algorithms plugin path\n" );

  vector<string> entries;

  // Default location can be set programatically
  std::string default_dir = AlgorithmFactory::getDefaultAlgDir();
    
  if ( ! default_dir.empty() ) {

    Log::instance()->debug( "Using programatic setting for algorithms location\n" );

    entries.push_back( default_dir );
    return entries;
  }

  // Otherwise check configuration
  if ( Settings::count( "ALGS_DIRECTORY" ) == 1 ) {

    Log::instance()->debug( "Using configuration setting for algorithms location\n" );
    entries.push_back( Settings::get( "ALGS_DIRECTORY" ) );
    return entries;
  }

  // Or environment variable
  char *env = getenv( "OM_ALGS_DIR" );

  if ( env != 0 ) {

    string envpath( (char const *)env );

    // Ignore empty string
    if ( ! envpath.empty() ) {

      Log::instance()->debug( "Using environment setting for algorithms location\n" );

      // Parse the OM_ALGS_DIR with semi-colon (';') delimiters just like all other 
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

  // Finally compiler constant
  Log::instance()->debug( "Using default algorithms location\n" );

  entries.reserve(1);
  entries.push_back( OM_ALGS_DIR );
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

/*******************/
/*** init Random ***/
dllexp int
initRandom( unsigned int new_seed )
{
  static unsigned int seed = 0;

  if ( seed && !new_seed ) {

    // reseeding rand can decrease the randomness, so avoid doing it
    return 1;
  }

  if ( new_seed ) {

    seed = new_seed;
  }
  else {

    seed = (unsigned int) time( NULL );
  }

  Log::instance()->debug( "Setting random seed %u\n", seed );

  srand( seed );

  return 1;
}

/*****************************************/
/*** rand_r implementation for Windows ***/
dllexp int
rand_r( unsigned * seed )
{
  * seed = (* seed) * 1103515245 + 12345;
  return ((unsigned)(*seed / 65536) % 32768);
}

/************************/
/*** get Working path ***/
std::string
getWorkingPath()
{
   char temp[MAX_PATH];
   return ( _getcwd(temp, MAX_PATH) ? std::string( temp ) : std::string("") );
}

/*******************/
/*** path Exists ***/
bool
pathExists( const std::string path )
{
  DWORD ftyp = GetFileAttributesA( path.c_str() );

  if ( ftyp == INVALID_FILE_ATTRIBUTES ) {

    return false; // something is wrong with the path
  }

  if ( ftyp & FILE_ATTRIBUTE_DIRECTORY ) {

    return true; // this is a directory
  }

  return false; // not a directoy
}

/*******************/
/*** create Path ***/
bool
createPath( const std::string path )
{
  static const std::string separators("\\/");
 
  DWORD file_attr = ::GetFileAttributes( (LPCSTR) path.c_str() );

  // If the specified directory name doesn't exist
  if ( file_attr == INVALID_FILE_ATTRIBUTES ) {
 
    // Recursively do it all again for the parent directory, if any
    std::size_t slash_idx = path.find_last_of( separators.c_str() );

    if ( slash_idx != std::string::npos ) {

      createPath( path.substr(0, slash_idx) );
    }
 
    // Create the last directory on the path (the recursive calls will have taken
    // care of the parent directories by now)
    ::CreateDirectory( (LPCSTR) path.c_str(), NULL );
    return pathExists( path );
  }

  // Specified directory name already exists as a file or directory
  return false;
}
