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
  "Probability is inversely proportional to the cartesian\
 distance in environmental space to the nearest presence point.",

  // Description.
  "Normalizes the environmental variables values and the\
 parameter (according to the number of environmental variables).\
 Calculates the distance between the given environmental conditions\
 to each occurrence point and selects the closest distance.\n\
 If distance 'dist' is within [0, MaxDist] then probability will\
 be in [0,1]. If 'dist' > MaxDist then probability will be zero.",


  "Mauro E. S. Munoz",  // Algorithm author.
  "",     	        // Bibliography.

  "Mauro E. S. Munoz",       // Code author.
  "mauro [at] cria.org.br",  // Code author's contact.

  0,  // Does not accept categorical data.
  0,  // Does not need (pseudo)absence points.

  NUM_PARAM,   // Algorithm's parameters.
  parameters
};



/****************************************************************/
/****************** Algorithm's factory function ****************/

dllexp
AlgorithmImpl *
algorithmFactory()
{
  return new MinimumDistance();
}

dllexp
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}

/****************************************************************/
/************************ Minimum Distance **********************/

/*******************/
/*** constructor ***/

MinimumDistance::MinimumDistance() :
  AlgorithmImpl( &metadata ),
  _done( false ),
  _dist(0.0),
  _hasCategorical( false ),
  _numLayers( 0 ),
  _presences()
{
}


/******************/
/*** destructor ***/

MinimumDistance::~MinimumDistance()
{
}


/**************************/
/*** need Normalization ***/
int
MinimumDistance::needNormalization( Scalar *min, Scalar *max ) const
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

  // Distance should range from 0 to 1
  if (_dist > 1.0)       _dist = 1.0;
  else if (_dist < 0.0)  _dist = 0.0;

  int dim = _samp->numIndependent();

  _dist *= sqrt( (double) dim );

  if ( _samp->numPresence() == 0 ) {
    g_log.warn( "MinDistance: No occurrences inside the mask!\n" );
    return 0;
  }

  _presences = _samp->getPresences();

  _numLayers = _samp->numIndependent();
  _isCategorical.resize( _numLayers );

  for( int i = 0; i < _numLayers; ++i ) {
    if ( _samp->isCategorical( i ) ) {
      _hasCategorical = true;
      _isCategorical[i] = 1.0;
    }
  }

  _done = true;

  return 1;

}


/***************/
/*** iterate ***/
int
MinimumDistance::iterate()
{
  return 1;
}


/************/
/*** done ***/
int
MinimumDistance::done() const
{
  return _done;
}


/*****************/
/*** get Value ***/
Scalar
MinimumDistance::getValue( const Sample& x ) const
{
  // Calculate the smallest distance between *x and the occurrence
  // points.
  Scalar min = -1;

  OccurrencesImpl::const_iterator pit = _presences->begin();
  OccurrencesImpl::const_iterator fin = _presences->end();
  while ( pit != fin ) {

    Scalar dist = findDist( x, (*pit)->environment() );

    if ( (dist >= 0) && (dist < min || min < 0) )
      min = dist;

    ++pit;
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
MinimumDistance::findDist( const Sample& x, const Sample& pnt ) const
{
  int layer = 0;

  if ( _hasCategorical ) {
    for( int i=0; i< _numLayers ; ++i ) {
      if ( _isCategorical[i] ) {
	if ( x[i] != pnt[i] ) {
	  return -1.0;
	}
      }
    }
  }
  
  Sample dif = x;
  dif -= pnt;
  
  return dif.norm();

}


/****************************************************************/
/****************** configuration *******************************/
void
MinimumDistance::_getConfiguration( ConfigurationPtr& config ) const
{
  if (!_done )
    return;

  ConfigurationPtr model_config( new ConfigurationImpl("MinimumDistanceModel") );
  config->addSubsection( model_config );

  model_config->addNameValue( "Distance", _dist );
}

void
MinimumDistance::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "MinimumDistanceModel",false );

  if (!model_config)
    return;

  _done = true;
  _dist = model_config->getAttributeAsDouble( "Distance", 0.0 );

}
