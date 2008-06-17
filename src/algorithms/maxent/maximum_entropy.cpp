/**
 * Definition of the base (abstract) class for MaxEnt algorithms.
 * 
 * @author Elisangela S. da C. Rodrigues (elisangela . rodrigues [at] poli . usp . br)
 * @author Renato De Giovanni (renato [at] cria . org . br)
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2007 by CRIA -
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

#include "maximum_entropy.hh"
#include <openmodeller/ScaleNormalizer.hh>
#include <openmodeller/Sampler.hh>

#include "meevent.hpp"
#include "maxentmodel.hpp"
#include "modelfile.hpp"

#include <iostream>
#include <sstream>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

using boost::shared_ptr;
using boost::shared_array;

using namespace std;
using namespace maxent;

typedef MaxentModel::context_type me_context_type;
typedef MaxentModel::outcome_type me_outcome_type;

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 5

#define PSEUDO_ID        "NumberOfPseudoAbsences"
#define ITERATIONS_ID    "NumberOfIterations"
#define METHOD_ID        "TrainingMethod"
#define GAUSSIAN_COEF_ID "GaussianPriorSmoothingCoeficient"
#define TOLERANCE_ID     "TerminateTolerance"

#define MAXENT_LOG_PREFIX "Maxent: "

/******************************/
/*** Algorithm's parameters ***/

static AlgParamMetadata parameters[NUM_PARAM] = {

  // Number of pseudo absences to be generated
  {
    PSEUDO_ID,                   // Id.
    "Number of pseudo-absences", // Name.
    Integer,                     // Type.
    "Number of pseudo-absences to be generated (when no absences have been provided).", // Overview
    "Number of pseudo-absences to be generated (when no absences have been provided.", // Description.
    1,         // Not zero if the parameter has lower limit.
    1,         // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    0,         // Parameter's upper limit.
    "500"      // Parameter's typical (default) value.
  },
  // Number of iterations
  {
    ITERATIONS_ID,          // Id.
    "Number of iterations", // Name.
    Integer,                // Type.
    "Number of iterations.", // Overview
    "Number of iterations.", // Description.
    1,         // Not zero if the parameter has lower limit.
    1,         // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    0,         // Parameter's upper limit.
    "15"     // Parameter's typical (default) value.
  },
  // Training method
  {
    METHOD_ID,                         // Id.
    "Training method (gis or lbfgs)", // Name.
    String,                            // Type.
    "Training method (gis or lbfgs) used to estimate the maximum entropy parameters.", // Overview
    "Training method (gis or lbfgs) used to estimate the maximum entropy parameters. Possible values are: gis (Generalized Iterative Scaling) or lbfgs (Limited-Memory Variable Metric).", // Description.
    0,         // Not zero if the parameter has lower limit.
    0,         // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    0,         // Parameter's upper limit.
    "gis"      // Parameter's typical (default) value.
  },
  // Gaussian Prior Smoothing Coeficient
  {
    GAUSSIAN_COEF_ID, // Id.
    "Gaussian Prior Smoothing Coeficient (variance)", // Name.
    Real, // Type.
    "Gaussian Prior Smoothing Coeficient (variance).", // Overview
    "Gaussian Prior Smoothing Coeficient (variance). Zero turns off Gaussian Prior Smoothing.", // Description.
    1,  // Not zero if the parameter has lower limit.
    0,  // Parameter's lower limit.
    0,  // Not zero if the parameter has upper limit.
    0,  // Parameter's upper limit.
    "0.0" // Parameter's typical (default) value.
  },
  // Terminate tolerance
  {
    TOLERANCE_ID, // Id.
    "Terminate tolerance", // Name.
    Real, // Type.
    "Tolerance for detecting model convergence.", // Overview
    "Tolerance for detecting model convergence.", // Description.
    0,  // Not zero if the parameter has lower limit.
    0,  // Parameter's lower limit.
    0,  // Not zero if the parameter has upper limit.
    0,  // Parameter's upper limit.
    "0.00001" // Parameter's typical (default) value.
  },
};



