/**
 * Definition of the Virtual Niche Generator.
 * 
 * @author Renato De Giovanni <renato at cria.org.br
 * @date 2007-08-22
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2012 by CRIA -
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

#include "virtual_niche.hh"

#include <openmodeller/Configuration.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/Random.hh>

#include <stdio.h>
#include <cmath>
#include <vector>

using namespace std;

#ifndef PI
#define PI 3.141592653589793238462643
#endif

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 4

#define BACKGROUND_ID   "NumberOfBackgroundPoints"
#define USE_ABSENCES_ID "UseAbsencesAsBackground"
#define THRESHOLD_ID    "SuitabilityThreshold"
#define STD_FACTOR_ID   "StandardDeviationFactor"

/****************************/
/*** Algorithm parameters ***/

static AlgParamMetadata parameters[NUM_PARAM] = {
  
  // Number of background points to be generated
  {
    BACKGROUND_ID,                 // Id.
    "Number of background points", // Name.
    Integer,                       // Type.
    "Number of background points to be generated.", // Overview
    "Number of background points to be generated, which will be used to estimate the standard deviation of each variable in the area of interest.", // Description.
    1,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    1,      // Not zero if the parameter has upper limit.
    10000,  // Parameter's upper limit.
    "10000" // Parameter's typical (default) value.
  },
  // Use absence points as background
  {
    USE_ABSENCES_ID,     // Id.
    " Use absence points as background", // Name.
    Integer,  // Type.
    " Use absence points as background", // Overview
    "When absence points are provided, this parameter can be used to instruct the algorithm to use them as background points. This would prevent the algorithm to randomly generate them, also facilitating comparisons between different algorithms.", // Description.
    1,   // Not zero if the parameter has lower limit.
    0,   // Parameter's lower limit.
    1,   // Not zero if the parameter has upper limit.
    1,   // Parameter's upper limit.
    "0"  // Parameter's typical (default) value.
  },
  // Suitability threshold
  {
    THRESHOLD_ID,            // Id.
    "Suitability threshold", // Name.
    Real,                    // Type.
    "Suitability threshold to get a binary niche.", // Overview
    "Suitability threshold to get a binary niche. Use 1 if you want to keep the continuous niche.", // Description.
    1,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    1,      // Not zero if the parameter has upper limit.
    1,      // Parameter's upper limit.
    "1.0"   // Parameter's typical (default) value.
  },
  // Standard Deviation Factor
  {
    STD_FACTOR_ID,               // Id.
    "Standard deviation factor", // Name.
    Real,                        // Type.
    "Standard deviation factor.", // Overview
    "Factor (x) used to control the minimum limit of the random standard deviation for each variable. The random standard deviation will be a value between [x*S, S], where S is the standard deviation of the entire native region. Increase the factor to get larger niches, especially when using many environmental variables.", // Description.
    1,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    1,      // Not zero if the parameter has upper limit.
    1,      // Parameter's upper limit.
    "0.0"   // Parameter's typical (default) value.
  },
};

/**************************/
/*** Algorithm metadata ***/

static AlgMetadata metadata = {

  "VNG",                     // Id
  "Virtual Niche Generator", // Name
  "0.2",                     // Version

  // Overview
  "Algorithm used to create virtual niches using the\
 first presence point as a reference for optimum\
 environmental conditions. The niche is represented by\
 a multivariate Gaussian distribution with the mean value\
 based on the optimum conditions and a random standard\
 deviation smaller than the standard deviation of the\
 region of interest. Suitability is calculated by assuming\
 independence between all variables.",
  // Description.
  "Algorithm used to create virtual niches using the\
 first presence point as a reference for optimum\
 environmental conditions. The niche is represented by\
 a multivariate Gaussian distribution with the mean value\
 based on the optimum conditions and a random standard\
 deviation. Suitability is calculated by assuming\
 independence between all variables, i.e., the final\
 value is the product of the individual suitability\
 for each variable. Individual suitabilities are\
 calculated as the result of the Gaussian probability\
 density function scaled by a factor to make the\
 optimum condition correspond to 1. Standard deviations\
 for each variable are randomly chosen within the range\
 [x*S, S], where S is the standard deviation of the entire\
 native region (calculated based on the background\
 points) and x is the standard deviation factor parameter\
 between 0 and 1.",

  "Renato De Giovanni",  // Author

  // Bibliography.
  "renato [at] cria.org.br",

  "Renato De Giovanni",      // Code author
  "renato [at] cria.org.br", // Code author's contact

  0,  // Does not accept categorical data.
  0,  // Does not need (pseudo)absence points.

  NUM_PARAM,
  parameters
};



