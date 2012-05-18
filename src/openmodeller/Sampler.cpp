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
#include <openmodeller/Model.hh>

#include <string.h>

#include <stdio.h>

#include <string>

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
  _env(),
  _normalized( false )
{
#ifdef DEBUG_MEMORY
  Log::instance()->debug("SamplerImpl::SamplerImpl() at %x\n",this);
#endif
}

SamplerImpl::SamplerImpl( const EnvironmentPtr& env, 
			  const OccurrencesPtr& presence,
			  const OccurrencesPtr& absence,
			  bool  isNormalized ) :
  ReferenceCountedObject(),
  _presence( presence ),
  _absence( absence ),
  _env( env ),
  _normalized( isNormalized )
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
  // Copy data from environment into the presence and absence points.

  if ( _presence && ! _presence->hasEnvironment() ) {

    _presence->setEnvironment( _env, "Presence" );
  }

  if ( _absence && ! _absence->hasEnvironment() ) {

    _absence->setEnvironment( _env, "Absence" );
  }
}

/*********************/
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

  if ( _absence ) {

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

  Log::instance()->debug( "Getting presence\n" );
  OccurrencesPtr presence( new OccurrencesImpl(1.0) );
  presence->setConfiguration( config->getSubsection( "Presence" ) );

  Log::instance()->debug( "Getting absence\n" );
  OccurrencesPtr absence;

  if ( ConstConfigurationPtr absence_config = config->getSubsection( "Absence", false ) ) {

    absence = new OccurrencesImpl(0.0);
    absence->setConfiguration( absence_config );
  }

  if ( ! presence->numOccurrences() ) { 

    std::string msg = "No presence points available.\n";

    Log::instance()->error( msg.c_str() );

    throw SamplerException( msg );
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

/******************/
/*** get MinMax ***/
void SamplerImpl::getMinMax( Sample * min, Sample * max ) const
{
  // normalize samples in occs objects
  // first get all occurrence objects in the same container
  OccurrencesPtr allOccs;

  if ( _presence ) {

    allOccs = new OccurrencesImpl( _presence->name(), _presence->coordSystem() );
  }
  else {

    allOccs = new OccurrencesImpl( _absence->name(), _absence->coordSystem() );
  }

  allOccs->appendFrom( _presence );
  allOccs->appendFrom( _absence );

  // now compute normalization parameters
  allOccs->getMinMax( min, max );
}

/*****************/
/*** normalize ***/
void SamplerImpl::normalize( Normalizer * normalizerPtr )
{
  // Avoid renormalizing the sampler
  if ( _normalized ) {

    return;
  }

  if ( ! _env ) {

    std::string msg = "Cannot normalize sampler without an Environment.\n";

    Log::instance()->error( msg.c_str() );

    throw SamplerException( msg );
  }

  // set env in all occurrences before normalizing env so that
  // occurrences get the unnormalized values
  setEnvironmentInOccurrences();
  _env->normalize( normalizerPtr );

  // need to normalize presences and absences even if _env is present
  // because environment in occurrences was set with unnormalized values
  // if _env doesn't exist, then normalize occurrences anyway
  if ( _presence && _presence->numOccurrences() ) {

    _presence->normalize( normalizerPtr, _env->numCategoricalLayers() );
  }

  if ( _absence && _absence->numOccurrences() ) {

    _absence->normalize( normalizerPtr, _env->numCategoricalLayers() );
  }

  _normalized = true;
}

/***********************/
/*** num Independent ***/
int
SamplerImpl::numIndependent() const
{
  if ( _env ) {
    // get number of dimensions from environment object if it exists
    return _env->numLayers();
  }
  else if ( _presence && _presence->hasEnvironment() ) {
    // otherwise try to get it from presences
    return _presence->dimension();
  }
  else if ( _absence && _absence->hasEnvironment() ) {
    // otherwise try to get it from absences
    return _absence->dimension();
  }

  // neither object has dimensions defined
  return 0;
}


/*********************/
/*** num Dependent ***/
int
SamplerImpl::numDependent() const
{
  return _presence ? _presence->numAttributes() : _absence->numAttributes();
}


/********************/
/*** num Presence ***/
int
SamplerImpl::numPresence() const
{
  return _presence ? _presence->numOccurrences() : 0;
}


/*******************/
/*** num Absence ***/
int
SamplerImpl::numAbsence() const
{
  return _absence ? _absence->numOccurrences() : 0;
}


/**********************/
/*** get One Sample ***/
ConstOccurrencePtr
SamplerImpl::getOneSample( ) const
{
  Random rnd;

  if ( ! _presence ) { 

    std::string msg = "No presence points available for sampling.\n";

    Log::instance()->error( msg.c_str() );

    throw SamplerException( msg );
  }

  if ( ! _presence->numOccurrences() ) { 

    std::string msg = "Cannot use zero presence points for sampling.\n";

    Log::instance()->error( msg.c_str() );

    throw SamplerException( msg );
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

/******************************/
/*** generate Random Sample ***/
ConstOccurrencePtr
SamplerImpl::generateRandomSample(Scalar abundance) const 
{
  if ( ! _env ) {

    std::string msg = "Cannot generate random samples without an Environment object.\n";

    Log::instance()->error( msg.c_str() );

    throw SamplerException( msg );
  }

  // Generate a random sample
  static const Sample mysample( numDependent() );
  Coord x,y;

  Sample env( _env->getRandom( &x, &y ) );

  ConstOccurrencePtr oc = new OccurrenceImpl( "?", x, y, 0.0, abundance, mysample, env );

  return oc;
}

/**************************/
/*** get Pseudo Absence ***/
ConstOccurrencePtr
SamplerImpl::getPseudoAbsence() const 
{
  return generateRandomSample(0.0);
}

/**************************/
/*** get Pseudo Absence ***/
ConstOccurrencePtr 
SamplerImpl::getPseudoAbsence( const Model& model, const Scalar threshold ) const
{
   double prob = 0.0;

   ConstOccurrencePtr occ;

   int max_loop = 5000;

   int loop = 0;

   do {

     occ = getPseudoAbsence();

     if ( model ) {

       prob = model->getValue( occ->environment() );
     }

     loop++;

   } while ( ( prob > threshold ) && ( loop < max_loop ) );

   if ( loop == max_loop ) {

     std::string msg = "Exceeded maximum number of attempts to generate point outside the probability threshold.\n";

     Log::instance()->error( msg.c_str() );

     throw SamplerException( msg );
   }

   return occ;
}

/**************************/
/*** get Pseudo Absence ***/
ConstOccurrencePtr 
SamplerImpl::getPseudoAbsenceOutsideInterval( const Sample * minimum, const Sample * maximum ) const
{
   bool not_found = true;

   ConstOccurrencePtr occ;
   Sample x;

   int max_loop = 5000;

   int loop = 0;

   do {

     occ = getPseudoAbsence();
     x = occ->environment();

     for ( unsigned int i = 0; i < x.size(); i++ ) {

       if ( x[i] == -32768 ) 
         break;

       if ( x[i] < (*minimum)[i] || x[i] > (*maximum)[i] ) {

         not_found = false;
         break;
       }
     }

     loop++;

   } while ( ( not_found ) && ( loop < max_loop ) );

   if ( loop == max_loop ) {

     std::string msg = "Exceeded maximum number of attempts to generate point outside the probability threshold.\n";

     Log::instance()->error( msg.c_str() );

     throw SamplerException( msg );
   }

   return occ;
}

/***************************/
/*** get Pseudo Absences ***/
OccurrencesPtr 
SamplerImpl::getPseudoAbsences( const int& numPoints, const Model& model, const Scalar threshold, const bool geoUnique, const bool envUnique) const 
{
   int i = 0;

   OccurrencesPtr occurrences( new OccurrencesImpl(0.0) );

   do
   {
     ConstOccurrencePtr point;

     if ( model ) {

       point = getPseudoAbsence( model, threshold );
     }
     else {

       point = getPseudoAbsence();
     }

     switch ( geoUnique ) {

       case false:

         if ( envUnique ) {

           if ( isEnvironmentallyUnique( occurrences, point ) && 
                isEnvironmentallyUnique( _presence, point ) && 
                isEnvironmentallyUnique( _absence, point ) ) {

             occurrences->insert( point );
             i++;
           }
         }
         else {

           occurrences->insert( point );
           i++;
         }

         break;

       case true:

         if ( envUnique ) {

           if ( isEnvironmentallyUnique( occurrences, point ) && 
                isEnvironmentallyUnique( _presence, point ) && 
                isEnvironmentallyUnique( _absence, point ) ) {

               occurrences->insert( point );
               i++;
           }
         }
         else {

           if ( isSpatiallyUnique( occurrences, point ) && 
                isSpatiallyUnique( _presence, point ) && 
                isSpatiallyUnique( _absence, point ) ) {

             occurrences->insert( point );
             i++;
           }
         }

         break;
     }

   } while ( i < numPoints );

   return occurrences;
}

/***************************/
/*** get Pseudo Absences ***/
OccurrencesPtr 
SamplerImpl::getPseudoAbsences( const int& numPoints, const Sample * minimum, const Sample * maximum, const bool geoUnique, const bool envUnique) const 
{
   int i = 0;

   OccurrencesPtr occurrences( new OccurrencesImpl(0.0) );

   do
   {
     ConstOccurrencePtr point;

     point = getPseudoAbsenceOutsideInterval( minimum, maximum );

     switch ( geoUnique ) {

       case false:

         if ( envUnique ) {

           if ( isEnvironmentallyUnique( occurrences, point ) && 
                isEnvironmentallyUnique( _presence, point ) && 
                isEnvironmentallyUnique( _absence, point ) ) {

             occurrences->insert( point );
             i++;
           }
         }
         else {

           occurrences->insert( point );
           i++;
         }

         break;

       case true:

         if ( envUnique ) {

           if ( isEnvironmentallyUnique( occurrences, point ) && 
                isEnvironmentallyUnique( _presence, point ) && 
                isEnvironmentallyUnique( _absence, point ) ) {

               occurrences->insert( point );
               i++;
           }
         }
         else {

           if ( isSpatiallyUnique( occurrences, point ) && 
                isSpatiallyUnique( _presence, point ) && 
                isSpatiallyUnique( _absence, point ) ) {

             occurrences->insert( point );
             i++;
           }
         }

         break;
     }

   } while ( i < numPoints );

   return occurrences;
}


/**********************/
/*** is Categorical ***/
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
  Log::instance()->info( "Applying filter: enviromentally unique\n" );

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

        Log::instance()->info( "%s Point \"%s\" at (%f,%f) has no unique environment. It will be discarded.\n", type, ((*next)->id()).c_str(), (*next)->x(), (*next)->y() );

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
  Log::instance()->info( "Applying filter: spatially unique\n" );

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

        Log::instance()->info( "%s Point \"%s\" at (%f,%f) has no unique geography. It will be discarded.\n", type, ((*next)->id()).c_str(), (*next)->x(), (*next)->y() );

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


/*********************************/
/*** is Environmentally Unique ***/
bool 
SamplerImpl::isEnvironmentallyUnique( const OccurrencesPtr& occurrences, const OccurrencePtr& point )const
{
  if ( ! ( occurrences && occurrences->numOccurrences() ) ) {

    return true;
  }

  OccurrencesImpl::iterator it   = occurrences->begin();
  OccurrencesImpl::iterator last = occurrences->end();

  Sample sample = _env->getUnnormalized( point->x(), point->y() );

  while ( it != last ) {

    Sample nextSample = _env->getUnnormalized( (*it)->x(), (*it)->y() );

    if ( sample.equals( nextSample ) ) {

      return false;
    }
    else {

      ++it;
    }
  }

  return true;
}

/***************************/
/*** is Spatially Unique ***/
bool 
SamplerImpl::isSpatiallyUnique( const OccurrencesPtr& occurrences, const OccurrencePtr& point ) const
{
  if ( ! ( occurrences && occurrences->numOccurrences() ) ) {

    return true;
  }

  Map *mask = _env->getMask();

  // If mask is undefined, use first layer as a mask
  if ( ! mask ) {

    mask = _env->getLayer( 0 );
  }

  OccurrencesImpl::iterator it   = occurrences->begin();
  OccurrencesImpl::iterator last = occurrences->end();

  int row, col;
  mask->getRowColumn( point->x(), point->y(), &row, &col );

  while ( it != last ) {

    int next_row, next_col;

    mask->getRowColumn( (*it)->x(), (*it)->y(), &next_row, &next_col );

    if ( row == next_row && col == next_col ) {

      return false;
    }
    else {

      ++it;
    }
  }

  return true;
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


/************/
/*** dump ***/
void
SamplerImpl::dump() const
{
  if ( _presence ) {

    _presence->dump( "Presences" );
  }
  if ( _absence ) {

    _absence->dump( "Absences" );
  }
}


/***********************/
/**** split Sampler ****/
void splitSampler(const SamplerPtr& orig, 
		  SamplerPtr * train,
		  SamplerPtr * test,
		  double propTrain)
{
  // split presences
  OccurrencesPtr presence = orig->getPresences();

  OccurrencesPtr test_presence;

  OccurrencesPtr train_presence;

  if ( presence ) {

    test_presence = new OccurrencesImpl( presence->name(), presence->coordSystem() );

    train_presence = new OccurrencesImpl( presence->name(), presence->coordSystem() );

    splitOccurrences( presence, train_presence, test_presence, propTrain );
  }

  // split absences
  OccurrencesPtr train_absence;
  OccurrencesPtr test_absence;

  OccurrencesPtr absence = orig->getAbsences();

  if ( absence ) { 

    test_absence = new OccurrencesImpl( absence->name(), absence->coordSystem() );

    train_absence = new OccurrencesImpl( absence->name(), absence->coordSystem() );

    splitOccurrences( absence, train_absence, test_absence, propTrain );
  }

  *train = new SamplerImpl( orig->getEnvironment(), 
                            train_presence, train_absence, 
                            orig->isNormalized() );

  *test = new SamplerImpl( orig->getEnvironment(),
                           test_presence, test_absence,
                           orig->isNormalized() );
}
