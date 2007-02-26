/**
 * Definition of OccurrencesFile class.
 * 
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-02-25
 * @author Alexandre Copertino Jardim <alexcj@dpi.inpe.br>
 * @date 2006-03-21
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

#include <openmodeller/occ_io/OccurrencesFile.hh>

#include <openmodeller/Occurrences.hh>  // List of occurrences.

#include <vector>
using std::vector;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Win32 defines
#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#define strcasecmp _stricmp

// Default coordinate system for occurrence points.
//
#define open	_open
#define close	_close
#define fdopen  _fdopen

#else
#include <fcntl.h>
#include <unistd.h>
#endif

/*******************/
/*** Constructor ***/
OccurrencesFile::OccurrencesFile( const char *file_name,
				  const char *coord_system )
{
  _coord_system = (char *) coord_system;
  loadOccurrences( file_name );
}


/******************/
/*** Destructor ***/
OccurrencesFile::~OccurrencesFile()
{
}


/***********************/
/*** load Ocurrences ***/
int
OccurrencesFile::loadOccurrences( const char *file_name )
{
  // Opens the input file.
  FILE *file = fopen( file_name, "r" );

  if ( ! file )
  {
    g_log.warn( "Can't open file %s.\n", file_name );
    return 0;
  }

  // Fixme: read this from file.
  Scalar error     	= -1.0;
  int    num_attributes = 0;
  Scalar *attributes    = 0;

  // Columns to be read.
  char id[50];
  char label[256];
  double x, y;
  int abundance;

  // Read all occurrences line by line inserting into the
  // appropriate object.
  const int line_len = 600;
  char line[line_len];
  char *pos;

  while ( fgets( line, line_len, file ) )
  {
    // Remove \r that DOS loves to use.
    pos = strchr( line, '\r' );
    if ( pos )
    {
      *pos = '\0';
    }

    if ( *line != '#' && sscanf( line, "%[^\t] %[^\t] %lf %lf %u", id, label, &x, &y, &abundance ) == 5 )
    {
      Coord lg = Coord( x );
      Coord lt = Coord( y );

      addOccurrence( id, label, lg, lt, error, (Scalar)abundance, num_attributes, attributes );
    }
    else if ( *line != '#' && sscanf( line, "%[^\t] %[^\t] %lf %lf", id, label, &x, &y ) == 4 )
    {
      // When no abundance is provided, assume 1 (single presence)

      Coord lg = Coord( x );
      Coord lt = Coord( y );

      addOccurrence( id, label, lg, lt, error, 1.0, num_attributes, attributes );
    }
    else
    {
	g_log.debug( "Skipping line: %s\n", line );
    }
  }

  fclose( file );

  return 0;
}
