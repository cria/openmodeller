/**
 * Declaration of Occurrences class.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-02-25
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


#ifndef _OCCURRENCESHH_
#define _OCCURRENCESHH_

#include "om_defs.hh"

class Occurrence;
class GeoTransform;

template<class T> class DList;


/****************************************************************/
/************************* Occurrences **************************/

/** 
 * Representation of a set of occurrences.
 *
 */
class Occurrences
{
  typedef DList<Occurrence *> LstOccur;


public:

  /** Creates a collection of occurrences points.
   *  @param name Collection of occurrences' name.
   *  @param coord_system Coordinate system of the occurrences
   *   points to be inserted in this collection (in WKT format).
   */
  Occurrences( char *name, char *coord_system=OM_WGS84 );
  ~Occurrences();

  char *name()   { return _name; }

  /** Insert an occurrence.
   * 
   * @param longitude Longitude in decimal degrees.
   * @param latitude Latitude in decimal degrees.
   * @param error (longitude, latitude) uncertanty in meters.
   * @param abundance Number of items found in (longitude,
   *  latitude).
   * @param num_attributes Number of possible modelling
   *  attributes.
   * @param attributes Vector with possible modelling attributes.
   */
  void insert( Coord longitude, Coord latitude, Scalar error,
	       Scalar abundance=1.0, int num_attributes=0,
	       Scalar *attributes=0 );

  /** Insert an occurrence without uncertanty. */
  void insert( Coord longitude, Coord latitude,
	       Scalar abundance=1.0, int num_attributes=0,
	       Scalar *attributes=0 );

  /** Number of attributes of the thing occurred. This is the
   *  number of dependent variables, ie the variables to be
   *  modelled.
   *  
   *  Fix: By now this is hardcoded to 1 (= abundance).
   */
  int numAttributes()  { return 1; }

  /** Number of occurrences. */
  int numOccurrences();

  /** Navigate on the list of occurrences. */
  void head();
  void next();
  Occurrence *get();

  /** Remove from list the current coordinate, returning it. */
  Occurrence *remove();

  /** Choose an occurrence at random. */
  Occurrence *getRandom();

  /** Print occurrence data and its points. */
  void print( char *msg="" );


private:

  /** Build a vector view for the occurrences in _occur */
  void buildVector();


  char *_name; ///< List of occurrences' name (e.g. species name).

  /** Object to transform between different coordinate systems. */
  GeoTransform *_gt;

  LstOccur *_occur;  ///< Coordinates of the occurrences.

  Occurrence **_vector; ///< Vector view to the occurrences.
};


#endif
