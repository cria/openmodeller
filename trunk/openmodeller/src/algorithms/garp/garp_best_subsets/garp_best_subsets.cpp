/**
 * Declaration of GARP with Best Subsets Procedure
 * 
 * @file   garp_best_subsets.cpp
 * @author Ricardo Scachetti Pereira (ricardo [at] tdwg . org)
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

#define NUM_PARAM 11

/************************************/
/*** Algorithm parameter metadata ***/

AlgParamMetadata parameters_bs[NUM_PARAM] = 
{
  // Best Subsets Parameters
  {
    "TrainingProportion",          // Id.
    "Training Proportion",         // Name.
    "Double",                      // Type.

    // Overview.
    "Proportion of the occurrence data to be used to train the models.",

    // Description.
    "Proportion of the occurrence data to be used to train the models.",

    1,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    1,      // Not zero if the parameter has upper limit.
    1,      // Parameter's upper limit.
    "0.5"   // Parameter's typical (default) value.
  },

  {
    "TotalRuns",            // Id.
    "Total Runs",           // Name.
    "Integer",              // Type.

    // Overview.
    "Maximum number of GARP runs to be performed.",

    // Description.
    "Maximum number of GARP runs to be performed.",

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
    "Maximum acceptable omission error. Set to 100% to use only soft omission.",

    // Description.
    "Maximum acceptable omission error. Set to 100% to use only soft omission.",

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
    "Minimum number of models below omission threshold.",

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
    "Percentage of distribution of models to be taken regarding commission error.",

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
    "Number of samples used to calculate commission error.",

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
    "Maximum number of threads of executions to run simultaneously.",

    1,        // Not zero if the parameter has lower limit.
    1,        // Parameter's lower limit.
    1,        // Not zero if the parameter has upper limit.
    1024,     // Parameter's upper limit.
    "1"       // Parameter's typical (default) value.
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
    "400"   // Parameter's typical (default) value.
  },

  {
    "ConvergenceLimit",        // Id.
    "Convergence limit",       // Name.
    "Real",                    // Type.

    // Overview.
    "Defines the convergence value that makes the algorithm stop\
 (before reaching MaxGenerations).",

    // Description.
    "Defines the convergence value that makes the algorithm stop\
 (before reaching MaxGenerations).",

    1,     // Not zero if the parameter has lower limit.
    0.0,   // Parameter's lower limit.
    1,     // Not zero if the parameter has upper limit.
    1.0,   // Parameter's upper limit.
    "0.01"  // Parameter's typical (default) value.
  },

  {
    "PopulationSize",        // Id.
    "Population size",       // Name.
    "Integer",               // Type.

    "Maximum number of rules to be kept in solution.", // Overview.
    "Maximum number of rules to be kept in solution.", // Description

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
    "Number of points sampled (with replacement) used to test rules.",

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
  
  "GARP_BS",                                                  // Id.
  "GARP with best subsets - new openModeller implementation", // Name.
  "3.0.1 alpha",                                              // Version.

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
  "ricardo [at] tdwg . org",    // Code author's contact.

  0,  // Does not accept categorical data.
  1,  // Does not need (pseudo)absence points.

  NUM_PARAM,   // Algorithm's parameters.
  parameters_bs
};


/****************************************************************/
/****************** Algorithm's factory function ****************/

OM_ALG_DLL_EXPORT 
AlgorithmImpl * 
algorithmFactory()
{
  return new GarpBestSubsets();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata_bs;
}

/****************************************************************/
/****************** Garp class **********************************/

GarpBestSubsets::GarpBestSubsets()
  : BestSubsets(& metadata_bs)
{
}

// ****************************************************************
GarpBestSubsets::~GarpBestSubsets()
{
}

// ****************************************************************
AlgorithmImpl * GarpBestSubsets::getBSAlgorithm()
{
  return new Garp();
}

// ****************************************************************
// ************* needNormalization ********************************

int GarpBestSubsets::needNormalization( Scalar *min, Scalar *max ) const
{
  *min = -1.0;
  *max = +1.0;

  return 1;
}
  
// ****************************************************************
// ************* initialize ***************************************

int GarpBestSubsets::transferParametersToAlgorithm()
{
  // GARP parameters
  if (!getParameter("MaxGenerations",   &_max_gen))        
      Log::instance()->error(1, "Parameter MaxGenerations not set properly.\n");

  if (!getParameter("ConvergenceLimit", &_conv_limit))     
      Log::instance()->error(1, "Parameter ConvergenceLimit not set properly.\n");

  if (!getParameter("PopulationSize",   &_popsize))        
      Log::instance()->error(1, "Parameter PopulationSize not set properly.\n");

  if (!getParameter("Resamples",        &_resamples))      
      Log::instance()->error(1, "Parameter Resamples not set properly.\n");

  if (_alg_params)
    delete[] _alg_params;

  _nparam = 4;
  _alg_params = new AlgParameter[_nparam];

  char buffer[1024];
  sprintf(buffer, "%d", _max_gen); 
  _alg_params[0].setId("MaxGenerations");
  _alg_params[0].setValue(buffer);
  
  sprintf(buffer, "%f", _conv_limit); 
  _alg_params[1].setId("ConvergenceLimit");
  _alg_params[1].setValue(buffer);

  sprintf(buffer, "%d", _popsize); 
  _alg_params[2].setId("PopulationSize");
  _alg_params[2].setValue(buffer);

  sprintf(buffer, "%d", _resamples); 
  _alg_params[3].setId("Resamples");
  _alg_params[3].setValue(buffer);

  return 1;
}
  
