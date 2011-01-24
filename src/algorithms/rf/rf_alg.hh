/**
 * Declaration of RF algorithm class.
 * 
 * @author Renato De Giovanni (renato [at] cria org br)
 * $Id:  $
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2010 by CRIA -
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

#ifndef _RF_ALGHH_
#define _RF_ALGHH_

#include <vector>

#include <openmodeller/om.hh>
#include <openmodeller/Random.hh>

#include "librf/instance_set.h"
#include "librf/tree.h"

/*****************************************/
/************* Random Forests ************/

/** 
 * Algorithm to generate models using Random Forests.
 */
class RfAlgorithm : public AlgorithmImpl
{
public:

  RfAlgorithm();

  ~RfAlgorithm();

  int needNormalization();
  int initialize();
  int iterate();
  float getProgress() const;
  int done() const;

  Scalar getValue( const Sample& x ) const;
  int getConvergence( Scalar * const val ) const;

protected:

  void _sampleToLine( Sample sample, stringstream& ss ) const;

  void _getConfiguration( ConfigurationPtr& ) const;

  void _setConfiguration( const ConstConfigurationPtr& );

  bool _done;

  int _num_trees;

  int _k;

  bool _initialized;

  Random _rand;

  librf::InstanceSet* _set; // only used for model creation

  vector<int> _class_weights;

  vector<librf::Tree*> _trees;
};


#endif

