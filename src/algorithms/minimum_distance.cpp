/**
 * Definition of Minimum Distance algorithm.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-09-30
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

#include "minimum_distance.hh"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//debug
#include <iostream>


/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 1


/******************************/
/*** Algorithm's parameters ***/

static AlgParamMetadata parameters[NUM_PARAM] = {

  // Metadata of the first parameter.
  {
    "MaxDist",          // Id.
    "Maximum distance", // Name.
    "Real",             // Type.
    "Maximum cartesian distance to be considered", // Overview
    "Maximum cartesian distance to be considered", // Description.

    1,         // Not zero if the parameter has lower limit.
    0.0,       // Parameter's lower limit.
    1,         // Not zero if the parameter has upper limit.
    1.0,       // Parameter's upper limit.
    "0.1"      // Parameter's typical (default) value.
  },
};


/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "MinimumDistance", 	// Id.
  "Minimum distance", 	// Name.
  "0.1",       	        // Version.

  // Overview
  "Probability proporcional to the inverse of the cartesian \
distance to the nearest presence point.",

  // Description.
  "Finds the distance between the given environmental conditions \
to each occurrence point.\n\
 Then choose the closest distance. \n\
 If the distance 'dist' is in [0, MaxDist] then the output will \
be in [0,1]. If 'dist' > MaxDist the output will be Zero.",


  "Mauro E. S. Muñoz",  // Algorithm author.
  "",     	        // Bibliography.

  "Mauro E. S. Muñoz",       // Code author.
  "mauro [at] cria.org.br",  // Code author's contact.

  0,  // Does not accept categorical data.
  0,  // Does not need (pseudo)absence points.

  NUM_PARAM,   // Algorithm's parameters.
  parameters
};



/****************************************************************/
/****************** Algorithm's factory function ****************/

dllexp
Algorithm *
algorithmFactory()
{
  return new MinimumDistance;
}



/****************************************************************/
/************************ Minimum Distance **********************/

/*******************/
/*** constructor ***/

MinimumDistance::MinimumDistance()
  : Algorithm( &metadata )
{
  _done  = 0;
  _presence = 0;
}


/******************/
/*** destructor ***/

MinimumDistance::~MinimumDistance()
{
  if ( _presence )
    delete _presence;
}


/**************************/
/*** need Normalization ***/
int
MinimumDistance::needNormalization( Scalar *min, Scalar *max )
{
  *min = 0.0;
  *max = 1.0;
  return 1;
}


/******************/
/*** initialize ***/
int
MinimumDistance::initialize()
{
  if ( ! getParameter( "MaxDist", &_dist ) )
    return 0;

  int dim = _samp->numIndependent();

  // Distance should range from 0 to 1
  if (_dist > 1.0)       _dist = 1.0;
  else if (_dist < 0.0)  _dist = 0.0;

  // Normalize the distance parameter according to the number
  // of layers
  _dist *= sqrt( dim );

  _presence = new SampledData;


  // Get occurrences from Samples object.
  if ( _samp->getPresence( _presence ) )
    return 1;

  g_log.warn( "MinDistance: No occurrences inside the mask!\n" );
  return 0;
}


/***************/
/*** iterate ***/
int
MinimumDistance::iterate()
{
  return _done = 1;
}


/************/
/*** done ***/
int
MinimumDistance::done()
{
  return _done;
}


/*****************/
/*** get Value ***/
Scalar
MinimumDistance::getValue( Scalar *x )
{
  int dim  = _presence->numIndependent();
  int npnt = _presence->numSamples();

  // Calculate the smallest distance between *x and the occurrence
  // points.
  Scalar **pnt = _presence->getIndependentBase();
  Scalar **end = pnt + npnt;

  Scalar min = -1;
  Scalar dist;
  while ( pnt < end )
    {
      dist = findDist( x, *pnt++, dim );

      if ( (dist >= 0) && (dist < min || min < 0) )
	min = dist;
    }

  // Too far away or categories didn't match any occurrence
  if ( min < 0 || min > _dist )
    return 0.0;

  return 1.0 - (min / _dist);
}


/***********************/
/*** get Convergence ***/
int
MinimumDistance::getConvergence( Scalar *val )
{
  *val = 1.0;
  return 1;
}


/*****************/
/*** find Dist ***/
Scalar
MinimumDistance::findDist( Scalar *x, Scalar *pnt, int dim )
{
  int i = 0;
  int *categ = getCategories();

  Scalar dif;
  Scalar dist = 0.0;
  for ( Scalar *end = pnt + dim;  pnt < end;  x++, pnt++ )
    {
      // If dealing with a categorical variable, continue only if
      // it is equal the input.
      if ( *categ++ )
	{
	  if ( *x != *pnt )
	    return -1.0;
	}

      else
	{
	  dif = *x - *pnt;
	  dist += dif * dif;
	}
    }
  
  return sqrt( dist );
}

