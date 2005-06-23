/**
 * Declaration of Model class.
 * 
 * @file   Module.hh
 * @author Kevin Ruland kruland <at> ku <dot> edu
 * @date   2003-05-26
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2003 by CRIA -
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

#ifndef _MODEL_HH
#define _MODEL_HH

#include <openmodeller/om_defs.hh>

#include <openmodeller/Sampler.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/refcount.hh>

class ModelImpl;

typedef ReferenceCountedPointer<ModelImpl> Model;

class ModelImpl : public ReferenceCountedObject {

  friend class ReferenceCountedPointer<ModelImpl>;

public:
  
  virtual ~ModelImpl() {};
  
  /** Normalize the given environment.
   * @param samp Sampler to normalize.
   */
  virtual void setNormalization( const SamplerPtr& samp ) const = 0;
  
  /** Normalize the given environment.
   * @param env Environment to normalize.
   */
  virtual void setNormalization( const EnvironmentPtr& env ) const = 0;
  
  /** Compute a value in the Model
   * @param x Environment vector.
   */
  virtual Scalar getValue( const Sample& x ) const = 0;

};


#endif
