/**
 * Declaration of Best Subsets Procedure template class
 * 
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
#include <openmodeller/Exceptions.hh>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string>
using std::string;

#ifdef WIN32
#include <windows.h>
#define SLEEP(secs) Sleep(secs * 1000)
#else
#include <unistd.h>
#define SLEEP(secs) sleep(secs);
#endif


/****************************************************************/
void BestSubsets::printListOfRuns(string msg, AlgorithmRun ** runs, int numOfRuns)
{
  printf("%s\n", msg.c_str());
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
// ************* initialize ***************************************

int BestSubsets::initialize()
{
  // BS parameters
  if (!getParameter("TrainingProportion", &_trainProp)) {
    Log::instance()->error("Parameter TrainingProportion not set properly.\n");
    return 0;
  }

  if (!getParameter("TotalRuns", &_totalRuns)) {
    Log::instance()->error("Parameter TotalRuns not set properly.\n");
    return 0;
  }

  if (!getParameter("HardOmissionThreshold", &_omissionThreshold)) {
    Log::instance()->error("Parameter HardOmissionThreshold not set properly.\n");
    return 0;
  }

  if (!getParameter("ModelsUnderOmissionThreshold", &_modelsUnderOmission)) {
    Log::instance()->error("Parameter ModelsUnderOmissionThreshold not set properly.\n");
    return 0;
  }

  if (!getParameter("CommissionThreshold", &_commissionThreshold)) {
    Log::instance()->error("Parameter CommissionThreshold not set properly.\n");
    return 0;
  }

  if (!getParameter("CommissionSampleSize", &_commissionSampleSize)) {
    Log::instance()->error("Parameter CommissionSampleSize not set properly.\n");
    return 0;
  }

  if (!getParameter("MaxThreads", &_maxThreads)) {
    Log::instance()->error("Parameter MaxThreads not set properly.\n");
    return 0;
  }

  if ( _maxThreads < 1 )
  {
    _maxThreads = 1;
  }
  else if ( _maxThreads > 1 )
  {
    // When maxThreads is greater than 1, if the machine has only one processor om
    // can crash. If the machine has more than one processor GDAL can output lots
    // of IO errors (current GDAL version does not seem to be thread safe).
    Log::instance()->warn("Multithreading is still experimental. When max threads is greater than 1, depending on software and hardware configuration this application may crash or you may see lots of raster IO warnings. In these cases, we recommend you to set this parameter to 1.\n");
  }

  if (_trainProp <= 1.0)
  {
    Log::instance()->warn("The specified training proportion value is less than or equals 1. Please note that there was a change in the valid range for this parameter from 0-1 to 0-100. Small values may result in zero presence points being used to train the model.\n");
  }

  // convert percentages (100%) to proportions (1.0) for external parameters 
  _trainProp /= 100.0;
  _commissionThreshold /= 100.0;
  _omissionThreshold /= 100.0;

  _softOmissionThreshold = (_omissionThreshold >= 1.0);
  if (_modelsUnderOmission > _totalRuns)
  {
    Log::instance()->warn("ModelsUnderOmission (%d) is greater than the number of runs (%d). ModelsUnderOmission will be reduced to (%d)\n", _modelsUnderOmission, _totalRuns, _totalRuns);
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
      //Log::instance()->info("%5d] Waiting for a slot to run next thread (%d out of %d)\n", iterations, active, _maxThreads);
      SLEEP(2); 
    }

    else
    {
      int runId = _numFinishedRuns + _numActiveRuns;

      //Log::instance()->info("%5d] There is an empty slot to run next thread (%d out of %d) - %d\n", iterations, active, _maxThreads, runId);

      // start new Algorithm
      SamplerPtr train, test;
      splitSampler( _samp, &train, &test, _trainProp );

      Log::instance()->debug( "Presences: orig=%d, train=%d, test=%d\n", _samp->numPresence(), train->numPresence(), test->numPresence() );
      Log::instance()->debug( "Absences:  orig=%d, train=%d, test=%d\n", _samp->numAbsence(), train->numAbsence(), test->numAbsence() );

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
           Log::instance()->info("%5d] Waiting for %d active thread(s) to finish.\n", 
           iterations, active);
           */
        SLEEP(2); 
      }

      else
      {
        // all running threads terminated
        // calculate best subset and exit
        //Log::instance()->info("%5d] Calculating best and terminating algorithm.\n", i);
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

  for (i = 0; i < _numActiveRuns; i++)
  {
    run = _activeRun[i];

    if (!run->running())
    {
      //Log::instance()->info("Thread %d has just finished.\n", run->getId());

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

  Log::instance()->info("Calculating best subset of models.\n");

  // make a copy of finished runs to play with
  AlgorithmRun ** runList = new AlgorithmRun*[_numFinishedRuns];
  for (i = 0; i < _numFinishedRuns; i++)
  { runList[i] = _finishedRun[i]; }

  printListOfRuns("Finished Runs:", runList, _numFinishedRuns);

  // get list of models that pass omission test
  // sort runs by omission
  // first <_modelsUnderOmission> runs are the selected ones
  sortRuns(runList, _numFinishedRuns, 0);

  printListOfRuns("Finished Runs by Omission:", runList, _numFinishedRuns);

  // get list of models that pass commission test
  sortRuns(runList, _modelsUnderOmission, 1);

  printListOfRuns("Best Omission Runs by Commission:", runList, _numFinishedRuns);

  _numBestRuns = (int)( _commissionThreshold * (double)_modelsUnderOmission + 0.5 );
  int medianRun = _modelsUnderOmission / 2;
  int firstRun = (int)ceil( (double)medianRun - (double)_numBestRuns / 2.0 );

  _bestRun = new AlgorithmRun*[_numBestRuns];

  for (i = 0; i < _numBestRuns; i++)
  { _bestRun[i] = runList[i + firstRun]; }

  printListOfRuns("Best Runs:", _bestRun, _numBestRuns);

  printf("Median: %d First: %d\n", medianRun, firstRun);


  delete[] runList;

  Log::instance()->info("Selected best %d models out of %d.\n", _numBestRuns, _totalRuns);

  return 1;
}

/****************************************************************/
void BestSubsets::sortRuns(AlgorithmRun ** runList, 
    int nelements, int errorType)
{
  int i, j;
  AlgorithmRun * runJ0, * runJ1;

  Log::instance()->info("Sorting list %d of %d elements by index %d.\n", runList, nelements, errorType);

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

/****************************************************************/
/****************** getConfiguration ****************************/
void BestSubsets::_getConfiguration( ConfigurationPtr& config ) const
{
  if ( !_done ) {

    return;
  }

  ConfigurationPtr model_config( new ConfigurationImpl("BestSubsets") );
  config->addSubsection( model_config );

  model_config->addNameValue("Count", _numBestRuns);

  for ( int i=0; i < _numBestRuns; i++ ) {

    ConfigurationPtr child_config( new ConfigurationImpl("Run") );
    child_config->addNameValue( "Id", i );
    child_config->addNameValue( "OmissionError", _bestRun[i]->getOmission() * 100 );
    child_config->addNameValue( "CommissionError", _bestRun[i]->getCommission() * 100 );

    ConfigurationPtr alg_config = _bestRun[i]->getAlgorithm()->getConfiguration();
    child_config->addSubsection( alg_config );

    model_config->addSubsection( child_config );
  }
}

/***************************************************************/
/****************** setConfiguration ***************************/
void BestSubsets::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "BestSubsets", false );

  if ( ! model_config ) {

    return;
  }

  _done = true;

  _numBestRuns = model_config->getAttributeAsInt( "Count", 0 );

  _bestRun = new AlgorithmRun*[_numBestRuns];

  Configuration::subsection_list runs = model_config->getAllSubsections();

  Configuration::subsection_list::const_iterator fin = runs.end();
  Configuration::subsection_list::const_iterator it = runs.begin();
  // The index i is used to populate the _bestRuns array it is incremented after each
  // new algorithm is found.
  int i;
  for ( i = 0; it != fin; ++it ) {

    // Test this here rather than at the bottom of loop.
    // This needs to be done after checking for loop terminal condition.
    if ( i == _numBestRuns ) {

      throw ConfigurationException( "Number of deserialized algorithms exceeds Count" );
    }

    ConstConfigurationPtr run_config = *it;

    if ( run_config->getName() != "Run" ) {

      continue;
    }

    AlgorithmPtr alg = AlgorithmFactory::newAlgorithm( run_config->getSubsection("Algorithm") );

    _bestRun[i] = new AlgorithmRun( alg );

    // increment i after adding algorithmRun to _bestRun
    ++i;
  }

  if ( i < _numBestRuns ) {

    throw ConfigurationException( "Number of deserialized algorithms is smaller than Count" );
  }
}
