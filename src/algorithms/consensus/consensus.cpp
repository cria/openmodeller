/**
 * Definition of Consensus algorithm class.
 * 
 * @author Renato De Giovanni (renato [at] cria org br)
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2011 by CRIA -
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

#include "consensus.hh"

#include <string>
#include <algorithm>
#include <sstream>

using namespace std;

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 7
#define MAX_ALGORITHMS 5

#define CONSENSUS_LOG_PREFIX "Consensus: "

/******************************/
/*** Algorithm's parameters ***/

static AlgParamMetadata parameters[NUM_PARAM] = {
  
  // Algorithm 1
  {
    "Alg1",       // Id.
    "Algorithm1", // Name.
    String,       // Type.
    "Algorithm 1", // Overview
    "First algorithm to be used in the consensus. It must be specified by its id followed by a sequence of parameter_name=parameter_value separated by comma and enclosed by a parentheses, such as: RF(NumTrees=10,VarsPerTree=0,ForceUnsupervisedLearning=0). Existing algorithm ids and parameter names can be found in the end of the om_console request file that comes with the openModeller command line interface.", // Description.
    0,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    0,      // Not zero if the parameter has upper limit.
    0,      // Parameter's upper limit.
    "RF(NumTrees=10,VarsPerTree=0,ForceUnsupervisedLearning=1)" // Parameter's typical (default) value.
  },
  // Algorithm 2
  {
    "Alg2",       // Id.
    "Algorithm2", // Name.
    String,       // Type.
    "Algorithm 2", // Overview
    "Second algorithm to be used in the consensus. It must be specified by its id followed by a sequence of parameter_name=parameter_value separated by comma and enclosed by a parentheses, such as: RF(NumTrees=10,VarsPerTree=0,ForceUnsupervisedLearning=0). Existing algorithm ids and parameter names can be found in the end of the om_console request file that comes with the openModeller command line interface. Leave empty if you don't want to use any further algorithms", // Description.
    0,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    0,      // Not zero if the parameter has upper limit.
    0,      // Parameter's upper limit.
    ""      // Parameter's typical (default) value.
  },
  // Algorithm 3
  {
    "Alg3",       // Id.
    "Algorithm3", // Name.
    String,       // Type.
    "Algorithm 3", // Overview
    "Third algorithm to be used in the consensus. It must be specified by its id followed by a sequence of parameter_name=parameter_value separated by comma and enclosed by a parentheses, such as: RF(NumTrees=10,VarsPerTree=0,ForceUnsupervisedLearning=0). Existing algorithm ids and parameter names can be found in the end of the om_console request file that comes with the openModeller command line interface. Leave empty if you don't want to use any further algorithms", // Description.
    0,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    0,      // Not zero if the parameter has upper limit.
    0,      // Parameter's upper limit.
    ""      // Parameter's typical (default) value.
  },
  // Algorithm 4
  {
    "Alg4",       // Id.
    "Algorithm4", // Name.
    String,       // Type.
    "Algorithm 4", // Overview
    "Fourth algorithm to be used in the consensus. It must be specified by its id followed by a sequence of parameter_name=parameter_value separated by comma and enclosed by a parentheses, such as: RF(NumTrees=10,VarsPerTree=0,ForceUnsupervisedLearning=0). Existing algorithm ids and parameter names can be found in the end of the om_console request file that comes with the openModeller command line interface. Leave empty if you don't want to use any further algorithms", // Description.
    0,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    0,      // Not zero if the parameter has upper limit.
    0,      // Parameter's upper limit.
    ""      // Parameter's typical (default) value.
  },
  // Algorithm 5
  {
    "Alg5",       // Id.
    "Algorithm5", // Name.
    String,       // Type.
    "Algorithm 5", // Overview
    "Fifth algorithm to be used in the consensus. It must be specified by its id followed by a sequence of parameter_name=parameter_value separated by comma and enclosed by a parentheses, such as: RF(NumTrees=10,VarsPerTree=0,ForceUnsupervisedLearning=0). Existing algorithm ids and parameter names can be found in the end of the om_console request file that comes with the openModeller command line interface. Leave empty if you don't want to use any further algorithms", // Description.
    0,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    0,      // Not zero if the parameter has upper limit.
    0,      // Parameter's upper limit.
    ""      // Parameter's typical (default) value.
  },
  // Weigths
  {
    "Weights", // Id.
    "Weights", // Name.
    String,    // Type.
    "Weights", // Overview
    "Sequence of weights, each one related to the corresponding algorithm, separated by space. This can be used to give more importance to certain algorithms. Use dot as decimal separator.", // Description.
    0,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    0,      // Not zero if the parameter has upper limit.
    0,      // Parameter's upper limit.
    "1.0 0.0 0.0 0.0 0.0" // Parameter's typical (default) value.
  },
  // Minimum level of agreement
  {
    "Agreement", // Id.
    "Agreement", // Name.
    Integer,     // Type.
    "Minimum level of agreement", // Overview
    "Minimum level of agreement between the algorithms. Only predictions that are agreed between the specified number of algorithms will be returned as a positive value.", // Description.
    1,      // Not zero if the parameter has lower limit.
    1,      // Parameter's lower limit.
    1,      // Not zero if the parameter has upper limit.
    5,      // Parameter's upper limit.
    "1"     // Parameter's typical (default) value.
  },
};

