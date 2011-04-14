/**
 * Definition of LinearFeature class
 * 
 * @author Renato De Giovanni
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

#include "linear_feature.hh"

LinearFeature::LinearFeature( Scalar layerIndex )
{
  _type = F_LINEAR;
  _layerIndex = layerIndex;
  _lower = 0.0;
  _upper = 0.0;
  _mean = 0.0;
  _std = 0.0;
  _exp = 0.0;
  _samp_exp = 0.0;
  _samp_dev = 0.0;
  _lambda = 0.0;
  _prevLambda = 0.0;
  _last_exp_change = -1;
}

LinearFeature::LinearFeature( const ConstConfigurationPtr & config )
{
  _type = F_LINEAR;
  _lower = 0.0;
  _upper = 0.0;
  _mean = 0.0;
  _std = 0.0;
  _exp = 0.0;
  _samp_exp = 0.0;
  _samp_dev = 0.0;
  _lambda = 0.0;
  _prevLambda = 0.0;
  _last_exp_change = -1;
  setConfiguration( config );
}

LinearFeature::~LinearFeature() {}

Scalar 
LinearFeature::getVal( const Sample& sample ) const
{
  return sample[_layerIndex];
}

std::string
LinearFeature::getDescription( const EnvironmentPtr& env ) const
{
  std::string desc = env->getLayerPath(_layerIndex);
  return desc;
}

ConfigurationPtr 
LinearFeature::getConfiguration() const
{
  ConfigurationPtr config( new ConfigurationImpl("Feature") );

  config->addNameValue( "Type", _type );

  config->addNameValue( "Ref", _layerIndex );

  config->addNameValue( "Lambda", _lambda );

  return config;
}

void 
LinearFeature::setConfiguration( const ConstConfigurationPtr & config )
{
  int type = config->getAttributeAsInt( "Type", -1 );

  if ( type != F_LINEAR ) {

    // TODO: throw exception
  }

  _layerIndex = config->getAttributeAsInt( "Ref", -1 );

  if ( _layerIndex == -1 ) {

    // TODO: throw exception
  }

  _lambda = config->getAttributeAsDouble( "Lambda", 0.0 );
}


