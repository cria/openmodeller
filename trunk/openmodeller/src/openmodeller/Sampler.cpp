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


/***************/
/*** dim Env ***/
int
Sampler::dimEnv()
{
  return _env->numLayers();
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
Sampler::getPresence( SampledData *env, int npnt )
{
  return getOccurrence( _presence, env, npnt );
}


/*******************/
/*** get Absence ***/
int
Sampler::getAbsence( SampledData *env, int npnt )
{
  return _absence ? getOccurrence( _absence, env, npnt ) : 0;
}


/**************************/
/*** get Pseudo Absence ***/
int
Sampler::getPseudoAbsence( SampledData *env, int npnt )
{
  if ( npnt <= 0 )
    return 0;

  int dim = dimEnv();

  env->redim( npnt, dim );
  Scalar *pnt = env->pnt;

  for ( Scalar *end = pnt + npnt * dim; pnt < end; pnt += dim )
    _env->getRandom( pnt );

  return dim;
}


/*******************/
/*** get Samples ***/
int
Sampler::getSamples( SampledData *presence, SampledData *absence,
		     int npnt )
{
  if ( ! npnt )
    return 0;

  getPresence( presence, npnt - npnt/2 );
  getAbsence( absence, npnt - presence->npnt );

  // If it do not read all the needed points, try to get the
  // missing points through pseudo-absence method.
  int missing = npnt - presence->npnt - absence->npnt;
  if ( missing )
    {
      SampledData pseudo;
      getPseudoAbsence( &pseudo, missing );

      absence->add( pseudo );
    }

  return presence->npnt + absence->npnt;
}


/*****************/
/*** var Types ***/
int
Sampler::varTypes( int *types )
{
  return _env->varTypes( types );
}


/**********************/
/*** get Occurrence ***/
int
Sampler::getOccurrence( Occurrences *occur, SampledData *env,
			int npnt )
{
  if ( ! npnt )
    return 0;

  // Get all occurrences.
  if ( npnt < 0 )
    npnt = occur->numOccurrences();

  // Number of environmental variables.
  int dim = dimEnv();

  env->redim( npnt, dim );
  Scalar *pnt = env->pnt;

  Occurrence *oc = 0;
  int n = 0;
  for ( ; n < npnt && (oc = occur->get()); occur->next() )
    {
      // Read environmental variables.
      if ( _env->get( oc->x, oc->y, pnt ) )
        {
          pnt += dim;
          n++;
        }
    }

  // If finished by reaching the end of list, go back to the
  // beginning so that the next call can read again the
  // occurrences.
  if ( ! oc )
    occur->head();

  return n;
}

