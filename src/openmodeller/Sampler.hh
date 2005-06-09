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

#include <om_defs.hh>
#include <om_occurrences.hh>
#include <occurrence.hh>
#include <environment.hh>
#include <configurable.hh>
#include <normalizable.hh>

#include <refcount.hh>


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
SamplerPtr createSampler(const EnvironmentPtr& env,
			 const OccurrencesPtr& presence,
			 const OccurrencesPtr& absence = OccurrencesPtr() );

SamplerPtr createSampler( const ConstConfigurationPtr& config );

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
  friend SamplerPtr createSampler( const ConstConfigurationPtr & );
public:

  SamplerImpl();
  SamplerImpl( const EnvironmentPtr& env,
	       const OccurrencesPtr& presences,
	       const OccurrencesPtr& absence);
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
  void getMinMax(Sample * min, Sample * max ) const;

  /** Set specific normalization parameters
   */
  void normalize( bool use_norm, const Sample& offsets, const Sample& scales );

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
   * Get one Presence point
   */
  ConstOccurrencePtr getPresence() const 
  { return getRandomOccurrence( _presence ); }

  /**
   * Get one Absence point
   */
  ConstOccurrencePtr getAbsence() const 
  { return getRandomOccurrence( _absence ); }

  /**
   * Get one pseudoAbsence point
   */
  ConstOccurrencePtr getPseudoAbsence() const;

  /** Returns 1 if i-th variable is categorical,
   * otherwise returns 0.
   */
  int isCategorical( int i );

  ConfigurationPtr getConfiguration() const;

  void setConfiguration ( const ConstConfigurationPtr& );

private:

  ConstOccurrencePtr getRandomOccurrence( const OccurrencesPtr& occur ) const;

  // this was former method ::initialize()
  void setEnvironmentInOccurrences();
  
  OccurrencesPtr _presence;
  OccurrencesPtr _absence;
  EnvironmentPtr _env;
};


#endif
