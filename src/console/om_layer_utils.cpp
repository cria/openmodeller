/**
 * Definition of command-line utility functions.
 * 
 * @author Renato De Giovanni (renato at cria . org . br)
 * $Id $
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2013 by CRIA -
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

#include <om_layer_utils.hh>

#include <string>

#include <dirent.h>

#include "gdal_priv.h"

#include <openmodeller/Log.hh>
#include <openmodeller/FileParser.hh>

/***********************/
/**** readDirectory ****/
bool 
readDirectory( const char* dir, const char* label, ostream &xml, int depth, int* seq )
{
  bool r = true;

  if ( depth == 1 ) {

    GDALAllRegister();

    CPLSetErrorHandler( CPLQuietErrorHandler );
  }

  ++depth;

  // Maximum depth level to recurse on directories
  if ( depth > 20 )
  {
    return false;
  }

  // Create copy of dir and add slash to the end if necessary
  string myDir( dir );
  
  //skip hidden dirs
  if ( myDir.find( "." ) == 1 ) {
    return false;
  }
  if ( myDir.find_first_of( ".",0 ) == 1 ) {
    return false;
  }

  if ( myDir.find_last_of( "/" ) != myDir.size() - 1 ) {

    myDir.append( "/" );
  }

  // Open directory
  struct dirent **nameList;
  int n;
  struct stat buf;

  n = scandir( myDir.c_str(), &nameList, 0, alphasort );

  if ( n < 0 ) {

    // Problems opening directory
    return false;
  }
  else if ( n == 0 ) {
  
    // Empty directories are useless
    return true;
  }

  xml << "<LayersGroup Id=\"" << *seq << "\">";

  ++*seq;

  xml << "<Label>" << label << "</Label>";

  string xmlFiles;

  for ( int i = 0; i < n; i++ ) {

    // Skip "." and ".." directories
    if ( strcmp( nameList[i]->d_name, "." ) == 0 || strcmp( nameList[i]->d_name, ".." ) == 0 ) {

      continue;
    }

    // Skip ".aux" files
    if ( strstr( nameList[i]->d_name, ".aux" ) != 0 ) {

      continue;
    }

    // Need the full name to "stat"
    string fullName( myDir );
    fullName.append( nameList[i]->d_name );

    // Get file attributes (skip on failure)
    if ( stat( fullName.c_str(), &buf ) == -1 ) {

      continue;
    }

    // Directory
    if ( S_ISDIR( buf.st_mode ) ) {

      Log::instance()->debug( "Checking directory: %s\n", fullName.c_str() );

      if ( isValidGdalFile( fullName.c_str() ) ) {

        string hasProj = ( hasValidGdalProjection( fullName.c_str() ) ) ? "1" : "0";

        string label = getLayerLabel( myDir, nameList[i]->d_name, true );

        xmlFiles.append( "<Layer Id=\"" );
        xmlFiles.append( fullName );
        xmlFiles.append( "\" HasProjection=\"");
        xmlFiles.append( hasProj );
        xmlFiles.append( "\">" );
        xmlFiles.append( "<Label>" );
        xmlFiles.append( label );
        xmlFiles.append( "</Label>" );
        xmlFiles.append( "</Layer>" );
      }
      else {

	  r = readDirectory( fullName.c_str(), nameList[i]->d_name, xml, depth, seq );
      }
    }
    // Regular file
    else if ( S_ISREG( buf.st_mode ) ) {

      Log::instance()->debug( "Checking file: %s\n", fullName.c_str() );

      if ( isValidGdalFile( fullName.c_str() ) ) {

        string hasProj = ( hasValidGdalProjection( fullName.c_str() ) ) ? "1" : "0";

        string label = getLayerLabel( myDir, nameList[i]->d_name, false );

        xmlFiles.append( "<Layer Id=\"" );
        xmlFiles.append( fullName );
        xmlFiles.append( "\" HasProjection=\"");
        xmlFiles.append( hasProj );
        xmlFiles.append( "\">" );
        xmlFiles.append( "<Label>" );
        xmlFiles.append( label );
        xmlFiles.append( "</Label>" );
        xmlFiles.append( "</Layer>" );
      }
    }
    // Symbolic link
    //else if ( S_ISLNK( buf.st_mode ) ) {

      // What should we do with symlinks?
      //readDirectory( fullName.c_str(), nameList[i]->d_name, xml, depth, seq );
    //}
  }

  // openModeller.xsd mandates that files should always come after directories
  xml << xmlFiles;

  xml << "</LayersGroup>";

  return r;
}

/***********************/
/**** getLayerLabel ****/
string 
getLayerLabel( const string path, const string name, bool isDir )
{
  string metaFile = path;

  // It is a directory
  if ( isDir ) {

    // append last dir name and ".meta"
    metaFile = metaFile.append(name).append(".meta"); 
  }
  // It is a file
  else {

    size_t pos = name.find_last_of( "." );

    if ( pos != string::npos ) {

      // replace extension with ".meta"
      string metaName = name.substr( 0, pos ).append(".meta");
      metaFile = metaFile.append( metaName ); 
    }
    else {

      // try just appending name and ".meta"
      metaFile = metaFile.append(name).append(".meta"); 
    }
  }

  FILE * file = fopen( metaFile.c_str(), "r" );

  if ( file != NULL ) {

    fclose( file );

    FileParser fParser( metaFile.c_str() );

    return fParser.get( "LABEL" );
  }

  return name;  
}

/*************************/
/**** isValidGdalFile ****/
bool 
isValidGdalFile( const char* fileName )
{
  // test whether the file is GDAL compatible
  GDALDataset * testFile = (GDALDataset *)GDALOpen( fileName, GA_ReadOnly );

  if ( testFile == NULL ) {

    // not GDAL compatible
    return false;
  }
  else {

    // is GDAL compatible
    GDALClose( testFile );
    return true;  
  }
}

/********************************/
/**** hasValidGdalProjection ****/
bool 
hasValidGdalProjection( const char* fileName )
{
  // test whether the file has GDAL projection info
  GDALDataset * testFile = (GDALDataset *)GDALOpen( fileName, GA_ReadOnly );
      
  const char *projectionString = testFile->GetProjectionRef();

  GDALClose( testFile );
      
  if ( projectionString ) {

    // has projection info
    return true;
  }

  // does not have projection info
  return false;
}
