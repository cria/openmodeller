/**
 * Declaration of ProductFeature class
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

#ifndef _PRODUCTFEATURE_HH
#define _PRODUCTFEATURE_HH

#include "feature.hh"

class dllexp ProductFeature : public Feature {

public:

    ProductFeature( Scalar layerIndex1, Scalar layerIndex2 );

  ProductFeature( const ConstConfigurationPtr & config );

  ~ProductFeature();

  Scalar getVal( const Sample& sample ) const;

  bool isBinary() const { return false; }

  std::string getDescription( const EnvironmentPtr& env ) const;

  ConfigurationPtr getConfiguration() const;
  
  void setConfiguration( const ConstConfigurationPtr & config );

private:
  
  Scalar _layerIndex1;
  Scalar _layerIndex2;
};

#endif
