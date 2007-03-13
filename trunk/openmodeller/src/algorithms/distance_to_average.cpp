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

#include <openmodeller/Configuration.hh>
#include <openmodeller/Exceptions.hh>

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
  "Probability is inversely proportional to the cartesian distance\
 in environmental space to the mean point (calculated with all\
 presence points).",

  // Description.
  "Normalizes the environmental variables values and the\
 parameter (according to the number of environmental variables).\
 Calculates the mean point in environmental space\
 considering all given presence points. When projecting the\
 result, it calculates the Euclidean distance between the\
 average point and each point in the environmental space.\
 If the distance 'dist' is in [0, MAXDIST] then the probability\
 of occurrence will be in [1,0] (linear decay).\
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

OM_ALG_DLL_EXPORT
AlgorithmImpl *
algorithmFactory()
{
  return new DistanceToAverage();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}

/****************************************************************/
/************************ Distance To Average *******************/

/*******************/
/*** constructor ***/

DistanceToAverage::DistanceToAverage() :
  AlgorithmImpl( &metadata ),
  _done( false ),
  _dist(0.0),
  _min(0.0),
  _max(0.0),
  _avg()
{
}


/******************/
/*** destructor ***/

DistanceToAverage::~DistanceToAverage()
{
  Log::instance()->info( "\nMinimum distance found: %f", _min );
  Log::instance()->info( "\nMaximum distance found: %f\n\n", _max );
}


/**************************/
/*** need Normalization ***/
int
DistanceToAverage::needNormalization( Scalar *min, Scalar *max ) const
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

  Log::instance()->info( "Parameter %s: %f\n", PARAM_MAXDIST, _dist );

  // Distance should range from 0 to 1
  if ( _dist > 1.0 )  _dist = 1.0;
  else if ( _dist < 0.0 ) _dist = 0.0;

  int dim = _samp->numIndependent();

  // Normalize the distance parameter according to the number
  // of layers.
  _dist *= sqrt( (double) dim );

  Log::instance()->info( "\nEnvironmental layers: %d\n", dim );
  Log::instance()->info( "Parameter normalized: %f\n\n", _dist );

  //
  // Generate model from the average of given points.
  //

  Log::instance()->info( "Reading %d-dimensional occurrence points.\n", dim );

  int npnt = _samp->numPresence();
  if ( npnt == 0 ) {
    Log::instance()->info( "All occurrences are outside the mask!\n" );
    return 0;
  }

  // Read all presence occurence points.
  OccurrencesPtr presences = _samp->getPresences();
  OccurrencesImpl::const_iterator pres = presences->begin();
  OccurrencesImpl::const_iterator fin = presences->end();

  Log::instance()->info( "Finding average from %d occurrences.\n", npnt );

  // Redimension _avg.
  _avg.resize( dim );

  while ( pres != fin ) {
    _avg += (*pres)->environment();
    ++pres;
  }

  _avg /= npnt;

  Log::instance()->info( "Average related to occurrences: " );
  for ( int d = 0; d < dim; d++ )
    Log::instance()->info( "%f ", _avg[d] );
  Log::instance()->info( "\n\n" );

  _done = true;

  return 1;
}


/***************/
/*** iterate ***/
int
DistanceToAverage::iterate()
{
  return 1;
}


/************/
/*** done ***/
int
DistanceToAverage::done() const
{
  return _done;
}


/*****************/
/*** get Value ***/
Scalar
DistanceToAverage::getValue( const Sample& x ) const
{
  static int first_time = 1;

  // Calculate distance between *x and _avg.
  
  Sample dif = x;
  dif -= _avg;
  Scalar dist = dif.norm();

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

/****************************************************************/
/****************** configuration *******************************/
void
DistanceToAverage::_getConfiguration( ConfigurationPtr& config ) const
{
  if (!_done )
    return;

  ConfigurationPtr model_config( new ConfigurationImpl("DistanceToAverage") );
  config->addSubsection( model_config );

  model_config->addNameValue( "Distance", _dist );
  model_config->addNameValue( "Average", _avg );
}

void
DistanceToAverage::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "DistanceToAverage",false );

  if (!model_config)
    return;

  _done = true;
  _dist = model_config->getAttributeAsDouble( "Distance", 0.0 );
  _avg = model_config->getAttributeAsSample( "Average" );

}