/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {
  
  "MAXENT",          // Id.
  "Maximum Entropy", // Name.
  "0.1",       	     // Version.

  // Overview.
  "The principle of maximum entropy is a method for analyzing available qualitative information in order to determine a unique epistemic probability distribution. It states that the least biased distribution that encodes certain given information is that which maximizes the information entropy (content retrieved from Wikipedia on the 19th of May, 2008: http://en.wikipedia.org/wiki/Maximum_entropy). The openModeller implementation of this algorithm makes use of the Maximum Entropy Modeling Toolkit written by Zhang Le, which offers two methods to estimate the maximum entropy parameters: GIS (Generalized Iterative Scaling) and L-BFGS (Limited-Memory Variable Metric). Please note that the maxent sofware package uses its own method (not implemented here) to find the maximum entropy parameters.",

  // Description.
  "The principle of maximum entropy is a method for analyzing available qualitative information in order to determine a unique epistemic probability distribution. It states that the least biased distribution that encodes certain given information is that which maximizes the information entropy (content retrieved from Wikipedia on the 19th of May, 2008: http://en.wikipedia.org/wiki/Maximum_entropy). E.T. Jaynes introduced the maximum entropy principle in 1957 saying that 'Information theory provides a constructive criterion for setting up probability distributions on the basis of partial knowledge, and leads to a type of statistical inference which is called the maximum entropy estimate. It is the least biased estimate possible on the given information; i.e., it is maximally noncommittal with regard to missing information'. The openModeller implementation of this algorithm makes use of the Maximum Entropy Modeling Toolkit written by Zhang Le, which offers two methods to estimate the maximum entropy parameters: GIS (Generalized Iterative Scaling) and L-BFGS (Limited-Memory Variable Metric). Please note that the maxent sofware package uses its own method (not implemented here) to find the maximum entropy parameters.",

  "", // Algorithm author.

  "1) Jaynes, E.T. (1957) Information Theory and Statistical Mechanics. In Physical Review, Vol. 106, #4 (pp 620-630). 2) Berger, A. L., Pietra, S. A. D. and Pietra, V. J. D. (1996). A maximum entropy approach to natural language processing. Computational Linguistics, 22, 39-71. 3) Darroch, J.N. and Ratcliff, D. (1972) Generalized iterative scaling for log-linear models. The Annals of Mathematical Statistics, Vol. 43: pp 1470-1480. 4) Malouf, R. (2003) A comparison of algorithms for maximum entropy parameter estimation. Proceedings of the Sixth Conference on Natural Language Learning. 5) Phillips, S.J., Dudík, M. and Schapire, R.E. (2004) A maximum entropy approach to species distribution modeling. Proceedings of the Twenty-First International Conference on Machine Learning, pp 655-662.", // Bibliography.

  "Elisangela S. da C. Rodrigues, Renato De Giovanni", // Code author.

  "elisangela.rodrigues [at] poli . usp . br, renato [at] cria . org . br", // Code author's contact.

  0, // Does not accept categorical data.
  0, // Does not need (pseudo)absence points.

  NUM_PARAM, // Algorithm's parameters.
  parameters
};

/****************************************************************/
/****************** Algorithm's factory function ****************/

OM_ALG_DLL_EXPORT
AlgorithmImpl *
algorithmFactory()
{
  return new MaximumEntropy();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}

/****************************************************************/
/************************ Maximum Entropy ***********************/

/*******************/
/*** constructor ***/

MaximumEntropy::MaximumEntropy() :
  AlgorithmImpl(&metadata),
  _done(false),
  _num_layers(0)
{ 
  _normalizerPtr = new ScaleNormalizer( 0.0, 1.0, true );
}

/******************/
/*** destructor ***/

MaximumEntropy::~MaximumEntropy()
{
}

/******************/
/*** initialize ***/

