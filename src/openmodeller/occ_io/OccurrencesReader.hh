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


#ifndef _OCCURRENCES_READERHH_
#define _OCCURRENCES_READERHH_

#include <openmodeller/om_defs.hh>
#include <openmodeller/Occurrences.hh>
#include <vector>

//! A common interface to occurrence readers.
/**
 * Implementations need to define the loadOccurrences method and 
 * a constructor with parameters "const char *source" and "const char *coord_system"
 */
class dllexp OccurrencesReader
{
public:

    virtual ~OccurrencesReader() {};

    /** Load occurrences from a specific source.
     * @param source Source of occurrences (like an url or a file name).
     */
    virtual int loadOccurrences( const char *source ) = 0;

    /** Return the number of available occurrences.
     * @return total number of occurrences.
     */
    int numOccurrences() { return numPresences() + numAbsences(); }

    /** Return the number of available presences.
     * @return total number of presences.
     */
    int numPresences() { return _presences.size(); }

    /** Return the number of available absences.
     * @return total number of absences.
     */
    int numAbsences() { return _absences.size(); }

    /** Return the presences (abundance > 0) from a specific group.
     * @param groupId Identifier for a group of occurrences (usually a species name).
     * @return Pointer to occurrences of the specified group, or to the last 
     * added group of available occurrences (if no group was specified), or 0 if 
     * the group was not found.
     */
    OccurrencesPtr getPresences( const char *groupId );

    /** Return the absences (abundance == 0) from a specific group.
     * @param groupId Identifier for a group of occurrences (usually a species name).
     * @return Pointer to occurrences of the specified group, or to the last 
     * added group of available occurrences (if no group was specified), or 0 if 
     * the group was not found.
     */
    OccurrencesPtr getAbsences( const char *groupId );

    /** Print the occurrences to cout.
     * @param msg Optional string to be printed before the occurrences.
     */
    void printOccurrences( char *msg="" );

protected:

    /**
     * Add a new occurrence. Each occurrence belongs to a group (usually a species name).
     * @param id Occurrence unique identifier.
     * @param groupId Group identifier (usually a species name).
     * @param lg Longitude.
     * @param lt Latitude.
     * @param error Associated error.
     * @param abundance Number of "individuals".
     * @param num_attributes Number of extra attributes.
     * @param attributes Extra attributes.
     * @return 0 if occurrence was added to an existing group, 1 if group was created.
     */
    int addOccurrence( const char *id, const char *groupId, Coord lg, Coord lt, Scalar error,
                       Scalar abundance, int num_attributes, Scalar *attributes );

    typedef std::vector<OccurrencesPtr> LstOccurrences;

    // List of presences by group name.
    LstOccurrences _presences;

    // List of absences by group name.
    LstOccurrences _absences;
	
    // Coordinate System
    char *_coord_system;
	
    // Protected Constructor because it is a abstract class (interface).
    OccurrencesReader() {};

private:

    /**
     * Add a new presence. Each presence belongs to a group (usually a species name).
     * @param id Occurrence unique identifier.
     * @param groupId Group identifier (usually a species name).
     * @param lg Longitude.
     * @param lt Latitude.
     * @param error Associated error.
     * @param abundance Number of "individuals".
     * @param num_attributes Number of extra attributes.
     * @param attributes Extra attributes.
     * @return 0 if presence was added to an existing group, 1 if group was created.
     */
    int _addPresence( const char *id, const char *groupId, Coord lg, Coord lt, Scalar error, 
                      Scalar abundance, int num_attributes, Scalar *attributes );

    /**
     * Add a new absence. Each absence belongs to a group (usually a species name).
     * @param id Occurrence unique identifier.
     * @param groupId Group identifier (usually a species name).
     * @param lg Longitude.
     * @param lt Latitude.
     * @param error Associated error.
     * @param num_attributes Number of extra attributes.
     * @param attributes Extra attributes.
     * @return 0 if absence was added to an existing group, 1 if group was created.
     */
    int _addAbsence( const char *id, const char *groupId, Coord lg, Coord lt, Scalar error, 
                     int num_attributes, Scalar *attributes );
};

#endif

