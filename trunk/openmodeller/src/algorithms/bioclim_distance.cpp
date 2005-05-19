/**
 * Declaration of Bioclimatic Envelope Algorithm - Nix, 1986.
 * 
 * @file
 * @author Mauro Muñoz <mauro@cria.org.br>
 * @date 2004-05-05
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2004 by CRIA -
 * Centro de Referência em Informação Ambiental
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

#include "bioclim_distance.hh"

#include <configuration.hh>
#include <Exceptions.hh>

#include <string.h>
#include <stdio.h>
#include <math.h>

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 1

#define CUTOFF_ID "StandardDeviationCutoff"


/*************************************/
/*** Algorithm parameters metadata ***/

static AlgParamMetadata parameters[NUM_PARAM] = {

  // Metadata of the first parameter.
  {
    CUTOFF_ID,                 // Id
    "Standard deviation cutoff", // Name.
    "Real",                      // Type.

    // Overview
    "The envelop is determined by multiplying this parameter and the\
 standard deviation.",

    "Standard deviation cutoff for all bioclimatic envelop.\
 Examples of (fraction of inclusion, parameter value) are:\
 (50.0%, 0.674); (68.3%, 1.000); (90.0%, 1.645); (95.0%, 1.960);\
 (99.7%, 3.000)",

    1,         // Not zero if the parameter has lower limit.
    0.0,       // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    0.0,       // Parameter's upper limit.
    "0.674"    // Parameter's typical (default) value.
  },
};


/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "BioclimDistance",     // Id.
  "Bioclim distance",  // Name.
  "0.2",                 // Version.

  // Overview
  "Uses mean and standard deviation for each environmental\
 variable separately to calculate bioclimatic envelops.\
 Probability of presence is inversely proportional to the euclidean\
 distance to the mean point, and is limited to Suitable regions.",

  // Description.
  "Implements a variant of the Bioclimatic Envelop Algorithm.\
 For each given environmental variable the algorithm finds the mean\
 and standard deviation (assuming normal distribution) associated\
 to the occurrence points. Each variable has its own envelop\
 represented by the interval [m - c*s, m + c*s], where 'm' is the\
 mean; 'c' is the cutoff input parameter; and 's' is the standard\
 deviation. The original Bioclim specification defines three regions:\
 Suitable, Marginal and Unsuitable. The Suitable region relates to the\
 envelop mentioned before, and is the only region considered in this\
 implementation (i.e. points falling inside Marginal or Unsuitable\
 regions have probability 0 here). Probability of presence for points\
 inside the Suitable region is inversely proportional to the normalized\
 euclidean distance between a point and the mean point in environmental\
 space.",

  "Nix, H. A. Modified by Mauro Munoz",  // Author.
  "",                                    // Bibliography.

  "Mauro Munoz",              // Code author.
  "mauro [at] cria.org.br",   // Author's contact.

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
  return new BioclimDistance();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}


/****************************************************************/
/*********************** Bioclim Distance ***********************/

/*******************/
/*** constructor ***/

BioclimDistance::BioclimDistance() :
  AlgorithmImpl( &metadata ),
  _done( false ),
  _max_distance(0.0),
  _minimum(),
  _maximum(),
  _mean(),
  _std_dev()
{ }


/******************/
/*** destructor ***/

BioclimDistance::~BioclimDistance()
{
}


/**************************/
/*** need Normalization ***/
int
BioclimDistance::needNormalization( Scalar *min, Scalar *max ) const
{
  *min = 0.0;
  *max = 1.0;
  return 1;
}


/******************/
/*** initialize ***/
int
BioclimDistance::initialize()
{
  Scalar cutoff = 0.0;
  // Read and check the standard deviation cutoff parameter.
  if ( ! getParameter( CUTOFF_ID, &cutoff ) ) {
    g_log.error(1, "Parameter " CUTOFF_ID " not set properly.\n");
    return 0;
  }

  if ( cutoff <= 0 ) {
    g_log.warn( "BioclimDistance - parameter out of range: %f\n", cutoff );
    return 0;
  }

  // Number of independent variables.
  int dim = _samp->numIndependent();
  g_log.info( "Reading %d-dimensional occurrence points.\n", dim );

  // Check the number of presence points.
  int npnt = _samp->numPresence();
  if ( npnt < 2 ) {
    g_log.error( 1, "BioclimDistance needs at least 2 points inside the mask!\n" );
  }

  g_log.info( "Using %d points to find the bioclimatic envelop.\n", npnt );

  computeStats( _samp->getPresences() );

  // Stores the real standard deviation cutoff value and
  // calculates the standard deviation vector module to be used
  // as the maximum possible distance of a accepted point to the
  // points' mean.
  _std_dev *= cutoff;
  
  _max_distance = _std_dev.norm();

  _done = true;

  return 1;
}


