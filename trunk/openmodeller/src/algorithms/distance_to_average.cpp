/**
 * Definition of cartesian DistanceToAverageModel algorithm.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-09-12
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

#include "distance_to_average.hh"

#include <string.h>
#include <stdio.h>
#include <math.h>


/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM     1
#define PARAM_MAXDIST "MaxDist"


/*************************************/
/*** Algorithm parameters metadata ***/

static AlgParamMetadata parameters[NUM_PARAM] = {

  // Metadata of the first parameter.
  {
    PARAM_MAXDIST,   // Name.
    "Real",          // Type.
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

  0,                    // Internal usage.
  "DistanceToAverage", 	// Name.
  "0.1",       	        // Version.
  "none",     	        // Bibliography.

  // Description.
  "Find the distance between the given environmental conditions \
and the point represented by the average of all occurrences.\n \
 If the distance 'dist' is in [0, MaxDist] then the output will \
be in [0,1]. If 'dist' > MaxDist the output will be Zero.",

  "Mauro E. S. Muñoz",       // Author.
  "mauro [at] cria.org.br",  // Author's contact.

  0,  // Does not accept categorical data.
  0,  // Does not need (pseudo)absence points.

  NUM_PARAM,   // Algorithm's parameters.
  parameters
};



/****************************************************************/
/****************** Algorithm's factory function ****************/

Algorithm *
algorithmFactory()
{
  return new DistanceToAverage;
}



/****************************************************************/
/************************ Distance To Average *******************/

/*******************/
/*** constructor ***/

DistanceToAverage::DistanceToAverage()
  : Algorithm( &metadata )
{
  _min = _max = _dist = 0.0;

  _done = 0;
  _avg  = 0;
}


/******************/
/*** destructor ***/

DistanceToAverage::~DistanceToAverage()
{
  if ( _avg )
    {
      g_log( "\nMinimum distance found: %f", _min );
      g_log( "\nMaximum distance found: %f\n\n", _max );

      delete _avg;
    }
}


/**************************/
/*** need Normalization ***/
int
DistanceToAverage::needNormalization( Scalar *min, Scalar *max )
{
  *min = 0.0;
  *max = 1.0;
  return 1;
}


/******************/
/*** initialize ***/
int
DistanceToAverage::initialize( int ncicle )
{
  if ( ! getParameter( PARAM_MAXDIST, &_dist ) )
    return 0;

  _dim = _samp->numIndependent();

  g_log( "Parameter %s: %f\n", PARAM_MAXDIST, _dist );

  // Distance should range from 0 to 1
  if ( _dist > 1.0 )  _dist = 1.0;
  else if ( _dist < 0.0 ) _dist = 0.0;

  // Normalize the distance parameter according to the number
  // of layers.
  _dist *= sqrt( _dim );

  g_log( "\nEnvironmental layers: %d\n", _dim );
  g_log( "Parameter normalized: %f\n\n", _dist );

  _avg = new Scalar[_dim];


  //
  // Generate model from the average of given points.
  //

  g_log( "Reading %d-dimensional occurrence points.\n", _dim );

  // Read all presence occurence points.
  SampledData presence;
  int npnt = _samp->getPresence( &presence );
  if ( ! npnt )
    {
      g_log( "All occurrences are outside the mask!\n" );
      return 0;
    }
  Scalar **points = presence.getIndependentBase();

  g_log( "Finding average from %d occurrences.\n", npnt );

  // Sum of the environmental values for all occurrence points.
  memset( _avg, 0, _dim * sizeof(Scalar) );
  for ( int i = 0; i < npnt; i++ )
    {
      Scalar *sample_i = *points++;
      for ( int d = 0; d < _dim; d++ )
	_avg[d] += *sample_i++;
    }

  // Average value.
  for ( int d = 0; d < _dim; d++ )
    _avg[d] /= npnt;

  g_log( "Average related to occurrences: " );
  for ( int d = 0; d < _dim; d++ )
    g_log( "%f ", _avg[d] );
  g_log( "\n\n" );

  return 1;
}


/***************/
/*** iterate ***/
int
DistanceToAverage::iterate()
{
  return _done = 1;
}


/************/
/*** done ***/
int
DistanceToAverage::done()
{
  return _done;
}


/*****************/
/*** get Value ***/
Scalar
DistanceToAverage::getValue( Scalar *x )
{
  static int first_time = 1;

  // First position is the number of occurrences.
  Scalar *avg = _avg;
  Scalar *end = _avg + _dim;

  // Calculate distance between *x and _avg.
  Scalar dif;
  Scalar dist = 0.0;
  while ( avg < end )
    {
      dif = *x++ - *avg++;
      dist += dif * dif;
    }
  dist = sqrt( dist );

  // Minimum and maximum distances found. Only for log!
  if ( first_time )
    {
      _min = _max = dist;
      first_time = 0;
    }
  else
    {
      if ( dist < _min ) _min = dist;
      if ( dist > _max ) _max = dist;
    }

  return (dist <= _dist) ? 1.0 - (dist / _dist) : 0.0;
}


/***********************/
/*** get Convergence ***/
int
DistanceToAverage::getConvergence( Scalar *val )
{
  *val = 1.0;
  return 1;
}

