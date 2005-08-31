/**
 * Generic Best Subsets Procedure - template
 * 
 * @file   GenericBestSubsets.cpp
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

#include "GenericBestSubsets.hh"

#include <openmodeller/om.hh>
#include <openmodeller/Random.hh>
#include <openmodeller/Sampler.hh>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define NUM_PARAM 8

/************************************/
/*** Algorithm parameter metadata ***/

AlgParamMetadata parameters_bs[NUM_PARAM] = 
{
  // Best Subsets Parameters
  {
    "SubAlgorithm",
    "SubAlgorithm",
    "String",
    "Name of the algorithm to use.",
    "",
    0,
    0,
    0,
    0,
    "DG_GARP"
  },
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
    "1"       // Parameter's typical (default) value.
  }
};

/************************************/
/*** Algorithm's general metadata ***/

AlgMetadata metadata_bs = {
  
  "BestSubsets",                        // Id.
  "Generic Best Subsets",  // Name.
  "3.0.1 alpha",                       // Version.

  // Overview.
  "",

  // Description.
  "",

  // Author
  "",  

  // Bibliography.
  "",

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
  return new GenericBestSubsets();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata_bs;
}

/****************************************************************/
/****************** Garp class **********************************/

GenericBestSubsets::GenericBestSubsets()
  : AbstractBestSubsets(& metadata_bs)
{
}

// ****************************************************************
GenericBestSubsets::~GenericBestSubsets()
{
}

