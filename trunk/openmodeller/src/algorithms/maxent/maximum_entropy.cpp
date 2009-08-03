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

#ifdef WIN32
// avoid warnings caused by problems in VC headers
#define _SCL_SECURE_NO_DEPRECATE
#endif

#include "maximum_entropy.hh"
#include <openmodeller/ScaleNormalizer.hh>
#include <openmodeller/Sampler.hh>

#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>

#include <limits>

using namespace std;

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 3

#define BACKGROUND_ID      "NumberOfBackgroundPoints"
#define ITERATIONS_ID      "NumberOfIterations"
#define TOLERANCE_ID       "TerminateTolerance"

#define MAXENT_LOG_PREFIX "Maxent: "

/******************************/
/*** Algorithm's parameters ***/

static AlgParamMetadata parameters[NUM_PARAM] = {
  
  // Number of background points to be generated
  {
    BACKGROUND_ID,                 // Id.
    "Number of background points", // Name.
    Integer,                       // Type.
    "Number of background points to be generated.", // Overview
    "Number of background points to be generated.", // Description.
    1,         // Not zero if the parameter has lower limit.
    0,         // Parameter's lower limit.
    1,         // Not zero if the parameter has upper limit.
    10000,     // Parameter's upper limit.
    "10000"    // Parameter's typical (default) value.
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
    "500"      // Parameter's typical (default) value.
  },
  // Terminate tolerance
  {
    TOLERANCE_ID, // Id.
    "Terminate tolerance", // Name.
    Real, // Type.
    "Tolerance for detecting model convergence.", // Overview
    "Tolerance for detecting model convergence.", // Description.
    1,    // Not zero if the parameter has lower limit.
    0.0,  // Parameter's lower limit.
    0,    // Not zero if the parameter has upper limit.
    0,    // Parameter's upper limit.
    "0.00001" // Parameter's typical (default) value.
  },
};

/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {
  
  "MAXENT",          // Id.
  "Maximum Entropy", // Name.
  "0.2",       	     // Version.

  // Overview.
  "The principle of maximum entropy is a method for analyzing available qualitative information in order to determine a unique epistemic probability distribution. It states that the least biased distribution that encodes certain given information is that which maximizes the information entropy (content retrieved from Wikipedia on the 19th of May, 2008: http://en.wikipedia.org/wiki/Maximum_entropy).",

  // Description.
  "The principle of maximum entropy is a method for analyzing available qualitative information in order to determine a unique epistemic probability distribution. It states that the least biased distribution that encodes certain given information is that which maximizes the information entropy (content retrieved from Wikipedia on the 19th of May, 2008: http://en.wikipedia.org/wiki/Maximum_entropy). E.T. Jaynes introduced the maximum entropy principle in 1957 saying that 'Information theory provides a constructive criterion for setting up probability distributions on the basis of partial knowledge, and leads to a type of statistical inference which is called the maximum entropy estimate. It is the least biased estimate possible on the given information; i.e., it is maximally noncommittal with regard to missing information'.",

  "", // Algorithm author.

  "1) Jaynes, E.T. (1957) Information Theory and Statistical Mechanics. In Physical Review, Vol. 106, #4 (pp 620-630). 2) Berger, A. L., Pietra, S. A. D. and Pietra, V. J. D. (1996). A maximum entropy approach to natural language processing. Computational Linguistics, 22, 39-71. 3) Darroch, J.N. and Ratcliff, D. (1972) Generalized iterative scaling for log-linear models. The Annals of Mathematical Statistics, Vol. 43: pp 1470-1480. 4) Malouf, R. (2003) A comparison of algorithms for maximum entropy parameter estimation. Proceedings of the Sixth Conference on Natural Language Learning. 5) Phillips, S.J., Dudík, M. and Schapire, R.E. (2004) A maximum entropy approach to species distribution modeling. Proceedings of the Twenty-First International Conference on Machine Learning, pp 655-662.", // Bibliography.

  "Elisangela S. da C. Rodrigues, Renato De Giovanni", // Code author.

  "elisangela.rodrigues [at] poli . usp . br, renato [at] cria . org . br", // Code author's contact.

  1, // Accept categorical data.
  1, // Does not need (pseudo)absence points.

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
  delete regularization_parameters;
  delete features_mean;
  delete feat_stan_devi;
  delete q_lambda_f;
  delete lambda;
  delete q_lambda_x;
}

