/**
 * Definition of HingeFeature class
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

#include "hinge_feature.hh"
#include <openmodeller/Exceptions.hh>

HingeFeature::HingeFeature( int layerIndex, Scalar min, Scalar max ):Feature()
{
  _type = F_HINGE;
  _layerIndex = layerIndex;
  _min = min;
  _max = max;
  _scale = _max - _min;

}

HingeFeature::HingeFeature( const ConstConfigurationPtr & config ):Feature()
{
  _type = F_HINGE;
  setConfiguration( config );
}

HingeFeature::~HingeFeature() {}

Scalar 
HingeFeature::getRawVal( const Sample& sample ) const
{
  double val = sample[_layerIndex];
  return (val > _min) ? (val-_min)/(_scale) : 0.0;
}

Scalar 
HingeFeature::getVal( const Sample& sample ) const
{
  return getRawVal(sample);
}

std::string
HingeFeature::getDescription( const EnvironmentPtr& env )
{
  std::string desc("H");
  std::string path = env->getLayerPath(_layerIndex);
  desc.append( path.substr( path.rfind("/") + 1 ) );
  return desc;
}

ConfigurationPtr 
HingeFeature::getConfiguration() const
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
HingeFeature::setConfiguration( const ConstConfigurationPtr & config )
{
  int type = config->getAttributeAsInt( "Type", -1 );

  if ( type != F_HINGE ) {

    std::string msg = "Incompatible feature type in hinge feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  _layerIndex = config->getAttributeAsInt( "Ref", -1 );

  if ( _layerIndex == -1 ) {

    std::string msg = "Missing 'Ref' parameter in hinge feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  _min = config->getAttributeAsDouble( "Min", -1 );

  if ( _min == -1 ) {

    std::string msg = "Missing 'Min' parameter in hinge feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  _max = config->getAttributeAsDouble( "Max", -1 );

  if ( _max == -1 ) {

    std::string msg = "Missing 'Max' parameter in hinge feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  _scale = _max - _min;

  _lambda = config->getAttributeAsDouble( "Lambda", 0.0 );
}


