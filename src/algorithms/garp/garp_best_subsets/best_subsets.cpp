/**
 * Declaration of Best Subsets Procedure template class
 * 
 * @file   best_subsets.cpp
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


#include "best_subsets.hh"

#include <openmodeller/om.hh>
#include <openmodeller/Random.hh>
#include <openmodeller/Sampler.hh>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef WIN32
#include <windows.h>
#define SLEEP(secs) Sleep(secs * 1000)
#else
#include <unistd.h>
#define SLEEP(secs) sleep(secs);
#endif


/****************************************************************/
void BestSubsets::printListOfRuns(char * msg, AlgorithmRun ** runs, int numOfRuns)
{
  printf("%s\n", msg);
  for (int i = 0; i < numOfRuns; i++)
    printf("%4d] om=%5.3f comm=%5.3f (id=%d)\n", i, 
        runs[i]->getOmission(), 
        runs[i]->getCommission(),
        runs[i]->getId());
}

/****************************************************************/
/****************** Garp class **********************************/

  BestSubsets::BestSubsets(AlgMetadata * metadata)
: AlgorithmImpl(metadata)
{
  _trainProp = 0.0;
  _totalRuns = 0;
  _omissionThreshold = 0.0;
  _modelsUnderOmission = 0;
  _commissionThreshold = 0.0;
  _commissionSampleSize = 0;   

  _softOmissionThreshold = false;
  _currentModelsUnderOmissionThreshold = 0;

  _finishedRun = NULL;
  _activeRun = NULL;
  _bestRun = NULL;

  _nparam = 0;
  _alg_params = NULL;

  _numFinishedRuns = 0;
  _numActiveRuns = 0;
  _done = false;

  _maxProgress = 0.0;

}

// ****************************************************************
BestSubsets::~BestSubsets()
{
  int i;

  if (_finishedRun)
  {
    for (i = 0; i < _numFinishedRuns; i++)
    { delete _finishedRun[i]; }
    delete[] _finishedRun;
  }

  if (_activeRun)
  { delete[] _activeRun; }

  // bestRun just point to objects referenced by _finishedRun object
  if (_bestRun)
  { delete[] _bestRun; }

  if (_alg_params)
    delete[] _alg_params;
}

// ****************************************************************
// ************* needNormalization ********************************

int BestSubsets::needNormalization( Scalar *min, Scalar *max ) const
{
  *min = -1.0;
  *max = +1.0;

  return 1;
}

// ****************************************************************
// ************* initialize ***************************************

int BestSubsets::initialize()
{
  // BS parameters
  if (!getParameter("TrainingProportion", &_trainProp))        
    g_log.error(1, "Parameter TrainingProportion not set properly.\n");

  if (!getParameter("TotalRuns", &_totalRuns))        
    g_log.error(1, "Parameter TotalRuns not set properly.\n");

  if (!getParameter("HardOmissionThreshold", &_omissionThreshold))        
    g_log.error(1, "Parameter HardOmissionThreshold not set properly.\n");

  if (!getParameter("ModelsUnderOmissionThreshold", &_modelsUnderOmission))        
    g_log.error(1, "Parameter ModelsUnderOmissionThreshold not set properly.\n");

  if (!getParameter("CommissionThreshold", &_commissionThreshold))        
    g_log.error(1, "Parameter CommissionThreshold not set properly.\n");

  if (!getParameter("CommissionSampleSize", &_commissionSampleSize))        
    g_log.error(1, "Parameter CommissionSampleSize not set properly.\n");

  if (!getParameter("MaxThreads", &_maxThreads))        
    g_log.error(1, "Parameter MaxThreads not set properly.\n");

  // convert percentages (100%) to proportions (1.0) for external parameters 
  _commissionThreshold /= 100.0;
  _omissionThreshold /= 100.0;

  _softOmissionThreshold = (_omissionThreshold >= 1.0);
  if (_modelsUnderOmission > _totalRuns)
  {
    g_log.warn("ModelsUnderOmission (%d) is greater than the number of runs (%d)\n",
        _modelsUnderOmission, _totalRuns);
    _modelsUnderOmission = _totalRuns;
  }

  _finishedRun = new AlgorithmRun*[_totalRuns];
  _activeRun = new AlgorithmRun*[_maxThreads];

  transferParametersToAlgorithm();

  return 1;
}

