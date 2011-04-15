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
#include <openmodeller/Exceptions.hh>

LinearFeature::LinearFeature( int layerIndex ):Feature()
{
  _type = F_LINEAR;
  _layerIndex = layerIndex;
}

LinearFeature::LinearFeature( const ConstConfigurationPtr & config ):Feature()
{
  _type = F_LINEAR;
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
  std::string desc("L");
  std::string path = env->getLayerPath(_layerIndex);
  desc.append( path.substr( path.rfind("/") + 1 ) );
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

    std::string msg = "Incompatible feature type in linear feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  _layerIndex = config->getAttributeAsInt( "Ref", -1 );

  if ( _layerIndex == -1 ) {

    std::string msg = "Missing 'Ref' parameter in linear feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  _lambda = config->getAttributeAsDouble( "Lambda", 0.0 );
}


