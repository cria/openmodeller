/**
 * Declaration of SVM algorithm class.
 * 
 * @author Renato De Giovanni (renato [at] cria org br)
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

#ifndef _SVM_ALGHH_
#define _SVM_ALGHH_

#include <openmodeller/om.hh>

#include "svm.h"

/**************************************************/
/************* Support Vector Machines ************/

/** 
 * Algorithm to generate models using Support Vector Machines.
 */
class SvmAlgorithm : public AlgorithmImpl
{
public:

  SvmAlgorithm();

  ~SvmAlgorithm();

  int needNormalization();
  int initialize();
  int iterate();
  int done() const;

  Scalar getValue( const Sample& x ) const;
  int getConvergence( Scalar * const val ) const;

protected:

  void _getNode( svm_node * node, const Sample& sample ) const;

  void _getConfiguration( ConfigurationPtr& ) const;

  void _setConfiguration( const ConstConfigurationPtr& );

  bool _done;

  int _num_layers;

  svm_parameter _svm_parameter;
  
  svm_problem _svm_problem;

  svm_model * _svm_model;

  // Index of presence class in SVM arrays. No need to serialize it since it is
  // detected at run time. -1 means "to be determined".
  int _presence_index;
};


#endif

