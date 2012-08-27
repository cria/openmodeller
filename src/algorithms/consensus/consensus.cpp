/**
 * Definition of Consensus algorithm class.
 * 
 * @author Renato De Giovanni (renato [at] cria org br)
 * $Id:  $
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

#define NUM_PARAM 5

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
    "BIOCLIM(StandardDeviationCutoff=0.674)" // Parameter's typical (default) value.
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
    "ENVSCORE" // Parameter's typical (default) value.
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
    "" // Parameter's typical (default) value.
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
    "" // Parameter's typical (default) value.
  },
};

/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "CONSENSUS", 	// Id.
  "Consensus",  // Name.
  "0.1",       	// Version.

  // Overview
  "Consensus",

  // Description.
  "Consensus",

  "", // Algorithm author.
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
  _initialized( false )
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

  if ( _algs.size() == 0 ) {

    Log::instance()->error( CONSENSUS_LOG_PREFIX "Consensus needs at least one algorithm. No algorithm could be instantiated based on the parameters.\n" );
    return 0;
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

    return true;
  }

  // There are parentheses

  // extract ID
  std::string alg_id = alg_str.substr(0, ini_p);

  Log::instance()->info( CONSENSUS_LOG_PREFIX "ID is %s\n", alg_id.c_str() );

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

  Log::instance()->info( CONSENSUS_LOG_PREFIX "PARAMS is %s\n", alg_params.c_str() );

  vector<string> pairs;
  stringstream ss(alg_params);
  string pair;
  int nparam = 0;
  while ( getline(ss, pair, ',') ) {

    pairs.push_back(pair);
    ++nparam;
  }

  AlgParameter *param = new AlgParameter[nparam];

  for ( int i = 0; i < nparam; i++, param++) {

    size_t eq = pairs[i].find( "=" );

    if ( eq == string::npos or eq == 0 ) {

      Log::instance()->error( CONSENSUS_LOG_PREFIX "Algorithm parameter failed to match key=value pair format.\n" );
      delete[] param;
      return false;
    }

    std::string param_id = pairs[i].substr(0, eq);
    std::string param_val = pairs[i].substr(eq+1);

    param->setId( param_id );
    param->setValue( param_val.c_str() );

    Log::instance()->info( CONSENSUS_LOG_PREFIX "Setting %s = %s\n", param_id.c_str(), param_val.c_str() );
  }

  AlgorithmPtr alg = AlgorithmFactory::newAlgorithm( alg_id );

  alg->setParameters( nparam, param );

  _algs.push_back( alg );

  delete[] param;

  return true;
}

/***************/
/*** iterate ***/
int
ConsensusAlgorithm::iterate()
{
  return 1;
}

/********************/
/*** get Progress ***/
float ConsensusAlgorithm::getProgress() const
{
  return 1.0;
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

  for ( int i=0; i < (int)_algs.size(); i++ ) {

    Sample y( x );

    if ( _norms[i] ) {

      _norms[i]->normalize( &y );
    }

    v = _algs[i]->getValue( x );

    if ( v >= _thresholds[i] ) {

      prob +=  v * _weights[i];
    }
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

  model_config->addNameValue( "Weights", _weights );

  model_config->addNameValue( "Thresholds", _thresholds );

  ConfigurationPtr algs_config( new ConfigurationImpl("Algorithms") );
  model_config->addSubsection( algs_config );

  for ( int i=0; i < (int)_algs.size(); i++ ) {

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

  _weights = model_config->getAttributeAsSample( "Weights" );

  _sum_weights = 0.0;

  for ( int i=0; i < (int)_weights.size(); i++ ) {

    _sum_weights += _weights[i];
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

  _initialized = true;

  _done = true;
}