int
MaximumEntropy::initialize()
{
  // Check the number of presences.
  int num_presences = _samp->numPresence();

  if ( num_presences == 0 ) {

    Log::instance()->error( MAXENT_LOG_PREFIX "No presence points inside the mask!\n" );
    return 0;
  }
  
  // Load presence points.
  _presences = _samp->getPresences();

  // Number of iterations
  if ( ! getParameter( ITERATIONS_ID, &_num_iterations ) ) {

    Log::instance()->error( MAXENT_LOG_PREFIX "Parameter '" ITERATIONS_ID "' not passed.\n" );
    return 0;
  }

  if ( _num_iterations <= 0 ) {

    Log::instance()->error( MAXENT_LOG_PREFIX "Parameter '" ITERATIONS_ID "' must be greater then zero.\n" );
    return 0;
  }

  // Training method
  if ( ! getParameter( METHOD_ID, &_method ) ) {

    Log::instance()->error( MAXENT_LOG_PREFIX "Parameter '" METHOD_ID "' not passed.\n" );
    return 0;
  }

  if ( _method != "gis" && _method != "lbfgs" ) {

    Log::instance()->error( MAXENT_LOG_PREFIX "Parameter '" METHOD_ID "' must be either gis or lbfgs.\n" );
    return 0;
  }

  // Gaussian prior smoothing
  if ( ! getParameter( GAUSSIAN_COEF_ID, &_gaussian_coef ) ) {

    Log::instance()->warn( MAXENT_LOG_PREFIX "Parameter '" GAUSSIAN_COEF_ID "' not passed. Gaussian smoothing will be turned off.\n" );
    _gaussian_coef = 0.0;
  }

  // Tolerance
  if ( ! getParameter( TOLERANCE_ID, &_tolerance ) ) {

    Log::instance()->warn( MAXENT_LOG_PREFIX "Parameter '" TOLERANCE_ID "' not passed. Using default value (1-E05)\n" );
    _tolerance = 0.00001;
  }
  else {

    if ( _tolerance <= 0.0 ) {

      Log::instance()->error( MAXENT_LOG_PREFIX "Parameter '" TOLERANCE_ID "' must be greater than zero\n" );
      return 0;
    }
  }

  // Check the number of absences.
  int num_absences = _samp->numAbsence();

  if ( num_absences == 0 ) {

    Log::instance()->info( MAXENT_LOG_PREFIX "Generating pseudo-absences.\n" );

    if ( ! getParameter( PSEUDO_ID, &num_absences ) ) {

      Log::instance()->warn( MAXENT_LOG_PREFIX "Number of pseudo absences unspecified. Default will be 500.\n" );

      num_absences = 500;
    }
    else if ( num_absences <= 0 ) {

      Log::instance()->warn( MAXENT_LOG_PREFIX "Number of pseudo absences must be greater than zero.\n" );
      return 0;
    }

    _absences = new OccurrencesImpl( _presences->name(), _presences->coordSystem() );

    for ( int i = 0; i < num_absences; ++i ) {

      OccurrencePtr oc = _samp->getPseudoAbsence();
      _absences->insert( oc ); 
    }

    // Compute normalization with all points
    SamplerPtr mySamplerPtr = createSampler( _samp->getEnvironment(), _presences, _absences );

    _normalizerPtr->computeNormalization( mySamplerPtr );

    setNormalization( _samp );

    _absences->normalize( _normalizerPtr );
  }
  else {

    // should be normalized already
    _absences = _samp->getAbsences();
  }

  _num_layers = _samp->numIndependent();

  return 1;
}

/***************/
/*** iterate ***/

int
MaximumEntropy::iterate()
{
  _model.begin_add_event();

  // Notes:  
  // outcome = class (presence / absence)
  // feature = env layer
  // context = sample

  // Presences

  OccurrencesImpl::const_iterator p_iterator = _presences->begin();
  OccurrencesImpl::const_iterator p_end = _presences->end();

  while ( p_iterator != p_end ) {

    Sample sample = (*p_iterator)->environment();

    me_context_type context;
    me_outcome_type outcome("p"); // p = presence

    for ( int i = 0; i < _num_layers; ++i ) {

      stringstream out;
      out << i;
      context.push_back( make_pair( out.str(), (float)sample[i] ) );
    }

    _model.add_event( context, outcome, 1 );
     
    ++p_iterator;
  }

  Log::instance()->info( MAXENT_LOG_PREFIX "Added %d presence events.\n", _presences->numOccurrences() );

  // Absences

  p_iterator = _absences->begin();
  p_end = _absences->end();

  while ( p_iterator != p_end ) {

    Sample sample = (*p_iterator)->environment();

    me_context_type context;
    me_outcome_type outcome("a"); // a = absence

    for ( int i = 0; i < _num_layers; ++i ) {

      stringstream out;
      out << i;
      context.push_back( make_pair( out.str(), (float)sample[i] ) );
    }

    _model.add_event( context, outcome, 1 );
     
    ++p_iterator;
  }

  Log::instance()->info( MAXENT_LOG_PREFIX "Added %d absence events.\n", _absences->numOccurrences() );

  _model.end_add_event();

  _model.train( _num_iterations, _method, _gaussian_coef, _tolerance );

  _done = true;

  return 1;
}