/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "CONSENSUS", 	// Id.
  "Consensus",  // Name.
  "0.3",       	// Version.

  // Overview
  "Builds a consensus model with the specified algorithms",

  // Description.
  "This is a kind of meta algorithm that receives other algorithms as parameters so that it can generate the individual models and then merge the results into an aggregated model. The maximum number of algorithms is limited to 5. Leave the algorithm parameter blank if you want to use fewer algorithms. IMPORTANT: To specify an algorithm you need to know the algorithm id and its parameters names in openModeller (you can do this by inspecting the request.txt file that comes as an exemple in the command-line interface). Before merging the models, each individual model is transformed into a binary model using the lowest presence threshold. You can assign different weights to each algorithm and also specify the minimum level of agreement between the algorithms. A minimum level of 3 when 5 algorithms are used means that, when less than 3 algorithms agree on a prediction, the result will be zero, so the final model only shows areas where the specified number of algorithms agree on the prediction.",

  "Renato De Giovanni", // Algorithm author.
  "", // Bibliography.

  "Renato De Giovanni", // Code author.
  "renato [at] cria . org . br", // Code author's contact.

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
  return new ConsensusAlgorithm();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}


/*********************************************/
/************** SVM algorithm ****************/

/*******************/
/*** constructor ***/

ConsensusAlgorithm::ConsensusAlgorithm() :
  AlgorithmImpl( &metadata ),
  _done( false ),
  _initialized( false ),
  _num_algs( 0 ),
  _agreement( 1 )
{
}


/******************/
/*** destructor ***/

ConsensusAlgorithm::~ConsensusAlgorithm()
{
  for ( int i=0; i < (int)_algs.size(); i++ ) {

    if ( _norms[i] ) {

      delete _norms[i];
    }
  }
}

/**************************/
/*** need Normalization ***/
int ConsensusAlgorithm::needNormalization()
{
  return 0;
}

/******************/
/*** initialize ***/
int
ConsensusAlgorithm::initialize()
{
  std::string alg;

  if ( getParameter( "Alg1", &alg ) ) {

    if ( !_setAlgorithm( alg ) ) return 0;
  }

  if ( getParameter( "Alg2", &alg ) ) {

    if ( !_setAlgorithm( alg ) ) return 0;
  }

  if ( getParameter( "Alg3", &alg ) ) {

    if ( !_setAlgorithm( alg ) ) return 0;
  }

  if ( getParameter( "Alg4", &alg ) ) {

    if ( !_setAlgorithm( alg ) ) return 0;
  }

  if ( getParameter( "Alg5", &alg ) ) {

    if ( !_setAlgorithm( alg ) ) return 0;
  }

  _num_algs = (int)_algs.size();

  if ( _num_algs == 0 ) {

    Log::instance()->error( CONSENSUS_LOG_PREFIX "Consensus needs at least one algorithm. No algorithm could be instantiated based on the parameters.\n" );
    return 0;
  }

  if ( ! getParameter( "Agreement", &_agreement ) ) {

    _agreement = _num_algs; // default value
  }
  else {

    if ( _agreement < 1 || _agreement > _num_algs ) {

      _agreement = _num_algs;
    }
  }

  _thresholds = Sample(MAX_ALGORITHMS, 1.0); // start with maximum threshold

  _weights.resize(MAX_ALGORITHMS);

  std::string weights_param;

  int nw = 0;

  _sum_weights = 0.0;

  if ( getParameter( "Weights", &weights_param ) ) {

    stringstream ss(weights_param);
    string weight;
    double weight_val;
    while ( getline(ss, weight, ' ') ) {

      weight_val = 1.0;
      sscanf( weight.c_str(), "%lf", &weight_val );
      _weights[nw] = weight_val;
      _sum_weights += weight_val;
      ++nw;

      if ( nw == MAX_ALGORITHMS ) {
        break;
      }
    }
  }

  for ( int i=nw; i < MAX_ALGORITHMS; ++i ) {

    _weights[i] = 1.0;
    _sum_weights += 1.0;
  }

  for ( int j=0; j < _num_algs; j++ ) {

    SamplerPtr fresh_sampler = cloneSampler(_samp);

    if ( _algs[j]->needNormalization() ) {

      fresh_sampler->resetNormalization();
      _norms[j]->computeNormalization( fresh_sampler );
      fresh_sampler->normalize( _norms[j] );
    }

    _algs[j]->setSampler( fresh_sampler );
    _algs[j]->initialize();
  }

  return 1;
}

