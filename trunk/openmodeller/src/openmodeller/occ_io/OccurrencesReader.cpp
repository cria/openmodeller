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
 * Copyright � 2006 INPE
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
  #define open   _open
  #define close  _close
  #define fdopen _fdopen

  #define strcasecmp _stricmp

#else
  #include <fcntl.h>
  #include <unistd.h>
#endif


/*********************/
/*** get Presences ***/
OccurrencesPtr
OccurrencesReader::getPresences( const char *groupId )
{
    // If group was not specified,
    // get the last entry and return it.
    if ( ! groupId || *groupId == '\0' )
    {
        OccurrencesPtr last = _presences.back();
        _presences.pop_back();
        return last;
    }

    LstOccurrences::iterator ocs = _presences.begin();
    LstOccurrences::iterator end = _presences.end();
    while ( ocs != end )
    {
        OccurrencesPtr oc = *ocs;
        if ( ! strcasecmp( groupId, oc->name() ) )
        {
            _presences.erase( ocs );
            return oc;
        }
        ++ocs;
    }
    return OccurrencesPtr(); 
}


/********************/
/*** get Absences ***/
OccurrencesPtr
OccurrencesReader::getAbsences( const char *groupId )
{
    // If group was not specified,
    // get the last entry and return it.
    if ( ! groupId || *groupId == '\0' )
    {
        OccurrencesPtr last = _absences.back();
        _absences.pop_back();
        return last;
    }

    LstOccurrences::iterator ocs = _absences.begin();
    LstOccurrences::iterator end = _absences.end();
    while ( ocs != end )
    {
        OccurrencesPtr oc = *ocs;
        if ( ! strcasecmp( groupId, oc->name() ) )
        {
            _absences.erase( ocs );
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

    printf( "\nPresences" );

    LstOccurrences::const_iterator ocs = _presences.begin();
    LstOccurrences::const_iterator end = _presences.end();
    while ( ocs != end )
    {
        (*ocs)->dump( "\n****************" );
        ++ocs;
    }

    printf( "\nAbsences" );

    ocs = _absences.begin();
    end = _absences.end();
    while ( ocs != end )
    {
        (*ocs)->dump( "\n****************" );
        ++ocs;
    }
}

/**********************/
/*** add Occurrence ***/
int
OccurrencesReader::addOccurrence( const char *id, const char *groupId, Coord lg, Coord lt,
                                  Scalar error, Scalar abundance,
                                  int num_attributes, Scalar *attributes )
{
    if ( abundance > 0 ) {

	_addPresence( id, groupId, lg, lt, error, abundance, num_attributes, attributes );
    }
    else {

	_addAbsence( id, groupId, lg, lt, error, num_attributes, attributes );
    }
    return 1;
}

/********************/
/*** add Presence ***/
int
OccurrencesReader::_addPresence( const char *id, const char *groupId, Coord lg, Coord lt,
                                 Scalar error, Scalar abundance,
                                 int num_attributes, Scalar *attributes )
{
    // If a presence group with the same name already exists, 
    // just add another occurrence to it.

    // We search backwards because new Occurrences are pushed
    // onto the back, so most likely, the Occurrences we are looking
    // for is at the back.
    LstOccurrences::const_reverse_iterator ocs = _presences.rbegin();
    LstOccurrences::const_reverse_iterator end = _presences.rend();
    while ( ocs != end )
    {
        OccurrencesPtr const & group = *ocs;
        if ( ! strcasecmp( group->name(), groupId ) )
        {
            group->createOccurrence( id, lg, lt, error, abundance, num_attributes, attributes );
            return 0;
        }
        ++ocs;
    }

    // If the occurrences group does not exist, create it.
    OccurrencesImpl *newgroup = new OccurrencesImpl( groupId, _coord_system );
    newgroup->createOccurrence( id, lg, lt, error, abundance, num_attributes, attributes );
    _presences.push_back( newgroup );

    return 1;
}

/*******************/
/*** add Absence ***/
int
OccurrencesReader::_addAbsence( const char *id, const char *groupId, Coord lg, Coord lt,
                                Scalar error, int num_attributes, Scalar *attributes )
{
    // If an absence group with the same name already exists, 
    // just add another occurrence to it.

    // We search backwards because new Occurrences are pushed
    // onto the back, so most likely, the Occurrences we are looking
    // for is at the back.
    LstOccurrences::const_reverse_iterator ocs = _absences.rbegin();
    LstOccurrences::const_reverse_iterator end = _absences.rend();
    while ( ocs != end )
    {
        OccurrencesPtr const & group = *ocs;
        if ( ! strcasecmp( group->name(), groupId ) )
        {
            group->createOccurrence( id, lg, lt, error, 0.0, num_attributes, attributes );
            return 0;
        }
        ++ocs;
    }

    // If the occurrences group does not exist, create it.
    OccurrencesImpl *newgroup = new OccurrencesImpl( groupId, _coord_system );
    newgroup->createOccurrence( id, lg, lt, error, 0.0, num_attributes, attributes );
    _absences.push_back( newgroup );

    return 1;
}

