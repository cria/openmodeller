/**
 * Declaration of Jackknife class 
 * 
 * @author Fabrício Rodrigues (fabricio . poliusp at gmail . com)
 * @author Renato De Giovanni (renato at cria . org . br)
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c), CRIA, Reference Center on Environmental Information 
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

#ifndef _PRE_JACKKNIFE_
#define _PRE_JACKKNIFE_

#include "PreAlgorithm.hh"

#include <openmodeller/Sampler.hh>
#include <openmodeller/Algorithm.hh>

/**
 * Jackknife class
 */
class dllexp PreJackknife : public PreAlgorithm 
{
public:
  /** 
   * Default constructor.
   */
  PreJackknife();

  /** 
   * Destructor.
   */
  ~PreJackknife();

  //Return description about the algorithm
  string getDescription() const { return "This technique first generates a model \
to calculate a particular measure (accuracy) for the entire layerset. Then for \
each layer a new model is generated without that particular layer and the accuracy \
is calculated again. All models are trained with the same set of points that are \
randomly selected from the specified occurrence points given the specified proportion, \
and the accuracy is calculated with the remaining testing points. The algorithm can \
also be specified as a parameter."; }

  //Checks if the supplied parameters fits the requirements of PRE algorithm implementation.
  //return true if the parameters are OK. false if not.
  bool checkParameters( const PreParameters& parameters ) const;

  //Runs the current algorithm implementation.
  //return true if OK. false on error.
  bool runImplementation();

  //Reset the params state to the params_ state.
  void resetState( PreParameters& params );

  //get input parameters
  void getAcceptedParameters ( stringMap& info );

  //get output information
  void getLayersetResultSpec ( stringMap& info);

  //get output information for each layer
  void getLayerResultSpec ( stringMap& info);

};

#endif
