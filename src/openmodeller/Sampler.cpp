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

#include <om_sampler.hh>
#include <om_sampled_data.hh>
#include <om_occurrences.hh>
#include <random.hh>

#include <string.h>

#include <stdio.h>


/****************************************************************/
/*************************** Sampler ****************************/

/*******************/
/*** constructor ***/

Sampler::Sampler( Environment *env, Occurrences *presence,
		  Occurrences *absence )
{
  _env = env;

  _presence = presence;
  _presence->head();

  if ( _absence = absence )
    _absence->head();
}


/******************/
/*** destructor ***/

Sampler::~Sampler()
{
}


/***********************/
/*** num Independent ***/
int
Sampler::numIndependent()
{
  return _env->numLayers();
}


/*********************/
/*** num Dependent ***/
int
Sampler::numDependent()
{
  return _presence->numAttributes();
}


/********************/
/*** num Presence ***/
int
Sampler::numPresence()
{
  return _presence->numOccurrences();
}


/*******************/
/*** num Absence ***/
int
Sampler::numAbsence()
{
  return _absence ? _absence->numOccurrences() : 0;
}


/********************/
/*** get Presence ***/
int
Sampler::getPresence( SampledData *data, int npnt )
{
  return getOccurrence( _presence, data, npnt );
}


/*******************/
/*** get Absence ***/
int
Sampler::getAbsence( SampledData *data, int npnt )
{
  return _absence ? getOccurrence( _absence, data, npnt ) : 0;
}


/**************************/
/*** get Pseudo Absence ***/
int
Sampler::getPseudoAbsence( SampledData *data, int npnt )
{
  if ( npnt <= 0 )
    return 0;

  int num_indep = numIndependent();
  int num_dep   = numDependent();
  data->redim( npnt, num_indep, num_dep );

  Scalar **indep = data->getIndependentBase();
  Scalar **dep   = data->getDependentBase();
  Scalar **end   = dep + npnt;

  // Size (in bytes) of the dependent variable vectors.
  int dep_size = num_dep * sizeof(Scalar);

  while ( dep < end )
    {
      // Independent (environment) variables.
      _env->getRandom( *indep++ );

      // Dependent variables. For absence they are always null.
      memset( *dep++, 0, dep_size );
    }

  return npnt;
}


/*******************/
/*** get Samples ***/
int
Sampler::getSamples( SampledData *data, int npnt )
{
  data->redim( npnt, numIndependent(), numDependent() );

  Scalar **indep = data->getIndependentBase();
  Scalar **dep   = data->getDependentBase();
  Scalar **end   = dep + npnt;

  while ( dep < end )
    getOneSample( *indep++, *dep++ );

  return npnt;
}


/**********************/
/*** get One Sample ***/
int
Sampler::getOneSample( Scalar *indep, Scalar *dep )
{
  Random rnd;

  // Probability of 0.5 of get a presence point.
  if ( rnd() < 0.5 )
    {
      getRandomOccurrence( _presence, indep, dep );
      return 1;
    }

  //
  // Probability of 0.5 of get an absence point.
  //

  // If there are real absence points...
  if ( _absence )
    {
      getRandomOccurrence( _absence, indep, dep );

      // FIXME: abundance, which is the first dependent variable
      // is hardcoded to be always 1 in Occurrences::insert()
      // should fix that so absences can have abundance = 0
      // this is a temporary fix, until absences are properly
      // handled.
      memset( dep, 0, numDependent() * sizeof(Scalar) );
      return 0;
    }

  //
  // Get a random pseudo-absence point.
  //

  // Independent (environment) variables.
  _env->getRandom( indep );

  // Dependent variables. For absence they are always null.
  memset( dep, 0, numDependent() * sizeof(Scalar) );

  // Got an absence point.
  return 0;
}


/*****************/
/*** var Types ***/
int
Sampler::varTypes( int *types )
{
  return _env->varTypes( types );
}


