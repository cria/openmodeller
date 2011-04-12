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
#include <map>
#include <set>
#include <algorithm>

#include <limits>
#include <math.h>


using namespace std;

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 6

#define BACKGROUND_ID      "NumberOfBackgroundPoints"
#define USE_ABSENCES_ID    "UseAbsencesAsBackground"
#define MERGE_POINTS_ID    "IncludePresencePointsInBackground"
#define ITERATIONS_ID      "NumberOfIterations"
#define TOLERANCE_ID       "TerminateTolerance"
#define OUTPUT_ID          "OutputFormat"

#define MAXENT_LOG_PREFIX "Maxent: "

#define MINVAL 0.0
#define MAXVAL 1.0
#define MINLIMIT 1.0e-6

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
  // Indication if presence points should be merged with background points
  {
    MERGE_POINTS_ID,               // Id.
    "Include input points in the background", // Name.
    Integer,                       // Type.
    "Include input points in the background: 0=No, 1=Yes.", // Overview
    "Include input points in the background: 0=No, 1=Yes.", // Description.
    1,   // Not zero if the parameter has lower limit.
    0,   // Parameter's lower limit.
    1,   // Not zero if the parameter has upper limit.
    1,   // Parameter's upper limit.
    "1"  // Parameter's typical (default) value.
  },
  // Number of iterations
  {
    ITERATIONS_ID,          // Id.
    "Number of iterations", // Name.
    Integer,                // Type.
    "Number of iterations.", // Overview
    "Number of iterations.", // Description.
    1,    // Not zero if the parameter has lower limit.
    1,    // Parameter's lower limit.
    0,    // Not zero if the parameter has upper limit.
    0,    // Parameter's upper limit.
    "500" // Parameter's typical (default) value.
  },
  // Terminate tolerance
  {
    TOLERANCE_ID, // Id.
    "Terminate tolerance", // Name.
    Real, // Type.
    "Tolerance for detecting model convergence.", // Overview
    "Tolerance for detecting model convergence.", // Description.
    1,        // Not zero if the parameter has lower limit.
    0.0,      // Parameter's lower limit.
    0,        // Not zero if the parameter has upper limit.
    0,        // Parameter's upper limit.
    "0.00001" // Parameter's typical (default) value.
  },
  // Output Format
  {
    OUTPUT_ID, // Id.
    "Output Format", // Name
    Integer, // Type
    "Output Format: " // Overview
    "1 = Raw, "
    "2 = Logistic. ",
    "Output Format: " // Description
    "1 = Raw, "
    "2 = Logistic. ",
    1,  // Not zero if the parameter has lower limit
    1,  // Parameter's lower limit
    1,  // Not zero if the parameter has upper limit
    2,  // Parameter's upper limit
    "2" // Parameter's typical (default) value
  },
};

