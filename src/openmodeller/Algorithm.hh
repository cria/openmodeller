/**
 * Declaration of Algorithm class.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
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


#ifndef _ALGORITHMHH_
#define _ALGORITHMHH_


#include <om_sampler.hh>
#include <om_algorithm_metadata.hh>

class AlgParameter;


/**
 * Declares the function and function pointer to be used for the
 * algorithms Dynamic Linked to instantiate an object of its own
 * class. It is in C to avoid C++ name signatures.
 */
class Algorithm;
typedef Algorithm *(*TAlgFactory)();
extern "C"
{
#ifdef WIN32
__declspec( dllexport )
#endif
  Algorithm *algorithmFactory();
}



/****************************************************************/
/*************************** Algorithm **************************/

/** 
 * Base class for all distribution modeling algorithms. Provide 
 * methods to all derived classes so that they can do data sampling
 * and access the environmental layers to build the model.
 */
class Algorithm
{
public:

  Algorithm( AlgMetadata *metadata );
  virtual ~Algorithm();

  /** Set the sampler object. Need to be called before start
   *  the algorithm initialization or iteration.
   */
  void setSampler( Sampler *samp );

  /** Configure the algorithm's parameters.
   * @param nparam Number of parameters.
   * @param param Vector with all parameters. The address 'param'
   *  points to must exists while this object is used.
   */
  void setParameters( int nparam, AlgParameter *param );

  char *getID()   { return _metadata ? _metadata->id : 0; }


  /** The algorithm should return != 0 if it needs normalization
   *  of environmental variables (non categorical ones).
   */
  virtual int needNormalization( Scalar *min, Scalar *max )
  { return 0; }


  /** Initiate a new training.
   */
  virtual int initialize() { return 1; }

  /** One step further on the training. Return 0 if something 
   * wrong happened.
   */
  virtual int iterate() = 0;

  /** Return != 0 if algorithm finished. */
  virtual int done() = 0;

  /** Read algorithm state. */
  virtual int load( int fd )  { return 0; }

  /** Store algorithm state. */
  virtual int save( int fd )  { return 0; }

  /** The algorithm must return the occurrence probability at
   * the given environment conditions.
   *
   * @param x Environmental conditions.
   * 
   * @return The occurrence probability in the range [0,1].
   */
  virtual Scalar getValue( Scalar *x ) = 0;


  /** Returns the algorithm's convergence value at the moment */
  virtual int getConvergence( Scalar *val )  { return 0; }

  AlgMetadata *getMetadata()  { return _metadata; }


protected:

  /**
   *  All protected methods should not be called before
   *  initialized(). This is because the sampler object _samp
   *  is not initialized in the constructor and the algorithm's
   *  parameters were not set yet.
   */

  /** @return A vertor describing the environmental variable
   *  types: 0 for continuous and 1 for categorical.
   */
  int *getCategories()   { return _categ; }

  /** Return != 0 if variable "i" in the domain is categorical.
   *  Obs: i = 0 is the probability of occurrrence, therefore it
   *   is never categorical. This method can only be called after
   *   setSampler().
   */
  int isCategorical( int i )  { return _categ[i]; }

  /** Dimension of problem domain: number of independent 
   *  variables (climatic + soil) added to the number of
   *  dependent variables (occurrence prediction). */
  int dimDomain()  { return _samp ? _samp->numIndependent() : 0; }

  /** Returns an algorithm parameter of string type.
   *   
   *  @param name Parameters name.
   *  @param value Filled with parameter's value.
   *  @return Zero if the parameter does not exists or the
   *   parameters were not set yet.
   **/
  int getParameter( char *name, char **value );

  /** Returns an algorithm parameter of floating point type.
   *   
   *  @param name Parameters name.
   *  @param value Filled with parameter's value.
   *  @return Zero if the parameter does not exists or the
   *   parameters were not set yet.
   **/
  int getParameter( char *name, double *value );
  int getParameter( char *name, float  *value );

  /** Returns an algorithm parameter of int point type.
   *   
   *  @param name Parameters name.
   *  @param value Filled with parameter's value.
   *  @return Zero if the parameter does not exists or the
   *   parameters were not set yet.
   **/
  int getParameter( char *name, int *value );

  Sampler *_samp;


private:

  /** Allocates and fills md->id with the algorithm ID. */
  int allocID( AlgMetadata *md );


  int *_categ; ///< f_categ[i] != 0 if map "i" is categorical.

  AlgMetadata *_metadata;

  int            _nparam;
  AlgParameter *_param;
};


#endif
