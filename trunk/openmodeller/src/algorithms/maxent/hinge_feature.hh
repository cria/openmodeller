/**
 * Declaration of HingeFeature class
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

#ifndef _HINGEFEATURE_HH
#define _HINGEFEATURE_HH

#include "feature.hh"

class HingeFeature : public Feature {

public:
  HingeFeature( int layerIndex, Scalar min, Scalar max );
  HingeFeature( const ConstConfigurationPtr & config );
  ~HingeFeature();

  Scalar getVal( const Sample& sample ) const;
  bool isBinary() const;

  bool postGenerated() const {
    return true;
  }

  std::string getDescription( const EnvironmentPtr& env ) const;
  ConfigurationPtr getConfiguration() const;
  void setConfiguration( const ConstConfigurationPtr & config );

private:
  int _layerIndex;
  Scalar _min;
  Scalar _max;
};

#endif
