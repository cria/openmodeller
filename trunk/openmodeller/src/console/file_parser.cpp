/**
 * Definition of FileParser class.
 * 
 * @file
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

#include <file_parser.hh>
#include <defs.hh>

#include <list.cpp>

#include <stdio.h>
#include <string.h>


/****************************************************************/
/**************************** File Parser ***********************/

static char error[256];

/******************/
/*** constructor ***/

FileParser::FileParser( char *file )
{
  if ( ! load( file ) )
    {
      sprintf( error, "File '%s' was not found.\n",
               file );
      fprintf( stderr, "%s", error );
      throw error;
    }
}


/******************/
/*** destructor ***/

FileParser::~FileParser()
{
  clear();
}


/************/
/*** load ***/
int
FileParser::load( char *file )
{
  FILE *fd = fopen( file, "r" );
  if ( ! fd )
    return 0;

  clear();

  const int size = 1024;
  char line[size];

  Item *item;
  char *sep;
  while ( fgets( line, size, fd ) )
    {
      // Remove comments
      if ( sep = strchr( line, '#' ) )
	*sep = '\0';

      // Separate key and value
      if ( sep = strchr( line, '=' ) )
	{
	  *sep++ = '\0';
	  f_lst.Append( item = new Item );
	  transfer( &item->key, line );
	  transfer( &item->val, sep );
	}
    }

  fclose( fd );
  return 1;
}


/***********/
/*** get ***/
char *
FileParser::get( char *key )
{
  Item *i;
  for ( f_lst.Head(); i = f_lst.Get(); f_lst.Next() )
    if ( ! strcasecmp( key, i->key ) )
      return i->val;

  return 0;
}


/*************/
/*** count ***/
int
FileParser::count( char *key )
{
  int n = 0;

  Item *i;
  for ( f_lst.Head(); i = f_lst.Get(); f_lst.Next() )
    if ( ! strcasecmp( key, i->key ) )
      n++;

  return n;
}


/***************/
/*** get All ***/
int
FileParser::getAll( char *key, char **values )
{
  int n = 0;

  Item *i;
  for ( f_lst.Head(); i = f_lst.Get(); f_lst.Next() )
    if ( ! strcasecmp( key, i->key ) )
      {
	*values++ = i->val;
	n++;
      }

  return n;
}


/****************/
/*** transfer ***/
void
FileParser::transfer( char **dst, char *src )
{
  // Ignore leading white characters
  while ( (*src == ' ') || (*src == '\t') ||
	  (*src == '\n') || (*src == '\r') )
    src++;

  // Find the end
  int len = strlen(src);
  char *end = src + len - 1;
  while ( (*end == ' ') || (*end == '\t') ||
	  (*end == '\n') || (*end == '\r') )
    end--;

  // 'end' will point to the first character that should not be copied
  end++;

  // Copy string
  char *d = *dst = new char[len+1];
  while ( src < end )
    *d++ = *src++;
  *d = '\0';
}


/*************/
/*** clear ***/
void
FileParser::clear()
{
  Item *i;
  for ( f_lst.Head(); i = f_lst.Get(); f_lst.Next() )
    {
      delete i->key;
      delete i->val;
      delete i;
    }
}


