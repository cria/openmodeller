/**
 * Declaration of ScaleNormalizer class
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

#ifndef _SCALENORMALIZER_HH
#define _SCALENORMALIZER_HH

#include <openmodeller/Normalizer.hh>

class ScaleNormalizer : public Normalizer {

public:

  // Used only during deserialization
  ScaleNormalizer();

  ScaleNormalizer( Scalar min, Scalar max, bool useLayerAsRef );

  ~ScaleNormalizer();

  void computeNormalization( const ReferenceCountedPointer<const SamplerImpl>& samplerPtr );

  void normalize( Sample * samplePtr );

  ConfigurationPtr getConfiguration() const;
  
  void setConfiguration( const ConstConfigurationPtr & );

private:
  
  Scalar _min;
  Scalar _max;
  bool   _use_layer_as_ref;
  Sample _offsets;
  Sample _scales;

};

#endif
