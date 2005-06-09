/**
 * Declaration of Occurrences class.
 * 
 * @file
 * @author Mauro E S Mu�oz (mauro@cria.org.br)
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

#include <om_defs.hh>
#include <occurrence.hh>
#include <configurable.hh>
#include <environment.hh>
#include <normalizable.hh>
#include <refcount.hh>
#include <env_io/geo_transform.hh>

#include <vector>
#include <string>

class OccurrencesImpl;
typedef ReferenceCountedPointer<OccurrencesImpl> OccurrencesPtr;
typedef ReferenceCountedPointer<const OccurrencesImpl> ConstOccurrencesPtr;

/****************************************************************/
/************************* Occurrences **************************/

/** 
 * Representation of a set of occurrences.
 *
 */
class dllexp OccurrencesImpl : public Configurable, public Normalizable, private ReferenceCountedObject
{
  friend class ReferenceCountedPointer<OccurrencesImpl>;
  friend class ReferenceCountedPointer<const OccurrencesImpl>;

public:

  typedef std::vector<OccurrencePtr>::const_iterator const_iterator;
  typedef std::vector<OccurrencePtr>::iterator iterator;

  /** Creates a collection of occurrences points.
   *
   */
  inline OccurrencesImpl( double default_abundance ) :
    name_( ),
    cs_( GeoTransform::cs_default ),
    gt_( 0 ),
    occur_( ),
    default_abundance_( default_abundance )
  {
    initGeoTransform();
  };

  /** Creates a collection of occurrences points.
   *  @param name Collection of occurrences' name.
   *  @param coord_system Coordinate system of the occurrences
   *   points to be inserted in this collection (in WKT format).
   */
  inline OccurrencesImpl( const std::string& name, 
			  const std::string& coord_system=GeoTransform::cs_default ) :
    name_( name ),
    cs_( coord_system ),
    gt_( 0 ),
    occur_(),
    default_abundance_( 0.0 )
  {
    initGeoTransform();
  };

  ~OccurrencesImpl();

  /** Deep copy of this. */
  OccurrencesImpl* clone() const;

  /** change the reserve setting for the container */
  void reserve( int estimate ) { occur_.reserve( estimate ); }

  /** name of the occurrences/species */
  char const * name() const { return name_.c_str(); }

  /** coordinate system name */
  char const * coordSystem() const { return cs_.c_str(); }

  /* Indicates whether the object contains environmental samples
   * that can be used instead of a full blown Environment object
   */
  bool hasEnvironment() const;

  /* Dimension of environmental space in samples stored in this object
   */
  int dimension() const;

  /** set the coordinate system
   *
   *  All occurrences inserted after this call are assumed to be
   *  in this coordinate system.
   */
  void setCoordinateSystem( const std::string& cs );

  /** Create an occurrence.
   * 
   * @param longitude Longitude in native coordinates
   * @param latitude Latitude in native coordinates
   * @param error (longitude, latitude) uncertanty in meters.
   * @param abundance - the abundance of the species: 0 for absence points.
   * @param num_attributes Number of possible modelling attributes.
   * @param attributes Vector with possible modelling attributes.
   * @param num_env Number of environment variables at sample
   * @param env Vector with environment variables.
   * 
   * deprecated by STL vector version.
   */
  void createOccurrence( Coord longitude, Coord latitude,
			 Scalar error, Scalar abundance,
			 int num_attributes = 0, Scalar *attributes = 0,
			 int num_env = 0, Scalar *env = 0 );

  /** Create an occurrence using STL vectors to input attributes and
   *  environment data
   */
  void createOccurrence( Coord longitude, Coord latitude,
			 Scalar error, Scalar abundance,
			 std::vector<double> attributes,
			 std::vector<double> env);

  /** Add an occurrence created outside. */
  void insert( const OccurrencePtr& );

  /** Remove a single entry */
  iterator erase( const iterator& it );

  /** Number of attributes of the thing occurred. This is the
   *  number of dependent variables, ie the variables to be
   *  modelled.
   * 
   *  Fixme - this is currently hard-coded to 0.
   */
  int numAttributes() const { return 0; }

  /** Number of occurrences. */
  int numOccurrences() const { return occur_.size(); }

  /** Test for empty. */
  bool isEmpty() const { return occur_.empty(); }

  /** Vector/Random access */
  ConstOccurrencePtr operator[]( int i ) const { return occur_[i]; }

  /** Linear access */
  const_iterator begin() const { return occur_.begin(); }
  const_iterator end() const { return occur_.end(); }
  iterator begin() { return occur_.begin(); }
  iterator end() { return occur_.end(); }

  /** Choose an occurrence at random. */
  ConstOccurrencePtr getRandom() const;

  // normalizable interface
  void getMinMax( Sample * min, Sample * max ) const;

  void normalize( bool useNormalization, 
		  const Sample& offsets, const Sample& scales );


  /** Sets environment object in each occurrence object
   */
  void setEnvironment( const EnvironmentPtr& env, 
		       const char *type = "Sample" );

  /** Appends all occurrences from source
   * @param source Occurrences object where occurrence pointers will
   *        be appended from.
   */
  void appendFrom(const OccurrencesPtr& source);

  /** Print occurrence data and its points. */
  void print( char *msg="" ) const;

  virtual ConfigurationPtr getConfiguration() const;

  virtual void setConfiguration( const ConstConfigurationPtr & );

private:

  void initGeoTransform();

  double default_abundance_;

  std::string name_; ///< List of occurrences' name (e.g. species name).
  std::string cs_;   ///< Coordinate system name

  /** Object to transform between different coordinate systems. */
  GeoTransform *gt_;

  std::vector< OccurrencePtr > occur_;  ///< Coordinates of the occurrences.
};


#endif