/****************************************************************/
/****************** Algorithm's factory function ****************/

OM_ALG_DLL_EXPORT
AlgorithmImpl *
algorithmFactory()
{
  return new VirtualNicheGenerator();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}


/****************************************************************/
/**************************** EnvelopeScore ***************************/

/*******************/
/*** constructor ***/

VirtualNicheGenerator::VirtualNicheGenerator() :
  AlgorithmImpl( &metadata ),
  _done( false ),
  _minimum(),
  _maximum(),
  _mean(),
  _std(),
  _scale(),
  _threshold(1.0),
  _std_factor(0.0)
{ }


/******************/
/*** destructor ***/

VirtualNicheGenerator::~VirtualNicheGenerator()
{
}


/******************/
/*** initialize ***/
int
VirtualNicheGenerator::initialize()
{
  Log::instance()->warn( "This algorithm creates virtual niches - do not use it to generate models for real species!\n" );

  if ( ! getParameter( THRESHOLD_ID, &_threshold ) ) {

    _threshold = 1.0;
  }

  // Check number of points
  if ( _samp->numPresence() > 1 ) {

    Log::instance()->warn( "Virtual Niche uses only one point (the first). All other points will be ignored.\n" );
  }

  // Background points
  bool use_absences_as_background = false;
  int use_abs;
  int num_absences = 0;
  if ( getParameter( USE_ABSENCES_ID, &use_abs ) && use_abs == 1 ) {

    use_absences_as_background = true;
  }

  if ( use_absences_as_background ) {

    num_absences = _samp->numAbsence();

    if ( num_absences ) {

      _num_background = num_absences;
    }
    else {

      Log::instance()->warn( "No absence points provided. Generating 10000 background points.\n" );
      _num_background = 10000;
    }
  }
  else {

    if ( ! getParameter( BACKGROUND_ID, &_num_background ) ) {

      Log::instance()->warn( "Parameter '" BACKGROUND_ID "' unspecified. Using default value (10000).\n");

      _num_background = 10000;
    }

    if ( _num_background <= 0 ) {
	
      Log::instance()->warn( "Parameter '" BACKGROUND_ID "' must be greater than zero.\n" );
      return 0;
    }
  }

  OccurrencesPtr presences = _samp->getPresences();

  _background = new OccurrencesImpl( presences->label(), presences->coordSystem() );

  if ( use_absences_as_background && num_absences >=0 ) {

    _background->appendFrom( _samp->getAbsences() );
  }
  else {

    // Generate random background points
    Log::instance()->info( "Generating random background points.\n" );

    for ( int i = 0; i < _num_background; ++i ) {

      OccurrencePtr oc = _samp->getPseudoAbsence();
      _background->insert( oc ); 
    }
  }

  if ( ! getParameter( STD_FACTOR_ID, &_std_factor ) ) {

    Log::instance()->warn( "Parameter '" STD_FACTOR_ID "' unspecified. Using default value (0).\n");

    _std_factor = 0.0;
  }

  if ( _std_factor < 0.0 ) {

    _std_factor = 0.0;
  }
  else {

    if ( _std_factor > 1.0 ) {

      _std_factor = 1.0;
    }
  }

  return 1;
}


