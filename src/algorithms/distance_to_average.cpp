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
    PARAM_MAXDIST,      // Id
    "Maximum distance", // Name
    "Real",             // Type.
    "Maximum cartesian distance to be considered", // Overview
    "Maximum cartesian distance to be considered", // Description

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

  "DistanceToAverage",    // Id.
  "Distance to average",  // Name.
  "0.1",       	          // Version.

  // Overview
  "Probability proporcional to inverse of cartesian distance \
to the mean of all presence points.",

  // Description.
  "Normalizes the environmental variables values and the \
parameter (according to the number of environmental variables). \
Calculates the average point in the environmental space \
considering all given presence points. When projecting the \
result, it calculates the Euclidean distance between the \
average point and each point in the environmental space. \
If the distance 'dist' is in [0, MAXDIST] then the probability \
of occurrence will be in [1,0] (linear decay). \
If 'dist' > MAXDIST then the probability will be zero.",

  "Mauro E. S. Munoz",  // Algorithm author
  "",     	        // Bibliography.

  "Mauro E. S. Munoz",       // Code author.
  "mauro [at] cria.org.br",  // Code author contact.

  0,  // Does not accept categorical data.
  0,  // Does not needs absence points to run.

  NUM_PARAM,   // Algorithm's parameters.
  parameters
};



/****************************************************************/
/****************** Algorithm's factory function ****************/

#ifdef WIN32
__declspec( dllexport )
#endif
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
DistanceToAverage::initialize()
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
  int d;
  for ( d = 0; d < _dim; d++ )
    _avg[d] /= npnt;

  g_log( "Average related to occurrences: " );
  for ( d = 0; d < _dim; d++ )
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


/*****************/
/*** serialize ***/
int
DistanceToAverage::serialize(Serializer * s)
{
  s->writeStartSection("DistanceToAverageModel");
  s->writeInt("Dimension", _dim);
  s->writeDouble("Distance", _dist);
  s->writeArrayDouble("Average", _avg, _dim);
  s->writeEndSection("DistanceToAverageModel");
  return 1;
}


/*******************/
/*** deserialize ***/
int
DistanceToAverage::deserialize(Deserializer * s)
{
  int size;

  s->readStartSection("DistanceToAverageModel");
  _dim  = s->readInt("Dimension");
  _dist = s->readDouble("Distance");
  _avg  = s->readArrayDouble("Average", &size);
  s->readEndSection("DistanceToAverageModel");

  return (_dim == size) ? _done = 1 : 0;
}