/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {
  
  "MAXENT",          // Id.
  "Maximum Entropy", // Name.
  "0.6",       	     // Version.

  // Overview.
  "The principle of maximum entropy is a method for analyzing available qualitative information in order to determine a unique epistemic probability distribution. It states that the least biased distribution that encodes certain given information is that which maximizes the information entropy (content retrieved from Wikipedia on the 19th of May, 2008: http://en.wikipedia.org/wiki/Maximum_entropy).",

  // Description.
  "The principle of maximum entropy is a method for analyzing available qualitative information in order to determine a unique epistemic probability distribution. It states that the least biased distribution that encodes certain given information is that which maximizes the information entropy (content retrieved from Wikipedia on the 19th of May, 2008: http://en.wikipedia.org/wiki/Maximum_entropy). E.T. Jaynes introduced the maximum entropy principle in 1957 saying that 'Information theory provides a constructive criterion for setting up probability distributions on the basis of partial knowledge, and leads to a type of statistical inference which is called the maximum entropy estimate. It is the least biased estimate possible on the given information; i.e., it is maximally noncommittal with regard to missing information'.",

  "", // Algorithm author.

  "1) Jaynes, E.T. (1957) Information Theory and Statistical Mechanics. In Physical Review, Vol. 106, #4 (pp 620-630). 2) Berger, A. L., Pietra, S. A. D. and Pietra, V. J. D. (1996). A maximum entropy approach to natural language processing. Computational Linguistics, 22, 39-71. 3) Darroch, J.N. and Ratcliff, D. (1972) Generalized iterative scaling for log-linear models. The Annals of Mathematical Statistics, Vol. 43: pp 1470-1480. 4) Malouf, R. (2003) A comparison of algorithms for maximum entropy parameter estimation. Proceedings of the Sixth Conference on Natural Language Learning. 5) Phillips, S.J., Dudík, M. and Schapire, R.E. (2004) A maximum entropy approach to species distribution modeling. Proceedings of the Twenty-First International Conference on Machine Learning, pp 655-662.", // Bibliography.

  "Elisangela S. da C. Rodrigues, Renato De Giovanni", // Code author.

  "elisangela.rodrigues [at] poli . usp . br, renato [at] cria . org . br", // Code author's contact.

  0, // Does not accept categorical data.
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
  _num_layers(0),
  _iteration(0)
{ 
  // TODO: Switch back to use layers as ref to avoid problems in native projections(?)
  //       Otherwise we should make sure that environmental values won't extrapolate 
  //       min/max when calculating suitabilities.
  bool use_layers_as_reference = false; // original Maxent always use background as ref
  _normalizerPtr = new ScaleNormalizer( MINVAL, MAXVAL, use_layers_as_reference );
}

/******************/
/*** destructor ***/

MaximumEntropy::~MaximumEntropy()
{
  delete[] _f_lambda;
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
  if ( ! getParameter( ITERATIONS_ID, &_max_iterations ) ) {

    Log::instance()->warn( MAXENT_LOG_PREFIX "Parameter '" ITERATIONS_ID "' not passed. Using default value (500)\n" );
    _max_iterations = 500;
  }
  else {

    if ( _max_iterations <= 0 ) {
      
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

  bool use_absences_as_background = false;
  int use_abs;
  if ( getParameter( USE_ABSENCES_ID, &use_abs ) && use_abs == 1 ) {

    use_absences_as_background = true;
  }

  if ( use_absences_as_background ) {

    _num_background = _samp->numAbsence();

    int param_background;

    if ( _num_background && getParameter( BACKGROUND_ID, &param_background ) && param_background > _num_background ) {

    Log::instance()->warn( MAXENT_LOG_PREFIX "Number of absence points (%d) is less than the specified number of background points (%d). Using the first.\n", _num_background, param_background);
    }
  }
  else {

    if ( ! getParameter( BACKGROUND_ID, &_num_background ) ) {

      Log::instance()->warn( MAXENT_LOG_PREFIX "Parameter '" BACKGROUND_ID "' unspecified. Using default value (10000).\n");

      _num_background = 10000;
    }
  }

  if ( _num_background <= 0 ) {
	
    Log::instance()->warn( MAXENT_LOG_PREFIX "Parameter '" BACKGROUND_ID "' must be greater than zero.\n" );
    return 0;
  }

  bool merge_points = true; // default
  int merge_param;
  if ( getParameter( MERGE_POINTS_ID, &merge_param ) && !merge_param ) {

    merge_points = false;
  }
    
  _background = new OccurrencesImpl( _presences->name(), _presences->coordSystem() );

  if ( use_absences_as_background ) {

    _background->appendFrom( _samp->getAbsences() );
  }
  else {

    // Generate background points

    Log::instance()->info( MAXENT_LOG_PREFIX "Generating random background points.\n" );

    for ( int i = 0; i < _num_background; ++i ) {

      OccurrencePtr oc = _samp->getPseudoAbsence();
      _background->insert( oc ); 
    }
  }

  if ( merge_points ) {

    _num_background += _num_presences;

    // Mixing points with different abundance values (0 or 1) won't hurt
    _background->appendFrom( _presences );

    // Using a mixed occurrence object shouldn't affect the normalization procedure
    SamplerPtr mySamplerPtr = createSampler( _samp->getEnvironment(), _background );

    //mySamplerPtr->spatiallyUnique();

    _normalizerPtr->computeNormalization( mySamplerPtr );
  }
  else {

    // Compute normalization with all points
    SamplerPtr mySamplerPtr = createSampler( _samp->getEnvironment(), _presences, _background );

    //mySamplerPtr->spatiallyUnique();

    _normalizerPtr->computeNormalization( mySamplerPtr );
  }

  setNormalization( _samp );

  _background->normalize( _normalizerPtr, _samp->getEnvironment()->numCategoricalLayers() );
  
  _num_layers = _samp->numIndependent();

  // Output Format
  if ( ! getParameter( OUTPUT_ID, &_output_format ) ) {

    Log::instance()->warn( MAXENT_LOG_PREFIX "Parameter '" OUTPUT_ID "' not passed. Using default value (Logistic)\n" );
    _output_format = 2;
  }
  else {

    if ( _output_format != 1 && _output_format != 2 ) {

      Log::instance()->error( MAXENT_LOG_PREFIX "Parameter '" OUTPUT_ID "' must be 1 or 2\n" );
      return 0;
    }
  }

  _min.resize( _num_layers );
  _max.resize( _num_layers );
  _background->getMinMax( &_min, &_max );

  initTrainer();
  
  return 1;
}

/***************/
/*** iterate ***/

int
MaximumEntropy::iterate()
{
  if ( _iteration == _max_iterations ) {

    endTrainer();
    return 1;
  }

  // Determine best feature
  int best_id = -1;
  double best_dlb = 1.0;
  double infinity = std::numeric_limits<double>::infinity();
  double alpha = 0.0;

  for ( int i = 0; i < _len; i++ ) {

    double dlb = 0.0;

    // Calculate delta loss bound
    double w1 = _f_exp[i];
    double w0 = 1.0 - w1;
    double n1 = _f_samp_exp[i];
    double beta1 = _f_samp_dev[i];
    double lambda = _f_lambda[i];

    if ( n1 != -1.0 ) {

      // Determine alpha
      alpha = getAlpha(i);

      if ( alpha < infinity ) {

        dlb = -n1 * alpha + log( w0 + w1 * exp(alpha) ) + beta1 * ( fabs(lambda + alpha) - fabs(lambda) );

        if ( isnan( dlb ) ) {

          dlb = 0.0;
        }
      }
    }

    if ( dlb >= best_dlb ) {

      continue;
    }

    best_id = i;
    best_dlb = dlb;
  }

  if ( best_id == -1 ) {

    endTrainer();
    return 1;
  }

  // Get loss
  alpha = runNewtonStep( best_id );
  alpha = decreaseAlpha( alpha );
  _new_loss = increaseLambda( best_id, alpha );

  double delta_loss = _new_loss - _old_loss;

  if ( delta_loss > best_dlb) {

    Log::instance()->debug("Undoing: newLoss %f, oldLoss %f, deltaLossBound %f\n", _new_loss, _old_loss, best_dlb);
    increaseLambda( best_id, -alpha );
    alpha = searchAlpha( best_id, getAlpha( best_id ) );
    alpha = decreaseAlpha( alpha );
    _new_loss = increaseLambda( best_id, alpha );
  }

  // TODO: calculate feature contribution!

  delta_loss = _new_loss - _old_loss;
 
  _old_loss = _new_loss;

  _gain = log((double)_num_background) - _old_loss;

  displayInfo(best_id, best_dlb, _new_loss, delta_loss, alpha);

  if ( terminationTest(_new_loss) ) {

    endTrainer();
    return 1;
  }
  
  ++_iteration;

  return 1;
}

/********************/
/*** init Trainer ***/

void
MaximumEntropy::initTrainer()
{
  _convergence_test_frequency = 20;
  _previous_loss = std::numeric_limits<double>::infinity();

  _len = _num_layers;

  _f_mean = new double[_len];
  _f_std = new double[_len];
  _f_lambda = new double[_len];
  
  _linear_pred = new double[_num_background];

  _f_samp_exp = new double[_len];
  _f_samp_dev = new double[_len];

  _f_exp = new double[_len];

  for ( int i = 0; i < _len; ++i ) {

    _f_mean[i] = 0.0;
    _f_std[i] = 0.0;
    _f_samp_exp[i] = 0.0;
    _f_samp_dev[i] = 0.0;
    _f_lambda[i] = 0.0;
  }

  setLinearPred();

  _beta_l = interpol ( 'l' );

  Log::instance()->debug( "Regularization value = %f \n", _beta_l );

  // calculate observed feature expectations - pi~[f] (empirical average of f)
  OccurrencesImpl::const_iterator p_iterator = _presences->begin();
  OccurrencesImpl::const_iterator p_end = _presences->end();
  int j = 0;

  // sum the feature values to calculate the mean.  
  while ( p_iterator != p_end ) {

    Sample sample = (*p_iterator)->environment();

    for ( int i = 0; i < _num_layers; ++i ) {

      _f_mean[i] += sample[i];
      _f_std[i] += pow(sample[i], 2);
    }
    ++p_iterator;
    ++j;
  }

  double *f_lower = new double[_len];
  double *f_upper = new double[_len];

  // calculate the mean, standard deviation and expected values for each feature
  for ( int i = 0; i < _len; ++i ) {

    if ( _num_presences == 1 ) {

      _f_std[i] = (MAXVAL - MINVAL)*0.5;
    }
    else {

      _f_mean[i] /= (double)_num_presences;

      if ( _f_std[i] < (double)_num_presences * pow(_f_mean[i], 2) ) {

        _f_std[i] = MINVAL;
      }
      else {

        _f_std[i] = sqrt((_f_std[i] - (double)_num_presences * pow(_f_mean[i], 2)) / (double)(_num_presences - 1));
      }

      if ( _f_std[i] > (MAXVAL - MINVAL)*0.5 ) {

        _f_std[i] = (MAXVAL - MINVAL)*0.5;
      }
    }

    f_lower[i] = _f_mean[i] - (_beta_l / sqrt((double)_num_presences)) * _f_std[i];
    f_upper[i] = _f_mean[i] + (_beta_l / sqrt((double)_num_presences)) * _f_std[i];

    if ( f_lower[i] < MINVAL ) {

      f_lower[i] = MINVAL;
    }
    if ( f_upper[i] > MAXVAL ) {

      f_upper[i] = MAXVAL;
    }

    _f_samp_exp[i] = 0.5*(f_upper[i]+f_lower[i]);
    _f_samp_dev[i] = 0.5*(f_upper[i]-f_lower[i]);

    if ( _f_samp_dev[i] < MINLIMIT ) {

      // TODO: Change here when including binary features in the future
      _f_samp_dev[i] = MINLIMIT;
    }
  }

  delete[] f_lower;
  delete[] f_upper;

  _density = new double[_num_background];

  calcDensity();

  // Initialize reg
  for ( int j = 0; j < _len; j++ ) {

    _reg += fabs( _f_lambda[j] ) * _f_samp_dev[j];
  }

  _new_loss = getLoss();
  Log::instance()->debug( "Initial loss = %f \n", _new_loss );

  _old_loss = _new_loss;

  _gain = 0.0;
}


/*******************/
/*** end Trainer ***/

void
MaximumEntropy::endTrainer()
{
  // Calculate entropy
  _entropy = 0.0;

  for ( int j = 0; j < _num_background; ++j ) {
    
    double dd = _density[j] /= _z_lambda;

    if ( dd > 0.0 ) {

      _entropy += -dd * log(dd);
    }
  }

  Log::instance()->info( MAXENT_LOG_PREFIX "Gain\t %f \n", _gain );
  Log::instance()->info( MAXENT_LOG_PREFIX "Entropy\t %.2f \n", _entropy );

  delete[] _f_mean;
  delete[] _f_std;
  delete[] _f_samp_exp;
  delete[] _f_samp_dev;
  delete[] _density;
  delete[] _linear_pred;
  delete[] _f_exp;

  _done = true;
}

/***********************/
/****** interpol *******/

double
MaximumEntropy::interpol( char type_feat )
{
  const int arraySize = 3;
  int ts_l[arraySize] = { 10, 30, 100 }; //linear features thresholds 
  double betas_l[arraySize] = { 1.0, 0.2, 0.05 };
  int ts_c[arraySize] = { 0, 10, 17 }; //categorical features thresholds
  double betas_c[arraySize] = { 0.65, 0.5, 0.25 };

  if ( type_feat == 'l' ) {
    int i = 0;
    for ( ; i < arraySize; ++i) {
      if ( _num_presences <= ts_l[i] ) {
	break;
      }
    }
    if ( i == 0 ) {
      return betas_l[0];
    }
    if ( i == arraySize ) {
      return betas_l[arraySize-1];
    }
    return betas_l[(i-1)] + (betas_l[i] - betas_l[(i-1)]) * (_num_presences - ts_l[(i-1)]) / (ts_l[i] - ts_l[(i-1)]);
  }
  
  else {
    int i = 0;
    for ( ; i < arraySize; ++i) {
      if ( _num_presences <= ts_c[i] ) {
	break;
      }
    }
    if ( i == 0 ) {
      return betas_c[0];
    }
    if ( i == arraySize ) {
      return betas_c[arraySize-1];
    }
    return betas_c[(i-1)] + (betas_c[i] - betas_c[(i-1)]) * (_num_presences - ts_c[(i-1)]) / (ts_c[i] - ts_c[(i-1)]);
  }
}

/***********************/
/*** set Linear Pred ***/

void
MaximumEntropy::setLinearPred()
{
  for ( int i = 0; i < _num_background; ++i ) {

    _linear_pred[i] = 0.0;
  }
  
  OccurrencesImpl::const_iterator p_iterator = _background->begin();
  OccurrencesImpl::const_iterator p_end = _background->end();
  int i = 0;

  while ( p_iterator != p_end ) {
    
    Sample sample = (*p_iterator)->environment();

    for ( int j = 0; j < _len; ++j ) {

      if ( _f_lambda[j] != 0.0 ) {

        _linear_pred[i] += _f_lambda[j] * sample[j];
      }
    }
    ++i;
    ++p_iterator;
  }

  setLinearNormalizer();
}

/*****************************/
/*** set Linear Normalizer ***/

void
MaximumEntropy::setLinearNormalizer()
{
  _linear_normalizer = _linear_pred[0];

  for ( int i = 1; i < _num_background; ++i ) {

    if ( _linear_pred[i] > _linear_normalizer ) {

      _linear_normalizer = _linear_pred[i]; 
    }
  }
}

/********************/
/*** calc Density ***/

void
MaximumEntropy::calcDensity()
{
  _z_lambda = 0.0;
  
  double *aux = new double[_len];

  OccurrencesImpl::const_iterator p_iterator = _background->begin();
  OccurrencesImpl::const_iterator p_end = _background->end();
  int i = 0;

  while ( p_iterator != p_end ) {
    
    _density[i] = exp( _linear_pred[i] - _linear_normalizer );

    _z_lambda += _density[i];

    Sample sample = (*p_iterator)->environment();

    for ( int j = 0; j < _len; ++j ) {

      aux[j] += _density[i] * sample[j];
    }

    ++i;
    ++p_iterator;
  }

  for ( int j = 0; j < _len; ++j ) {

    _f_exp[j] = aux[j] / _z_lambda;
  }

  delete[] aux;
}

/*****************/
/*** get Alpha ***/

double
MaximumEntropy::getAlpha( int feature_index )
{
  double alpha = 0.0;

  double w1 = _f_exp[feature_index];
  double w0 = 1.0 - w1;
  double n1 = _f_samp_exp[feature_index];
  double n0 = 1.0 - n1;
  double beta1 = _f_samp_dev[feature_index];
  double lambda = _f_lambda[feature_index];

  if ( ( w0 >= 1.0e-6 ) && ( w1 >= 1.0e-6 ) ) {

    if ( n1 - beta1 > MINLIMIT ) {

      alpha = log( (n1 - beta1) * w0 / ((n0 + beta1) * w1) );

      if ( alpha + lambda <= 0.0 ) {

        if ( n0 - beta1 > MINLIMIT ) {

          alpha = log( (n1 + beta1) * w0 / ( (n0 - beta1) * w1) );

          if ( alpha + lambda >= 0.0 ) {

            alpha = -lambda;
          }
        }
      }
    }
    else {

      if ( n0 - beta1 > MINLIMIT ) {

        alpha = log( (n1 + beta1) * w0 / ( (n0 - beta1) * w1) );

        if ( alpha + lambda >= 0.0 ) {

          alpha = -lambda;
        }
      }
      else {

        alpha = -lambda;
      }
    }
  }

  return alpha;
}

/********************/
/*** search Alpha ***/

double 
MaximumEntropy::searchAlpha( int feature_index, double alpha )
{
  double ini_loss = lossChange( feature_index, alpha );
  double current_loss = ini_loss;
  double tentative_loss;

  while ( true ) {

    tentative_loss = lossChange( feature_index, alpha * 4.0 );

    if ( !finite(tentative_loss) || tentative_loss >= current_loss ) {

      break;
    }

    current_loss = tentative_loss;
    alpha *= 4.0;
  }

  tentative_loss = lossChange( feature_index, alpha * 2.0 );

  if ( finite(tentative_loss) && tentative_loss < current_loss ) {

    current_loss = tentative_loss;
    alpha *= 2.0;
  }

  return alpha;
}

/*******************/
/*** loss Change ***/

double 
MaximumEntropy::lossChange( int feature_index, double alpha )
{
  double lambda = _f_lambda[feature_index];
  double n1 = _f_samp_exp[feature_index];
  double beta1 = _f_samp_dev[feature_index];

  OccurrencesImpl::const_iterator p_iterator = _background->begin();
  OccurrencesImpl::const_iterator p_end = _background->end();
  int i = 0;
  double zz = 0.0;

  while ( p_iterator != p_end ) {

    Sample sample = (*p_iterator)->environment();

    zz += _density[i] * exp(alpha * sample[feature_index]);

    ++i;
    ++p_iterator;
  }

  double change = -alpha * n1 + log(zz) + (fabs(lambda + alpha) - fabs(lambda)) * beta1;
 
  return change;
}

/***********************/
/*** run Newton Step ***/

double 
MaximumEntropy::runNewtonStep(int feature_index) {

  double w1 = _f_exp[feature_index];
  double wu = 0.0;

  OccurrencesImpl::const_iterator p_iterator = _background->begin();
  OccurrencesImpl::const_iterator p_end = _background->end();
  int i = 0;

  while ( p_iterator != p_end ) {

    Sample sample = (*p_iterator)->environment();

    wu += _density[i] * pow( sample[feature_index], 2 );

    ++i;
    ++p_iterator;
  }

  wu = wu / _z_lambda - pow(w1, 2);

  if ( wu < 1.0e-12 ) return 0.0;
 
  double step = -getDeriv(feature_index) / wu;
  double lambda = _f_lambda[feature_index];

  if ( lambda * (step + lambda) < 0.0 ) {

    step = -lambda;
  }

  return step;
}

/*****************/
/*** get Deriv ***/

double
MaximumEntropy::getDeriv( int feature_index )
{
  double w1 = _f_exp[feature_index];
  double n1 = _f_samp_exp[feature_index];
  double beta1 = _f_samp_dev[feature_index];
  double lambda = _f_lambda[feature_index];
  double deriv = w1 - n1;

  if ( lambda < 0.0 ) {

     return deriv - beta1;
  }

  if ( lambda > 0.0 ) {

     return deriv + beta1;
  }

  if ( deriv + beta1 > 0.0 ) {

     return deriv + beta1;
  }

  if ( deriv - beta1 < 0.0 ) {

    return deriv - beta1;
  }

  return 0.0;
}

/***********************/
/*** increase Lambda ***/

double 
MaximumEntropy::increaseLambda(int feature_index, double alpha)
{
  double beta1 = _f_samp_dev[feature_index];
  double lambda = _f_lambda[feature_index];

  _reg += ( fabs(lambda + alpha) - fabs(lambda) ) * beta1;

  if ( alpha != 0.0 ) {

    _f_lambda[feature_index] += alpha;

    OccurrencesImpl::const_iterator p_iterator = _background->begin();
    OccurrencesImpl::const_iterator p_end = _background->end();
    int i = 0;

    while ( p_iterator != p_end ) {

      Sample sample = (*p_iterator)->environment();

      _linear_pred[i] += alpha * sample[feature_index];

      if ( i == 0 || _linear_pred[i] > _linear_normalizer ) {

        _linear_normalizer = _linear_pred[i];
      }

      ++i;
      ++p_iterator;
    }

    calcDensity();
  }

  return getLoss() + _reg;
}

/********************/
/*** display Info ***/

void
MaximumEntropy::displayInfo(int best_id, double loss_bound, double new_loss, double delta_loss, double alpha)
{
  Log::instance()->debug( "%d: loss = %f \n", _iteration, new_loss );

  Log::instance()->debug( "%s: lambda = %f min = %f max = %f\n", _samp->getEnvironment()->getLayerPath(best_id).c_str(), _f_lambda[best_id], _min[best_id], _max[best_id] );

  Log::instance()->debug( "alpha = %f lossBound = %f W1 = %f N1 = %f deltaLoss = %f \n", alpha, loss_bound, _f_exp[best_id], _f_samp_exp[best_id], delta_loss );

}

/************************/
/*** termination Test ***/

bool
MaximumEntropy::terminationTest(double new_loss)
{
  if ( _iteration == 0 ) {

    _previous_loss = new_loss;
    return false;
  }

  if ( _iteration % _convergence_test_frequency != 0 ) {

    return false;
  }

  if ( _previous_loss - new_loss < _tolerance ) {

    Log::instance()->info( MAXENT_LOG_PREFIX "Algorithm converged after %d iteration(s) \n", _iteration );
    return true;
  }

  _previous_loss = new_loss;

  return false;
}

/**********************/
/*** decrease Alpha ***/

double
MaximumEntropy::decreaseAlpha(double alpha)
{
  if ( _iteration < 10 ) {
    return alpha / 50.0;
  }
  if ( _iteration < 20 ) {
    return alpha / 10.0;
  }
  if ( _iteration < 50 ) {
    return alpha / 3.0;
  }
  return alpha;
}

/****************/
/*** get Loss ***/

double
MaximumEntropy::getLoss()
{
  double sum_mid_lambda = 0.0;

  for ( int i = 0; i < _len; ++i ) {
    
    Log::instance()->debug( "lambda %f exp %f \n", _f_lambda[i], _f_samp_exp[i] );
    sum_mid_lambda += _f_samp_exp[i] * _f_lambda[i];
  }
  
  Log::instance()->debug( "linearPredictorNormalizer %f \n", _linear_normalizer );
  Log::instance()->debug( "densityNormalizer %f \n", log(_z_lambda) );

  return ( log(_z_lambda) - sum_mid_lambda + _linear_normalizer );
}

/********************/
/*** get Progress ***/

float 
MaximumEntropy::getProgress() const
{
  if ( done() ) {

    return 1.0;
  }

  return (float)_iteration / (float)_max_iterations;
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
  double prob = 0.0 ;
  double val;
  
  for ( int i = 0; i < _len; ++i ) {

    // Clamp values if necessary
    val = x[i];
    val = ( val < MINVAL ) ? MINVAL : val; 
    val = ( val > MAXVAL ) ? MAXVAL : val; 

    prob += _f_lambda[i] * val;
  }

  prob = exp(prob - _linear_normalizer)/_z_lambda;

  if ( !finite(prob) ) {

    // Beware that on windows max is also defined as a macro
    // in <windows.h> so it should be enclosed in () in the
    // next line to cause the macro not to be expanded 
    prob = (std::numeric_limits<double>::max)(); // DBL_MAX;
  }
 
  if ( _output_format == 1 ) {

    return prob;
  }
  else {

    return ( ( exp(_entropy) * prob ) / ( 1 + ( exp(_entropy) * prob ) ) );
  }
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
  model_config->addNameValue( "Z", _z_lambda );
  model_config->addNameValue( "LN", _linear_normalizer );
  model_config->addNameValue( "Entropy", _entropy);
  model_config->addNameValue( "OutputFormat", _output_format );

  // write lambda
  ConfigurationPtr lambda_config( new ConfigurationImpl( "Lambda" ) );
  model_config->addSubsection( lambda_config );

  double *lambda_values = new double[_num_layers];
  
  for ( int i = 0; i < _num_layers; ++i ) {

    lambda_values[i] = _f_lambda[i];
  }

  lambda_config->addNameValue( "Values", lambda_values, _num_layers );

  delete[] lambda_values;
}

void
MaximumEntropy::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "MaximumEntropy", false );

  if ( ! model_config ) {
    return;
  }
  
  _num_layers = model_config->getAttributeAsInt( "NumLayers", 0 );

  _len = _num_layers;

  _z_lambda = model_config->getAttributeAsDouble( "Z", 0.0 );

  _linear_normalizer = model_config->getAttributeAsDouble( "LN", 0.0 );

  _entropy = model_config->getAttributeAsDouble( "Entropy", 0.0 );

  _output_format = model_config->getAttributeAsInt( "OutputFormat", 2 );
  
  // Lambda
  ConstConfigurationPtr lambda_config = model_config->getSubsection( "Lambda", false );

  if ( ! lambda_config ) {
    return;
  }

  std::vector<double> lambda_values = lambda_config->getAttributeAsVecDouble( "Values" );

  _f_lambda = new double[_num_layers];

  for ( int i = 0; i < _num_layers; ++i ) {

    _f_lambda[i] = lambda_values[i];
  }
  
  _done = true;
}
