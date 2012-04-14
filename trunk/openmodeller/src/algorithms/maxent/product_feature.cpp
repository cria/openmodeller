/**
 * Definition of ProductFeature class
 * 
 * @author Renato De Giovanni
 * $Id: $
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

#include "product_feature.hh"
#include <openmodeller/Exceptions.hh>

ProductFeature::ProductFeature( int layerIndex1, int layerIndex2 ):Feature()
{
  _type = F_PRODUCT;
  _layerIndex1 = layerIndex1;
  _layerIndex2 = layerIndex2;
}

ProductFeature::ProductFeature( const ConstConfigurationPtr & config ):Feature()
{
  _type = F_PRODUCT;
  setConfiguration( config );
}

ProductFeature::~ProductFeature() {}

Scalar 
ProductFeature::getRawVal( const Sample& sample ) const
{
  return sample[_layerIndex1]*sample[_layerIndex2];
}

Scalar 
ProductFeature::getVal( const Sample& sample ) const
{
  return (getRawVal( sample ) - _min) / _scale;
}

bool ProductFeature::isActive() const {
  return _active;
}

std::string
ProductFeature::getDescription( const EnvironmentPtr& env )
{
  std::string desc("P");
  std::string path1 = env->getLayerPath(_layerIndex1);
  std::string path2 = env->getLayerPath(_layerIndex2);
  desc.append( path1.substr( path1.rfind("/") + 1 ) ).append("X").append( path2.substr( path2.rfind("/") + 1 ) );
  return desc;
}

ConfigurationPtr 
ProductFeature::getConfiguration() const
{
  ConfigurationPtr config( new ConfigurationImpl("Feature") );

  config->addNameValue( "Type", _type );

  config->addNameValue( "Ref1", _layerIndex1 );
  config->addNameValue( "Ref2", _layerIndex2 );

  config->addNameValue( "Min", _min );

  config->addNameValue( "Max", _max );

  config->addNameValue( "Lambda", _lambda );

  return config;
}

void 
ProductFeature::setConfiguration( const ConstConfigurationPtr & config )
{
  int type = config->getAttributeAsInt( "Type", -1 );

  if ( type != F_PRODUCT ) {

    std::string msg = "Incompatible feature type in product feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  _layerIndex1 = config->getAttributeAsInt( "Ref1", -1 );

  if ( _layerIndex1 == -1 ) {

    std::string msg = "Missing 'Ref1' parameter in product feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  _layerIndex2 = config->getAttributeAsInt( "Ref2", -1 );

  if ( _layerIndex2 == -1 ) {

    std::string msg = "Missing 'Ref2' parameter in product feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  _min = config->getAttributeAsDouble( "Min", -1 );

  if ( _min == -1 ) {

    std::string msg = "Missing 'Min' parameter in product feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  _max = config->getAttributeAsDouble( "Max", -1 );

  if ( _max == -1 ) {

    std::string msg = "Missing 'Max' parameter in product feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  _scale = _max - _min;

  _lambda = config->getAttributeAsDouble( "Lambda", 0.0 );
}