/**************************/
/*** need Normalization ***/
int MaximumEntropy::needNormalization()
{
  // It will be necessary to generate background points, so do not waste
  // time normalizing things because normalization should ideally consider
  // all points. In this specific case, normalization will take place in initialize().
  return 0;
}

/******************/
/*** initialize ***/

int
MaximumEntropy::initialize()
{
  // Check the number of presences.
  _num_presences = _samp->numPresence();

  if ( _num_presences == 0 ) {

    Log::instance()->error( MAXENT_LOG_PREFIX "No presence points inside the mask!\n" );
    return 0;
  }
  
  // Load presence points.
  _presences = _samp->getPresences();
  
  // Number of iterations
  if ( ! getParameter( ITERATIONS_ID, &_num_iterations ) ) {

    Log::instance()->warn( MAXENT_LOG_PREFIX "Parameter '" ITERATIONS_ID "' not passed. Using default value (500)\n" );
    _num_iterations = 500;
  }
  else {
    if ( _num_iterations <= 0 ) {
      
      Log::instance()->error( MAXENT_LOG_PREFIX "Parameter '" ITERATIONS_ID "' must be greater then zero.\n" );
      return 0;
    }
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

  // Generate background points

  Log::instance()->info( MAXENT_LOG_PREFIX "Generating background points.\n" );

  if ( ! getParameter( BACKGROUND_ID, &_num_background ) ) {

    Log::instance()->warn( MAXENT_LOG_PREFIX "Parameter '" BACKGROUND_ID "' unspecified. Using default value (10000).\n");

    _num_background = 10000;
  }
  else {

    if ( _num_background < 0 ) {
	
      Log::instance()->warn( MAXENT_LOG_PREFIX "Parameter '" BACKGROUND_ID "' must be greater than zero.\n" );
      return 0;
    }
  }
    
  _background = new OccurrencesImpl( _presences->name(), _presences->coordSystem() );

  for ( int i = 0; i < _num_background; ++i ) {

    OccurrencePtr oc = _samp->getPseudoAbsence();
    _background->insert( oc ); 
  }

  // Compute normalization with all points
  SamplerPtr mySamplerPtr = createSampler( _samp->getEnvironment(), _presences, _background );

  _normalizerPtr->computeNormalization( mySamplerPtr );

  setNormalization( _samp );

  _background->normalize( _normalizerPtr, _samp->getEnvironment()->numCategoricalLayers() );
  
  _num_layers = _samp->numIndependent();

  // Identify categorical layers
  _is_categorical.resize( _num_layers );
  
  for ( int i = 0; i < _num_layers; ++i ) {

    _is_categorical[i] = (Scalar)_samp->isCategorical( i );

    if ( _is_categorical[i] ) {

      // Get possible values of each categorical layer based on presence and absence/background points

      std::set<Scalar> values;

      OccurrencesImpl::const_iterator p_iterator = _presences->begin();
      OccurrencesImpl::const_iterator p_end = _presences->end();

      while ( p_iterator != p_end ) {

        Sample sample = (*p_iterator)->environment();

        values.insert( sample[i] ); // std::set already avoids duplicate values

        ++p_iterator;
      }

      p_iterator = _background->begin();
      p_end = _background->end();

      while ( p_iterator != p_end ) {

        Sample sample = (*p_iterator)->environment();

        values.insert( sample[i] );

        ++p_iterator;
      }

      _categorical_values.insert( std::pair< int, std::set<Scalar> >( i, values ) );
    }
  }

  _num_samples = _num_presences + _num_background;

  return 1;
} // initialize

/***************/
/*** iterate ***/

int
MaximumEntropy::iterate()
{
  /*
  OccurrencesImpl::const_iterator p_iterator = _presences->begin();
  OccurrencesImpl::const_iterator p_end = _presences->end();

  while ( p_iterator != p_end ) {

    Sample sample = (*p_iterator)->environment();

    int id = 0;

    for ( int i = 0; i < _num_layers; ++i ) {

      stringstream out;
      out << id;

      if ( _is_categorical[i] ) {

        std::set<Scalar>::iterator it = _categorical_values[i].begin();
        std::set<Scalar>::iterator it_end = _categorical_values[i].end();

        while ( it != it_end ) {

          if ( sample[i] == *it ) {

          }
          else {

          }

          out << ++id;
          ++it;
	}
      }
    }

    ++p_iterator;
  }

  p_iterator = _background->begin();
  p_end = _background->end();

  while ( p_iterator != p_end ) {

    Sample sample = (*p_iterator)->environment();

    int id = 0;

    for ( int i = 0; i < _num_layers; ++i ) {

      stringstream out;
      out << id;

      if ( _is_categorical[i] ) {

        std::set<Scalar>::iterator it = _categorical_values[i].begin();
        std::set<Scalar>::iterator it_end = _categorical_values[i].end();

        while ( it != it_end ) {

          if ( sample[i] == *it ) {

          }
          else {

          }

          out << ++id;

          ++it;
	}
      }
    }
    
    ++p_iterator;
  }
  */    
  train( _num_iterations, _tolerance );

  _done = true;
  
  return 1;
}

/********************/
/*** init_trainer ***/

void
MaximumEntropy::init_trainer()
{
  regularization_parameters = new double[_num_layers];

  features_mean = new double[_num_layers]; // mean of each feature

  feat_stan_devi = new double[_num_layers]; // standard deviation of each feature

  q_lambda_f = new double[_num_layers];

  lambda = new double[_num_layers]; // weight of each feature

  q_lambda_x = new double[_num_samples]; // probability of each point
  
  for ( int i = 0; i < _num_layers; ++i ) {

    regularization_parameters[i] = 0.0;
    features_mean[i] = 0.0;
    feat_stan_devi[i] = 0.0;
    q_lambda_f[i] = 0.0;
    lambda[i] = 1.0;
  }

  for ( int i = 0; i < _num_samples; ++i ) {

    q_lambda_x[i] = 0.0;
  }
    
  // calculate observed feature expectations - pi~[f] (empirical average of f)
  OccurrencesImpl::const_iterator p_iterator = _presences->begin();
  OccurrencesImpl::const_iterator p_end = _presences->end();

  // sum the feature values to calculate the mean.  
  while ( p_iterator != p_end ) {
    
    Sample sample = (*p_iterator)->environment();

    for ( int i = 0; i < _num_layers; ++i ) {

      features_mean[i] += sample[i];
    }
    ++p_iterator;
  }

  p_iterator = _background->begin();
  p_end = _background->end();
  
  while ( p_iterator != p_end ) {
    
    Sample sample = (*p_iterator)->environment();

    for ( int i = 0; i < _num_layers; ++i ) {

      features_mean[i] += sample[i];
    }
    ++p_iterator;
  }
  
  // calculate the features mean
  for ( int i = 0; i < _num_layers; ++i ) {

    features_mean[i] /= _num_samples;
  }

  // calculate the variance of each feature
  p_iterator = _presences->begin();
  p_end = _presences->end();

  while ( p_iterator != p_end ) {
    
    Sample sample = (*p_iterator)->environment();
    
    for ( int i = 0; i < _num_layers; ++i ) {
      
      feat_stan_devi[i] += pow((sample[i] - features_mean[i]),2);
    }
    ++p_iterator;
  }
  
  p_iterator = _background->begin();
  p_end = _background->end();

  while ( p_iterator != p_end ) {
    
    Sample sample = (*p_iterator)->environment();
    
    for ( int i = 0; i < _num_layers; ++i ) {
      
      feat_stan_devi[i] += pow((sample[i] - features_mean[i]),2);
    }
    ++p_iterator;
  }

  for ( int i = 0; i < _num_layers; ++i ) {

    feat_stan_devi[i] /= ( _num_samples - 1 );
  }

  // initialize the regularization parameters
  for ( int i = 0; i < _num_layers; ++i ) {

    regularization_parameters[i] = 1.0 / sqrt((double)(_num_samples)) * sqrt(min(0.25,feat_stan_devi[i]));

    if ( regularization_parameters[i] < 0.00001 ) {

      regularization_parameters[i] = 0.00001;
    }
  }
} // init_trainer();

/***********************/
/*** calc_q_lambda_x ***/

void
MaximumEntropy::calc_q_lambda_x()
{
  Z_lambda = 0.0;
  
  OccurrencesImpl::const_iterator p_iterator = _presences->begin();
  OccurrencesImpl::const_iterator p_end = _presences->end();

  int i = 0;
  while ( p_iterator != p_end ) {
    
    Sample sample = (*p_iterator)->environment();

    double sum_lambdaj_fj = 0.0;

    for ( int j = 0; j < _num_layers; ++j ) {

      sum_lambdaj_fj += lambda[j] * sample[j];
    }
    
    q_lambda_x[i] = exp(sum_lambdaj_fj);
    Z_lambda += q_lambda_x[i]; // normalization constant

    ++i;
    ++p_iterator;
  }
  
  p_iterator = _background->begin();
  p_end = _background->end();
  
  while ( p_iterator != p_end ) {
    
    Sample sample = (*p_iterator)->environment();

    double sum_lambdaj_fj = 0.0;

    for ( int j = 0; j < _num_layers; ++j ) {

      sum_lambdaj_fj += lambda[j] * sample[j];
    }
    
    q_lambda_x[i] = exp(sum_lambdaj_fj);
    Z_lambda += q_lambda_x[i]; // normalization constant

    ++i;
    ++p_iterator;
  }
  
  // normalize all q_lambda_x
  for ( int j = 0; j < _num_samples; ++j ) {
    
    q_lambda_x[j] /= Z_lambda;
  }
}

/***********************/
/*** calc_q_lambda_f ***/

void
MaximumEntropy::calc_q_lambda_f()
{
  for ( int i = 0; i < _num_layers; ++i ) {

    q_lambda_f[i] = 0.0;
  }

  OccurrencesImpl::const_iterator p_iterator = _presences->begin();
  OccurrencesImpl::const_iterator p_end = _presences->end();

  int i = 0;

  while ( p_iterator != p_end ) {
    
    Sample sample = (*p_iterator)->environment();

    for ( int j = 0; j < _num_layers; ++j ) {

      q_lambda_f[j] += q_lambda_x[i] * sample[j];
    }
    ++i;
    ++p_iterator;
  }
  
  p_iterator = _background->begin();
  p_end = _background->end();
  
  while ( p_iterator != p_end ) {
    
    Sample sample = (*p_iterator)->environment();

    for ( int j = 0; j < _num_layers; ++j ) {

      q_lambda_f[j] += q_lambda_x[i] * sample[j];
    }
    ++i;
    ++p_iterator;
  }
}

/*************/
/*** train ***/

void
MaximumEntropy::train( size_t iter, double tol )
{
  init_trainer();

  double loss = std::numeric_limits<double>::infinity();
  double new_loss = 0.0;

  for ( size_t niter = 0; niter < iter; ++niter ) {
    
    double *F;
    double *alfa;
    double *alfa_pos_neg;
    double min_F = std::numeric_limits<double>::infinity();
    double sum_mean_lambda = 0.0;
    double sum_regu_lambda = 0.0;
    double *midpoint;
    int *sign_pos;
    int *sign_neg;
    int *signs;
    int neg;
    int pos;
    int best_id = -1;

    calc_q_lambda_x();
    
    for ( int i = 0; i < _num_layers; ++i ) {

      sum_mean_lambda += -features_mean[i] * lambda[i];
      sum_regu_lambda += regularization_parameters[i] * fabs(lambda[i]);
    }

    new_loss = sum_mean_lambda + log(Z_lambda) + sum_regu_lambda;

    if ( (loss - new_loss) < tol ) {
      break;
    }

    loss = new_loss;

    calc_q_lambda_f();
    
    midpoint = new double[_num_layers];
    
    for ( int i = 0; i < _num_layers; ++i ) {

      midpoint[i] = -features_mean[i] + q_lambda_f[i] / ( exp(lambda[i]) + ( 1 - exp(lambda[i]) ) * q_lambda_f[i]);
    }
    
    sign_pos = new int[_num_layers];
    sign_neg = new int[_num_layers];

    for ( int i = 0; i < _num_layers; ++i ) {

      if ( (midpoint[i] + regularization_parameters[i]) < 0.0 ) {
	sign_pos[i] = -1;
      }
      else {
	if ( (midpoint[i] + regularization_parameters[i]) > 0.0 ) {
	  sign_pos[i] = 1;
	}
	else{
	  sign_pos[i] = 0;
	}
      }

      if ( (midpoint[i] - regularization_parameters[i]) < 0.0 ) {
	sign_neg[i] = -1;
      }
      else {
	if ( (midpoint[i] - regularization_parameters[i]) > 0.0 ) {
	  sign_neg[i] = 1;
	}
	else {
	  sign_neg[i] = 0;
	}
      }
    }
    
    signs = new int[_num_layers];

    for ( int i = 0; i < _num_layers; ++i ) {
      
      if ( sign_neg[i] > 0 ) neg = 1;
      else neg = 0;

      if ( sign_pos[i] < 0 ) pos = 1;
      else pos = 0;

      signs[i] = neg - pos;
    }
    
    F = new double[_num_layers];
    alfa = new double[_num_layers];
    alfa_pos_neg = new double[_num_layers];

    for ( int i = 0; i < _num_layers; ++i ) {

      alfa_pos_neg[i] = log((features_mean[i]+signs[i]*regularization_parameters[i])*(1-q_lambda_f[i])
			    /((1-features_mean[i]-signs[i]*regularization_parameters[i])*q_lambda_f[i]));
    }

    for ( int i = 0; i < _num_layers; ++i ) {

      alfa[i] = -lambda[i];
    }

    for ( int i = 0; i < _num_layers; ++i ) {

      if ( signs[i] != 0 ) {
	alfa[i] = alfa_pos_neg[i];
      }
    }

    for ( int i = 0; i < _num_layers; ++i ) {

      F[i] = -alfa[i] * features_mean[i] + log(1 + (exp(alfa[i]) - 1) * q_lambda_f[i])
	     + regularization_parameters[i] * (fabs(lambda[i] + alfa[i]) - fabs(lambda[i]));

      if ( min_F > F[i] ) {
	min_F = F[i];
	best_id = i;
      }
    }

    lambda[best_id] += min_F;

  } // for ( size_t niter = 0; niter < iter; ++niter )
} // train

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
  double prob = 0.0 ;
  
  //int id = 0;

  for ( int i = 0; i < _num_layers; ++i ) {
    /*
    stringstream out;
    out << id;

    if ( _is_categorical[i] ) {

      std::map< int, std::set<Scalar> >::const_iterator layer_categories = _categorical_values.find(i);

      if ( layer_categories != _categorical_values.end() ) {

        std::set<Scalar>::const_iterator it = layer_categories->second.begin();
        std::set<Scalar>::const_iterator it_end = layer_categories->second.end();

        while ( it != it_end ) {

          if ( x[i] == *it ) {

          }
          else {

          }

          out << ++id;
          ++it;
        }
      }
    }
    else {
    */
    prob += lambda[i] * x[i];
    //}
  }

  prob = exp(prob);
  prob = exp(prob)/Z_lambda;

  if ( !finite(prob) ) {

    prob = std::numeric_limits<double>::max(); // DBL_MAX;
  }
  return prob;
}

