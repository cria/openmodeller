/**
 * Declaration of Best Subsets Procedure base class
 * 
 * @file   best_subsets.hh
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-12-10
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


#ifndef _BEST_SUBSETS_HH_
#define _BEST_SUBSETS_HH_

#include <om.hh>

#include "bs_algorithm_factory.hh"
#include "garp_run.hh"


/****************************************************************/
/************************* GARP Algorithm ***********************/

/**
  * Implements Best Subset Procedure (Anderson et al. 2003).
  * 
  * It runs <n> Garp models, filter the best ones based on the
  * procedure defined by Anderson et al. 2003 and sum them to obtain
  * a probability map for the species distribution.
  * 
  * Current implementation uses multi-threading to run multiple 
  * Garp runs in parallel. Takes advantage of multi-processor 
  * servers and workstations.
  */
class BestSubsets : public AlgorithmImpl, public BSAlgorithmFactory
{
public:
  BestSubsets(AlgMetadata * metadata);
  virtual ~BestSubsets();

  /** This method is used when you want to ensure that all variables in all
    * environmental layers are scaled to the same value range. 
    * GARP requires values to be normalized between -1.0 and 1.0.
    * @param Scalar pointer min that the post normalised will be fitted to
	*        Always set to -1.0 in GARP_BS
    * @param Scalar pointer max that the post normalised will be fitted to
	*        Always set to +1.0 in GARP_BS
    * @return Always return 1 for GARP_BS.
   */
  int needNormalization( Scalar *min, Scalar *max ) const;
  
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
  int done() const;

  /** Return progress so far */
  float getProgress() const;

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
  Scalar getValue( const Sample& x ) const;
  
  /** Returns a value that represents the convergence of the algorithm
    * expressed as a number between 0 and 1 where 0 represents model
    * completion. 
    * @return 
    * @param Scalar *val 
    */
  int getConvergence( Scalar *val );

  /*
   */

protected:
  int _nparam;
  AlgParameter * _alg_params;


private:
  virtual AlgorithmImpl * getBSAlgorithm() = 0;
  virtual int transferParametersToAlgorithm() = 0;

  int numActiveThreads();
  int earlyTerminationConditionMet();
  int calculateBestSubset();
  void sortRuns(AlgorithmRun ** runList, int nelements, int errorType);


  //
  // best subsets parameters
  //

  double _trainProp;
  int _totalRuns;
  double _omissionThreshold;
  int _modelsUnderOmission;
  double _commissionThreshold;
  int _commissionSampleSize;
  int _maxThreads;

  bool _softOmissionThreshold;
  int _currentModelsUnderOmissionThreshold;

  //
  // Internal data structures for Best Subsets
  //
  AlgorithmRun ** _finishedRun;
  AlgorithmRun ** _activeRun;
  AlgorithmRun ** _bestRun;

  int _numFinishedRuns;
  int _numActiveRuns;
  int _numBestRuns;

  int _done;

  mutable float _maxProgress;
};


#endif

