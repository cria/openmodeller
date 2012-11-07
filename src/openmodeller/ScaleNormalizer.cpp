/**
 * Definition of ScaleNormalizer class
 * 
 * @author Renato De Giovanni
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

#include <openmodeller/ScaleNormalizer.hh>
#include <openmodeller/Sampler.hh>
#include <openmodeller/Log.hh>

using namespace std;

/*******************/
/*** constructor ***/
ScaleNormalizer::ScaleNormalizer( Scalar min, Scalar max, bool useLayerAsRef ) :
  _min( min ),
  _max( max ),
  _use_layer_as_ref( useLayerAsRef ),
  _offsets(),
  _scales()
{
}

/*******************/
/*** constructor ***/
ScaleNormalizer::ScaleNormalizer() :
  _min( 0.0 ),
  _max( 1.0 ),
  _use_layer_as_ref( true ),
  _offsets(),
  _scales()
{
}

/******************/
/*** destructor ***/
ScaleNormalizer::~ScaleNormalizer() {}

/****************/
/*** get Copy ***/
Normalizer * ScaleNormalizer::getCopy() {

  return new ScaleNormalizer( *this );
}

/*****************************/
/*** compute Normalization ***/
void ScaleNormalizer::computeNormalization( const ReferenceCountedPointer<const SamplerImpl>& samplerPtr ) {

  int dim = samplerPtr->numIndependent();
  Sample min(dim), max(dim);

  bool get_minmax_from_sampler = true;

  if ( _use_layer_as_ref ) {

    Log::instance()->debug( "Using min/max from layer as reference for normalization.\n");

    EnvironmentPtr envPtr = samplerPtr->getEnvironment();

    if ( envPtr ) { 

      envPtr->getMinMax( &min, &max );

      get_minmax_from_sampler = false;
    }
    else {

      Log::instance()->warn( "Sampler has no environment. ScaleNormalizaer will get min/max directly from samples instead.\n");
    }
  }
  else {

    Log::instance()->debug( "Using min/max from input samples as reference for normalization.\n");
  }

  if ( get_minmax_from_sampler ) {

    samplerPtr->getMinMax( &min, &max );
  }

  _scales.resize(dim);
  _offsets.resize(dim);

  for ( int i = 0; i < dim; ++i ) {

    if ( max[i] == min[i] ) {

      // Avoid zero division
      _scales[i] = 1.0;
    }
    else {

      _scales[i] = (_max - _min) / (max[i] - min[i]);
    }

    _offsets[i] = _min - _scales[i] * min[i];
  }
}

/*****************/
/*** normalize ***/
void ScaleNormalizer::normalize( Sample * samplePtr ) {

  if ( samplePtr->size() != 0 ) {

    *samplePtr *= _scales;
    *samplePtr += _offsets;
  }
}

/*************************/
/*** get configuration ***/
ConfigurationPtr ScaleNormalizer::getConfiguration() const {

  ConfigurationPtr config( new ConfigurationImpl("Normalization") );

  config->addNameValue( "Class", "ScaleNormalizer" );

  int temp_val = (_use_layer_as_ref) ? 1 : 0;

  config->addNameValue( "UseLayerAsRef", temp_val );

  config->addNameValue( "Min", _min );
  config->addNameValue( "Max", _max );
  
  config->addNameValue( "Offsets", _offsets );
  config->addNameValue( "Scales", _scales );

  return config;
}

/*************************/
/*** set configuration ***/
void ScaleNormalizer::setConfiguration( const ConstConfigurationPtr &config ) {

  int temp_val = config->getAttributeAsInt( "UseLayerAsRef", 1 );

  _use_layer_as_ref = (temp_val == 1) ? true : false;

  _min = config->getAttributeAsDouble( "Min", 0.0 );
  _max = config->getAttributeAsDouble( "Max", 1.0 );

  _offsets = config->getAttributeAsSample( "Offsets" );
  _scales  = config->getAttributeAsSample( "Scales" );
}
