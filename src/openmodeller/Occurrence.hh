/**
 * Declaration of Occurrence class.
 * 
 * @file
 * @author Mauro E S Mu�oz (mauro@cria.org.br)
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
#include <Sample.hh>

#include <refcount.hh>

/****************************************************************/
/************************** Occurrence **************************/

class OccurrenceImpl;
typedef ReferenceCountedPointer< OccurrenceImpl > OccurrencePtr;
typedef ReferenceCountedPointer< const OccurrenceImpl > ConstOccurrencePtr;

/**
 * Stores a georeferenced occurrence locality (longitude,
 * latitude and uncertanty) and its attributes (abundance and
 * possibly others optional attributes).
 * 
 */
class OccurrenceImpl : private ReferenceCountedObject
{
  friend class ReferenceCountedPointer<OccurrenceImpl>;
  friend class ReferenceCountedPointer<const OccurrenceImpl>;
public:

  /** Occurrence default constructor
   */
  OccurrenceImpl( ) :
    x_( 0.0 ),
    y_( 0.0 ),
    error_( 0.0 ),
    abundance_( 0.0 ),
    attr_(),
    env_()
  {}

  /** Occurrence constructor with uncertanty.
   * 
   * @param x Longitude of the occurrence (decimal degrees).
   * @param y Latitude of the occurrence (decimal degrees).
   * @param error (x,y) uncertanty (meters).
   * @param num_attributes Number of possible modelling
   *  attributes. - abundance must be first attribute
   * @param attributes Vector with possible modelling attributes.
   */
  OccurrenceImpl( Coord x, Coord y, Scalar error,
		  Scalar abundance,
		  int num_attributes=0,
		  Scalar *attributes=0,
		  int num_env=0,
		  Scalar *env=0) :
    x_( x ),
    y_( y ),
    error_( error ),
    abundance_( abundance ),
    attr_( num_attributes, attributes ),
    env_( num_env, env )
  { }

  /** Occurrence constructor with uncertanty.
   * 
   * @param x Longitude of the occurrence (decimal degrees).
   * @param y Latitude of the occurrence (decimal degrees).
   * @param error (x,y) uncertanty (meters).
   * @param num_attributes Number of possible modelling
   *  attributes. - abundance must be first attribute
   * @param attributes Vector with possible modelling attributes.
   */
  OccurrenceImpl( Coord x, Coord y, Scalar error,
		  Scalar abundance,
		  const Sample& attributes,
		  const Sample& env) :
    x_( x ),
    y_( y ),
    error_( error ),
    abundance_( abundance ),
    attr_( attributes ),
    env_( env )
  { }

  /** Occurrence constructor without uncertanty.
   * 
   * @param x Longitude of the occurrence (decimal degrees).
   * @param y Latitude of the occurrence (decimal degrees).
   * @param error (x,y) uncertanty (meters).
   * @param num_attributes Number of possible modelling
   *  attributes. - abundance must be first attribute
   * @param attributes Vector with possible modelling attributes.
   */
  OccurrenceImpl( Coord x, Coord y,
		  Scalar abundance,
		  int num_attributes=0,
		  Scalar *attributes=0 ) :
    x_( x ),
    y_( y ),
    error_( -1.0 ),
    abundance_( abundance ),
    attr_( num_attributes, attributes ),
    env_()
  { }

  ~OccurrenceImpl();

  OccurrenceImpl( const OccurrenceImpl& rhs ) :
    x_( rhs.x_ ),
    y_( rhs.y_ ),
    error_( rhs.error_ ),
    abundance_( rhs.abundance_ ),
    attr_( rhs.attr_ ),
    env_( rhs.env_ )
  {};

  OccurrenceImpl& operator=(const OccurrenceImpl & );

  // Access to the locality information.
  Coord  x() const         { return x_; }
  Coord  y() const         { return y_; }
  Scalar error() const     { return error_; }
  Scalar abundance() const { return abundance_; }

  Sample const & attributes() const { return attr_; }

  Sample const & environment() const { return env_; }

  void setEnvironment( const Sample& );

private:
  Coord  x_;
  Coord  y_;
  Scalar error_;  ///< (x,y) uncertanty in meters.
  Scalar abundance_;

  Sample attr_;
  Sample env_;
};

#endif
