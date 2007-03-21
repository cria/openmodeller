/**
 * Definition of Sampler and Samples classes.
 * 
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

#include <openmodeller/Environment.hh>
#include <openmodeller/Occurrence.hh>
#include <openmodeller/Configuration.hh>

#include <openmodeller/Sampler.hh>
#include <openmodeller/Occurrences.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Random.hh>
#include <openmodeller/env_io/Map.hh>

#include <string.h>

#include <stdio.h>

#include <string>
#include <algorithm>

#include <openmodeller/Exceptions.hh>

using std::string;

#undef DEBUG_MEMORY

/****************************************************************/
/*************************** Sampler ****************************/

SamplerPtr createSampler( const EnvironmentPtr& env,
			  const OccurrencesPtr& presence,
			  const OccurrencesPtr& absence )
{
  return SamplerPtr( new SamplerImpl( env, presence, absence ) );
}

SamplerPtr createSampler( const ConstConfigurationPtr& config )
{
  SamplerPtr samp( new SamplerImpl() );
  samp->setConfiguration( config );
  return samp;
}

/*******************/
/*** constructor ***/

SamplerImpl::SamplerImpl() :
  ReferenceCountedObject(),
  _presence(),
  _absence(),
  _env()
{
#ifdef DEBUG_MEMORY
  Log::instance()->debug("SamplerImpl::SamplerImpl() at %x\n",this);
#endif
}

SamplerImpl::SamplerImpl( const EnvironmentPtr& env, 
			  const OccurrencesPtr& presence,
			  const OccurrencesPtr& absence ) :
  ReferenceCountedObject(),
  _presence( presence ),
  _absence( absence ),
  _env( env )
{
#ifdef DEBUG_MEMORY
  Log::instance()->debug("SamplerImpl::SamplerImpl( args ) at %x\n",this);
#endif
  setEnvironmentInOccurrences();
}

/******************/
/*** destructor ***/

SamplerImpl::~SamplerImpl()
{
#ifdef DEBUG_MEMORY
  Log::instance()->debug("SamplerImpl::~SamplerImpl() at %x\n",this);
#endif
}

void
SamplerImpl::setEnvironmentInOccurrences()
{
  // renamed former method ::initialize()

  if ( !_presence->hasEnvironment() )
    {
      // Copy data from environment into the presence and absence points.
      _presence->setEnvironment( _env, "Presence" );

      if ( _absence && _absence->numOccurrences() ) {

	_absence->setEnvironment( _env, "Absence" );
      }
    }
}

/******************/
/*** configuration ***/

ConfigurationPtr
SamplerImpl::getConfiguration( ) const
{

  ConfigurationPtr config( new ConfigurationImpl( "Sampler" ) );

  if ( _env ) {

    config->addSubsection( _env->getConfiguration() );
  }

  if ( _presence ) {

    ConfigurationPtr cfg( _presence->getConfiguration() );
    cfg->setName( "Presence" );
    config->addSubsection( cfg );
  }

  if (_absence ) {

    ConfigurationPtr cfg( _absence->getConfiguration() );
    cfg->setName( "Absence" );
    config->addSubsection( cfg );
  }

  return config;
}

void
SamplerImpl::setConfiguration( const ConstConfigurationPtr& config )
{
  EnvironmentPtr env;
  if ( ConstConfigurationPtr env_config = config->getSubsection( "Environment", false ) ) {

    env = createEnvironment();
    env->setConfiguration( env_config );
  }

  //
  // Here's a hack for ya.  We need the Presence points to have abundance=1.0
  // So the Occurrences container now has a default abundance so if
  // the record in the configuration doesn't have an abundance for a point
  // it is set to the default value.
  //
  // As of now, the configuration for occurrences do not set/get
  // abundance values.
  //

  Log::instance()->debug( "Getting presence\n");
  OccurrencesPtr presence( new OccurrencesImpl(1.0) );
  presence->setConfiguration( config->getSubsection( "Presence" ) );

  Log::instance()->debug( "Getting absence\n");
  OccurrencesPtr absence;

  if ( ConstConfigurationPtr absence_config = config->getSubsection( "Absence", false ) ) {

    absence = new OccurrencesImpl(0.0);
    absence->setConfiguration( absence_config );
  }

  if ( ! presence->numOccurrences() ) { 

    throw SamplerException( "No presence points available." );
  }

  Log::instance()->debug( "Loaded %u presence(s)\n", presence->numOccurrences() );

  int num_absences = 0;

  if ( absence ) {

    num_absences = absence->numOccurrences();
  }

  Log::instance()->debug( "Loaded %u absence(s)\n", num_absences );

  _env = env;
  _presence = presence;
  _absence  = absence;

  setEnvironmentInOccurrences();
}

