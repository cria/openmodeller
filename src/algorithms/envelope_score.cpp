/**
 * Declaration of Envelope Score Algorithm - Yesson & Sutton 2007.
 * 
 * @author EnvelopeScore implemantation by Tim Sutton and Chris Yesson
 * @date 2007-08-22
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Based on Bioclim alg 
 * Copyright(c) 2004 by CRIA -
 * Centro de Referência em Informação Ambiental
 *
 * http://www.cria.org.br
 * 
 * Envelope Score implementation (c) 2007 Chris Yesson and Tim Sutton 
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

#include "envelope_score.hh"

#include <openmodeller/Configuration.hh>

#include <openmodeller/Exceptions.hh>

#include <string.h>
#include <stdio.h>
#include <math.h>

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 0


/*************************************/
/*** Algorithm parameters metadata ***/

//static AlgParamMetadata parameters[NUM_PARAM] = {};


/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "ENVSCORE",   // Id.
  "Envelope Score",   // Name.
  "0.1",       // Version.

  // Overview
  "Uses the minimum and maximum observed value for each\
 environmental variable to define bioclimatic envelopes.\
 Probability of occurrence for a point is determined by\
 the number of environmental variables within which the\
 environmental values at the point fall into the min/max\
 criteria.",
  // Description.
  "Implements a lax Bioclimatic Envelope Algorithm. For\
 each given environmental variable the algorithm finds\
 the minimum and maximum at all occurrence sites. During\
 model projection, the probability of occurrences is\
 determined as:\n\n\
 p = layers within min-max threshold / number of layers\n\n\
 Thus choosing a threshold of p=1 gives you the same model\
 output as the original Bioclim model taking both the\
 Suitable & Marginal classes as predicted presences.\
 The Envelope Score algorithm is equivalent to the\
 inclusive 'OR' implementation of Bioclim described\
 in Pineiro et al (2007).",

  "Nix, H. A.",  // Author

  // Bibliography.
  "Nix, H.A. (1986) A biogeographic analysis of Australian elapid\
 snakes. In: Atlas of Elapid Snakes of Australia. (Ed.) R. Longmore,\
 pp. 4-15. Australian Flora and Fauna Series Number 7. Australian\
 Government Publishing Service: Canberra.\n\n\
 Piñeiro, R., Aguilar, J. F., Munt, D. D. & Feliner, G. N. (2007)\
 Ecology matters: Atlantic-Mediterranean disjunction in the sand-dune\
 shrub Armeria pungens (Plumbaginaceae).\
 Molecular Ecology. 16, 2155-2171.",

  "Tim Sutton and Chris Yesson",             // Code author.
  "tim [at] linfiniti.com", // Code author's contact.

  0,  // Does not accept categorical data.
  0,  // Does not need (pseudo)absence points.

  NUM_PARAM   // Algorithm's parameters.

};



/****************************************************************/
/****************** Algorithm's factory function ****************/

OM_ALG_DLL_EXPORT
AlgorithmImpl *
algorithmFactory()
{
  return new EnvelopeScore();
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

EnvelopeScore::EnvelopeScore() :
  AlgorithmImpl( &metadata ),
  _done( false ),
  _minimum(),
  _maximum()
{ }


/******************/
/*** destructor ***/

EnvelopeScore::~EnvelopeScore()
{
}


/******************/
/*** initialize ***/
int
EnvelopeScore::initialize()
{
  
  // Number of independent variables.
  int dim = _samp->numIndependent();
  Log::instance()->info( "Reading %d-dimensional occurrence points.\n", dim );

  // Check the number of sampled points.
  int npnt = _samp->numPresence();
  if (  npnt < 1 ) {
    Log::instance()->error( "EnvelopeScore needs at least 1 point inside the mask!\n" );
    return 0;
  }

  Log::instance()->info( "Using %d points to find the bioclimatic envelope.\n", npnt );

  computeStats( _samp->getPresences() );

  _done = true;

  return 1;
}


/***************/
/*** iterate ***/
int
EnvelopeScore::iterate()
{
  return 1;
}


/************/
/*** done ***/
int
EnvelopeScore::done() const
{
  // This is not an iterative algorithm.
  return _done;
}


/*****************/
/*** get Value ***/
Scalar
EnvelopeScore::getValue( const Sample& x ) const
{

  unsigned int myMatchCount=0;
  unsigned int myLayerCount = x.size();

  for( unsigned int i=0; i<x.size(); i++) {

    if ( (x[i] >= _minimum[i]) && (x[i] <= _maximum[i]) ) {
      ++myMatchCount;
    }
  }
  if (myMatchCount==0) {
    return 0;
  }
  Scalar myProbability = myMatchCount / static_cast<double>(myLayerCount)  ;

  return myProbability;
}


/***********************/
/*** get Convergence ***/
int
EnvelopeScore::getConvergence( Scalar * const val ) const
{
  *val = 1.0;
  return 1;
}


/*******************/
/*** get Minimum ***/
void
EnvelopeScore::computeStats( const OccurrencesPtr& occs )
{

  // Compute minimum and maximum.
  {
    OccurrencesImpl::const_iterator oc = occs->begin();
    OccurrencesImpl::const_iterator end = occs->end();

    // Initialize _minimum and _maximum to the values of the first point
    // and increment to get it out of the loop.
    Sample const & sample = (*oc)->environment();
    _minimum = sample;
    _maximum = sample;
    
    ++oc;
    
    // For each Occurrence, update the statistics for _minimum and
    // _maximum.
    while ( oc != end ) {
      
      Sample const& sample = (*oc)->environment();
      
      _minimum &= sample;
      _maximum |= sample;

      ++oc;
    }

  }

}

/****************************************************************/
/****************** configuration *******************************/
void
EnvelopeScore::_getConfiguration( ConfigurationPtr& config ) const
{
  if ( !_done )
    return;

  ConfigurationPtr model_config( new ConfigurationImpl("EnvelopeScore") );
  config->addSubsection( model_config );

  model_config->addNameValue( "Minimum", _minimum );
  model_config->addNameValue( "Maximum", _maximum );

}

void
EnvelopeScore::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection("EnvelopeScore");

  if (!model_config)
    return;

  _done = true;

  _minimum = model_config->getAttributeAsSample( "Minimum" );
  _maximum = model_config->getAttributeAsSample( "Maximum" );

  return;
}

/********************/
/*** log Envelope ***/
void
EnvelopeScore::logEnvelope()
{
  Log::instance()->info( "Envelope with %d dimensions (variables).\n\n", _minimum.size() );

  for ( unsigned int i = 0; i < _minimum.size(); i++ )
    {
      Log::instance()->info( "Variable %02d:", i );
      Log::instance()->info( " Minimum  : %f\n", _minimum[i] );
      Log::instance()->info( " Maximum  : %f\n", _maximum[i] );
      Log::instance()->info( "\n" );
    }
}