/*****************************/
/*** get Random Occurrence ***/
int
Sampler::getRandomOccurrence( Occurrences *occur,
			      Scalar *indep, Scalar *dep )
{
  Occurrence *oc;

  // Choose an occurrence point with defined environmental
  // variable values.
  while ( (oc = occur->getRandom()) &&
	  ! _env->get( oc->x(), oc->y(), indep ) )
  { 
      printf("Point is no good: %8.3f, %8.3f\r", oc->x(), oc->y()); 
  };

  oc->readAttributes( dep );
  
  return 1;
}


/**********************/
/*** get Occurrence ***/
int
Sampler::getOccurrence( Occurrences *occur, SampledData *data,
			int npnt )
{
 if ( ! npnt )
    return 0;

  // Get all occurrences.
  if ( npnt < 0 )
    npnt = occur->numOccurrences();

  // Redimension of 'data' to stores the presence samples.
  int num_indep = numIndependent();
  int num_dep   = occur->numAttributes();
  data->redim( npnt, num_indep, num_dep );

  int dep_size = num_dep * sizeof(Scalar);

  Scalar **indep = data->getIndependentBase();
  Scalar **dep   = data->getDependentBase();

  Occurrence *oc = 0;
  int n = 0;
  for ( ; n < npnt && (oc = occur->get()); occur->next() )
    {
      // Read environmental variables (independent variables).
      if ( _env->get( oc->x(), oc->y(), *indep ) )
        {
	  // Read occurrence attributes (dependent variables).
	  oc->readAttributes( *dep++ );

	  // Prepare to read next sample.
	  indep++;
          n++;
        }
    }

  // If finished by reaching the end of list, go back to the
  // beginning so that the next call can read again the
  // occurrences.
  if ( ! oc )
    occur->head();

  // Number of occurrences read.
  return n;
}

/********************/
/**** split *********/
void Sampler::split(Sampler ** train, Sampler ** test, double propTrain)
{
  Occurrences * train_presence, * train_absence, 
    * test_presence, * test_absence;

  // copy presence and absence occurrences
  test_presence  = new Occurrences(_presence->name(), _presence->coordSystem());
  train_presence = copyOccurrences(_presence);
  moveRandomOccurrences(train_presence, test_presence, propTrain);
  
  if (_absence)
    { 
      test_absence  = new Occurrences(_absence->name(), _absence->coordSystem());
      train_absence = copyOccurrences(_absence);
      moveRandomOccurrences(train_presence, test_presence, propTrain);
    }
  else
    { train_absence = test_absence = NULL; }

  *train = new Sampler(_env, train_presence, train_absence);
  *test  = new Sampler(_env, test_presence,  test_absence);
}

/******************************/
/**** copyOccurrences *********/
Occurrences * Sampler::copyOccurrences(Occurrences * occs)
{
  Occurrence * oc;
  Occurrences * newOccs = new Occurrences(occs->name(), occs->coordSystem());

  for (occs->head(); oc = occs->get(); occs->next())
    { newOccs->insert(oc->x(), oc->y(), oc->error(), 1.0, 0, 0); }

  return newOccs;
}

/************************************/
/**** moveRandomOccurrences *********/
void Sampler::moveRandomOccurrences(Occurrences * train, 
				    Occurrences * test, double propTrain)
{
  Occurrence * oc;

  int i = 0;
  int n = (int) ((1.0 - propTrain) * ((double) train->numOccurrences()));

  Random rnd;

  train->head();
  while (i < n)
    {
      oc = train->get();
      if (!oc)
	{ 
	  train->head(); 
	  oc = train->get();
	}

      // flip a coin
      if (rnd() > propTrain)
	{ 
	  i++;
	  oc = train->remove(); 
	  test->insert(oc->x(), oc->y(), oc->error(), 1.0, 0, 0);
	  delete oc;
	}
      else
	{ train->next(); }
    }
}