/************/
/*** done ***/

int
MaximumEntropy::done() const
{
  return _done;
}

/*****************/
/*** get Value ***/

Scalar
MaximumEntropy::getValue( const Sample& x ) const
{
  me_context_type context;

  for ( int i = 0; i < _num_layers; ++i ) {

    stringstream out;
    out << i;
    context.push_back( make_pair( out.str(), (float)x[i] ) );
  }

  me_outcome_type outcome("p"); // p = presence

  return _model.eval( context, outcome ); // probability of presence
}

/***********************/
/*** get Convergence ***/
int
MaximumEntropy::getConvergence( Scalar *val )
{
  *val = 1.0;
  return 1;
}

/****************************************************************/
/****************** configuration *******************************/

void
MaximumEntropy::_getConfiguration( ConfigurationPtr& config ) const
{
  if ( ! _done ) {

    return;
  }
  
  ConfigurationPtr model_config( new ConfigurationImpl( "MaximumEntropy" ) );
  config->addSubsection( model_config );

  model_config->addNameValue( "NumLayers", _num_layers );

  MaxentModelFile model_file = _model.save();

  shared_ptr<me::ParamsType> m_params;
  size_t m_n_theta;
  shared_array<double> m_theta;

  // Get maxent model parameters
  model_file.params( m_params, m_n_theta, m_theta );

  // write theta
  ConfigurationPtr theta_config( new ConfigurationImpl( "Theta" ) );
  model_config->addSubsection( theta_config );

  double *theta_values = new double[m_n_theta];

  for ( size_t i = 0; i < m_n_theta; ++i ) {

    theta_values[i] = m_theta[i];
  }

  theta_config->addNameValue( "Values", theta_values, m_n_theta );

  delete[] theta_values;
}

void
MaximumEntropy::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "MaximumEntropy", false );

  if ( ! model_config ) {

    return;
  }

  _num_layers = model_config->getAttributeAsInt( "NumLayers", 0 );

  MaxentModelFile model_file;

  // note: pred_map has the same size of _num_layers, and the index starts with zero. 

  shared_ptr<me::PredMapType> m_pred_map( new me::PredMapType );

  for ( int i = 0; i < _num_layers; ++i ) {

    stringstream out;
    out << i;
    m_pred_map->add( out.str() );
  }

  model_file.set_pred_map( m_pred_map );

  // note: outcome_map has size 2 and values "p" and "a"

  shared_ptr<me::OutcomeMapType> m_outcome_map( new me::OutcomeMapType );

  m_outcome_map->add( "p" );
  m_outcome_map->add( "a" );

  model_file.set_outcome_map( m_outcome_map );

  // note: params seems to always have _num_layers size, and each
  //       param seems to always have outcomes size (2) with index starting at zero.

  shared_ptr<me::ParamsType> m_params( new me::ParamsType );

  std::vector<pair<me::outcome_id_type, size_t> > params;

  size_t fid = 0;

  for ( int i = 0; i < _num_layers; ++i ) {

    params.clear();
    params.push_back( make_pair( 0, fid++ ) );
    params.push_back( make_pair( 1, fid++ ) );

    m_params->push_back( params );
  }

  // Theta

  ConstConfigurationPtr theta_config = model_config->getSubsection( "Theta", false );

  if ( ! theta_config ) {

    return;
  }

  std::vector<double> theta_values = theta_config->getAttributeAsVecDouble( "Values" );

  size_t m_n_theta = theta_values.size();

  shared_array<double> m_theta( new double[m_n_theta] );

  for ( unsigned int i = 0; i < m_n_theta; ++i ) {

    m_theta[i] = theta_values[i];
  }

  model_file.set_params( m_params, m_n_theta, m_theta );

  _model.load( model_file );

  _done = true;
}
