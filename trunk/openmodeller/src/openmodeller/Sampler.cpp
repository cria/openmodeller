/**
 * Definition of Sampler and Samples classes.
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

#include <environment.hh>
#include <occurrence.hh>
#include <configuration.hh>

#include <om_sampler.hh>
#include <om_occurrences.hh>
#include <om_log.hh>
#include <random.hh>

#include <string.h>

#include <stdio.h>

#include <string>
#include <algorithm>

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
  g_log.debug("SamplerImpl::SamplerImpl() at %x\n",this);
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
  g_log.debug("SamplerImpl::SamplerImpl( args ) at %x\n",this);
#endif
  initialize();
}

/******************/
/*** destructor ***/

SamplerImpl::~SamplerImpl()
{
#ifdef DEBUG_MEMORY
  g_log.debug("SamplerImpl::~SamplerImpl() at %x\n",this);
#endif
}

static
void SetEnvironmentInOccurrences( EnvironmentPtr& env, OccurrencesPtr& occurs, const char *type = "Sample" )
{

  if ( !occurs )
    return;

  if ( occurs->isEmpty() )
    return;

  OccurrencesImpl::iterator oc = occurs->begin();
  OccurrencesImpl::iterator fin = occurs->end();

  while (oc != fin ) {

    Sample sample = env->get( (*oc)->x(), (*oc)->y() );

    if ( sample.size() == 0 ) {

      g_log( "%s Point at (%f,%f) has no environment.  It is removed.\n", type, (*oc)->x(), (*oc)->y() );
      oc = occurs->erase( oc );
      fin = occurs->end();

    } else {

      (*oc)->setEnvironment( sample );

      ++oc;
    }

  }
}

void
SamplerImpl::initialize()
{
  // Copy data from environment into the presence and absence points.
  SetEnvironmentInOccurrences( _env, _presence, "Presence" );
  SetEnvironmentInOccurrences( _env, _absence, "Absence" );
}

/******************/
/*** configuration ***/

ConfigurationPtr
SamplerImpl::getConfiguration( ) const
{

  ConfigurationPtr config( new ConfigurationImpl( "Sampler" ) );

  config->addSubsection( _env->getConfiguration() );

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

  EnvironmentPtr env = createEnvironment();
  env->setConfiguration( config->getSubsection( "Environment" ) );

  //
  // Here's a hack for ya.  We need the Presence points to have abundance=1.0
  // So the Occurrences container now has a default abundance so if
  // the record in the configuration doesn't have an abundance for a point
  // it is set to the default value.
  //
  // As of now, the configuration for occurrences do not set/get
  // abundance values.
  //

  g_log.debug( "Getting presence\n");
  OccurrencesPtr presence( new OccurrencesImpl(1.0) );
  presence->setConfiguration( config->getSubsection( "Presence" ) );

  g_log.debug( "Getting absence\n");
  OccurrencesPtr absence;

  if (ConstConfigurationPtr absence_config = config->getSubsection( "Absence", false ) ) {
    absence = new OccurrencesImpl(0.0);
    absence->setConfiguration( absence_config );
  }

  _env = env;
  _presence = presence;
  _absence = absence;

  initialize();

}

/*****************/
/*** normalize ***/
void SamplerImpl::computeNormalization(Scalar min, Scalar max, Sample *offsets, Sample *scales) const
{
  _env->computeNormalization(min, max, offsets, scales);
}

/** Set specific normalization parameters
 */
void SamplerImpl::setNormalization( bool use_norm, const Sample& offsets, const Sample& scales )
{
  _env->setNormalization( use_norm, offsets, scales );
  initialize();
}

/***********************/
/*** num Independent ***/
int
SamplerImpl::numIndependent() const
{
  return _env->numLayers();
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

  if (!_presence->numOccurrences())
    { 
      g_log.error(1, "No presence points available!!!\n"); 
    }

  // Probability of 0.5 of get a presence point.
  if ( rnd() < 0.5 )
    {
      return getPresence();
    }

  //
  // Probability of 0.5 of get an absence point.
  //

  // If there are real absence points...
  if ( _absence.operator bool() && _absence->numOccurrences())
    {
      return getAbsence();
    }

  return getPseudoAbsence();
}

ConstOccurrencePtr
SamplerImpl::getPseudoAbsence() const {

  //
  // Get a random pseudo-absence point.
  //
  static const Sample absenceSample( numDependent() );
  Coord x,y;

  Sample env( _env->getRandom( &x, &y ) );

  ConstOccurrencePtr oc = new OccurrenceImpl( x, y, 0.0, 0.0, absenceSample, env );

  return oc;
}

/*****************/
/*** var Types ***/
int
SamplerImpl::isCategorical( int i )
{
  return _env->isCategorical( i );
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
  OccurrencesPtr test_presence(new OccurrencesImpl(presence->name(),  
						   presence->coordSystem()));

  OccurrencesPtr train_presence(new OccurrencesImpl(presence->name(),
						    presence->coordSystem()));

  splitOccurrences(presence, train_presence, test_presence, propTrain);

  // split absences if there are any
  OccurrencesPtr train_absence;
  OccurrencesPtr test_absence;

  OccurrencesPtr absence = orig->getAbsences();

  if ( absence ) { 
      OccurrencesPtr test_absence = 
	new OccurrencesImpl( absence->name(), absence->coordSystem());

      OccurrencesPtr train_absence = 
	new OccurrencesImpl( absence->name(), absence->coordSystem());

      splitOccurrences(absence, train_absence, test_absence, propTrain);
  }

  *train = new SamplerImpl( orig->getEnvironment(), 
			    train_presence, train_absence );

}
