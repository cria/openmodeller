/**
 * Declaration of GARP with Best Subsets Procedure
 * 
 * @file   garp_best_subsets.cpp
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


#include "garp_best_subsets.hh"
#include "garp.hh"

#include <om.hh>
#include <random.hh>
#include <om_sampler.hh>
#include <om_sampled_data.hh>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef _WINDOWS
#include <windows.h>
#define SLEEP(secs) Sleep(secs * 1000)
#else
#include <unistd.h>
#define SLEEP(secs) sleep(secs);
#endif

#define NUM_PARAM 11

/************************************/
/*** Algorithm parameter metadata ***/

AlgParamMetadata parameters_bs[NUM_PARAM] = 
{
  // Best Subsets Parameters
  {
    "TrainingProportion",          // Id.
    "Training Proportion",          // Name.
    "Double",                      // Type.

    // Overview.
    "Proportion of the occurrence data to be used to train the models.",

    // Description.
    "",

    1,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    1,      // Not zero if the parameter has upper limit.
    0,      // Parameter's upper limit.
    "0.5"   // Parameter's typical (default) value.
  },

  {
    "TotalRuns",            // Id.
    "Total Runs",           // Name.
    "Integer",              // Type.

    // Overview.
    "Maximum number of GARP runs to be performed.",

    // Description.
    "",

    1,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    1,      // Not zero if the parameter has upper limit.
    10000,  // Parameter's upper limit.
    "10"   // Parameter's typical (default) value.
  },

  {
    "HardOmissionThreshold",          // Id.
    "Hard Omission Threshold",        // Name.
    "Double",                         // Type.

    // Overview.
    "Maximum acceptable omission error. Set to 100% to use only soft omission",

    // Description.
    "",

    1,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    1,      // Not zero if the parameter has upper limit.
    100,    // Parameter's upper limit.
    "100"    // Parameter's typical (default) value.
  },

  {
    "ModelsUnderOmissionThreshold",                   // Id.
    "Models Under Omission Threshold",                // Name.
    "Integer",                                        // Type.

    // Overview.
    "Minimum number of models below omission threshold.",

    // Description.
    "",

    1,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    1,      // Not zero if the parameter has upper limit.
    10000,  // Parameter's upper limit.
    "20"    // Parameter's typical (default) value.
  },

  {
    "CommissionThreshold",          // Id.
    "Commission Threshold",         // Name.
    "Double",                       // Type.

    // Overview.
    "Percentage of distribution of models to be taken regarding commission error.",

    // Description.
    "",

    1,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    1,      // Not zero if the parameter has upper limit.
    100,    // Parameter's upper limit.
    "50"    // Parameter's typical (default) value.
  },

  {
    "CommissionSampleSize",          // Id.
    "Commission Sample Size",        // Name.
    "Integer",                       // Type.

    // Overview.
    "Number of samples used to calculate commission error.",

    // Description.
    "",

    1,        // Not zero if the parameter has lower limit.
    1,        // Parameter's lower limit.
    0,        // Not zero if the parameter has upper limit.
    0,        // Parameter's upper limit.
    "10000"   // Parameter's typical (default) value.
  },

  {
    "MaxThreads",                       // Id.
    "Maximum Number of Threads",        // Name.
    "Integer",                          // Type.

    // Overview.
    "Maximum number of threads of executions to run simultaneously.",

    // Description.
    "",

    1,        // Not zero if the parameter has lower limit.
    1,        // Parameter's lower limit.
    1,        // Not zero if the parameter has upper limit.
    1024,     // Parameter's upper limit.
    "5"       // Parameter's typical (default) value.
  },

  // GARP parameters
  {
    "MaxGenerations",              // Id.
    "Max generations",             // Name.
    "Integer",                     // Type.

    // Overview.
    "Maximum number of iterations run by the Genetic Algorithm.",

    // Description.
    "Maximum number of iterations (generations) run by the Genetic\
 Algorithm.",

    1,      // Not zero if the parameter has lower limit.
    1,      // Parameter's lower limit.
    0,      // Not zero if the parameter has upper limit.
    0,      // Parameter's upper limit.
    "20"   // Parameter's typical (default) value.
  },

  {
    "ConvergenceLimit",        // Id.
    "Convergence limit",       // Name.
    "Real",                    // Type.

    // Overview.
    "Defines the convergence value that makes the algorithm stop\
 (before reaching MaxGenerations).",

    // Description.
    "",

    1,     // Not zero if the parameter has lower limit.
    0.0,   // Parameter's lower limit.
    1,     // Not zero if the parameter has upper limit.
    1.0,   // Parameter's upper limit.
    "0.05"  // Parameter's typical (default) value.
  },

  {
    "PopulationSize",        // Id.
    "Population size",       // Name.
    "Integer",               // Type.

    "Maximum number of rules to be kept in solution.", // Overview.
    "", // Description

    1,     // Not zero if the parameter has lower limit.
    1,   // Parameter's lower limit.
    1,     // Not zero if the parameter has upper limit.
    500,   // Parameter's upper limit.
    "50"  // Parameter's typical (default) value.
  },

  {
    "Resamples",      // Id.
    "Resamples",      // Name.
    "Integer",        // Type.

    // Overview.
    "Number of points sampled (with replacement) used to test rules.",

    // Description.
    "",

    1,     // Not zero if the parameter has lower limit.
    1,   // Parameter's lower limit.
    1,     // Not zero if the parameter has upper limit.
    100000,   // Parameter's upper limit.
    "2500"  // Parameter's typical (default) value.
    }
};

