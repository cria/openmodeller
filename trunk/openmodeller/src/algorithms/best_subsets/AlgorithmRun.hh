/**
 * Declaration of AlgorithmRun class
 * 
 * @file   garp_run.hh
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-08-25
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


#ifndef _ALGORITHM_RUN_HH_
#define _ALGORITHM_RUN_HH_

#include <openmodeller/om.hh>

class AlgorithmImpl;
class AlgParameter;

/****************************************************************/
/************************* AlgorithmRun *************************/

/**
  * Wraps up a single Algorithm run and its results (calculated error
  * components). Also handles multi-threading aspects of
  * computation.
  */
class AlgorithmRun
{
public:
  AlgorithmRun( const AlgorithmPtr& algo );
  ~AlgorithmRun();

  int initialize(int id, int comm_samples,
		 const SamplerPtr& train_sampler, 
		 const SamplerPtr& test_sampler );
  void run();
  void stop();


  void createModel();

  bool running() const;
  int getId() const { return _id; }
  float getProgress() const;
  double getOmission() const;
  double getCommission() const;
  double getError(int type) const;
  Scalar getValue(const Sample& x) const; 

  int calculateOmission();
  int calculateCommission();

  AlgorithmPtr getAlgorithm() {return _alg; }

private:

  AlgorithmPtr _alg;      /// Algorithm used in this run

  int _id;                   /// Identified for this particular garp run
  bool _running;             /// Indicates whether the thread is running
  double _omission;          /// Omission error for this run
  double _commission;        /// Commission error, approximated by area predicted present
  int _commission_samples;   /// Number of points used to calculate commission
  SamplerPtr _train_sampler;
  SamplerPtr _test_sampler;

  AlgorithmRun( const AlgorithmRun& );
  AlgorithmRun& operator=(const AlgorithmRun& );
};


#endif

