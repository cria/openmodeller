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
#include <openmodeller/os_specific.hh>

LinearFeature::LinearFeature( int layerIndex ):MxFeature()
{
  _type = F_LINEAR;
  _layerIndex = layerIndex;
}

LinearFeature::LinearFeature( const ConstConfigurationPtr & config ):MxFeature()
{
  _type = F_LINEAR;
  setConfiguration( config );
}

LinearFeature::~LinearFeature() {}

Scalar 
LinearFeature::getRawVal( const Sample& sample ) const
{
  return sample[_layerIndex];
}

Scalar 
LinearFeature::getVal( const Sample& sample ) const
{
  return (getRawVal( sample ) - _min) / _scale;
}

std::string
LinearFeature::getDescription( const EnvironmentPtr& env )
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

  config->addNameValue( "Min", _min );

  config->addNameValue( "Max", _max );

  config->addNameValue( "Lambda", _lambda );

  return config;
}

void 
LinearFeature::setConfiguration( const ConstConfigurationPtr & config )
{
  int type = -1;

  try {

    type = config->getAttributeAsInt( "Type", -1 );
  }
  catch ( AttributeNotFound& e ) {

    std::string msg = "Missing 'Type' parameter in hinge feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
    UNUSED(e);
  }

  if ( type != F_LINEAR ) {

    std::string msg = "Incompatible feature type in linear feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  try {

    _layerIndex = config->getAttributeAsInt( "Ref", -1 );
  }
  catch ( AttributeNotFound& e ) {

    std::string msg = "Missing 'Ref' parameter in linear feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  try {

    _min = config->getAttributeAsDouble( "Min", 0.0 );
  }
  catch ( AttributeNotFound& e ) {

    std::string msg = "Missing 'Min' parameter in linear feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  try {

    _max = config->getAttributeAsDouble( "Max", 0.0 );
  }
  catch ( AttributeNotFound& e ) {

    std::string msg = "Missing 'Max' parameter in linear feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  _scale = _max - _min;

  _lambda = config->getAttributeAsDouble( "Lambda", 0.0 );
}


