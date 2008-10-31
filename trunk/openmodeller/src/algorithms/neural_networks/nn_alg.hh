/**
 * Declaration of Neural Network algorithm class.
 * 
 * @author Fabrício Augusto Rodrigues (fabricio.rodrigues [at] poli . usp .br)
 * @author Alex Oshika Avilla (alex.avilla [at] poli . usp .br)
 * $Id:
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
  
  /** Returns a value that represents the convergence of the algorithm
    * expressed as a number between 0 and 1 where 0 represents model
    * completion. 
    * @return 
    * @param Scalar *val 
  */
  int getConvergence( Scalar *val );

 
protected:

protected:

  virtual void _getConfiguration( ConfigurationPtr& ) const;

  virtual void _setConfiguration( const ConstConfigurationPtr& );



  bool _done;

  int _num_layers;

  int num_presences;

  int num_absences;



  //Algorithm's variables
  
  Network network;

  int num_pseudoabesences;

  int training_points_cont;

  int testing_points_cont;


  double training_vector_input[10000][500]; // [training num_patterns][_num_layers]

  double training_vector_output[10000][1]; //[training num_pattern][_nn_parameter.outp]

  double testing_vector_input[10000][500]; // [testing num_patterns][_num_layers]

  double testing_vector_output[10000][1]; //[testing num_pattern][_nn_parameter.outp]

  // Work with occurrences 
  OccurrencesPtr training_presences;

  OccurrencesPtr training_absences;

  OccurrencesPtr testing_presences;

  OccurrencesPtr testing_absences;


  double *outputs;

  nn_parameter _nn_parameter;

};


#endif
