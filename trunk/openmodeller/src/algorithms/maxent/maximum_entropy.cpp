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

#include <limits>

#ifdef MPI_FOUND
#include "mpi.h"
#define WORKTAG 1
#define DIETAG 2
#endif

using namespace std;

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 4

#define BACKGROUND_ID      "NumberOfBackgroundPoints"
#define ITERATIONS_ID      "NumberOfIterations"
#define TOLERANCE_ID       "TerminateTolerance"
#define OUTPUT_ID          "OutputFormat"

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
    1, // Not zero if the parameter has lower limit
    1, // Parameter's lower limit
    1, // Not zero if the parameter has upper limit
    2, // Parameter's upper limit
    "2"         // Parameter's typical (default) value
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

  _num_samples = _num_presences + _num_background;

  // Identify categorical layers
  _num_values_cat = 0;
  _is_categorical.resize( _num_layers );
  _hasCategorical = false;

  for ( int i = 0; i < _num_layers; ++i ) {

    _is_categorical[i] = (Scalar)_samp->isCategorical( i );

    if ( _is_categorical[i] ) {

      _hasCategorical = true;

      // Get possible values of each categorical layer based on presence points
      std::set<Scalar> values;
 
      OccurrencesImpl::const_iterator p_iterator = _presences->begin();
      OccurrencesImpl::const_iterator p_end = _presences->end();

      while ( p_iterator != p_end ) {

        Sample sample = (*p_iterator)->environment();

        values.insert( sample[i] ); // std::set already avoids duplicate values
        ++p_iterator;
      }

      std::map<Scalar, std::vector<bool > > bin_each_value;

      set<Scalar>::const_iterator bin_iterator = values.begin();
      set<Scalar>::const_iterator bin_end = values.end();
      
      while ( bin_iterator != bin_end ) {

	std::vector<bool > bin_values;

	OccurrencesImpl::const_iterator pbin_iterator = _presences->begin();
	OccurrencesImpl::const_iterator pbin_end = _presences->end();

	while ( pbin_iterator != pbin_end ) {

	  Sample bin_sample = (*pbin_iterator)->environment();

	  if ( bin_sample[i] == *bin_iterator ) {

	    bin_values.push_back( 1 );
	  }
	  else {

	    bin_values.push_back( 0 );
	  }
	  ++pbin_iterator;
	}

	pbin_iterator = _background->begin();
	pbin_end = _background->end();

	while ( pbin_iterator != pbin_end ) {

	  Sample bin_sample = (*pbin_iterator)->environment();

	  if ( bin_sample[i] == *bin_iterator ) {

	    bin_values.push_back( 1 );
	  }
	  else {

	    bin_values.push_back( 0 );
	  }
	  ++pbin_iterator;
	}

	bin_each_value.insert( std::pair< Scalar, std::vector<bool > >( (Scalar) *bin_iterator, bin_values) );
	++bin_iterator;
      }
      
      _categorical_values.insert( std::pair< int, std::map< Scalar, std::vector<bool > > > ( i, bin_each_value ) );

      _num_values_cat += values.size();
    } // if ( _is_categorical[i] )
  } // for ( int i = 0; i < _num_layers; ++i )
  
  return 1;
} // initialize

/***************/
/*** iterate ***/

