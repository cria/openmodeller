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

#define MAXDIST_ID "MaximumDistance"


/******************************/
/*** Algorithm's parameters ***/

static AlgParamMetadata parameters[NUM_PARAM] = {

  // Metadata of the first parameter.
  {
    MAXDIST_ID,         // Id.
    "Maximum distance", // Name.
    "Real",             // Type.
    "Maximum cartesian distance to closest point", // Overview
    "Maximum cartesian distance to closest point", // Description.

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
  "0.2",       	        // Version.

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

OM_ALG_DLL_EXPORT
AlgorithmImpl *
algorithmFactory()
{
  return new MinimumDistance();
}

OM_ALG_DLL_EXPORT
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
  _numLayers( 0 )
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
  if ( ! getParameter( MAXDIST_ID, &_dist ) ) {
    Log::instance()->error(1, "Parameter '" MAXDIST_ID "' not set properly.\n");
    return 0;
  }

  // Distance should range from 0 to 1
  if (_dist > 1.0)       _dist = 1.0;
  else if (_dist < 0.0)  _dist = 0.0;

  int dim = _samp->numIndependent();

  _dist *= sqrt( (double) dim );

  if ( _samp->numPresence() == 0 ) {
    Log::instance()->warn( "MinDistance: No occurrences inside the mask!\n" );
    return 0;
  }

  OccurrencesPtr presences = _samp->getPresences();

  // Load vector with samples containing the environmental
  // values at each presence point
  OccurrencesImpl::const_iterator p_iterator = presences->begin();
  OccurrencesImpl::const_iterator p_end = presences->end();

  while ( p_iterator != p_end ) {

    Sample point = (*p_iterator)->environment();
 
    _envPoints.push_back(point);

    ++p_iterator;
  }

  // Identify categorical layers
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

  for( unsigned int i=0; i<_envPoints.size(); i++) {

    Scalar dist = findDist( x, _envPoints[i] );

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
MinimumDistance::findDist( const Sample& x, const Sample& pnt ) const
{

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

  ConfigurationPtr model_config( new ConfigurationImpl("MinimumDistance") );
  config->addSubsection( model_config );

  model_config->addNameValue( "IsCategoricalLayer", _isCategorical );
  model_config->addNameValue( "Distance", _dist );

  ConfigurationPtr envpoints_config( new ConfigurationImpl("EnvironmentalReferences") );
  model_config->addSubsection( envpoints_config );

  for( unsigned int i=0; i<_envPoints.size(); i++) {

    ConfigurationPtr point_config( new ConfigurationImpl("Reference") );
    envpoints_config->addSubsection( point_config );

    point_config->addNameValue( "Value", _envPoints[i] );
  }
}

void
MinimumDistance::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "MinimumDistance",false );

  if (!model_config)
    return;

  // Information about categorical layers
  _isCategorical = model_config->getAttributeAsSample( "IsCategoricalLayer" );
  _numLayers = (int)_isCategorical.size();

  for( int i=0; i<_numLayers; i++) {

    if ( _isCategorical[i] ) {

      _hasCategorical = true;
      break;
    }
  }

  // Maximum distance
  _dist = model_config->getAttributeAsDouble( "Distance", 0.0 );

  // Environmental points
  ConstConfigurationPtr envpoints_config = model_config->getSubsection( "EnvironmentalReferences",false );

  Configuration::subsection_list subs = envpoints_config->getAllSubsections();

  Configuration::subsection_list::iterator begin = subs.begin();
  Configuration::subsection_list::iterator end = subs.end();
  for ( ; begin != end; ++begin ) {

    if ( (*begin)->getName() != "Reference" ) 
      continue;

    Sample point = (*begin)->getAttributeAsSample( "Value" );

    _envPoints.push_back( point );
  }

  _done = true;
}
