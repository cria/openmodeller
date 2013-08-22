/**
 * This is the Broken-Stick cutoff implementation of csm 
 * 
 * @file csmbs
 * @author Tim Sutton (t.sutton@reading.ac.uk)
 * @date   2003-09-12
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2003 by CRIA -
 * Centro de Referencia em Informacao Ambiental
 *
 * http://www.cria.org.br
 * 
 * Copyright(c) Neil Caithness 2004 (Model Methodology)
 * Copyright(c) Tim Sutton 2004 (C++ implementation)
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

#ifndef CSMBS_H
#define CSMBS_H

#include "csm.hh"
#include <gsl/gsl_rng.h>

/**
 * CsmBS = Csm broken-stick cutoff for discarded components
 * @author Tim Sutton
*/
class CsmBS : public Csm
{
public:

  /** Constructor for Csm */
  CsmBS();
  /** This is the descructor for the Csm class */
  ~CsmBS();
  /** Initialise the model specifying a threshold / cutoff point.
   * Any model definition building stuff is done here.
   * This is optional (model dependent).
   * @note This method is inherited from the Algorithm class
   * @return 0 on error
   */
  int initialize();


protected:
  /** generate a matrix populated with random numbers between 0 and 1 */
  gsl_matrix * createRandomMatrix(int size1, int size2);

  /**
  shuffle / randomise the row ordering in each column
  -------------------------------------------------------------------------------------        
  e.g. before:      e.g. after
  33 | 56 | 88  |   12| 12 | 34
  34 | 12 | 63  |   34 | 44 | 63
  12 | 44 | 34  |   33 | 56 | 88
  -------------------------------------------------------------------------------------        
  Exchanging elements (GSL Documentation)

  The following function can be used to exchange, or permute, the elements of a vector.
  Function: int gsl_vector_swap_elements (gsl_vector * v, size_t i, size_t j)
  This function exchanges the i-th and j-th elements of the vector v in-place. 
  Function: int gsl_vector_reverse (gsl_vector * v)
  This function reverses the order of the elements of the vector v. 
  -------------------------------------------------------------------------------------        
  */
  gsl_matrix * randomiseColumns(gsl_matrix * original_matrix);

  /** Discard unwanted components.
   * This method uses the broken-stick cutoff.
   * @note This method must be called after center
   * @return 0 on error    
   */
  int discardComponents();


  /** How many standard deviations should be added to the random derived
   * eigenvalues when doing discard components */
  float numberOfStdDevsFloat;
  /**Increase this number to increase randomness of component selection eigen vector
   */
  int numberOfRandomisationsInt;
  /** A gsl random number generator for creating randomness randomly... */
  gsl_rng * _randomNumberGenerator;

};

#endif
