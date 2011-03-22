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

#ifdef MPI_FOUND
#include "mpi.h"
#define WORKTAG 1
#define DIETAG 2
#endif

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
  // Use absence points as background
  {
    USE_ABSENCES_ID,     // Id.
    " Use absence points as background", // Name.
    Integer,  // Type.
    " Use absence points as background", // Overview
    "When absence points are provided, this parameter can be used to instruct the algorithm to use them as background points. This would prevent the algorithm to randomly generate them, also facilitating comparisons between different algorithms.", // Description.
    1,         // Not zero if the parameter has lower limit.
    0,         // Parameter's lower limit.
    1,         // Not zero if the parameter has upper limit.
    1,         // Parameter's upper limit.
    "0"        // Parameter's typical (default) value.
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
  "0.5",       	     // Version.

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
  delete[] lambda;
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

    _normalizerPtr->computeNormalization( mySamplerPtr );
  }
  else {

    // Compute normalization with all points
    SamplerPtr mySamplerPtr = createSampler( _samp->getEnvironment(), _presences, _background );

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

  _min.resize( _num_layers );
  _max.resize( _num_layers );
  _background->getMinMax( &_min, &_max );
  
  return 1;
} // initialize

/***************/
/*** iterate ***/

int
MaximumEntropy::iterate()
{
  init_trainer();

  //  double old_loss = std::numeric_limits<double>::infinity();
  double new_loss = getLoss();
  Log::instance()->debug( "Initial loss = %f \n", new_loss );

  double Gain = 0.0;

  for ( iteration = 0; iteration < _max_iterations; ++iteration ) {

    double *alpha;
    double min_F = std::numeric_limits<double>::infinity();

    int best_id = -1;

    calc_q_lambda_x();

    new_loss = getLoss();
    double delta_loss = new_loss - old_loss;
    old_loss = new_loss;

    Gain = log((double)_num_samples) - old_loss;

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
      double F;
      double min_F_local = std::numeric_limits<double>::infinity();

      alpha = new double[_len];

      while ( 1 ) {
	
	// Receive a message from the master
	MPI_Recv( &i, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	// Check the tag of the received message.
	if ( status.MPI_TAG == DIETAG ) {

          end_trainer();	 
	  return 1;
	}

	// Do the work
	alpha[i] = -lambda[i];
	alpha[i] = reduceAlpha(alpha[i]);
	
	F = -alpha[i] * features_mean[i] + log(1 + (exp(alpha[i]) - 1) * q_lambda_f[i])
	    + regularization_parameters[i] * (fabs(lambda[i] + alpha[i]) - fabs(lambda[i]));
	
	if ( min_F_local > F ) {
	  min_F_local = F;
	}
	
	alpha[i] = log(((features_mean[i]-regularization_parameters[i])*(1-q_lambda_f[i]))
		      /((1-features_mean[i]-regularization_parameters[i])*q_lambda_f[i])); 
	alpha[i] = reduceAlpha(alpha[i]);

	F = -alpha[i] * features_mean[i] + log(1 + (exp(alpha[i]) - 1) * q_lambda_f[i])
	    + regularization_parameters[i] * (fabs(lambda[i] + alpha[i]) - fabs(lambda[i]));
	
	if( ( lambda[i] + alpha[i] ) >= 0.0 ) {
	  
	  if ( min_F_local > F ) {
	    min_F_local =  F;
	  }
	}
	
	alpha[i] = log(((features_mean[i]+regularization_parameters[i])*(1-q_lambda_f[i]))
		     /((1-features_mean[i]+regularization_parameters[i])*q_lambda_f[i])); 
	alpha[i] = reduceAlpha(alpha[i]);

	F = -alpha[i] * features_mean[i] + log(1 + (exp(alpha[i]) - 1) * q_lambda_f[i])
	         + regularization_parameters[i] * (fabs(lambda[i] + alpha[i]) - fabs(lambda[i]));
	
	if( ( lambda[i] + alpha[i] ) <= 0.0 ) {
	  
	  if ( min_F_local > F ) { 
	    min_F = F; 
	  }
	}
	
	// Send the result back
	MPI_Send( &min_F_local, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD );
	MPI_Send( &i, 1, MPI_INT, 0, 0, MPI_COMM_WORLD );

      } // While (1)
    } // else
#else

    double *F;
    F = new double[_len];
    alpha = new double[_len];

    for ( int i = 0; i < _len; ++i ) {

      alpha[i] = -lambda[i];
      alpha[i] = reduceAlpha(alpha[i]);

      F[i] = -alpha[i] * features_mean[i] + log(1 + (exp(alpha[i]) - 1) * q_lambda_f[i])
	     + regularization_parameters[i] * (fabs(lambda[i] + alpha[i]) - fabs(lambda[i]));

      if ( min_F > F[i] ) {
	min_F = F[i];
	best_id = i;
      }

      alpha[i] =log(((features_mean[i]-regularization_parameters[i])*(1-q_lambda_f[i]))
			    /((1-features_mean[i]-regularization_parameters[i])*q_lambda_f[i])); 
      alpha[i] = reduceAlpha(alpha[i]);

      F[i] = -alpha[i] * features_mean[i] + log(1 + (exp(alpha[i]) - 1) * q_lambda_f[i])
	     + regularization_parameters[i] * (fabs(lambda[i] + alpha[i]) - fabs(lambda[i]));

      if((lambda[i]+alpha[i])>=0.0){

	if ( min_F > F[i] ) {
	  min_F = F[i];
	  best_id = i;
	}
      }

      alpha[i] =log(((features_mean[i]+regularization_parameters[i])*(1-q_lambda_f[i]))
			    /((1-features_mean[i]+regularization_parameters[i])*q_lambda_f[i])); 
      alpha[i] = reduceAlpha(alpha[i]);

      F[i] = -alpha[i] * features_mean[i] + log(1 + (exp(alpha[i]) - 1) * q_lambda_f[i])
	     + regularization_parameters[i] * (fabs(lambda[i] + alpha[i]) - fabs(lambda[i]));

      if((lambda[i]+alpha[i])<=0.0){

	if ( min_F > F[i] ) {
	  min_F = F[i];
	  best_id = i;
	}
      }
    } // for ( int i = 0; i < _len; ++i )

    delete[] F;
#endif
    lambda[best_id] += min_F;

    if ( terminationTest(new_loss) ) {
      break;
    }

    displayInfo(best_id, new_loss, delta_loss, alpha[best_id], iteration);

    delete[] alpha;

  } // for ( iteration = 0; iteration < _max_iterations; ++iteration )

  Log::instance()->info( MAXENT_LOG_PREFIX "Gain\t %f \n", Gain );
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
  convergenceTestFrequency = 20;
  previousLoss = std::numeric_limits<double>::infinity();

  _len = _num_layers - _categorical_values.size() + _num_values_cat;

  regularization_parameters = new double[_len];

  features_mean = new double[_len];

  feat_stan_devi = new double[_len];

  q_lambda_f = new double[_len];

  lambda = new double[_len];
  
  q_lambda_x = new double[_num_samples];

  _features_mid = new double[_len];
  _features_dev = new double[_len];
  double *features_min = new double[_len];
  double *features_max = new double[_len];

  for ( int i = 0; i < _len; ++i ) {

    regularization_parameters[i] = 0.0;
    features_mean[i] = 0.0;
    feat_stan_devi[i] = 0.0;
    _features_mid[i] = 0.0;
    _features_dev[i] = 0.0;
    features_min[i] = 1.1; // environment was normalized between 0 and 1, so 1.1 is safe
    features_max[i] = -1.0;// environment was normalized between 0 and 1, so -1 is safe
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
          // TODO: handle min/max here!
	  ++index;
	  ++t_bin;
	}
      }
      else {
	features_mean[index] += sample[i];
	features_min[index] = std::min(sample[i], features_min[index]);
	features_max[index] = std::max(sample[i], features_max[index]);
	++index;
      }
    }
    ++p_iterator;
    ++j;
  }

  // calculate the feature attributes for the samples
  for ( int i = 0; i < _len; ++i ) {

    features_mean[i] /= _num_presences;
    _features_mid[i] = 0.5*(features_max[i]+features_min[i]);
    _features_dev[i] = 0.5*(features_max[i]-features_min[i]);
  }

  delete[] features_min;
  delete[] features_max;

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

  beta_l = interpol ( 'l' );

  if ( _hasCategorical ) {
    beta_c = interpol( 'c' );
    Log::instance()->debug( "Regularization values: Linear = %f \t Categorical = %f \n", beta_l, beta_c );
  }
  else {
    Log::instance()->debug( "Regularization value = %f \n", beta_l );
  }

  int index = 0;

  // initialize the regularization parameters
  for ( int i = 0; i < _num_layers; ++i ) {
    
    if ( _is_categorical[i] ) {

      std::map< int, std::map< Scalar, std::vector< bool > > >::const_iterator t;
      t = _categorical_values.find(i);
      
      for ( size_t tam = 0; tam < t->second.size(); ++tam ) {

	//	regularization_parameters[index] = beta_c / sqrt((double)(_num_presences)) * sqrt(min(0.25,feat_stan_devi[index]));
      	regularization_parameters[index] = beta_c / sqrt((double)(_num_presences));

	if ( regularization_parameters[index] < 0.00001 ) {

	  regularization_parameters[index] = 0.00001;
	}
	++index;
      }
    }
    else {

      //      regularization_parameters[index] = beta_l / sqrt((double)(_num_presences)) * sqrt(min(0.25,feat_stan_devi[index]));
      regularization_parameters[index] = beta_l / sqrt((double)(_num_presences));

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
  delete[] regularization_parameters;
  delete[] features_mean;
  delete[] feat_stan_devi;
  delete[] q_lambda_f;
  delete[] q_lambda_x;
  delete[] _features_mid;
  delete[] _features_dev;
} // end_trainer();

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

/********************/
/*** display Info ***/

void
MaximumEntropy::displayInfo(int best_id, double new_loss, double delta_loss, double alpha, int iteration)
{
  Log::instance()->debug( "%d: loss = %f \n", iteration, new_loss );

  Log::instance()->debug( "%s: lambda = %f min = %f max = %f\n", _samp->getEnvironment()->getLayerPath(best_id).c_str(), lambda[best_id], _min[best_id], _max[best_id] );

  Log::instance()->debug( "alpha = %f W1 = %f N1 = %f deltaLoss = %f \n", alpha, q_lambda_f[best_id], features_mean[best_id], delta_loss );

}

/************************/
/*** termination Test ***/

bool
MaximumEntropy::terminationTest(double newLoss)
{
  if ( iteration == 0 ) {
    previousLoss = newLoss;
    return false;
  }
  if ( iteration % convergenceTestFrequency != 0 ) {
    return false;
  }
  if ( previousLoss - newLoss < _tolerance ) {
    Log::instance()->info( MAXENT_LOG_PREFIX "The algorithm converged after %d iteration(s) \n", iteration );
    return true;
  }
  previousLoss = newLoss;
  return false;
}

/********************/
/*** reduce Alpha ***/

double
MaximumEntropy::reduceAlpha(double alpha)
{
  if ( iteration < 10 ) {
    return alpha / 50.0;
  }
  if ( iteration < 20 ) {
    return alpha / 10.0;
  }
  if ( iteration < 50 ) {
    return alpha / 3.0;
  }
  return alpha;
}

/****************/
/*** get Loss ***/

double
MaximumEntropy::getLoss()
{
  double sum_mid_lambda = 0.0; // N1
  double sum_regu_lambda = 0.0;

  for ( int i = 0; i < _len; ++i ) {
    
    sum_mid_lambda += _features_mid[i] * lambda[i];
    sum_regu_lambda += regularization_parameters[i] * fabs(lambda[i]);
  }
  
  return ( log(Z_lambda) - sum_mid_lambda + sum_regu_lambda );
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
