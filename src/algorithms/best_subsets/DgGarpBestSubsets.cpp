/**
 * Declaration of GARP with Best Subsets Procedure
 * 
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

#include "DgGarpBestSubsets.hh"

#include <openmodeller/om.hh>
#include <openmodeller/Random.hh>
#include <openmodeller/Sampler.hh>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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
    "Percentage of distribution models to be taken regarding commission error.",

    // Description.
    "Percentage of distribution models to be taken regarding commission error.",

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
    "Maximum number of iterations (generations) run by the Genetic Algorithm.",

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
    "Defines the convergence value that makes the algorithm stop (before reaching MaxGenerations).",

    // Description.
    "Defines the convergence value that makes the algorithm stop (before reaching MaxGenerations).",

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
  
  "DG_GARP_BS",                        // Id.
  "GARP with best subsets - DesktopGARP implementation",  // Name.
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

OM_ALG_DLL_EXPORT 
AlgorithmImpl * 
algorithmFactory()
{
  return new DgGarpBestSubsets();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata_bs;
}

/****************************************************************/
/****************** Garp class **********************************/

DgGarpBestSubsets::DgGarpBestSubsets()
  : AbstractBestSubsets(& metadata_bs)
{
  _subAlgorithm = "DG_GARP";
}

// ****************************************************************
DgGarpBestSubsets::~DgGarpBestSubsets()
{
}

