/**
 * Definition of OccurrencesReader class.
 * 
 * @author Alexandre Copertino Jardim <alexcj@dpi.inpe.br>
 * @date 2006-03-21
 * $Id$ 
 * 
 *
 * LICENSE INFORMATION
 * 
 * Copyright © 2006 INPE
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

#include <openmodeller/occ_io/OccurrencesReader.hh>
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

  // Default coordinate system for occurrence points.
  //
  #define open	_open
  #define close	_close
  #define fdopen  _fdopen

  #define strcasecmp _stricmp

#else
  #include <fcntl.h>
  #include <unistd.h>
#endif


/***********/
/*** get ***/
OccurrencesPtr
OccurrencesReader::get( const char *groupId )
{
	// If group was not specified,
	// get the last entry and return it.
	if ( ! groupId || *groupId == '\0' )
	{
		OccurrencesPtr last = f_sp.back();
		f_sp.pop_back();
		return last;
	}

	LstOccurrences::iterator ocs = f_sp.begin();
	LstOccurrences::iterator end = f_sp.end();
	while ( ocs != end )
	{
		OccurrencesPtr oc = *ocs;
		if ( ! strcasecmp( groupId, oc->name() ) )
		{
			f_sp.erase( ocs );
			return oc;
		}
		++ocs;
	}
	return OccurrencesPtr(); 
}


/*************/
/*** print ***/
void
OccurrencesReader::printOccurrences( char *msg )
{
	printf( "%s", msg );

	LstOccurrences::const_iterator ocs = f_sp.begin();
	LstOccurrences::const_iterator end = f_sp.end();
	while ( ocs != end )
	{
		(*ocs)->print( "\n****************" );
		++ocs;
	}
}

/**********************/
/*** add Occurrence ***/
int
OccurrencesReader::addOccurrence( const char *groupId, Coord lg, Coord lt,
                                  Scalar error, Scalar abundance,
                                  int num_attributes, Scalar *attributes )
{
	// If an occurrences group with the same name already exists, 
	// just add another occurrence to it.

	// We search backwards because new Occurrences are pushed
	// onto the back, so most likely, the Occurrences we are looking
	// for is at the back.
	LstOccurrences::const_reverse_iterator ocs = f_sp.rbegin();
	LstOccurrences::const_reverse_iterator end = f_sp.rend();
	while ( ocs != end )
	{
		OccurrencesPtr const & sp = *ocs;
		if ( ! strcasecmp( sp->name(), groupId ) )
		{
			sp->createOccurrence( lg, lt, error, abundance, num_attributes, attributes );
			return 0;
		}
		++ocs;
	}

	// If the occurrences group does not exist, create it.
	OccurrencesImpl *spi = new OccurrencesImpl( groupId, _coord_system );
	spi->createOccurrence( lg, lt, error, abundance, num_attributes, attributes );
	f_sp.push_back( spi );

	return 1;
}