/****************************************************************/
/****************** iterate *************************************/

int BestSubsets::iterate()
{
  static int iterations = 0;
  int active;
  AlgorithmRun * algRun;

  ++iterations;

  if (_done)
  { return 1; }

  // check if it should start new runs
  if ((_numFinishedRuns + _numActiveRuns < _totalRuns) && 
      !earlyTerminationConditionMet())
  {
    // it needs to start more runs
    // wait for a slot for a new thread
    if ((active = numActiveThreads()) >= _maxThreads)
    {
      //g_log("%5d] Waiting for a slot to run next thread (%d out of %d)\n", iterations, active, _maxThreads);
      SLEEP(2); 
    }

    else
    {
      int runId = _numFinishedRuns + _numActiveRuns;

      //g_log("%5d] There is an empty slot to run next thread (%d out of %d) - %d\n", iterations, active, _maxThreads, runId);

      // start new Algorithm
      SamplerPtr train, test;
      splitSampler(_samp, &train, &test, _trainProp);

      //	  printf("Presences: Orig=%d, train=%d, test=%d\n", _samp->numPresence(), train->numPresence(), test->numPresence());
      //	  printf("Absences:  Orig=%d, train=%d, test=%d\n", _samp->numAbsence(), train->numAbsence(), test->numAbsence());

      algRun = new AlgorithmRun();
      algRun->initialize(runId,
          _commissionSampleSize, train, test, 
          _nparam, _alg_params, 
          (BSAlgorithmFactory *) this);
      _activeRun[_numActiveRuns++] = algRun;
      algRun->run();
    }
  }

  else
  {
    // no more runs are needed
    // check if all active threads have finished
    active = numActiveThreads();
      if (active)
      {
        // there are still threads running
        /*
           g_log("%5d] Waiting for %d active thread(s) to finish.\n", 
           iterations, active);
           */
        SLEEP(2); 
      }

      else
      {
        // all running threads terminated
        // calculate best subset and exit
        //g_log("%5d] Calculating best and terminating algorithm.\n", i);
        calculateBestSubset();
        _done = true;
      }
  }

  return 1;
}

/****************************************************************/
int BestSubsets::numActiveThreads()
{
  int i;
  AlgorithmRun * run;
  bool atLeastOneRunFinished = false;

  for (i = 0; i < _numActiveRuns; i++)
  {
    run = _activeRun[i];

    if (!run->running())
    {
      //g_log("Thread %d has just finished.\n", run->getId());

      // run finished its work
      // move it to finished runs
      // and remove it from list of active runs
      _finishedRun[_numFinishedRuns++] = run;
      _activeRun[i] = _activeRun[--_numActiveRuns];
      _activeRun[_numActiveRuns] = NULL;

      // update count of models under omission threshold
      if (!_softOmissionThreshold)
      {
        if (run->getOmission() <= _omissionThreshold)
        { _currentModelsUnderOmissionThreshold++; }
      }

      atLeastOneRunFinished = true;
    }
  }

  return _numActiveRuns;
}

/****************************************************************/
int BestSubsets::earlyTerminationConditionMet()
{
  return (!_softOmissionThreshold) &&
    (_currentModelsUnderOmissionThreshold >= _modelsUnderOmission);
}

