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


#include "garp.hh"

#include <om.hh>
#include <random.hh>
#include <om_sampler.hh>
#include <om_sampled_data.hh>

#define NUM_PARAM 6

/************************************/
/*** Algorithm parameter metadata ***/

AlgParamMetadata parameters[NUM_PARAM] = 
{
  // Metadata of the first parameter.
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
    "100"   // Parameter's typical (default) value.
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
  },

  {
    "MutationRate",        // Id.
    "Mutation rate",       // Name.
    "Real",                // Type.

    // Overview.
    "Chance of a mutation occurring during reproduction.",

    // Description.
    "",

    1,     // Not zero if the parameter has lower limit.
    0.0,   // Parameter's lower limit.
    1,     // Not zero if the parameter has upper limit.
    1.0,   // Parameter's upper limit.
    "0.25"  // Parameter's typical (default) value.
  },

  {
    "CrossoverRate",      // Id.
    "Crossover rate",     // Name.
    "Real",               // Type.

    // Overview.
    "Chance of a crossover occurring during reproduction.",

    // Description.
    "",

    1,     // Not zero if the parameter has lower limit.
    0.0,   // Parameter's lower limit.
    1,     // Not zero if the parameter has upper limit.
    1.0,   // Parameter's upper limit.
    "0.25"  // Parameter's typical (default) value.
  },
};


/************************************/
/*** Algorithm's general metadata ***/

AlgMetadata metadata = {
  
  "GARPBS",                            // Id.
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
  "Anderson, R. P., Lew, D., Peterson, A. T.",  

  // Bibliography.
  "Anderson, R. P., D. Lew, and A. T. Peterson. 2003. \
Evaluating predictive models of species' distributions: criteria \
for selecting optimal models.Ecological Modelling, v. 162, p. 211 232.",

  "Ricardo Scachetti Pereira",  // Code author.
  "rpereira [at] ku.edu",       // Code author's contact.

  0,  // Does not accept categorical data.
  1,  // Does not need (pseudo)absence points.

  NUM_PARAM,   // Algorithm's parameters.
  parameters
};


/****************************************************************/
/****************** Algorithm's factory function ****************/

#ifdef WIN32
__declspec( dllexport )
#endif
Algorithm * algorithmFactory()
{
  return new Garp;
}


/****************************************************************/
/****************** Garp class **********************************/

GarpBestSubsets::Garp()
  : Algorithm(& metadata)
{
}

GarpBestSubsets::~Garp()
{
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
  g_log.set( Log::Debug, stdout, "Garp" );

  if (!getParameter("MaxGenerations",   &_max_gen))        
      g_log.error(1, "Parameter MaxGenerations not set properly.");

  if (!getParameter("ConvergenceLimit", &_conv_limit))     
      g_log.error(1, "Parameter ConvergenceLimit not set properly.");

  if (!getParameter("PopulationSize",   &_popsize))        
      g_log.error(1, "Parameter PopulationSize not set properly.");

  if (!getParameter("Resamples",        &_resamples))      
      g_log.error(1, "Parameter Resamples not set properly.");

  if (!getParameter("MutationRate",     &_mutation_rate))  
      g_log.error(1, "Parameter MutationRate not set properly.");

  if (!getParameter("CrossoverRate",    &_crossover_rate)) 
      g_log.error(1, "Parameter CrossoverRate not set properly.");

  // TODO: fill it in


  // setup threads
  

  return 1;
}
  
/****************************************************************/
/****************** Algorithm's factory function ****************/

int GarpBestSubsets::iterate()
{
  if (!done())
    {

      // wait for a slot for a new thread
      while (numActiveThreads() >= maxActiveThreads)
	{ SLEEP(1); }

      if (!done())
	{
	  garpRun = new GarpRun();
	  garpRun->initialize(runCount, sampler);
	  garpRun[runCount] = garpRun;
	  runCount++;
	  numActiveThreads++;
	}
    }

  return 1;
}
  
/****************************************************************/
/****************** Algorithm's factory function ****************/

int GarpBestSubsets::done()
{
}

/****************************************************************/
/****************** Algorithm's factory function ****************/

