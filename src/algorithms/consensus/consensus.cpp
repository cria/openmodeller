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

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 0

#define CONSENSUS_LOG_PREFIX "Consensus: "

/******************************/
/*** Algorithm's parameters ***/

//static AlgParamMetadata parameters[NUM_PARAM] = {};


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

  NUM_PARAM // Algorithm's parameters.
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
  Log::instance()->error( CONSENSUS_LOG_PREFIX "This algorithm is still not prepared to run interactively. It only works by loading an existing serialized model.\n" );
  return 0;
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