/***********************/
/*** get Convergence ***/
int
MaximumEntropy::getConvergence( Scalar * const val )
{
  *val = 1.0;
  return 1;
}

/****************************************************************/
/****************** configuration *******************************/

void
MaximumEntropy::_getConfiguration( ConfigurationPtr& config ) const
{
  //avoid serialization when something went wrong
  if ( ! _done ) {

    return;
  }

  //These two lines create a new XML element called "MaximumEntropy"
  ConfigurationPtr model_config( new ConfigurationImpl( "MaximumEntropy" ) );

  config->addSubsection( model_config );

  model_config->addNameValue( "NumLayers", _num_layers );
  model_config->addNameValue( "Z", Z_lambda );
  /*
  model_config->addNameValue( "Categorical", _is_categorical );

  ConfigurationPtr cat_section_config( new ConfigurationImpl( "CategoricalData" ) );
  model_config->addSubsection( cat_section_config );

  std::map< int, std::set<Scalar> >::const_iterator it = _categorical_values.begin();
  std::map< int, std::set<Scalar> >::const_iterator it_end = _categorical_values.end();

  while ( it != it_end ) {

    // There can be be multiple <Categories> elements, one for each categorical layer
    ConfigurationPtr cat_config( new ConfigurationImpl( "Categories" ) );
    cat_section_config->addSubsection( cat_config );

    int layer_index = (*it).first;
    std::set<Scalar> layer_categories = (*it).second;

    // Transform std::set into Sample
    Sample categories;
    categories.resize( layer_categories.size() );

    int i = 0;

    for ( std::set<Scalar>::iterator val_it = layer_categories.begin(); val_it != layer_categories.end(); val_it++ ) {

      categories[i] = (*val_it);
      ++i;
    }

    cat_config->addNameValue( "Index", layer_index );
    cat_config->addNameValue( "Values", categories );

    ++it;
  }
  */
  // write lambda
  ConfigurationPtr lambda_config( new ConfigurationImpl( "Lambda" ) );
  model_config->addSubsection( lambda_config );

  double *lambda_values = new double[_num_layers];
  
  for ( int i = 0; i < _num_layers; ++i ) {

    lambda_values[i] = lambda[i];
  }

  lambda_config->addNameValue( "Values", lambda_values, _num_layers );
}

