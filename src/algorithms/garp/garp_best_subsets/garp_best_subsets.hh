/**
 * Declaration of GARP with Best Subsets Procedure
 * 
 * @file   garp_best_subsets.hh
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-08-24
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 *
 * http://www.nhm.ku.edu
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
 * 
 */


#ifndef _GARP_BEST_SUBSETS_HH_
#define _GARP_BEST_SUBSETS_HH_

#include <om.hh>

#include "garp_run.hh"


/****************************************************************/
/************************* GARP Algorithm ***********************/

class GarpBestSubsets : public Algorithm
{
public:
  GarpBestSubsets();
  virtual ~GarpBestSubsets();

  /** This method is used when you want to ensure that all variables in all
    * environmental layers are scaled to the same value range. 
    * GARP requires values to be normalized between -1.0 and 1.0.
    * @param Scalar pointer min that the post normalised will be fitted to
	*        Always set to -1.0 in GARP
    * @param Scalar pointer max that the post normalised will be fitted to
	*        Always set to +1.0 in GARP
    * @return Always return 1 for GARP.
   */
  int needNormalization( Scalar *min, Scalar *max );
  
  /** Initialize data structures
    * @note This method is inherited from the Algorithm class
    * @return 0 on error
    */
  int initialize();
  
  /** Start model execution (build the model). Consists of a single GARP run.
   *  It is also the control loop for the threads
   *  @return 0 on error 
   */
  int iterate();
  
  /** Check if the model generation is completed (e.g. convergence
   *  point has been met. 
   *  @return Implementation specific but usually 1 for completion.
   */
  int done();

  //
  // Methods used to project the model
  //
  
  /** This method is used when projecting the model.
   *  @note This method is inherited from the Algorithm class
   *  @return     
   *  @param Scalar *x a pointer to a vector of openModeller Scalar 
   *         type (currently double). The vector should contain values 
   *         looked up on the environmental variable layers into which 
   *         the mode is being projected. 
   */
  Scalar getValue( Scalar *x );
  
  /** Returns a value that represents the convergence of the algorithm
    * expressed as a number between 0 and 1 where 0 represents model
    * completion. 
    * @return 
    * @param Scalar *val 
    */
  int getConvergence( Scalar *val );

  /*
   */
  int serialize(Serializer * serializer);
  int deserialize(Deserializer * deserializer);

private:

  //
  // Algorithm parameters
  //

  /** */

  //
  // GARP parameters
  //

  /** Maximum number of iterations to be executed (if convergence is not reached) */
  int _max_gen;

  /** Number of points to be considered after resampling (with replacement) */
  int _popsize;

  /** Number of points to be considered after resampling (with replacement) */
  int _resamples;

  /** Minimum posterior probability of a rule */
  double _acc_limit;

  /** Minimum value for convergence that triggers termination of execution */
  double _conv_limit;

  /** Percentage of rules that are killed at the end of a iteration */
  double _mortality;

  /** */
  double _significance; 
  double _crossover_rate;
  double _mutation_rate;
  double _gapsize;

  //
  // Internal data structures for Best Subsets
  //
  GarpRun * garpRun[];
  GarpRun * bestRun[];

  int activeRuns;
  int runCount;
  int done;
};


#endif