/*****************/
/*** normalize ***/
void SamplerImpl::getMinMax( Sample * min, Sample * max ) const
{
  // if Sampler has both Environment object and a sampled Occurrences, then
  // favor Environment when doing normalization
  if ( _env ) { 

    _env->getMinMax( min, max );
  }
  else {

    // no environment object exists, so normalize samples in occs objects
    // first get all occurrence objects in the same container
    OccurrencesPtr allOccs( new OccurrencesImpl( _presence->name(),
                                                 _presence->coordSystem() ) );
    allOccs->appendFrom( _presence );
    allOccs->appendFrom( _absence );

    // now compute normalization parameters
    allOccs->getMinMax(min, max);
  }
}

/** Set specific normalization parameters
 */
void SamplerImpl::normalize( bool use_norm, const Sample& offsets, const Sample& scales )
{
  if (_env) {

      // set env in all occurrences before normalizing env so that
      // occurrences get the unnormalized values
      setEnvironmentInOccurrences();
      _env->normalize( use_norm, offsets, scales );
  }

  // need to normalize presences and absences even if _env is present
  // because environment in occurrences was set with unnormalized values
  // if _env doesn't exist, then normalize occurrences anyway
  _presence->normalize( use_norm, offsets, scales );

  if ( _absence && _absence->numOccurrences() ) {

    _absence->normalize( use_norm, offsets, scales );
  }
}

/***********************/
/*** num Independent ***/
int
SamplerImpl::numIndependent() const
{
  if (_env)
    // get number of dimensions from environment object if it exists
    return _env->numLayers();

  else if (_presence->hasEnvironment())
    // otherwise tries to get it from occurrences
    return _presence->dimension();

  else
    // neither object has dimensions defined
    return 0;
}


/*********************/
/*** num Dependent ***/
int
SamplerImpl::numDependent() const
{
  return _presence->numAttributes();
}


/********************/
/*** num Presence ***/
int
SamplerImpl::numPresence() const
{
  return _presence->numOccurrences();
}


/*******************/
/*** num Absence ***/
int
SamplerImpl::numAbsence() const
{
  return _absence ? _absence->numOccurrences() : 0;
}


ConstOccurrencePtr
SamplerImpl::getOneSample( ) const
{
  Random rnd;

  if ( !_presence->numOccurrences() ) { 

    throw SamplerException("No presence points available."); 
  }

  // Probability of 0.5 of get a presence point.
  if ( rnd() < 0.5 ) {

    return getPresence();
  }

  // Probability of 0.5 of get an absence point.
  // (if there are real absence points...)
  if ( _absence && _absence->numOccurrences() ) {

    return getAbsence();
  }

  return getPseudoAbsence();
}

ConstOccurrencePtr
SamplerImpl::getPseudoAbsence() const 
{
  if ( !_env ) {

    throw SamplerException("Trying to get a pseudo absence point without setting up an Environment object");
  }

  //
  // Get a random pseudo-absence point.
  //
  static const Sample absenceSample( numDependent() );
  Coord x,y;

  Sample env( _env->getRandom( &x, &y ) );

  ConstOccurrencePtr oc = new OccurrenceImpl( "?", x, y, 0.0, 0.0, absenceSample, env );

  return oc;
}

/*****************/
/*** var Types ***/
int
SamplerImpl::isCategorical( int i )
{
  if ( _env ) {

    return _env->isCategorical( i );
  }
  else {
    // if there is no environment obj, assumes all variables are continuous
    // right now there is no mechanism to define whether a variable is 
    // continuous or categorical when occurrences already come with their
    // samples populated
    return false;
  }
}


/******************************/
/*** environmentally unique ***/
void 
SamplerImpl::environmentallyUnique( )
{
  // Presences
  this->environmentallyUnique( _presence, "Presence" );

  // Absences
  this->environmentallyUnique( _absence, "Absence" );
}


/******************************/
/*** environmentally unique ***/
void 
SamplerImpl::environmentallyUnique( OccurrencesPtr& occurrencesPtr, const char *type )
{
  if ( ! ( occurrencesPtr && occurrencesPtr->numOccurrences() ) ) {

    return;
  }

  OccurrencesImpl::iterator it   = occurrencesPtr->begin();
  OccurrencesImpl::iterator last = occurrencesPtr->end();

  while ( it != last ) {

    Sample sample = _env->getUnnormalized( (*it)->x(), (*it)->y() );

    OccurrencesImpl::iterator next = it + 1;

    while ( next != last ) {

      Sample nextSample = _env->getUnnormalized( (*next)->x(), (*next)->y() );

      if ( sample.equals( nextSample ) ) {

        Log::instance()->info ( "%s Point at (%f,%f) has no unique environment. It will be discarded.\n", 
               type, (*next)->x(), (*next)->y() );

        // Remove duplicates
        next = occurrencesPtr->erase( next );
        last = occurrencesPtr->end();

        // Increase abundance in original occurence
        (*it)->setAbundance( (*it)->abundance() + 1 );

        // No need to increment "next" because "erase" actually swaps 
        // the last element with the one that was just erased!
      }
      else {

        ++next;
      }
    }

    ++it;
  }
}


