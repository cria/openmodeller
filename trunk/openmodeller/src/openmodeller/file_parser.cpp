/**
 * Definition of FileParser class.
 * 
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-09-25
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2003 by CRIA - Centro de Referencia em Informacao Ambiental
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

#include <openmodeller/file_parser.hh>
#include <string.h>
#include <stdio.h>

#if 0
// Need an ostream inserter for the special icstring type.
inline std::ostream& operator<<( std::ostream& strm, const FileParser::icstring& s )
{
  return strm << std::string(s.data(),s.length() );
}
#endif

/****************************************************************/
/**************************** File Parser ***********************/

static char error[256];

/******************/
/*** constructor ***/

FileParser::FileParser( const std::string & file )
{
  if ( ! load( file ) ) {

    sprintf( error, "File '%s' was not found.\n", file.c_str() );
    fprintf( stderr, "%s", error );
    throw error;
  }
}


/******************/
/*** destructor ***/

FileParser::~FileParser()
{
}

/************/
/*** load ***/
int
FileParser::load( const std::string & file )
{
  FILE *fd = fopen( file.c_str(), "r" );
  if ( ! fd )
    return 0;

  f_lst.clear();

  const int size = 1024;
  char line[size];

  while ( fgets( line, size, fd ) ) {
    // Find the first # which indicates the start of comment.
    char *sep = strchr( line,'#' );
    // If it's at the beginning of the line
    // loop.
    if ( sep == line )
      continue;

    // If it's not at the beginning of the line,
    // assign it to "null", terminating the string,
    // and effectively commenting to end of line.
    if ( sep )
      *sep = '\0';

    // Find the start of the key.
    // Trim the whitespace from the front of the line.
    char *start_key = line;
    while ( isspace( *start_key ) && (*start_key != '\0') ) {
      start_key++;
    }

    // Nothing but whitespace.  Loop.
    if ( *start_key == '\0' )
      continue;

    // Separate key and value
    sep = strchr( line, '=' );
    if ( sep ) {
      // Find the start of the value.
      char *start_val = sep+1;
      // Left trim the whitespace.
      while ( isspace( *start_val ) && ( *start_val != '\0' ) ) {
	start_val++;
      }
      // No value?  loop.
      if ( *start_val == '\0' )
	continue;

      // Null terminate the key.
      *sep-- = '\0';
      // and right trim it.
      while( isspace( *sep ) )
	*sep-- = '\0';

      // Right trim the value.
      sep = start_val + strlen( start_val ) - 1;
      // Remember the \0 we substitued for the '='?
      // use that to terminate the loop.
      while ( isspace( *sep ) && ( *sep != '\0' ) )
	*sep-- = '\0';
      
      if ( *sep == '\0' )
	continue;
      
      f_lst.push_back( std::make_pair( start_key, start_val ) );
    }
  }
  
  fclose( fd );
  return 1;
}


/***********/
/*** get ***/
std::string
FileParser::get( const std::string & key ) const
{
  ItemList::const_iterator it = f_lst.begin();

  while ( it != f_lst.end() ) {

    if ( (*it).first == key.c_str() ) {

      return it->second;
    }

    ++it;
  }
  return "";
}


/*************/
/*** count ***/
int
FileParser::count( const std::string & key ) const
{
  int n = 0;

  ItemList::const_iterator it = f_lst.begin();

  while ( it != f_lst.end() ) {

    if (  it->first == key.c_str() ) {

      ++n;
    }
    ++it;
  }
  return n;
}

/***************/
/*** get All ***/
std::vector<std::string>
FileParser::getAll( const std::string & key ) const
{
  std::vector<std::string> values;
  ItemList::const_iterator it = f_lst.begin();

  while ( it != f_lst.end() ) {

    if ( it->first == key.c_str() ) {

      values.push_back( it->second );
    }

    ++it;
  }
  return values;
}