/************************************/
/*** Algorithm's general metadata ***/

AlgMetadata metadata_bs = {
  
  "GARP_BS",                           // Id.
  "GARP with Best Subsets Procedure",  // Name.
  "3.0.1 alpha",                       // Version.

  // Overview.
  "GARP is a genetic algorithm that creates ecological niche \
models for species. The models describe environmental conditions \
under which the species should be able to maintain populations. \
For input, GARP uses a set of point localities where the species \
is known to occur and a set of geographic layers representing \
the environmental parameters that might limit the species' \
capabilities to survive.",

  // Description.
  "GARP is a genetic algorithm that creates ecological niche \
models for species. The models describe environmental conditions \
under which the species should be able to maintain populations. \
For input, GARP uses a set of point localities where the species \
is known to occur and a set of geographic layers representing \
the environmental parameters that might limit the species' \
capabilities to survive.",

  // Author
  "Anderson, R. P., D. Lew, D. and A. T. Peterson.",  

  // Bibliography.
  "Anderson, R. P., D. Lew, and A. T. Peterson. 2003. \
Evaluating predictive models of species' distributions: criteria \
for selecting optimal models.Ecological Modelling, v. 162, p. 211 232.",

  "Ricardo Scachetti Pereira",  // Code author.
  "rpereira [at] ku.edu",       // Code author's contact.

  0,  // Does not accept categorical data.
  1,  // Does not need (pseudo)absence points.

  NUM_PARAM,   // Algorithm's parameters.
  parameters_bs
};


/****************************************************************/
/****************** Algorithm's factory function ****************/

dllexp 
Algorithm * 
algorithmFactory()
{
  return new GarpBestSubsets;
}


/****************************************************************/
/****************** Garp class **********************************/

GarpBestSubsets::GarpBestSubsets()
  : Algorithm(& metadata_bs)
{
  _trainProp = 0.0;
  _totalRuns = 0;
  _omissionThreshold = 0.0;
  _modelsUnderOmission = 0;
  _commissionThreshold = 0.0;
  _commissionSampleSize = 0;   
     
  _softOmissionThreshold = false;
  _currentModelsUnderOmissionThreshold = 0;

  _max_gen = 0;
  _popsize = 0;
  _resamples = 0;
  _conv_limit = 0.0;

  _finishedRun = NULL;
  _activeRun = NULL;
  _bestRun = NULL;

  _garp_nparam = 0;
  _garp_params = NULL;

  _finishedRuns = 0;
  _activeRuns = 0;
  _done = false;

}

// ****************************************************************
GarpBestSubsets::~GarpBestSubsets()
{
  int i;

  if (_finishedRun)
    {
      for (i = 0; i < _finishedRuns; i++)
	{ delete _finishedRun[i]; }
      delete[] _finishedRun;
    }

  if (_activeRun)
    { delete[] _activeRun; }

  // bestRun just point to objects referenced by _finishedRun object
  if (_bestRun)
    { delete[] _bestRun; }

  if (_garp_params)
    delete[] _garp_params;
}

// ****************************************************************
// ************* needNormalization ********************************

int GarpBestSubsets::needNormalization( Scalar *min, Scalar *max )
{
  *min = -1.0;
  *max = +1.0;

  return 1;
}
  
// ****************************************************************
// ************* initialize ***************************************

