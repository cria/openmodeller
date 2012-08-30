/**
 * Declaration of Sampler and Samples classes.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-05-27
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

#ifndef _SAMPLERHH_
#define _SAMPLERHH_

#include <openmodeller/om_defs.hh>
#include <openmodeller/Occurrences.hh>
#include <openmodeller/Occurrence.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/Configurable.hh>
#include <openmodeller/Normalizable.hh>
#include <openmodeller/Normalizer.hh>

#include <openmodeller/refcount.hh>

class ModelImpl;
typedef ReferenceCountedPointer<ModelImpl> Model;

/****************************************************************/
/*************************** Sampler ****************************/

class SamplerImpl;
typedef ReferenceCountedPointer<SamplerImpl> SamplerPtr;
typedef ReferenceCountedPointer<const SamplerImpl> ConstSamplerPtr;

/** Splits data points into to 2 new samplers
 *  @param orig  Original sampler to split
 *  @param train Pointer to train sampler to be returned
 *  @param test  Pointer to test sampler to be returned
 *  @param propTrain Percentage of points to go to train sampler
 */
dllexp void splitSampler(const SamplerPtr& orig, SamplerPtr *train, SamplerPtr *test, double propTrain);

/**
 * Factory method that creates a Sampler based on Environment
 * and Occurrences objects
 */
dllexp SamplerPtr createSampler(const EnvironmentPtr& env,
			 const OccurrencesPtr& presence,
			 const OccurrencesPtr& absence = OccurrencesPtr() );

dllexp SamplerPtr createSampler( const ConstConfigurationPtr& config );

/** 
 * Base class to create samplers of environmental variables and
 * occurrence data. Each derived class can implement different 
 * behaviors, such as proportional sampling according to the 
 * distribution of occurrence data, disproportional sampling
 * regarding presence and absence data, etc.
 */
class dllexp SamplerImpl : public Configurable, public Normalizable, private ReferenceCountedObject
{
  friend class ReferenceCountedPointer<SamplerImpl>;
  friend class ReferenceCountedPointer<const SamplerImpl>;

public:

  SamplerImpl();
  SamplerImpl( const EnvironmentPtr& env,
	       const OccurrencesPtr& presences,
	       const OccurrencesPtr& absence,
	       bool  isNormalized=false);
  ~SamplerImpl();

  EnvironmentPtr getEnvironment() { return _env; }
  ConstEnvironmentPtr getEnvironment() const { return _env; }

  OccurrencesPtr getPresences() { return _presence; }
  ConstOccurrencesPtr getPresences() const { return _presence; }

  OccurrencesPtr getAbsences() { return _absence; }
  ConstOccurrencesPtr getAbsences() const { return _absence; }

  /** Indicate that all non categorical variable layers must
   *  be normalized according to the interval [min, max].
   *  Returns through offsets, scales the normalization parameters.
   */
  void getMinMax( Sample * min, Sample * max ) const;

  /** Normalize sampler data */
  void normalize( Normalizer * normalizerPtr );

  /** Returns true if "normalized()" was called, false otherwise.
   */
  bool isNormalized() const { return _normalized; }

  /** Reset normalization */
  void resetNormalization();

  /** Number of independent variables (environmental variables). */
  int numIndependent() const;

  /** Number of dependent variables (attributes of the occurred
   *  thing). These are the variables to be modelled.
   */
  int numDependent() const;

  /** Number of presences (occurrence points). */
  int numPresence() const;

  /** Number of absences (localities with no occurrence). */
  int numAbsence() const;

  /** Samples one presence, absence or pseudo-absence point in
   * a uniform way.
   * There is 50% of chance to get a presence point and 50% of
   * get an absence or pseudo-absence point.
   * If there are real absence points (user input) then only
   * absence are sampled. If there are not real absence, samples
   * pseudo-absence points.
   * 
   * @param independent Filled in with values of the independent
   *  variables of the sample.
   *
   * @return Zero if got an absence or pseudo-absence point and
   *  not zero if got a presence point.
   */
  ConstOccurrencePtr getOneSample( ) const;

