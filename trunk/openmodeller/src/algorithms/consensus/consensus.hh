/**
 * Declaration of Consensus algorithm class.
 * 
 * @author Renato De Giovanni (renato [at] cria org br)
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

#ifndef _CONSENSUS_ALGHH_
#define _CONSENSUS_ALGHH_

#include <vector>
#include <openmodeller/om.hh>

/**********************************************/
/************* Consensus algorithm ************/

/** 
 * Algorithm to generate models with other algorithms.
 */
class ConsensusAlgorithm : public AlgorithmImpl
{
public:

  ConsensusAlgorithm();

  ~ConsensusAlgorithm();

  int needNormalization();
  int initialize();
  int iterate();
  float getProgress() const;
  int done() const;

  Scalar getValue( const Sample& x ) const;
  int getConvergence( Scalar * const val ) const;

protected:

  void _getConfiguration( ConfigurationPtr& ) const;

  void _setConfiguration( const ConstConfigurationPtr& );

  bool _done;

  bool _initialized;

  Sample _weights;
  Scalar _sum_weights;
  Sample _thresholds;

  vector<AlgorithmPtr> _algs;
  vector<Normalizer*> _norms;
};


#endif