int
MaximumEntropy::iterate()
{
  init_trainer();

  double loss = std::numeric_limits<double>::infinity();
  double new_loss = 0.0;
  int niter;

  for ( niter = 0; niter < _num_iterations; ++niter ) {
    
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

    for ( int i = 0; i < _len; ++i ) {

      sum_mean_lambda += -features_mean[i] * lambda[i];
      sum_regu_lambda += regularization_parameters[i] * fabs(lambda[i]);
    }

    new_loss = sum_mean_lambda + log(Z_lambda) + sum_regu_lambda;

    if ( (loss - new_loss) < _tolerance ) {
      break;
    }

    loss = new_loss;

    calc_q_lambda_f();
    
#ifdef MPI_FOUND
    int my_rank, n_tasks;
    MPI_Status status;
    MPI_Comm_rank( MPI_COMM_WORLD, &my_rank );
    MPI_Comm_size( MPI_COMM_WORLD, &n_tasks); // Find out how many processes there are in the default communicator

    if ( my_rank == 0 ){

      double result;
      int ind, i, rank;

      for ( rank = 1, i = 0; rank < n_tasks && rank <= _len; ++rank, ++i ) {

	MPI_Send(&i,                // i-th work to do
		 1,                 // one data item
		 MPI_INT,           // data item is an integer
		 rank,              // destination process rank
		 WORKTAG,           // user chosen message tag
		 MPI_COMM_WORLD);   // default communicator
      }

      while ( i < _len ) {

	// Receive F[i] from a slave
	MPI_Recv(&result,           // result from some slave
                 1,                 // one data item
		 MPI_DOUBLE,        // of type double real
		 MPI_ANY_SOURCE,    // receive from any sender
		 MPI_ANY_TAG,       // any type of message
		 MPI_COMM_WORLD,    // default communicator
		 &status);          // info about the received message

	// Receive i from the same slave
	MPI_Recv(&ind,              // index from the same slave that sent F[i]
                 1,                 // one data item
		 MPI_INT,           // of type int
		 status.MPI_SOURCE, // receive from the same sender
		 MPI_ANY_TAG,       // any type of message
		 MPI_COMM_WORLD,    // default communicator
		 &status);          // info about the received message

	// Send the slave a new work unit
	MPI_Send(&i,                // i-th work to do
		 1,                 // one data item
		 MPI_INT,           // data item is an integer
		 status.MPI_SOURCE, // to who we just received from
		 WORKTAG,           // user chosen message tag
		 MPI_COMM_WORLD);   // default communicator

	if ( min_F > result ) {
	  min_F = result;
	  best_id = ind;
	}

	++i;
      }

      // Receive all the outstanding results from the slaves.
      for ( rank = 1; rank < n_tasks; ++rank ) {

	MPI_Recv( &result, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );
	MPI_Recv( &ind, 1, MPI_INT, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );

	if ( min_F > result ) {
	  min_F = result;
	  best_id = ind;
	}
      }

      // Tell all the slaves to exit by sending an empty message with the DIETAG.
      for ( rank = 1; rank < n_tasks; ++rank ) {

	MPI_Send( 0, 0, MPI_INT, rank, DIETAG, MPI_COMM_WORLD );
      }
    } // if ( myrank == 0 )
    else {

      int i;
      double result;

      midpoint = new double[_len];
      sign_pos = new int[_len];
      sign_neg = new int[_len];
      signs = new int[_len];
      alfa = new double[_len];
      alfa_pos_neg = new double[_len];

      while ( 1 ) {
	
	// Receive a message from the master
	MPI_Recv( &i, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	// Check the tag of the received message.
	if ( status.MPI_TAG == DIETAG ) {

          end_trainer();	 
	  return 1;
	}

	// Do the work
	midpoint[i] = -features_mean[i] + q_lambda_f[i] / ( exp(lambda[i]) + ( 1 - exp(lambda[i]) ) * q_lambda_f[i]);
	
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
	
	if ( sign_neg[i] > 0 ) neg = 1;
	else neg = 0;
	
	if ( sign_pos[i] < 0 ) pos = 1;
	else pos = 0;
	
	signs[i] = neg - pos;
	
	alfa_pos_neg[i] = log((features_mean[i]+signs[i]*regularization_parameters[i])*(1-q_lambda_f[i])
			      /((1-features_mean[i]-signs[i]*regularization_parameters[i])*q_lambda_f[i]));
	
	alfa[i] = -lambda[i];
	
	if ( signs[i] != 0 ) {
	  alfa[i] = alfa_pos_neg[i];
	}
	
	result = -alfa[i] * features_mean[i] + log(1 + (exp(alfa[i]) - 1) * q_lambda_f[i])
	         + regularization_parameters[i] * (fabs(lambda[i] + alfa[i]) - fabs(lambda[i]));
	
	// Send the result back
	MPI_Send( &result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD );
	MPI_Send( &i, 1, MPI_INT, 0, 0, MPI_COMM_WORLD );

      } // While (1)
    } // else
#else
    double *F;
    midpoint = new double[_len];
    sign_pos = new int[_len];
    sign_neg = new int[_len];
    signs = new int[_len];
    F = new double[_len];
    alfa = new double[_len];
    alfa_pos_neg = new double[_len];

    for ( int i = 0; i < _len; ++i ) {

      midpoint[i] = -features_mean[i] + q_lambda_f[i] / ( exp(lambda[i]) + ( 1 - exp(lambda[i]) ) * q_lambda_f[i]);

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
      
      if ( sign_neg[i] > 0 ) neg = 1;
      else neg = 0;

      if ( sign_pos[i] < 0 ) pos = 1;
      else pos = 0;

      signs[i] = neg - pos;

      alfa_pos_neg[i] = log((features_mean[i]+signs[i]*regularization_parameters[i])*(1-q_lambda_f[i])
			    /((1-features_mean[i]-signs[i]*regularization_parameters[i])*q_lambda_f[i]));

      alfa[i] = -lambda[i];
 
      if ( signs[i] != 0 ) {
	alfa[i] = alfa_pos_neg[i];
      }

      F[i] = -alfa[i] * features_mean[i] + log(1 + (exp(alfa[i]) - 1) * q_lambda_f[i])
	     + regularization_parameters[i] * (fabs(lambda[i] + alfa[i]) - fabs(lambda[i]));

      if ( min_F > F[i] ) {
	min_F = F[i];
	best_id = i;
      }
    } // for ( int i = 0; i < _len; ++i )
#endif
    lambda[best_id] += min_F;

  } // for ( size_t niter = 0; niter < iter; ++niter )

  Log::instance()->info( MAXENT_LOG_PREFIX "Entropy\t %.2f \n", entropy );

  _done = true;

  end_trainer();
  
  return 1;
}

/********************/
/*** init_trainer ***/

void
MaximumEntropy::init_trainer()
{
  _len = _num_layers - _categorical_values.size() + _num_values_cat;

  regularization_parameters = new double[_len];

  features_mean = new double[_len]; // mean of each feature

  feat_stan_devi = new double[_len]; // standard deviation of each feature

  q_lambda_f = new double[_len];

  lambda = new double[_len]; // weight of each feature
  
  q_lambda_x = new double[_num_samples]; // probability of each point

  for ( int i = 0; i < _len; ++i ) {

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
  int j = 0;

  // sum the feature values to calculate the mean.  
  while ( p_iterator != p_end ) {

    Sample sample = (*p_iterator)->environment();
    int index = 0;

    for ( int i = 0; i < _num_layers; ++i ) {

      if ( _is_categorical[i] ) {

	std::map< int, std::map< Scalar, std::vector< bool > > >::const_iterator t;
	t = _categorical_values.find(i);

	std::map<Scalar, std::vector<bool > >::const_iterator t_bin = t->second.begin(); 
	std::map<Scalar, std::vector<bool > >::const_iterator t_bin_end = t->second.end(); 

	while ( t_bin != t_bin_end ) {
      
	  features_mean[index] += t_bin->second[j];
	  ++index;
	  ++t_bin;
	}
      }
      else {
	features_mean[index] += sample[i];
	++index;
      }
    }
    ++p_iterator;
    ++j;
  }

  // calculate the features mean
  for ( int i = 0; i < _len; ++i ) {

    features_mean[i] /= _num_presences;
  }

  // calculate the variance of each feature
  p_iterator = _presences->begin();
  p_end = _presences->end();
  j = 0;

  while ( p_iterator != p_end ) {
    
    Sample sample = (*p_iterator)->environment();
    int index = 0;

    for ( int i = 0; i < _num_layers; ++i ) {
      
      if ( _is_categorical[i] ) {

	std::map< int, std::map< Scalar, std::vector< bool > > >::const_iterator t;
	t = _categorical_values.find(i);

	std::map<Scalar, std::vector<bool > >::const_iterator t_bin = t->second.begin(); 
	std::map<Scalar, std::vector<bool > >::const_iterator t_bin_end = t->second.end(); 
	
	while ( t_bin != t_bin_end ) {
	  
	  feat_stan_devi[index] += pow((t_bin->second[j] - features_mean[index]),2);
	  ++index;
	  ++t_bin;
	}
      }
      else {
	
	feat_stan_devi[index] += pow((sample[i] - features_mean[index]),2);
	++index;
      }
    }
    ++p_iterator;
    ++j;
  }

  for ( int i = 0; i < _len; ++i ) {

    feat_stan_devi[i] /= ( _num_presences - 1 );
  }

  if ( _num_presences <= 10 ) {
    beta_l = 1.0;
  }
  else {
    if ( _num_presences >= 100 ) {
      beta_l = 0.05;
    }
    else {
      if ( _num_presences == 30 ) {
	beta_l = 0.2;
      }
      else{
	beta_l = interpol( 'l' );
      }
    }
  }

  if ( _hasCategorical ) {
    if ( _num_presences < 17 ) {
      if ( _num_presences == 10 ) {
	beta_c = 0.5;
      }
      beta_c = interpol( 'c' );
    }
    else {
      beta_c = 0.25;
    }
  }

  int index = 0;

  // initialize the regularization parameters
  for ( int i = 0; i < _num_layers; ++i ) {
    
    if ( _is_categorical[i] ) {

      std::map< int, std::map< Scalar, std::vector< bool > > >::const_iterator t;
      t = _categorical_values.find(i);
      
      for ( size_t tam = 0; tam < t->second.size(); ++tam ) {

	regularization_parameters[index] = beta_c / sqrt((double)(_num_presences)) * sqrt(min(0.25,feat_stan_devi[index]));

	if ( regularization_parameters[index] < 0.00001 ) {

	  regularization_parameters[index] = 0.00001;
	}
	++index;
      }
    }
    else {

      regularization_parameters[index] = beta_l / sqrt((double)(_num_presences)) * sqrt(min(0.25,feat_stan_devi[index]));
    
      if ( regularization_parameters[index] < 0.00001 ) {
	
	regularization_parameters[index] = 0.00001;
      }
      ++index;
    }
  }
} // init_trainer();


/********************/
/*** end_trainer ***/

void
MaximumEntropy::end_trainer()
{
  delete regularization_parameters;
  delete features_mean;
  delete feat_stan_devi;
  delete q_lambda_f;
  delete lambda;
  delete q_lambda_x;
} // end_trainer();


/***********************/
/****** interpol *******/

double
MaximumEntropy::interpol( char type_feat )
{
  std::map< int, double > table_beta_l;
  std::map< int, double > table_beta_c;
  double product = 0.0;
  double answer = 0.0;

  table_beta_l.insert( std::pair< int, double > ( 10, 1.0 ) );
  table_beta_l.insert( std::pair< int, double > ( 30, 0.2 ) );
  table_beta_l.insert( std::pair< int, double > ( 100, 0.05 ) );
  table_beta_c.insert( std::pair< int, double > ( 0, 0.65 ) );
  table_beta_c.insert( std::pair< int, double > ( 10, 0.5 ) );
  table_beta_c.insert( std::pair< int, double > ( 17, 0.25 ) );
 
  if ( type_feat == 'l' ) {

    std::map< int, double >::const_iterator it = table_beta_l.begin();
    std::map< int, double >::const_iterator it_end = table_beta_l.end();
    
    while ( it != it_end ){

      product = (*it).second;

      std::map< int, double >::const_iterator its = table_beta_l.begin();
      std::map< int, double >::const_iterator its_end = table_beta_l.end();

      while ( its != its_end ){

	if ( it != its ) {

	  product *= ( double ( _num_presences - (*its).first ) / double ( (*it).first - (*its).first ) );
	}
	++its;
      }
      answer += product;
      ++it;
    }
  }

  if ( type_feat == 'c' ) {

    std::map< int, double >::const_iterator it = table_beta_c.begin();
    std::map< int, double >::const_iterator it_end = table_beta_c.end();
    
    while ( it != it_end ){

      product = (*it).second;

      std::map< int, double >::const_iterator its = table_beta_c.begin();
      std::map< int, double >::const_iterator its_end = table_beta_c.end();

      while ( its != its_end ){

	if ( it != its ) {

	  product *= ( double ( _num_presences - (*its).first ) / double ( (*it).first - (*its).first ) );
	}
	++its;
      }
      answer += product;
      ++it;
    }
  }
  return answer;
}

/***********************/
/*** calc_q_lambda_x ***/

void
MaximumEntropy::calc_q_lambda_x()
{
  Z_lambda = 0.0;
  entropy = 0.0;
  
  OccurrencesImpl::const_iterator p_iterator = _presences->begin();
  OccurrencesImpl::const_iterator p_end = _presences->end();
  int i = 0;

  while ( p_iterator != p_end ) {
    
    Sample sample = (*p_iterator)->environment();
    int index = 0;
    double sum_lambdaj_fj = 0.0;

    for ( int j = 0; j < _num_layers; ++j ) {

      if ( _is_categorical[j] ) {

	std::map< int, std::map< Scalar, std::vector< bool > > >::const_iterator t;
	t = _categorical_values.find(j);

	std::map<Scalar, std::vector<bool > >::const_iterator t_bin = t->second.begin(); 
	std::map<Scalar, std::vector<bool > >::const_iterator t_bin_end = t->second.end(); 
	
	while ( t_bin != t_bin_end ) {
	  
	  sum_lambdaj_fj += lambda[index] * t_bin->second[i];
	  ++index;
	  ++t_bin;
	}
      }
      else {

	sum_lambdaj_fj += lambda[index] * sample[j];
	++index;
      }
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
    int index = 0;
    double sum_lambdaj_fj = 0.0;

    for ( int j = 0; j < _num_layers; ++j ) {

      if ( _is_categorical[j] ) {

	std::map< int, std::map< Scalar, std::vector< bool > > >::const_iterator t;
	t = _categorical_values.find(j);

	std::map<Scalar, std::vector<bool > >::const_iterator t_bin = t->second.begin(); 
	std::map<Scalar, std::vector<bool > >::const_iterator t_bin_end = t->second.end(); 
	
	while ( t_bin != t_bin_end ) {
	  
	  sum_lambdaj_fj += lambda[index] * t_bin->second[i];
	  ++index;
	  ++t_bin;
	}
      }
      else {

	sum_lambdaj_fj += lambda[index] * sample[j];
	++index;
      }
    }
    
    q_lambda_x[i] = exp(sum_lambdaj_fj);
    Z_lambda += q_lambda_x[i]; // normalization constant

    ++i;
    ++p_iterator;
  }

  // normalize all q_lambda_x
  for ( int j = 0; j < _num_samples; ++j ) {
    
    q_lambda_x[j] /= Z_lambda;
    entropy += (q_lambda_x[j] * log(q_lambda_x[j]));
  }
  entropy = -entropy;

  Log::instance()->info( MAXENT_LOG_PREFIX "Entropy\t %.2f \r", entropy );
}

/***********************/
/*** calc_q_lambda_f ***/

void
MaximumEntropy::calc_q_lambda_f()
{
  for ( int i = 0; i < _len; ++i ) {

    q_lambda_f[i] = 0.0;
  }

  OccurrencesImpl::const_iterator p_iterator = _presences->begin();
  OccurrencesImpl::const_iterator p_end = _presences->end();
  int i = 0;

  while ( p_iterator != p_end ) {
    
    Sample sample = (*p_iterator)->environment();
    int index = 0;

    for ( int j = 0; j < _num_layers; ++j ) {

      if ( _is_categorical[j] ) {

	std::map< int, std::map< Scalar, std::vector< bool > > >::const_iterator t;
	t = _categorical_values.find(j);

	std::map<Scalar, std::vector<bool > >::const_iterator t_bin = t->second.begin(); 
	std::map<Scalar, std::vector<bool > >::const_iterator t_bin_end = t->second.end(); 
	
	while ( t_bin != t_bin_end ) {

	  q_lambda_f[index] += q_lambda_x[i] * t_bin->second[i];
	  ++index;
	  ++t_bin;
	}
      }
      else {

	q_lambda_f[index] += q_lambda_x[i] * sample[j];
	++index;
      }
    }
    ++i;
    ++p_iterator;
  }
  
  p_iterator = _background->begin();
  p_end = _background->end();
  
  while ( p_iterator != p_end ) {
    
    Sample sample = (*p_iterator)->environment();
    int index = 0;

    for ( int j = 0; j < _num_layers; ++j ) {

      if ( _is_categorical[j] ) {

	std::map< int, std::map< Scalar, std::vector< bool > > >::const_iterator t;
	t = _categorical_values.find(j);

	std::map<Scalar, std::vector<bool > >::const_iterator t_bin = t->second.begin(); 
	std::map<Scalar, std::vector<bool > >::const_iterator t_bin_end = t->second.end(); 
	
	while ( t_bin != t_bin_end ) {

	  q_lambda_f[index] += q_lambda_x[i] * t_bin->second[i];
	  ++index;
	  ++t_bin;
	}
      }
      else {

	q_lambda_f[index] += q_lambda_x[i] * sample[j];
	++index;
      }
    }
    ++i;
    ++p_iterator;
  }
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
  int index = 0;
  
  for ( int i = 0; i < _num_layers; ++i ) {

    if ( _is_categorical[i] ) {

      std::map< int, std::set<Scalar> >::const_iterator layer_categories = _cat_values.find(i);

      if ( layer_categories != _cat_values.end() ) {

        std::set<Scalar>::const_iterator it = layer_categories->second.begin();
        std::set<Scalar>::const_iterator it_end = layer_categories->second.end();

        while ( it != it_end ) {

          if ( x[i] == *it ) {
	 
	    prob += lambda[index];
	    ++index;
          }
          else {

	    ++index;
          }
          ++it;
        }
      }
    }
    else {
    
      prob += lambda[index] * x[index];
      ++index;
    }
  }

  prob = exp(prob)/Z_lambda;

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
    return ( ( exp(entropy) * prob ) / ( 1 + ( exp(entropy) * prob ) ) );
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
  model_config->addNameValue( "Z", Z_lambda );
  model_config->addNameValue( "Entropy", entropy);
  model_config->addNameValue( "OutputFormat", _output_format );

  model_config->addNameValue( "Categorical", _is_categorical );

  ConfigurationPtr cat_section_config( new ConfigurationImpl( "CategoricalData" ) );
  model_config->addSubsection( cat_section_config );

  std::map< int, std::map< Scalar, std::vector< bool > > >::const_iterator it = _categorical_values.begin();
  std::map< int, std::map< Scalar, std::vector< bool > > >::const_iterator it_end = _categorical_values.end();

  while ( it != it_end ) {

    // There can be be multiple <Categories> elements, one for each categorical layer
    ConfigurationPtr cat_config( new ConfigurationImpl( "Categories" ) );
    cat_section_config->addSubsection( cat_config );

    int layer_index = (*it).first;
    std::map<Scalar, std::vector<bool > > layer_categories = (*it).second;

    // Transform std::set into Sample
    Sample categories;
    categories.resize( layer_categories.size() );

    int i = 0;
    std::map<Scalar, std::vector<bool > >::iterator val_it;

    for ( val_it = layer_categories.begin(); val_it != layer_categories.end(); val_it++ ) {

      //      categories[i] = (*val_it);
      categories[i] = (*val_it).first;
      ++i;
    }

    cat_config->addNameValue( "Index", layer_index );
    cat_config->addNameValue( "Values", categories );

    ++it;
  }

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

  entropy = model_config->getAttributeAsDouble( "Entropy", 0.0 );

  _output_format = model_config->getAttributeAsInt( "OutputFormat", 2 );
  
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

      _cat_values.insert( std::pair< int, std::set<Scalar> >( layer_index, values ) );
    }
  }
  
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
