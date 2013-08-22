/**
 * Declaration of Occurrence class.
 * 
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

#include <openmodeller/om_defs.hh>
#include <openmodeller/Sample.hh>
#include <openmodeller/Normalizer.hh>

#include <openmodeller/refcount.hh>

#include <string>

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
class dllexp OccurrenceImpl : private ReferenceCountedObject
{
  friend class ReferenceCountedPointer<OccurrenceImpl>;
  friend class ReferenceCountedPointer<const OccurrenceImpl>;
public:

  /** Occurrence default constructor
   */
  OccurrenceImpl( ) :
    id_( ),
    x_( 0.0 ),
    y_( 0.0 ),
    error_( 0.0 ),
    abundance_( 0.0 ),
    attr_(),
    unnormEnv_(),
    normEnv_()
  {}

  /** Occurrence constructor with uncertanty.
   * 
   * @param id Unique id.
   * @param x Longitude of the occurrence (decimal degrees).
   * @param y Latitude of the occurrence (decimal degrees).
   * @param error (x,y) uncertanty (meters).
   * @param abundance Abundance at point (0=absence, 1=single presence).
   * @param num_attributes Number of possible modelling
   *  attributes. - abundance must be first attribute
   * @param attributes Vector with possible modelling attributes.
   * @param num_env Number of environmental variables.
   * @param env Vector with unnormalized environmental data at the point.
   */
  OccurrenceImpl( const std::string& id, 
                  Coord x, Coord y, Scalar error,
		  Scalar abundance,
		  int num_attributes=0,
		  Scalar *attributes=0,
		  int num_env=0,
		  Scalar *env=0) :
    id_( id ),
    x_( x ),
    y_( y ),
    error_( error ),
    abundance_( abundance ),
    attr_( num_attributes, attributes ),
    unnormEnv_( num_env, env ),
    normEnv_()
  { }

  /** Occurrence constructor with uncertanty, using std::vector
   * 
   * @param id Unique id.
   * @param x Longitude of the occurrence (decimal degrees).
   * @param y Latitude of the occurrence (decimal degrees).
   * @param error (x,y) uncertanty (meters).
   * @param abundance Abundance at point (0=absence, 1=single presence).
   * @param attributes Vector with possible modelling attributes.
   * @param env Vector with unnormalized environmental data at the point.
   */
  OccurrenceImpl( const std::string& id, 
                  Coord x, Coord y, Scalar error,
		  Scalar abundance,
		  std::vector<Scalar> attributes,
		  std::vector<Scalar> env) :
    id_( id ),
    x_( x ),
    y_( y ),
    error_( error ),
    abundance_( abundance ),
    attr_( attributes ),
    unnormEnv_( env ),
    normEnv_()
  { }

  /** Occurrence constructor with uncertanty.
   * 
   * @param id Unique id.
   * @param x Longitude of the occurrence (decimal degrees).
   * @param y Latitude of the occurrence (decimal degrees).
   * @param error (x,y) uncertanty (meters).
   * @param abundance Abundance at point (0=absence, 1=single presence).
   * @param attributes Vector with possible modelling attributes.
   * @param env Vector with unnormalized environmental data at the point.
   */
  OccurrenceImpl( const std::string& id, 
                  Coord x, Coord y, Scalar error,
		  Scalar abundance,
		  const Sample& attributes,
		  const Sample& env) :
    id_( id ),
    x_( x ),
    y_( y ),
    error_( error ),
    abundance_( abundance ),
    attr_( attributes ),
    unnormEnv_( env ),
    normEnv_()
  { }

  /** Occurrence constructor without uncertanty.
   * 
   * @param id Unique id.
   * @param x Longitude of the occurrence (decimal degrees).
   * @param y Latitude of the occurrence (decimal degrees).
   * @param error (x,y) uncertanty (meters).
   * @param abundance Abundance at point (0=absence, 1=single presence).
   * @param num_attributes Number of possible modelling
   *  attributes. - abundance must be first attribute
   * @param attributes Vector with possible modelling attributes.
   */
  OccurrenceImpl( const std::string& id, 
                  Coord x, Coord y,
                  Scalar abundance,
                  int num_attributes=0,
                  Scalar *attributes=0 ) :
    id_( id ),
    x_( x ),
    y_( y ),
    error_( -1.0 ),
    abundance_( abundance ),
    attr_( num_attributes, attributes ),
    unnormEnv_(),
    normEnv_()
  { }

  ~OccurrenceImpl();

  OccurrenceImpl( const OccurrenceImpl& rhs ) :
    id_( rhs.id_ ),
    x_( rhs.x_ ),
    y_( rhs.y_ ),
    error_( rhs.error_ ),
    abundance_( rhs.abundance_ ),
    attr_( rhs.attr_ ),
    unnormEnv_( rhs.unnormEnv_ ),
    normEnv_( rhs.normEnv_ )
  {};

  OccurrenceImpl& operator=(const OccurrenceImpl & );

  // Access to the locality information.
  std::string id() const   { return id_; }
  Coord  x() const         { return x_; }
  Coord  y() const         { return y_; }
  Scalar error() const     { return error_; }
  Scalar abundance() const { return abundance_; }

  Sample const & attributes() const { return attr_; }

  Sample const & environment() const; 

  Sample const & originalEnvironment() const; 

  void normalize( Normalizer * normalizerPtr, size_t categoricalThreshold=0 );

  void setNormalizedEnvironment( const Sample& s );

  void setUnnormalizedEnvironment( const Sample& s );

  bool hasEnvironment() const;

  void setId( const std::string& id );

  void setAbundance( Scalar value );

  void dump() const;

private:

  std::string id_; // Unique identifier
  Coord  x_; // Longitude
  Coord  y_; // Latitude
  Scalar error_;  // coordinate uncertanty in meters (unused so far)
  Scalar abundance_; // used only to distinguish between presence (1) and absence (0)

  Sample attr_; // extra attributes
  Sample unnormEnv_; // unnormalized environment
  Sample normEnv_; // normalized enviroment
};

#endif