/*********************/
/*** set Algorithm ***/
bool
ConsensusAlgorithm::_setAlgorithm( std::string alg_str )
{
  // Remove spaces
  alg_str.erase( std::remove_if( alg_str.begin(), alg_str.end(), ::isspace ), alg_str.end() );

  if ( alg_str.size() == 0 ) {

    // Empty alg. Do nothing.
    return true;
  }

  size_t ini_p = alg_str.find( "(" );

  // No parentheses
  if ( ini_p == string::npos ) {

    // means no parameters, so just instantiate the algorithm
    AlgorithmPtr alg = AlgorithmFactory::newAlgorithm( alg_str );

    _algs.push_back( alg );

    _norms.push_back( alg->getNormalizer() );

    return true;
  }

  // There are parentheses

  // extract ID
  std::string alg_id = alg_str.substr(0, ini_p);

  // get parameters
  size_t end_p = alg_str.find( ")" );

  if ( end_p == string::npos ) {

    Log::instance()->error( CONSENSUS_LOG_PREFIX "Missing parenthesis in algorithm parameters.\n" );
    return false;
  }
  else if ( end_p < ini_p ) {

    Log::instance()->error( CONSENSUS_LOG_PREFIX "Mismatching parenthesis in algoroithm parameters.\n" );
    return false;
  }

  std::string alg_params = alg_str.substr(ini_p + 1, end_p - ini_p -1);

  vector<string> pairs;
  stringstream ss(alg_params);
  string pair;
  int nparam = 0;
  while ( getline(ss, pair, ',') ) {

    pairs.push_back(pair);
    ++nparam;
  }

  ParamSetType params;

  for ( int i = 0; i < nparam; i++) {

    size_t eq = pairs[i].find( "=" );

    if ( eq == string::npos || eq == 0 ) {

      Log::instance()->error( CONSENSUS_LOG_PREFIX "Algorithm parameter failed to match key=value pair format.\n" );
      return false;
    }

    std::string param_id = pairs[i].substr(0, eq);
    std::string param_val = pairs[i].substr(eq+1);

    params.insert( std::pair<icstring,std::string>(param_id, param_val) );
  }

  AlgorithmPtr alg = AlgorithmFactory::newAlgorithm( alg_id );

  alg->setParameters( params );

  _algs.push_back( alg );

  _norms.push_back( alg->getNormalizer() );

  return true;
}

/***************/
/*** iterate ***/
int
ConsensusAlgorithm::iterate()
{
  _done = true;

  for ( int j=0; j < _num_algs; j++ ) {

    if ( ! _algs[j]->done() ) {

      if ( _algs[j]->iterate() == 0 ) {

        return 0; // something wrong happened
      }

      if ( ! _algs[j]->done() ) {

        _done = false;
      }
    }
  }

  // get LPT
  if ( _done ) {

    OccurrencesPtr presences = _samp->getPresences();

    OccurrencesImpl::const_iterator p_iterator = presences->begin();
    OccurrencesImpl::const_iterator p_end = presences->end();

    Scalar val;

    while ( p_iterator != p_end ) {

      Sample env = (*p_iterator)->environment();

      for ( int j=0; j < _num_algs; j++ ) {

        if ( _norms[j] ) {

          Sample mysamp = Sample( env ); // deep copy
          _norms[j]->normalize( &mysamp );
          val = _algs[j]->getValue( mysamp );
        }
        else {

          val = _algs[j]->getValue( env );
	}

        if ( val < _thresholds[j] && val > 0.0 ) {

          _thresholds[j] = val;
        }
      }

      ++p_iterator;
    }
  }

  return 1;
}