/***************/
/*** iterate ***/
int
VirtualNicheGenerator::iterate()
{
  Log::instance()->info( "Generating virtual niche.\n" );

  // Mean of the normal distribution is the environmental value on the first point
  _mean = _samp->getPresence(0)->environment();

  EnvironmentPtr env = _samp->getEnvironment();

  // Minimum and maximum values for each environmental variable for the whole layer
  env->getExtremes(&_minimum, &_maximum);

  // Estimate standard deviation in the whole region of interest using background pts
  Sample background_std = Sample(_mean.size(), 0.0);
  Sample background_mean = Sample(_mean.size(), 0.0);

  OccurrencesImpl::const_iterator oc = _background->begin();
  OccurrencesImpl::const_iterator end = _background->end();

  // Compute mean
  while ( oc != end ) {

    Sample tmp( (*oc)->environment() );
    background_mean += tmp;
    ++oc;
  }
  background_mean /= Scalar( _num_background );

  oc = _background->begin();
  end = _background->end();

  // Compute variance
  while ( oc != end ) {

    Sample tmp( (*oc)->environment() );
    tmp -= background_mean;
    tmp *= tmp;
    background_std += tmp;
    ++oc;
  }

  Scalar npts = Scalar( _num_background - 1 );

  // Now divide and root to get deviation
  background_std /= npts;
  background_std.sqrt();

  // Get a random standard deviation for each variable for the niche function
  _std = _mean; // just initialize the sample
  Sample max_pdf_val = Sample(_mean.size(), 1.0); // just initialize the sample

  Random random;

  for (int i = 0; i < _samp->numIndependent(); ++i) {

    // Random standard deviation, restricting the interval using std factor
    // to avoid too constrained niches
    _std[i] = random(_std_factor*background_std[i], background_std[i]);

    // Maximum value of the probability density function
    max_pdf_val[i] = pdf(_mean[i], _std[i], _mean[i]);
  }

  _scale = Sample(_mean.size(), 1.0);
  _scale /= max_pdf_val;

  dump();

  _done = true;

  return 1;
}


/************/
/*** done ***/
int
VirtualNicheGenerator::done() const
{
  // This is not an iterative algorithm.
  return _done;
}


/*****************/
/*** get Value ***/
Scalar
VirtualNicheGenerator::getValue( const Sample& x ) const
{
  Scalar suitability = 1.0;

  for (int i=0; i < _samp->numIndependent(); i++) {

    Scalar pdf_value = pdf(_mean[i], _std[i], x[i]);

    suitability *= pdf_value*_scale[i];
  }

  if ( _threshold < 1.0 ) {

    return (suitability < _threshold) ? 0.0 : 1.0;
  }

  return suitability;
}


/***********************/
/*** get Convergence ***/
int
VirtualNicheGenerator::getConvergence( Scalar * const val ) const
{
  *val = 1.0;
  return 1;
}


/****************************************************************/
/****************** configuration *******************************/
void
VirtualNicheGenerator::_getConfiguration( ConfigurationPtr& config ) const
{
  if ( !_done )
    return;

  ConfigurationPtr model_config( new ConfigurationImpl("VirtualNiche") );
  config->addSubsection( model_config );

  model_config->addNameValue( "Minimum"  , _minimum );
  model_config->addNameValue( "Maximum"  , _maximum );
  model_config->addNameValue( "Mean"     , _mean );
  model_config->addNameValue( "Std"      , _std );
  model_config->addNameValue( "Scale"    , _scale );
  model_config->addNameValue( "Threshold", _threshold );
}

void
VirtualNicheGenerator::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection("VirtualNiche");

  if (!model_config)
    return;

  _done = true;

  _minimum   = model_config->getAttributeAsSample( "Minimum" );
  _maximum   = model_config->getAttributeAsSample( "Maximum" );
  _mean      = model_config->getAttributeAsSample( "Mean" );
  _std       = model_config->getAttributeAsSample( "Std" );
  _scale     = model_config->getAttributeAsSample( "Scale" );
  _threshold = model_config->getAttributeAsDouble( "Threshold", 1.0 );

  return;
}

/************/
/*** dump ***/
void
VirtualNicheGenerator::dump()
{
  Log::instance()->info( "Result for %d dimensions (variables)\n", _minimum.size() );

  for ( unsigned int i = 0; i < _minimum.size(); i++ ) {

    Log::instance()->info( "Variable %02d:\n", i );
    Log::instance()->info( " Minimum  : %f\n", _minimum[i] );
    Log::instance()->info( " Mean     : %f\n", _mean[i] );
    Log::instance()->info( " Maximum  : %f\n", _maximum[i] );
    Log::instance()->info( " Std      : %f\n", _std[i] );
    Log::instance()->info( " Scale    : %f\n", _scale[i] );
  }
}

/***********/
/*** pdf ***/
Scalar
VirtualNicheGenerator::pdf(Scalar mean, Scalar std, Scalar val) const
{
    return (1.0/(std*sqrt(2.0*PI)))*exp(-0.5*pow((val-mean)/std, 2));
}