/************************/
/*** spatially unique ***/
void 
SamplerImpl::spatiallyUnique( )
{
  // Presences
  this->spatiallyUnique( _presence, "Presence" );

  // Absences
  this->spatiallyUnique( _absence, "Absence" );
}


/************************/
/*** spatially unique ***/
void 
SamplerImpl::spatiallyUnique( OccurrencesPtr& occurrencesPtr, const char *type )
{
  if ( ! ( occurrencesPtr && occurrencesPtr->numOccurrences() ) ) {

    return;
  }

  Map *mask = _env->getMask();

  // If mask is undefined, use first layer as a mask
  if ( ! mask ) {

    mask = _env->getLayer( 0 );
  }

  OccurrencesImpl::iterator it   = occurrencesPtr->begin();
  OccurrencesImpl::iterator last = occurrencesPtr->end();

  while ( it != last ) {

    int row, col;

    mask->getRowColumn( (*it)->x(), (*it)->y(), &row, &col );

    OccurrencesImpl::iterator next = it + 1;

    while ( next != last ) {

      int nextRow, nextCol;

      mask->getRowColumn( (*next)->x(), (*next)->y(), &nextRow, &nextCol );

      if ( row == nextRow && col == nextCol ) {

        Log::instance()->info("%s Point at (%f,%f) has no unique geography. It will be discarded.\n", 
               type, (*next)->x(), (*next)->y() );

        // Remove duplicates
        next = occurrencesPtr->erase( next );
        last = occurrencesPtr->end();

        // Increase abundance in original occurence
        (*it)->setAbundance( (*it)->abundance() + 1 );

        // No need to increment "next" because "erase" actually swaps 
        // the last element with the one that was just erased!
      }
      else {

        ++next;
      }
    }

    ++it;
  }
}


/*****************************/
/*** get Random Occurrence ***/
ConstOccurrencePtr
SamplerImpl::getRandomOccurrence( const OccurrencesPtr& occur ) const
{
  // This has been rewritten to eliminate the
  // possibly endless loop.
  // It assumes that by the time this routine is called,
  // all occurrences have valid environment data.
  
  return occur->getRandom();

}


/**************************/
/**** splitOccurrences ****/
static void splitOccurrences(OccurrencesPtr& occurrences, 
			     OccurrencesPtr& trainOccurrences, 
			     OccurrencesPtr& testOccurrences, 
			     double propTrain)
{
  // add all samples to an array
  int i;
  int n = occurrences->numOccurrences();
  int k = (int) (n * propTrain);
  std::vector<int> goToTrainSet(n);

  // first k are set to go to train set
  for (i = 0; i < k; i++) {

    goToTrainSet[i] = 1;
  }

  // all others are set to go to test set
  for (; i < n; i++) {

    goToTrainSet[i] = 0;
  }

  // shuffle elements well
  std::random_shuffle(goToTrainSet.begin(), goToTrainSet.end());

  // traverse occurrences copying them to the right sampler
  OccurrencesImpl::const_iterator it = occurrences->begin();
  OccurrencesImpl::const_iterator fin = occurrences->end();

  i = 0;
  while( it != fin ) {

    if ( goToTrainSet[i] ) {

      trainOccurrences->insert( new OccurrenceImpl( *(*it) ) );
      //printf("+");
    }
    else {

      testOccurrences->insert( new OccurrenceImpl( *(*it) ) );
      //printf("-");
    }
    ++i; ++it;
  }

  //printf("\n");

}


/********************/
/**** split *********/
void splitSampler(const SamplerPtr& orig, 
		  SamplerPtr * train,
		  SamplerPtr * test,
		  double propTrain)
{
  // split presences
  OccurrencesPtr presence = orig->getPresences();
  OccurrencesPtr test_presence( new OccurrencesImpl( presence->name(),  
                                                     presence->coordSystem() ) );

  OccurrencesPtr train_presence( new OccurrencesImpl( presence->name(),
                                                      presence->coordSystem() ) );

  splitOccurrences( presence, train_presence, test_presence, propTrain );

  // split absences if there are any
  OccurrencesPtr train_absence;
  OccurrencesPtr test_absence;

  OccurrencesPtr absence = orig->getAbsences();

  if ( absence ) { 

    test_absence = new OccurrencesImpl( absence->name(), absence->coordSystem());

    train_absence = new OccurrencesImpl( absence->name(), absence->coordSystem());

    splitOccurrences(absence, train_absence, test_absence, propTrain);
  }

  *train = new SamplerImpl( orig->getEnvironment(), 
                            train_presence, train_absence );

  *test = new SamplerImpl( orig->getEnvironment(),
                           test_presence, test_absence );
}
