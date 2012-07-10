/**
 * Definition of ThresholdFeature class
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

#include "threshold_feature.hh"
#include <openmodeller/Exceptions.hh>
#include <openmodeller/os_specific.hh>

#include <sstream>
#include <iomanip>

ThresholdFeature::ThresholdFeature( int layerIndex, Scalar threshold ):MxFeature()
{
  _type = F_THRESHOLD;
  _layerIndex = layerIndex;
  _postgen = true;
  _t = threshold;
}

ThresholdFeature::ThresholdFeature( const ConstConfigurationPtr & config ):MxFeature()
{
  _type = F_THRESHOLD;
  _postgen = true;
  setConfiguration( config );
}

ThresholdFeature::~ThresholdFeature() {}

Scalar 
ThresholdFeature::getRawVal( const Sample& sample ) const
{
  return (sample[_layerIndex] > _t ) ? 1.0 : 0.0;
}

Scalar 
ThresholdFeature::getVal( const Sample& sample ) const
{
  return getRawVal(sample);
}

std::string
ThresholdFeature::getDescription( const EnvironmentPtr& env )
{
  if ( _description.size() ) {

    return _description;
  }

  _description = "T";

  std::ostringstream strs;
  strs << std::fixed << std::setprecision(13) << _t;
  std::string t_str = strs.str();
  // Remove trailing zeros
  size_t found = t_str.find(".");
  if ( found != std::string::npos) {

    size_t len = t_str.size();
    while ( t_str[len-1] == '0' ) {

      t_str = t_str.substr(0, len-1);
      --len;
    }
  }
  _description.append(t_str);
  _description.append("<");
  std::string path = env->getLayerPath(_layerIndex);
  _description.append( path.substr( path.rfind("/") + 1 ) );

  return _description;
}

ConfigurationPtr 
ThresholdFeature::getConfiguration() const
{
  ConfigurationPtr config( new ConfigurationImpl("Feature") );

  config->addNameValue( "Type", _type );

  config->addNameValue( "Ref", _layerIndex );
  config->addNameValue( "Threshold", _t );

  config->addNameValue( "Lambda", _lambda );

  return config;
}

void 
ThresholdFeature::setConfiguration( const ConstConfigurationPtr & config )
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

  if ( type != F_THRESHOLD ) {

    std::string msg = "Incompatible feature type in threshold feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
  }

  try {

    _layerIndex = config->getAttributeAsInt( "Ref", -1 );
  }
  catch ( AttributeNotFound& e ) {

    std::string msg = "Missing 'Ref' parameter in threshold feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
    UNUSED(e);
  }

  try {

    _t = config->getAttributeAsDouble( "Threshold", 0.0 );
  }
  catch ( AttributeNotFound& e ) {

    std::string msg = "Missing 'Threshold' parameter in threshold feature deserialization.\n";
    Log::instance()->error( msg.c_str() );
    throw InvalidParameterException( msg );
    UNUSED(e);
  }

  _lambda = config->getAttributeAsDouble( "Lambda", 0.0 );
}


