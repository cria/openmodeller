/**
 * Declaration of Occurrence class.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-25-02
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


#ifndef _OCCURRENCEHH_
#define _OCCURRENCEHH_

#include <om_defs.hh>


/****************************************************************/
/************************** Occurrence **************************/

/**
 * Stores a georeferenced occurrence locality (longitude,
 * latitude and uncertanty) and its attributes (abundance and
 * possibly others optional attributes).
 * 
 */
class Occurrence
{
public:

  /** Occurrence constructor with uncertanty.
   * 
   * @param x Longitude of the occurrence (decimal degrees).
   * @param y Latitude of the occurrence (decimal degrees).
   * @param error (x,y) uncertanty (meters).
   * @param abundance Number of items found in (x,y).
   * @param num_attributes Number of possible modelling
   *  attributes.
   * @param attributes Vector with possible modelling attributes.
   */
  Occurrence( Coord x, Coord y, Scalar error, Scalar abundance,
	      int num_attributes=0, Scalar *attributes=0 )
  {
    init( x, y, error, abundance, num_attributes, attributes );
  }

  /** Occurrence constructor without uncertanty.
   * 
   * @param x Longitude of the occurrence (decimal degrees).
   * @param y Latitude of the occurrence (decimal degrees).
   * @param error (x,y) uncertanty (meters).
   * @param abundance Number of items found in (x,y).
   * @param num_attributes Number of possible modelling
   *  attributes.
   * @param attributes Vector with possible modelling attributes.
   */
  Occurrence( Coord x, Coord y, Scalar abundance,
	      int num_attributes=0, Scalar *attributes=0 )
  {
    init( x, y, -1.0, abundance, num_attributes, attributes );
  }

  ~Occurrence();


  // Access to the locality information.
  Coord  x()       { return _x; }
  Coord  y()       { return _y; }
  Scalar error()   { return _error; }

  // Acces to attributes
  int numAttributes()        { return _nattr; }
  Scalar *attributes()       { return _attr; }
  Scalar attribute( int i )  { return _attr[i]; }

  /** Fills 'buffer' with the attributes.
   * @return The number of attributes read.
   */
  int readAttributes( Scalar *buffer );


  /** Abundance is the first attribute. */
  Scalar Abundance()    { return *_attr; }


private:

  /** Only to do not need to rewrite both constructors. */
  void init( Coord x, Coord y, Scalar error, Scalar abundance,
	     int nattr, Scalar *attr );

  Coord  _x;
  Coord  _y;
  Scalar _error;  ///< (x,y) uncertanty in meters.

  int    _nattr;  ///< Number of attributes.  
  Scalar *_attr;  ///< Possible modelling attributes.
};

typedef Occurrence *PtOccurrence;


#endif