/***************/
/*** iterate ***/
int
BioclimDistance::iterate()
{
  // This is not an iterative algorithm.
  return 1;
}


/************/
/*** done ***/
int
BioclimDistance::done() const
{
  // This is not an iterative algorithm.
  return _done;
}


/*****************/
/*** get Value ***/
Scalar
BioclimDistance::getValue( const Sample& x ) const
{
  // Finds the distance from each variable mean to the respective
  // point value.
  Sample dif = x;
  dif -= _mean;

  // Square of the distance between 'x' and '_mean'.
  Scalar square_distance = 0.0;

  for( int i=0; i<x.size(); i++) {

    if ( x[i] < _minimum[i] || x[i] > _maximum[i] ) {
      return 0.0;
    }

    Scalar cutoff = _std_dev[i];

    Scalar diffi = dif[i];
    
    // If some x[i] is out of its bioclimatic envelop, predicts
    // no occurrence.
    if ( dif[i] > cutoff || dif[i] < -cutoff ) {
      return 0.0;
    }

    square_distance += dif[i] * dif[i];

  }

  return 1.0 - (sqrt( square_distance ) / _max_distance);
}


/***********************/
/*** get Convergence ***/
int
BioclimDistance::getConvergence( Scalar *val )
{
  *val = 1.0;
  return 1;
}

/*******************/
/*** get Minimum ***/
void
BioclimDistance::computeStats( const OccurrencesPtr& occs )
{

  // Compute min, max, and mean
  {
    OccurrencesImpl::const_iterator oc = occs->begin();
    OccurrencesImpl::const_iterator end = occs->end();

    // Intialize _minimum, _maximum, and _mean
    // to the values of the first point, and increment
    // to get it out of the loop.
    Sample const & sample = (*oc)->environment();
    _minimum = sample;
    _maximum = sample;
    _mean = sample;
    
    ++oc;
    
    // For each Occurrence, update the
    // statistics for _minimum, _maximum, and _mean
    
    while ( oc != end ) {
      Sample const& sample = (*oc)->environment();
      
      _mean += sample;
      _minimum &= sample;
      _maximum |= sample;

      ++oc;
    }

    // Divide for the mean.
    _mean /= Scalar( occs->numOccurrences() );

  }

  // Now compute the std deviation by first computing the variance.
  {

    _std_dev.resize( _mean.size() );
    OccurrencesImpl::const_iterator oc = occs->begin();
    OccurrencesImpl::const_iterator end = occs->end();

    // Now we compute the variance.
    while ( oc != end ) {
      Sample tmp( (*oc)->environment() );
      tmp -= _mean;
      tmp *= tmp;
      _std_dev += tmp;
      ++oc;
    }

    // In variance, we divide by (npnt - 1), not npnt!
    Scalar npts = Scalar( occs->numOccurrences() - 1 );

    // Now divide and root to get deviation.
    _std_dev /= npts;
    _std_dev.sqrt();

  }

}

/******************/
/*** configuration ***/
void
BioclimDistance::_getConfiguration( ConfigurationPtr& config ) const
{
  if ( !_done ) 
    return;

  ConfigurationPtr model_config( new ConfigurationImpl( "BioclimDistanceModel") );
  config->addSubsection( model_config );
  
  model_config->addNameValue( "MaxDistance", _max_distance );
  model_config->addNameValue( "Mean", _mean );
  model_config->addNameValue( "StdDev", _std_dev );
  model_config->addNameValue( "Minimum", _minimum );
  model_config->addNameValue( "Maximum", _maximum );
}

void
BioclimDistance::_setConfiguration( const ConstConfigurationPtr & config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "BioclimDistanceModel", false );

  if (!model_config)
    return;

  _done = true;

  _max_distance = model_config->getAttributeAsDouble( "MaxDistance", 0.0 );
  _mean = model_config->getAttributeAsSample( "Mean" );
  _std_dev = model_config->getAttributeAsSample( "StdDev" );
  _minimum = model_config->getAttributeAsSample( "Minimum" );
  _maximum = model_config->getAttributeAsSample( "Maximum" );

}
