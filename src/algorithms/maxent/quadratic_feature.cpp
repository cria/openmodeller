/**
 * Definition of QuadraticFeature class
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

#include "quadratic_feature.hh"

QuadraticFeature::QuadraticFeature( Scalar layerIndex ):Feature()
{
  _type = F_QUADRATIC;
  _layerIndex = layerIndex;
}

QuadraticFeature::QuadraticFeature( const ConstConfigurationPtr & config ):Feature()
{
  _type = F_QUADRATIC;
  setConfiguration( config );
}

QuadraticFeature::~QuadraticFeature() {}

Scalar 
QuadraticFeature::getVal( const Sample& sample ) const
{
  return sample[_layerIndex]*sample[_layerIndex];
}

std::string
QuadraticFeature::getDescription( const EnvironmentPtr& env ) const
{
  std::string desc("Q");
  std::string path = env->getLayerPath(_layerIndex);
  desc.append( path.substr( path.rfind("/") + 1 ) );
  return desc;
}

ConfigurationPtr 
QuadraticFeature::getConfiguration() const
{
  ConfigurationPtr config( new ConfigurationImpl("Feature") );

  config->addNameValue( "Type", _type );

  config->addNameValue( "Ref", _layerIndex );

  config->addNameValue( "Lambda", _lambda );

  return config;
}

void 
QuadraticFeature::setConfiguration( const ConstConfigurationPtr & config )
{
  int type = config->getAttributeAsInt( "Type", -1 );

  if ( type != F_QUADRATIC ) {

    // TODO: throw exception
  }

  _layerIndex = config->getAttributeAsInt( "Ref", -1 );

  if ( _layerIndex == -1 ) {

    // TODO: throw exception
  }

  _lambda = config->getAttributeAsDouble( "Lambda", 0.0 );
}


