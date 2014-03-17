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

/**
 * A common interface to occurrence readers. Implementations need to define a constructor
 * with two const char * parameters: one receiving the source string and the other
 * receiving the default coordinate system in WKT. This way, each instance can only be
 * used for single source. The "load" method must also be defined and implemented in a way
 * that, if called more than once, data will not be duplicated.
 * Implementations must also define a static CreateOccurrencesReaderCallback method receiving 
 * the same constructor parameters and returning a valid instance.
 */
class dllexp OccurrencesReader
{
public:

  /** Destructor.
   */
  virtual ~OccurrencesReader() {};

  /** Try to load the driver given the source provided in the constructor.
   * @return bool True if driver could be loaded and occurrences can be read, false otherwise
   */
  virtual bool load() = 0;

  /** Return the presences (abundance > 0) from a specific group.
   * @param groupId Identifier for a group of occurrences (usually a species name).
   * @return Pointer to occurrences of the specified group. If the group was not found
   *         or not specified, an empty OccurrencesPtr is returned.
   */
  virtual OccurrencesPtr getPresences( const char *groupId );

  /** Return the absences (abundance == 0) from a specific group.
   * @param groupId Identifier for a group of occurrences (usually a species name).
   * @return Pointer to occurrences of the specified group. If the group was not found
   *         or not specified, an empty OccurrencesPtr is returned.
   */
  virtual OccurrencesPtr getAbsences( const char *groupId );

  /** Print the occurrences to cout.
   * @param msg Optional string to be printed before the occurrences.
   */
  void printOccurrences( const std::string& msg="" );

protected:

  typedef std::vector<OccurrencesPtr> LstOccurrences;

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
  int _addOccurrence( const char *id, const char *groupId, Coord lg, Coord lt, Scalar error,
                      Scalar abundance, int num_attributes, Scalar *attributes );

  // List of presences by group name.
  LstOccurrences _presences;

  // List of absences by group name.
  LstOccurrences _absences;
	
  // Occurrences source
  char *_source;
	
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

