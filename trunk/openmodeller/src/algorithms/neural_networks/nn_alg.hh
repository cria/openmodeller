/**
 * Declaration of Neural Network algorithm class.
 * 
 * @author Fabrício Augusto Rodrigues (fabricio.rodrigues [at] poli . usp .br)
 * @author Alex Oshika Avilla (alex.avilla [at] poli . usp .br)
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

#ifndef _NN_ALGHH_
#define _NN_ALGHH_

#include <vector>

#include <openmodeller/om.hh>

#include "nn.h"

/**************************************************/
/****************** Neural Network ****************/

/* 
 * Algorithm to generate models using Neural Network.
 */
class NNAlgorithm : public AlgorithmImpl
{

public:

  NNAlgorithm();

  ~NNAlgorithm();

  int needNormalization();

  /** Initialise the model specifying a threshold / cutoff point.
    * This is optional (model dependent).
    * @note This method is inherited from the Algorithm class
    * @return 0 on error
    */
  int initialize();

  /** Start model execution (build the model).     
    * @note This method is inherited from the Algorithm class
    * @return 0 on error 
    */
  int iterate();

  /** Return progress so far 
    */
  float getProgress() const;

  /** Use this method to find out if the model has completed (e.g. convergence
    * point has been met. 
    * @note This method is inherited from the Algorithm class
    * @return     
    * @return Implementation specific but usually 1 for completion. 
    */
  int done() const;


  //
  // Methods used to project the model
  //
    
  /** This method is used when projecting the model.  
    * @note This method is inherited from the Algorithm class
    * @return     
    * @param Scalar *x a pointer to a vector of openModeller Scalar type (currently double). 
    *    The vector should contain values looked up on the environmental variable layers into which the mode is being projected.
    */
  Scalar getValue( const Sample& x ) const;
  

 
protected:

  virtual void _getConfiguration( ConfigurationPtr& ) const;

  virtual void _setConfiguration( const ConstConfigurationPtr& );


  bool _done;

  int _num_layers;

  int num_presences;

  int num_absences;


  // Algorithm's variables
  
  Network network;

  vector<vector<double> > vector_input;

  vector<vector<double> > vector_output;

  nn_parameter _nn_parameter;

  double *outputs;

  unsigned long amount_epoch; // Number of epoch in training

  float _progress; // Percent of training

  int converged; // Used to verify if the training by minimum error converged


  // Work with occurrences 

  OccurrencesPtr absences;

  OccurrencesPtr presences;
};


#endif