/****************************************************************/
int BestSubsets::calculateBestSubset()
{
  int i;

  //g_log("Calculating best subset of models.\n");

  // make a copy of finished runs to play with
  AlgorithmRun ** runList = new AlgorithmRun*[_numFinishedRuns];
  for (i = 0; i < _numFinishedRuns; i++)
  { runList[i] = _finishedRun[i]; }

  //printListOfRuns("Finished Runs:", runList, _numFinishedRuns);

  // get list of models that pass omission test
  // sort runs by omission
  // first <_modelsUnderOmission> runs are the selected ones
  sortRuns(runList, _numFinishedRuns, 0);

  //printListOfRuns("Finished Runs by Omission:", runList, _numFinishedRuns);

  // get list of models that pass commission test
  sortRuns(runList, _modelsUnderOmission, 1);

  //printListOfRuns("Best Omission Runs by Commission:", runList, _numFinishedRuns);

  _numBestRuns = (int)(_commissionThreshold * 
      (double) _modelsUnderOmission);
  int medianRun = _modelsUnderOmission / 2;
  int firstRun = (int) ceil((double) medianRun - (double) _numBestRuns / 2.0);

  _bestRun = new AlgorithmRun*[_numBestRuns];

  for (i = 0; i < _numBestRuns; i++)
  { _bestRun[i] = runList[i + firstRun]; }

  //printListOfRuns("Best Runs:", _bestRun, _numBestRuns);

  //printf("Median: %d First: %d\n", medianRun, firstRun);


  delete[] runList;

  //g_log("Selected best %d models out of %d.\n", _numBestRuns, _totalRuns);

  return 1;
}

/****************************************************************/
void BestSubsets::sortRuns(AlgorithmRun ** runList, 
    int nelements, int errorType)
{
  int i, j;
  AlgorithmRun * runJ0, * runJ1;

  //g_log("Sorting list %d of %d elements by index %d.\n", runList, nelements, errorType);

  // bubble sort
  // TODO: change to quicksort if this becomes a bottleneck
  for (i = 0; i < nelements - 1; i++)
  { 
    for (j = 0; j < nelements - i - 1; j++)
    {
      runJ0 = runList[j];
      runJ1 = runList[j + 1];

      if (runJ0->getError(errorType) > runJ1->getError(errorType))
      {
        // exchange elements j and j + 1
        runList[j] = runJ1;
        runList[j + 1] = runJ0;
      }
    }
  }
}

/****************************************************************/
/****************** done ****************************************/

int BestSubsets::done() const
{
  return _done;
}

/****************************************************************/
/****************** done ****************************************/

float BestSubsets::getProgress() const
{
  if (done())
  { return 1.0; } 

  else
  {
    float progByTotalRuns = 0.0;
    float progByHardOmission = 0.0;

    float avgProgressActiveRuns = 0.0;
    for (int i = 0; i < _numActiveRuns; i++)
    { avgProgressActiveRuns += _activeRun[i]->getProgress(); }
    avgProgressActiveRuns /= _numActiveRuns;

    progByTotalRuns = (_numFinishedRuns + avgProgressActiveRuns) / (float) _totalRuns;

    if (!_softOmissionThreshold)
    {
      progByHardOmission = (_currentModelsUnderOmissionThreshold / 
          (float) _modelsUnderOmission); 
    }

    float progress = (progByTotalRuns > progByHardOmission)? progByTotalRuns : progByHardOmission;

    if (progress > _maxProgress)
    { _maxProgress = progress; }

    return _maxProgress;
  }
}

/****************************************************************/
/****************** getValue ************************************/

Scalar BestSubsets::getValue( const Sample& x ) const
{
  int i;
  double sum = 0.0;

  if (_done)
  {
    for (i = 0; i < _numBestRuns; i++)
    { sum += _bestRun[i]->getValue(x); }
  }

  return sum / (double) _numBestRuns;
}

/****************************************************************/
/****************** getConvergence ******************************/

int BestSubsets::getConvergence( Scalar * const val ) const
{
  *val = 0;
  return 0;
}