/********************/
/*** get Progress ***/
float ConsensusAlgorithm::getProgress() const
{
  float progress = 0.0;

  for ( int j=0; j < _num_algs; j++ ) {

    progress += _algs[j]->getProgress();
  }

  return progress/(float)_num_algs;
}


/************/
/*** done ***/
int
ConsensusAlgorithm::done() const
{
  return _done;
}

/*****************/
/*** get Value ***/
Scalar
ConsensusAlgorithm::getValue( const Sample& x ) const
{
  Scalar prob = 0.0;
  Scalar v;
  int agree = 0;

  for ( int i=0; i < _num_algs; i++ ) {

    if ( _norms[i] ) {

      Sample y( x );
      _norms[i]->normalize( &y );
      v = _algs[i]->getValue( y );
    }
    else {

      v = _algs[i]->getValue( x );
    }

    if ( v >= _thresholds[i] ) {

      prob +=  1.0 * _weights[i];
      agree++;
    }
  }

  if ( agree < _agreement ) {

    return 0.0;
  }

  return prob/_sum_weights;
}

/***********************/
/*** get Convergence ***/
int
ConsensusAlgorithm::getConvergence( Scalar * const val ) const
{
  *val = 1.0;
  return 1;
}

/****************************************************************/
/****************** configuration *******************************/
void
ConsensusAlgorithm::_getConfiguration( ConfigurationPtr& config ) const
{
  if ( ! _done )
    return;

  ConfigurationPtr model_config( new ConfigurationImpl("Consensus") );
  config->addSubsection( model_config );

  model_config->addNameValue( "Thresholds", _thresholds );

  ConfigurationPtr algs_config( new ConfigurationImpl("Algorithms") );
  model_config->addSubsection( algs_config );

  for ( int i=0; i < _num_algs; i++ ) {

    ConfigurationPtr alg_config = _algs[i]->getConfiguration();
    algs_config->addSubsection( alg_config );
  }
}

void
ConsensusAlgorithm::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "Consensus", false );

  if ( ! model_config )
    return;

  if ( ! getParameter("Agreement", &_agreement) ) {

    Log::instance()->error("Parameter 'Agreement' was not found in serialized model.\n");
    return;
  }
  else {

    if ( _agreement < 1 || _agreement > MAX_ALGORITHMS ) {

      _agreement = 2;
    }
  }

  _weights.resize(MAX_ALGORITHMS);

  std::string weights_param;

  int nw = 0;

  _sum_weights = 0.0;

  if ( ! getParameter( "Weights", &weights_param ) ) {

    Log::instance()->error("Parameter 'Weights' was not found in serialized model.\n");
    return;
  }
  else {

    stringstream ss(weights_param);
    string weight;
    double weight_val;
    while ( getline(ss, weight, ' ') ) {

      weight_val = 1.0;
      sscanf( weight.c_str(), "%lf", &weight_val );
      _weights[nw] = weight_val;
      _sum_weights += weight_val;
      ++nw;

      if ( nw == MAX_ALGORITHMS ) {
        break;
      }
    }
  }

  for ( int i=nw; i < MAX_ALGORITHMS; ++i ) {

    _weights[i] = 1.0;
    _sum_weights += 1.0;
  }

  _thresholds = model_config->getAttributeAsSample( "Thresholds" );

  ConstConfigurationPtr algs_config = model_config->getSubsection( "Algorithms", false );

  if ( ! algs_config ) {

    Log::instance()->error( CONSENSUS_LOG_PREFIX "No algorithms could be deserialized.\n" );
    return;
  }

  Configuration::subsection_list subelements = algs_config->getAllSubsections();

  Configuration::subsection_list::const_iterator end = subelements.end();
  Configuration::subsection_list::const_iterator it = subelements.begin();
  for ( ; it != end; ++it ) {

    ConstConfigurationPtr subelement = *it;

    if ( subelement->getName() == "Algorithm" ) {

      AlgorithmPtr alg = AlgorithmFactory::newAlgorithm( subelement );

      _algs.push_back( alg );

      _norms.push_back( alg->getNormalizer() );
    }
  }

  _num_algs = (int)_algs.size();

  _initialized = true;

  _done = true;
}