Scalar GarpBestSubsets::getValue( Scalar *x )
{
}
  
/****************************************************************/
/****************** Algorithm's factory function ****************/

int GarpBestSubsets::getConvergence( Scalar *val )
{
  *val = _convergence;
  return 0;
}

/******************/
/*** serialize ***/
int
GarpBestSubsets::serialize(Serializer * s)
{
  char type[16];
  int i, nrules, ngenes;

  s->writeStartSection("GarpModel");
  s->writeInt("Generations", _gen);
  s->writeInt("MaxGenerations", _max_gen);
  s->writeInt("PopulationSize", _popsize);
  s->writeInt("Resamples", _resamples);
  s->writeDouble("AccuracyLimit", _acc_limit);
  s->writeDouble("ConvergenceLimit", _conv_limit);
  s->writeDouble("Mortality", _mortality);
  s->writeDouble("Significance", _significance);
  s->writeDouble("FinalCrossoverRate", _crossover_rate);
  s->writeDouble("FinalMutationRate", _mutation_rate);
  s->writeDouble("FinalGapSize", _gapsize);

  // dump fittest rule set
  nrules = _fittest->numRules();
  ngenes = 2 * _custom_sampler->dim();
  s->writeStartSection("FittestRules", nrules);
  for (i = 0; i < nrules; i++)
    {
      GarpRule * rule = _fittest->get(i);
      sprintf(type, "%c", rule->type());
      s->writeStartSection("Rule");
      s->writeString("Type", type);
      s->writeDouble("Prediction", rule->getPrediction());
      s->writeArrayDouble("Genes", rule->getGenes(), rule->numGenes() * 2);
      s->writeArrayDouble("Performance", rule->getPerformanceArray(), 10);
      s->writeEndSection("Rule");
    }
  s->writeEndSection("FittestRules");
  s->writeEndSection("GarpModel");

  return 1;
}

/********************/
/*** deserialize ***/
int
GarpBestSubsets::deserialize(Deserializer * ds)
{
  int i, nelems, ngenes, nperf, nrules;

  ds->readStartSection("GarpModel");
  _gen = ds->readInt("Generations");
  _max_gen = ds->readInt("MaxGenerations");
  _popsize = ds->readInt("PopulationSize");
  _resamples = ds->readInt("Resamples");
  _acc_limit = ds->readDouble("AccuracyLimit");
  _conv_limit = ds->readDouble("ConvergenceLimit");
  _mortality = ds->readDouble("Mortality");
  _significance = ds->readDouble("Significance");
  _crossover_rate = ds->readDouble("FinalCrossoverRate");
  _mutation_rate = ds->readDouble("FinalMutationRate");
  _gapsize = ds->readDouble("FinalGapSize");

  // set parameters that were ommited above with values from parameter array
  // also initialize data structures with default values
  _offspring  = new GarpRuleSet(2 * _popsize);
  _fittest    = new GarpRuleSet(2 * _popsize);

  _custom_sampler = new GarpCustomSampler;
  _custom_sampler->initialize(_samp, _resamples);
  //_custom_sampler->createBioclimHistogram();

  // load fittest rule set
  nrules = ds->readStartSection("FittestRules");
  
  for (i = 0; i < nrules; i++)
    {
      GarpRule * rule = NULL;

      ds->readStartSection("Rule");
      char * type = ds->readString("Type");
      Scalar pred = ds->readDouble("Prediction");
      Scalar * genes = ds->readArrayDouble("Genes", &nelems); 
      ngenes = nelems / 2;
      Scalar * perfs = ds->readArrayDouble("Performance", &nperf);
      ds->readEndSection("Rule");

      switch (*type)
	{
	case 'a': rule = new AtomicRule(pred, ngenes, genes, perfs); break;
	case 'd': rule = new RangeRule(pred, ngenes, genes, perfs); break;
	case 'r': rule = new LogitRule(pred, ngenes, genes, perfs); break;
	case '!': rule = new NegatedRangeRule(pred, ngenes, genes, perfs); break;
	}

      delete genes;
      delete perfs;

      _fittest->add(rule);
    }

  ds->readEndSection("FittestRules");
  ds->readEndSection("GarpModel");

  return 1;
}

