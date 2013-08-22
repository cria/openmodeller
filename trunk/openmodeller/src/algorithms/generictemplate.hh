/**
 * This is a generic template intended to be used as the basis for
 * creating new algorithms.
 * 
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
 
#ifndef GENERIC_TEMPLATE_H
#define GENERIC_TEMPLATE_H

#include <om.hh>

/**
This is a simple template for creating new algorithms.
Note it does not actually run a model - rather it demonstrates
how to do a new algorithm implementation.

@author Tim Sutton
*/
class GenericTemplate : public Algorithm
{
public:

    /** Constructor for GenericTemplate */
    GenericTemplate();
    /** This is the descructor for the GenericTemplate class */
    ~GenericTemplate();
    
    public:

  //
  // Methods used to build the model
  //
  
  /** Initialise the model specifying a threshold / cutoff point.
    * This is optional (model dependent).
    * @note This method is inherited from the Algorithm class
    * @return 0 on error
    */
  int initialize( int ncycle );
  
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
    * @param x a pointer to a vector of openModeller Scalar type (currently double). The vector should contain values looked up on the environmental variable layers into which the mode is being projected. */
  Scalar getValue( Scalar const *x ) const;
  
  /** Returns a value that represents the convergence of the algorithm
    * expressed as a number between 0 and 1 where 0 represents model
    * completion. 
    * @return 
    * @param val 
  */
  int getConvergence( Scalar *val );


private:
  
  /** This member variable is used to indicate whether the model 
    * building process has completed yet. */
  int    f_done;
  
  Scalar *f_avg;  ///< Average related to occurrence points.
  int     f_dim;  ///< Dimension of environmental space.
  //set the class member that holds the number of occurences
  int f_localityCount;
};

#endif
