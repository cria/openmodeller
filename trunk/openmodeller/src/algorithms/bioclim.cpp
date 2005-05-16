/**
 * Declaration of Bioclimatic Envelope Algorithm - Nix, 1986.
 * 
 * @file
 * @author Mauro Mu�oz <mauro@cria.org.br>
 * @date 2004-05-05
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2004 by CRIA -
 * Centro de Refer�ncia em Informa��o Ambiental
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

#include "bioclim.hh"

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

    // Description.
    "Standard deviation cutoff for all bioclimatic envelops.\n\
 Examples of (fraction of inclusion, parameter value) are:\n\
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

  "Bioclim",   // Id.
  "Bioclim",   // Name.
  "0.2",       // Version.

  // Overview
  "Uses mean and standard deviation for each environmental\
 variable separately to calculate bioclimatic envelops.\
 Level of fitness between the environmental values on a point\
 and the respective envelops classifies points as\
 Suitable, Marginal, or Unsuitable for presence.",

  // Description.
  "Implements the Bioclimatic Envelop Algorithm.\
 For each given environmental variable the algorithm finds the mean\
 and standard deviation (assuming normal distribution) associated\
 to the occurrence points. Each variable has its own envelop\
 represented by the interval [m - c*s, m + c*s], where 'm' is the\
 mean; 'c' is the cutoff input parameter; and 's' is the standard\
 deviation. Besides the envelop, each environmental variable has\
 additional upper and lower limits taken from the maximum and\
 minimum values related to the set of occurrence points.\nIn this\
 model, any point can be classified as:\n\
 Suitable: if all associated environmental values fall within\
 the calculated envelops;\n\
 Marginal: if one or more associated environmental value falls\
 outside the calculated envelop, but still within the upper and\
 lower limits.\n\
 Unsuitable: if one or more associated enviromental value falls\
 outside the upper and lower limits.\n\
Bioclim's categorical output is mapped to probabilities\
 of 1.0, 0.5 and 0.0 respectively.",

  "Nix, H. A.",  // Author

  // Bibliography.
  "Nix, H.A. (1986) A biogeographic analysis of Australian elapid\
 snakes. In: Atlas of Elapid Snakes of Australia. (Ed.) R. Longmore,\
 pp. 4-15. Australian Flora and Fauna Series Number 7. Australian\
 Government Publishing Service: Canberra.",

  "Mauro Munoz",            // Code author.
  "mauro [at] cria.org.br", // Code author's contact.

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
  return new Bioclim();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}


/****************************************************************/
/**************************** Bioclim ***************************/

/*******************/
/*** constructor ***/

Bioclim::Bioclim() :
  AlgorithmImpl( &metadata ),
  _done( false ),
  _minimum(),
  _maximum(),
  _mean(),
  _std_dev()
{ }


/******************/
/*** destructor ***/

Bioclim::~Bioclim()
{
}


/******************/
/*** initialize ***/
int
Bioclim::initialize()
{
  Scalar cutoff = 0.0;
  // Read and check the standard deviation cutoff parameter.
  if ( ! getParameter( CUTOFF_ID, &cutoff ) ) {
    g_log.error(1, "Parameter " CUTOFF_ID " not set properly.\n");
    return 0;
  }

  if ( cutoff <= 0 ) {
    g_log.warn( "Bioclim - parameter out of range: %f\n", cutoff );
    return 0;
  }
  
  // Number of independent variables.
  int dim = _samp->numIndependent();
  g_log.info( "Reading %d-dimensional occurrence points.\n", dim );

  // Check the number of sampled points.
  int npnt = _samp->numPresence();
  if (  npnt < 2 ) {
    g_log.error( 1, "Bioclim needs at least 2 points inside the mask!\n" ); 
    // g_log.error() does a ::exit(rc).
  }

  g_log.info( "Using %d points to find the bioclimatic envelop.\n", npnt );

  computeStats( _samp->getPresences() );

  _std_dev *= cutoff;

  _done = true;

  return 1;
}


/***************/
/*** iterate ***/
int
Bioclim::iterate()
{
  return 1;
}


/************/
/*** done ***/
int
Bioclim::done() const
{
  // This is not an iterative algorithm.
  return _done;
}


/*****************/
/*** get Value ***/
Scalar
Bioclim::getValue( const Sample& x ) const
{
  // Zero if some point valuble is outside its respective envelop.
  Scalar outside_envelop = 0;

  // Finds the distance from each variable mean to the respective
  // point value.
  Sample dif = x;
  dif -= _mean;

  for( int i=0; i<x.size(); i++) {

    if ( x[i] < _minimum[i] || x[i] > _maximum[i] ) {
      return 0.0;
    }

    if ( ! outside_envelop ) {

      Scalar cutoff = _std_dev[i];

      Scalar diffi = dif[i];
    
      // If some x[i] is out of its bioclimatic envelop, predicts
      // no occurrence.
      if ( dif[i] > cutoff || dif[i] < -cutoff ) {
        outside_envelop = 1;
      }
    }

  }

  // If all point values are within the envelop, returns probability
  // 1.0. Else, if some point is outside the envelop but inside
  // the upper and lower ranges, returns 0.5 of probability.
  return outside_envelop ? 0.5 : 1.0;
}


/***********************/
/*** get Convergence ***/
int
Bioclim::getConvergence( Scalar *val )
{
  *val = 1.0;
  return 1;
}


/*******************/
/*** get Minimum ***/
void
Bioclim::computeStats( const OccurrencesPtr& occs )
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

/****************************************************************/
/****************** configuration *******************************/
void
Bioclim::_getConfiguration( ConfigurationPtr& config ) const
{
  if ( !_done )
    return;

  ConfigurationPtr model_config( new ConfigurationImpl("BioclimModel") );
  config->addSubsection( model_config );

  model_config->addNameValue( "Mean", _mean );
  model_config->addNameValue( "StdDev", _std_dev );
  model_config->addNameValue( "Minimum", _minimum );
  model_config->addNameValue( "Maximum", _maximum );

}

void
Bioclim::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection("BioclimModel",false );

  if (!model_config)
    return;

  _done = true;

  _mean = model_config->getAttributeAsSample( "Mean" );
  _std_dev = model_config->getAttributeAsSample( "StdDev" );
  _minimum = model_config->getAttributeAsSample( "Minimum" );
  _maximum = model_config->getAttributeAsSample( "Maximum" );

  return;
}

/*******************/
/*** log Envelop ***/
void
Bioclim::logEnvelop()
{
  g_log( "Envelop with %d dimensions (variables).\n\n", _mean.size() );

  for ( int i = 0; i < _mean.size(); i++ )
    {
      g_log( "Variable %02d:", i );
      g_log( " Mean     : %f\n", _mean[i] );
      g_log( " Deviation: %f\n", _std_dev[i] );
      g_log( " Minimum  : %f\n", _minimum[i] );
      g_log( " Maximum  : %f\n", _maximum[i] );
      g_log( "\n" );
    }
}
