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

#include <stdio.h>
#include <sampler.hh>

#include <occurrences.hh>
#include <environment.hh>
#include <occurrence.hh>


/****************************************************************/
/**************************** Samples ***************************/

/*******************/
/*** constructor ***/

Samples::Samples( int length, int dimension, Scalar *points )
{
  f_free = 0;

  redim( length, dimension, points );
}


/******************/
/*** destructor ***/

Samples::~Samples()
{
  if ( f_free )
    delete pnt;
}


/*************/
/*** redim ***/
int
Samples::redim( int length, int dimension, Scalar *points )
{
  dim = dimension;
  len = length;

  if ( f_free )
    delete pnt;

  if ( points )
    {
      pnt = points;
      f_free = 0;
    }
  else
    {
      pnt = new Scalar[ len * dim ];
      f_free = 1;
    }

  return 0;
}



/****************************************************************/
/*************************** Sampler ****************************/

/*******************/
/*** constructor ***/

Sampler::Sampler( Occurrences *oc, Environment *env )
{
  f_oc  = oc;
  f_env = env;

  f_oc->head();
}


/******************/
/*** destructor ***/

Sampler::~Sampler()
{
}


/***********/
/*** dim ***/
int
Sampler::dim()
{
  return 1 + f_env->numLayers();
}


/***********************/
/*** num Occurrences ***/
int
Sampler::numOccurrences()
{
  return f_oc->numOccurrences();
}


/***********************/
/*** get Occurrences ***/
int
Sampler::getOccurrences( int numPoints, Samples *data )
{
  if ( numPoints != data->len || dim() != data->dim )
    data->redim( numPoints, dim() );

  return getOccurrences( numPoints, data->pnt );
}


/***********************/
/*** get Occurrences ***/
int
Sampler::getOccurrences( int npnt, Scalar *pnt )
{
  // Number of environmental variables.
  int dim = f_env->numLayers();

  int n;
  Occurrence *ocur = NULL;

  for ( n = 0; n < npnt && (ocur = f_oc->get()); f_oc->next() )
    {
      // Read environmental variables.
      if ( f_env->get( ocur->x, ocur->y, pnt+1 ) )
        {
          // Number of individuals on the occurrence point.
          *pnt++ = ocur->pop;

          pnt += dim;
          n++;
        }
    }

  // If finished by reaching the end of list, go back to the
  // beginning so that the next call can read again the
  // occurrences.
  if ( ! ocur )
    f_oc->head();

  return n;
}


/*******************/
/*** get Samples ***/
int
Sampler::getSamples( int numPoints, Samples *data )
{
  data->redim( numPoints, dim() );

  return getSamples( numPoints, data->pnt );
}


/*******************/
/*** get Samples ***/
int
Sampler::getSamples( int npnt, Scalar *pnt )
{
  int dim = f_env->numLayers();

  // Fill 'pnt' with samples of presence and absence
  // alternatingly.
  int pres = 1;
  for ( int i = 0; i < npnt; pres = 1-pres )
    {
      // Absence point.
      if ( ! pres )
        {
          *pnt++ = 0.0;
          f_env->getRandom( pnt );
          pnt += dim;
          i++;
        }

      // Presence point.
      else if ( getOccurrences( 1, pnt ) )
	{
	  pnt += dim + 1;
	  i++;
	}
    }

  return npnt;
}


/*****************/
/*** var Types ***/
int
Sampler::varTypes( int *types )
{
  // The first variable is always the probability of occurrence, therefore
  // it is never categorical.
  *types++ = 0;

  return 1 + f_env->varTypes( types );
}