int GarpBestSubsets::initialize()
{
  // Reconfigure the global logger.
  //g_log.set( Log::Debug, stdout, "Garp" );
  //g_log.setLevel( Log::Debug );

  // BS parameters
  if (!getParameter("TrainingProportion", &_trainProp))        
      g_log.error(1, "Parameter TrainingProportion not set properly.");

  if (!getParameter("TotalRuns", &_totalRuns))        
      g_log.error(1, "Parameter TotalRuns not set properly.");

  if (!getParameter("HardOmissionThreshold", &_omissionThreshold))        
      g_log.error(1, "Parameter HardOmissionThreshold not set properly.");

  if (!getParameter("ModelsUnderOmissionThreshold", &_modelsUnderOmission))        
      g_log.error(1, "Parameter ModelsUnderOmissionThreshold not set properly.");

  if (!getParameter("CommissionThreshold", &_commissionThreshold))        
      g_log.error(1, "Parameter CommissionThreshold not set properly.");

  if (!getParameter("CommissionSampleSize", &_commissionSampleSize))        
      g_log.error(1, "Parameter CommissionSampleSize not set properly.");

  if (!getParameter("MaxThreads", &_maxThreads))        
      g_log.error(1, "Parameter MaxThreads not set properly.");


  _softOmissionThreshold = (_omissionThreshold < 100.0);
  if (_modelsUnderOmission > _totalRuns)
    {
      g_log.warn("ModelsUnderOmission (%d) is greater than the number of runs (%d)",
		 _modelsUnderOmission, _totalRuns);
      _modelsUnderOmission = _totalRuns;
    }


  // GARP parameters
  if (!getParameter("MaxGenerations",   &_max_gen))        
      g_log.error(1, "Parameter MaxGenerations not set properly.");

  if (!getParameter("ConvergenceLimit", &_conv_limit))     
      g_log.error(1, "Parameter ConvergenceLimit not set properly.");

  if (!getParameter("PopulationSize",   &_popsize))        
      g_log.error(1, "Parameter PopulationSize not set properly.");

  if (!getParameter("Resamples",        &_resamples))      
      g_log.error(1, "Parameter Resamples not set properly.");

  _finishedRun = new GarpRun*[_totalRuns];
  _activeRun = new GarpRun*[_maxThreads];

  // setup AlgParam object for GARP
  if (_garp_params)
    delete[] _garp_params;

  _garp_nparam = 6;
  _garp_params = new AlgParameter[_garp_nparam];

  char buffer[1024];
  sprintf(buffer, "%d", _max_gen); 
  _garp_params[0].setId("MaxGenerations");
  _garp_params[0].setValue(buffer);
  
  sprintf(buffer, "%d", _conv_limit); 
  _garp_params[1].setId("ConvergenceLimit");
  _garp_params[1].setValue(buffer);

  sprintf(buffer, "%d", _popsize); 
  _garp_params[2].setId("PopulationSize");
  _garp_params[2].setValue(buffer);

  sprintf(buffer, "%d", _resamples); 
  _garp_params[3].setId("Resamples");
  _garp_params[3].setValue(buffer);

  _garp_params[4].setId("MutationRate");
  _garp_params[4].setValue("0.25");

  _garp_params[5].setId("CrossoverRate");
  _garp_params[5].setValue("0.25");

  return 1;
}
  
/****************************************************************/
/****************** Algorithm's factory function ****************/

int GarpBestSubsets::iterate()
{
  static int i = 0;
  int active;
  GarpRun * garpRun;

  ++i;

  if (!_done)
    {
      // check if it should start new runs
      if ((_finishedRuns + _activeRuns < _totalRuns) && 
	  !earlyTerminationConditionMet())
	{
	  // it needs to start more runs
	  // wait for a slot for a new thread
	  if ((active = numActiveThreads()) >= _maxThreads)
	    {
	      g_log("%5d] Waiting for a slot to run next thread (%d out of %d)\n",
		    i, active, _maxThreads);
	      SLEEP(1); 
	    }
	  
	  else
	    {
	      g_log("%5d] There is an empty slot to run next thread (%d out of %d)\n",
		    i, active, _maxThreads);

	      // start new GarpRun
	      Sampler * train, * test;
	      _samp->split(&train, &test, _trainProp);
	      garpRun = new GarpRun();
	      garpRun->initialize(_finishedRuns + _activeRuns,
				  _commissionSampleSize, train, test, 
				  _garp_nparam, _garp_params);
	      _activeRun[_activeRuns++] = garpRun;
	      garpRun->run();
	    }
	}

      else
	{
	  // no more runs are needed
	  // check if all active threads have finished
	  if (active = numActiveThreads())
	    {
	      // there are still threads running
	      g_log("%5d] Waiting for %d active thread(s) to finish.\n", 
		    i, active);
	      SLEEP(1); 
	    }

	  else
	    {
	      // all running threads terminated
	      // calculate best subset and exit
	      g_log("%5d] Calculating best and terminating algorithm.\n", i);
	      calculateBestSubset();
	      _done = true;
	    }
	}
    }

  return 1;
}

