/**
 * Declaration of Normalizer Interface
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

#ifndef _NORMALIZER_HH
#define _NORMALIZER_HH

#include <openmodeller/Configurable.hh>
#include <openmodeller/Sample.hh>
// Note: avoid including Sampler.hh because of cross reference between the two classes. 
//       However it needs to be included in the .cpp of all implementations of this interface.

class SamplerImpl; // forward declaration

class Normalizer : public Configurable {

public:

  // Note: not using ConstSamplerPtr to avoid compilation problems. It was 
  //       replaced here by the associated definition. See the ConstSamplerPtr 
  //       typedef in Sampler.hh.
  virtual void computeNormalization( const ReferenceCountedPointer<const SamplerImpl>& samplerPtr ) = 0;

  virtual void normalize( Sample * samplePtr ) = 0;

  // Should return a pointer to copy of the object
  virtual Normalizer * getCopy() = 0;
};

#endif
