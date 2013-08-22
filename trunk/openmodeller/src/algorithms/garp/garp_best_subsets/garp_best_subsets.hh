/**
 * Declaration of GARP with Best Subsets Procedure
 * 
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

#include <openmodeller/om.hh>

#include "garp.hh"
#include "best_subsets.hh"


/****************************************************************/
/************************* GARP Algorithm ***********************/

/**
  * Implements GARP with Best Subset Procedure (Anderson et al. 2003).
  * 
  * It runs "n" Garp models, filter the best ones based on the
  * procedure defined by Anderson et al. 2003 and sum them to obtain
  * a probability map for the species distribution.
  * 
  * Current implementation uses multi-threading to run multiple 
  * Garp runs in parallel. Takes advantage of multi-processor 
  * servers and workstations.
  */
class GarpBestSubsets : public BestSubsets
{
public:
  GarpBestSubsets();
  virtual ~GarpBestSubsets();

private:
  // AlgorithmRun object factory
  AlgorithmImpl * getBSAlgorithm();
  int transferParametersToAlgorithm();

  //
  // GARP parameters
  //

  /** Maximum number of iterations to be executed (if convergence is not reached) */
  int _max_gen;

  /** Number of points to be considered after resampling (with replacement) */
  int _popsize;

  /** Number of points to be considered after resampling (with replacement) */
  int _resamples;

  /** Minimum value for convergence that triggers termination of execution */
  double _conv_limit;
};


#endif