/****************************************************************/
int GarpBestSubsets::numActiveThreads()
{
  int i;
  GarpRun * run;
  bool atLeastOneRunFinished = false;

  for (i = 0; i < _activeRuns; i++)
    {
      run = _activeRun[i];

      if (!run->running())
	{
	  g_log("Thread %d has just finished.\n", run->getId());

	  // run finished its work
	  // move it to finished runs
	  // and remove it from list of active runs
	  _finishedRun[_finishedRuns++] = run;
	  _activeRun[i] = _activeRun[--_activeRuns];
	  _activeRun[_activeRuns] = NULL;
	  
	  // update count of models under omission threshold
	  if (_softOmissionThreshold)
	    {
	      if (run->getOmission() < _omissionThreshold)
		{ _currentModelsUnderOmissionThreshold++; }
	    }

	  atLeastOneRunFinished = true;
	}
    }

  return _activeRuns;
}

/****************************************************************/
int GarpBestSubsets::earlyTerminationConditionMet()
{
  return (!_softOmissionThreshold) &&
         (_currentModelsUnderOmissionThreshold >= _modelsUnderOmission);
}

/****************************************************************/
int GarpBestSubsets::calculateBestSubset()
{
  int i;

  g_log("Calculating best subset of models.\n");
      
  // make a copy of finished runs to play with
  GarpRun ** runList = new GarpRun*[_finishedRuns];
  for (i = 0; i < _finishedRuns; i++)
    { runList[i] = _finishedRun[i]; }
  
  // get list of models that pass omission test
  // sort runs by omission
  // first <_modelsUnderOmission> runs are the selected ones
  sortRuns(runList, _finishedRuns, 0);

  // get list of models that pass commission test
  sortRuns(runList, _modelsUnderOmission, 1);

  _bestRuns = (int)(_commissionThreshold / 100.0 * 
		    (double) _modelsUnderOmission);
  int medianRun = _bestRuns / 2;
  int firstRun = (int) ceil((double) medianRun - (double) _bestRuns / 2.0);

  _bestRun = new GarpRun*[_bestRuns];

  for (i = 0; i < _bestRuns; i++)
    { _bestRun[i] = runList[i + firstRun]; }

  delete[] runList;

  g_log("Selected best %d models out of %d.\n", _bestRuns, _totalRuns);

  return 1;
}

/****************************************************************/
void GarpBestSubsets::sortRuns(GarpRun ** runList, 
			       int nelements, int errorType)
{
  int i, j;
  GarpRun * runJ0, * runJ1;

  g_log("Sorting list %d of %d elements by index %d.\n", 
	runList, nelements, errorType);

  // bubble sort
  // TODO: change to quicksort if this becomes a bottleneck
  for (i = 0; i < nelements - 1; i++)
    { 
      for (j = 0; j < nelements - i - 2; j++)
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
/****************** Algorithm's factory function ****************/

int GarpBestSubsets::done()
{
  return _done;
}

/****************************************************************/
/****************** Algorithm's factory function ****************/

Scalar GarpBestSubsets::getValue( Scalar *x )
{
  int i;
  double sum = 0.0;

  if (_done)
    {
      for (i = 0; i < _bestRuns; i++)
	{ sum += _bestRun[i]->getValue(x); }
    }

  return sum / (double) _bestRuns;
}
  
/****************************************************************/
/****************** Algorithm's factory function ****************/

int GarpBestSubsets::getConvergence( Scalar *val )
{
  *val = 0;
  return 0;
}

/******************/
/*** serialize ***/
int
GarpBestSubsets::serialize(Serializer * s)
{
  return 0;
}

/********************/
/*** deserialize ***/
int
GarpBestSubsets::deserialize(Deserializer * ds)
{
  return 0;
}