void
MaximumEntropy::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "MaximumEntropy", false );

  if ( ! model_config ) {
    return;
  }
  
  _num_layers = model_config->getAttributeAsInt( "NumLayers", 0 );

  Z_lambda = model_config->getAttributeAsDouble( "Z", 0.0 );
  
  /*
  _is_categorical = model_config->getAttributeAsSample( "Categorical" );

  ConstConfigurationPtr cat_section = model_config->getSubsection( "CategoricalData", false );

  if ( cat_section ) {

    Configuration::subsection_list categories = cat_section->getAllSubsections();

    Configuration::subsection_list::iterator it = categories.begin();
    Configuration::subsection_list::iterator it_end = categories.end();

    for ( ; it != it_end; ++it ) {

      int layer_index = (*it)->getAttributeAsInt( "Index", 0 );

      Sample layer_categories = (*it)->getAttributeAsSample( "Values" );

      // Convert from Sample to std::set
      std::set<Scalar> values;

      for ( unsigned int i = 0; i < layer_categories.size(); ++i ) {

        values.insert( layer_categories[i] );
      }

      _categorical_values.insert( std::pair< int, std::set<Scalar> >( layer_index, values ) );
    }
  }
  */
  
  // Lambda
  ConstConfigurationPtr lambda_config = model_config->getSubsection( "Lambda", false );

  if ( ! lambda_config ) {
    return;
  }

  std::vector<double> lambda_values = lambda_config->getAttributeAsVecDouble( "Values" );

  lambda = new double[_num_layers];

  for ( int i = 0; i < _num_layers; ++i ) {

    lambda[i] = lambda_values[i];
  }
  
  _done = true;
}