  /**
   * Get a random presence point
   */
  ConstOccurrencePtr getPresence() const 
  { return getRandomOccurrence( _presence ); }

  /**
   * Get a specific presence point by index
   */
  ConstOccurrencePtr getPresence( int index ) const 
  { return _presence->operator[]( index ); }

  /**
   * Get a random absence point
   */
  ConstOccurrencePtr getAbsence() const 
  { return getRandomOccurrence( _absence ); }

  /**
   * Get a specific absence point by index
   */
  ConstOccurrencePtr getAbsence( int index ) const 
  { return _absence->operator[]( index ); }

  /**
   * Generate a random sample
   */
  OccurrencePtr generateRandomSample( Scalar abundance=1.0 ) const;

  /**
   * Generate a pseudo absence sample
   */
  OccurrencePtr getPseudoAbsence() const;

  /**
   * Generate a pseudo absence sample outside the given model
   */
  OccurrencePtr getPseudoAbsence( const Model& model, const Scalar threshold ) const;

  /**
   * Generate a pseudo absence sample outside the minimum and maximum
   */
  OccurrencePtr getPseudoAbsenceOutsideInterval( const Sample * minimum, const Sample * maximum ) const;

  /**
   * Generate pseudo absence samples
   */
  OccurrencesPtr getPseudoAbsences( const int& numPoints, const Model& model, const Scalar threshold=0.5, const bool geoUnique=false, const bool envUnique=false) const;

  /**
   * Generate pseudo absence samples
   */
  OccurrencesPtr getPseudoAbsences( const int& numPoints, const Sample * minimum, const Sample * maximum, const bool geoUnique=false, const bool envUnique=false) const;

  /** Returns 1 if i-th variable is categorical,
   * otherwise returns 0.
   */
  int isCategorical( int i );

  /** Remove sample duplicates accross the environment (presences and absences are treated separately).
   *  After erasing a point, the remaining one increases the abundance by one.
   */
  void environmentallyUnique( );

  /** If OccurrencePtr point duplicates accross the environment, return false. Otherwise return true.
   *  Missae(set/2008)
   */
  bool isEnvironmentallyUnique( const OccurrencesPtr& occurrences, const OccurrencePtr& point ) const;

  /** Remove sample duplicates accross geographic space (presences and absences are treated separately).
   *  After erasing a point, the remaining one increases the abundance by one.
   *  Uniqueness is considered for row/col pairs defined in the input mask.
   *  If mask is undefined, use first layer as a mask.
   */
  void spatiallyUnique( );

   /** If OccurrencePtr point duplicates accross geographic space, return false. Otherwise return true.
   *  Uniqueness is considered for row/col pairs defined in the input mask.
   *  If mask is undefined, use first layer as a mask.
   *  Missae(set/2008)
   */
  bool isSpatiallyUnique( const OccurrencesPtr& occurrences, const OccurrencePtr& point ) const;

  SamplerPtr clone() const;

  ConfigurationPtr getConfiguration() const;

  void setConfiguration ( const ConstConfigurationPtr& );

  void dump() const;

private:

  ConstOccurrencePtr getRandomOccurrence( const OccurrencesPtr& occur ) const;

  // this was former method ::initialize()
  void setEnvironmentInOccurrences();

  // Remove sample duplicates accross the environment given one of the occurrences 
  // type (presence / absence)
  void environmentallyUnique( OccurrencesPtr& occurrencesPtr, const char *type );

  // Remove sample duplicates accross the geographic space given one of the occurrences 
  // type (presence / absence)
  void spatiallyUnique( OccurrencesPtr& occurrencesPtr, const char *type );

  OccurrencesPtr _presence;
  OccurrencesPtr _absence;
  EnvironmentPtr _env;
  bool _normalized;
};


#endif
