/**
 * Declaration of Genetic Algorithm for Rule-Set Production (GARP)
 * 
 * @file   garp.cpp
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-04-01
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 * Copyright(c), David R.B. Stockwell of Symbiotik Pty. Ltd.
 * Copyright(c), CRIA - Centro de Referencia em Informacao Ambiental
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
 * This is an implementation of the GARP algorithm first developed 
 * by David Stockwell
 * 
 */


#include "garp.hh"
#include "rules_base.hh"
#include "rules_range.hh"
#include "rules_negrange.hh"
#include "rules_logit.hh"
#include "rules_atomic.hh"
#include "ruleset.hh"
#include "garp_sampler.hh"

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
  
  "GARP",                                            // Id.
  "GARP: Genetic Algorithm for Rule Set Production", // Name.
  "3.0.1 alpha",                                     // Version.

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

  "Stockwell, D. R. B.",  // Author

  // Bibliography.
  "Stockwell, D. R. B. 1999. Genetic algorithms II. \
Pages 123-144 in A. H. Fielding, editor. \
Machine learning methods for ecological applications. \
Kluwer Academic Publishers, Boston.\
\n\
Stockwell, D. R. B., and D. P. Peters. 1999. \
The GARP modelling system: Problems and solutions to automated \
spatial prediction. International Journal of Geographic \
Information Systems 13:143-158.\
\n\
Stockwell, D. R. B., and I. R. Noble. 1992. \
Induction of sets of rules from animal distribution data: \
A robust and informative method of analysis. Mathematics and \
Computers in Simulation 33:385-390.",

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

Garp::Garp()
  : Algorithm(& metadata)
{
  // fill in default values for parameters
  _popsize        = 0;
  _resamples      = 0;
  _max_gen        = 0;
  _crossover_rate = 0.0;
  _mutation_rate  = 0.0;
  _conv_limit     = 0.0;

  _mortality      = 0.9;
  _gapsize        = 0.9;
  _acc_limit      = 0.0;

  _significance   = 2.70;

  // reset private attributes
  _fittest = _offspring = NULL;
  _custom_sampler = NULL;

  _gen = 0;
  _convergence = 1.0;
  _improvements = 0;

  int i;
  for (i = 0; i < 5; i++)
    {
      _curr_heur_count[i] = 0;
      _prev_heur_count[i] = 0;
    }
}

Garp::~Garp()
{
  // debug
  if ( _fittest )
    {
      g_log( "Resulting rules:\n" );
      _fittest->log();
    }

  if (_offspring)
    delete _offspring;
  
  if (_fittest)
    delete _fittest;

  if (_custom_sampler)
    delete _custom_sampler;
}

// ****************************************************************
// ************* needNormalization ********************************

int Garp::needNormalization( Scalar *min, Scalar *max )
{
  *min = -1.0;
  *max = +1.0;

  return 1;
}
  
// ****************************************************************
// ************* initialize ***************************************

int Garp::initialize()
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

  _offspring  = new GarpRuleSet(2 * _popsize);
  _fittest    = new GarpRuleSet(2 * _popsize);

  _custom_sampler = new GarpCustomSampler;
  _custom_sampler->initialize(_samp, _resamples);
  _custom_sampler->createBioclimHistogram();

  colonize(_offspring, _custom_sampler, _popsize);

  return 1;
}
  
/****************************************************************/
/****************** Algorithm's factory function ****************/

int Garp::iterate()
{
  char msg[256];
  const PerfIndex defaultPerfIndex = PerfSig;

  if (!done())
    {
      _gen++;

      g_log( "%4d] ", _gen );

      evaluate(_offspring, _custom_sampler);
      select(_offspring, _fittest, defaultPerfIndex);
      _fittest->trim(_popsize);

      double perfBest, perfWorst, perfAvg;
      _offspring->performanceSummary(PerfSig, 
                                     &perfBest, &perfWorst, &perfAvg);

      sprintf(msg, "[%2d] %+8.3f %+8.3f %+8.3f", 
	     _offspring->numRules(), perfBest, perfWorst, perfAvg );

      _fittest->performanceSummary(PerfSig, 
                                   &perfBest, &perfWorst, &perfAvg);

      g_log( "[%2d] %+8.3f %+8.3f %+8.3f | %s\n", 
	     _fittest->numRules(), perfBest, perfWorst, perfAvg, msg );
      
      if (done())
        {
          // finalize processing of model
          // by filtering out rules that have low performance 
          _fittest->filter(defaultPerfIndex, _significance);
        }

      else
        {
          // algorithm is not done yet
          // create new offspring
          reproduce(_fittest, _offspring, _gapsize);

          // fill rest
          //int new_popsize = (int) (_mortality * (double) _popsize);
          colonize(_offspring, _custom_sampler, _popsize);
          _offspring->trim(_popsize);
          mutate(_offspring);
          crossover(_offspring);
        }
    }

  return 1;
}
  
/****************************************************************/
/****************** Algorithm's factory function ****************/

int Garp::done()
{
  return ( (_gen >= _max_gen) || (_convergence < _conv_limit) );
}

/****************************************************************/
/****************** Algorithm's factory function ****************/

Scalar Garp::getValue( Scalar *x )
{
  return _fittest->getValue(x);
}
  
/****************************************************************/
/****************** Algorithm's factory function ****************/

int Garp::getConvergence( Scalar *val )
{
  *val = _convergence;
  return 0;
}


/****************************************************************/
/***************** GARP Algorithm private methods ***************/
/****************************************************************/


/****************************************************************/
/***************** select ***************************************/

void Garp::select(GarpRuleSet * source, GarpRuleSet * target, 
                  PerfIndex perfIndex)
{
  int i, n, accepted, similarIndex;
  GarpRule * candidateRule, * similarRule;
  //int discardedRuleOrigin;

  // step through source rule-set trying to insert rules into target
  n = source->numRules();
  for (i = 0; i < n; i++)
    {
      accepted = 0;
      candidateRule = source->get(i);
      similarIndex = target->findSimilar(candidateRule);
      if ((similarIndex < -1) || (similarIndex >= target->numRules()))
        g_log.error(1, "Index out of bounds (#8). Limits are (-1, %d), index is %d\n", target->numRules(), similarIndex);

      if (similarIndex >= 0)
        {
          // similar rule found replace it if better
          similarRule = target->get(similarIndex);
          if (candidateRule->getPerformance(perfIndex) > 
              similarRule->getPerformance(perfIndex))
            {
              // first create a clone of the rule, then replace the old one
              candidateRule = candidateRule->clone();
	      target->remove(similarIndex);
	      target->insert(perfIndex, candidateRule);
            }
        }
      else
        {
          // no similar rule found: try to insert it into existing set
          // first create a clone of the rule, then insert it into 
          // the target rs
          candidateRule = candidateRule->clone();
          target->insert(perfIndex, candidateRule);
        }
    }

  // TODO: trim target rule-set to population size, keeping track of what 
  //       rules where excluded
  // TODO: update heuristic rates based on who entered the target rule-set
}

/****************************************************************/
/***************** evaluate *************************************/

void Garp::evaluate(GarpRuleSet * ruleset, GarpCustomSampler * sampler)
{
  int i, n;
  
  n = ruleset->numRules();
  for (i = 0; i < n; i++)
  { 
    ruleset->get(i)->evaluate(sampler);
  }

  return;
}

/****************************************************************/
/***************** colonize *************************************/

void Garp::colonize(GarpRuleSet * ruleset, GarpCustomSampler * sampler, 
                    int numRules)
{
  int i, p;
  GarpRule * rule;
  
  for (i = ruleset->numRules(); i < numRules; i++)
    {
      // pick the next rule to be generated
      p = i % 4;

      switch (p)
	{
	case 0: rule = new RangeRule(); break;
	case 1: rule = new LogitRule(); break;
	case 2: rule = new NegatedRangeRule(); break;
	case 3: rule = new AtomicRule(); break;
	}

      //g_log("[%c] ", rule->type());

      rule->initialize(sampler);
      ruleset->add(rule);
    }
}

/****************************************************************/
/***************** reproduce ************************************/

void Garp::reproduce(GarpRuleSet * source, GarpRuleSet * target, 
                     double gapsize)
{
  Random rnd;
  int * sample;
  int i, j, k, n, temp;
  double perfBest, perfWorst, perfAvg;
  double sum, ptr, factor, expected, rulePerf, size;
  perfBest = perfWorst = perfAvg = 0.0;
  GarpRule * pRuleBeingInserted;
  
  source->performanceSummary(PerfSig, &perfBest, &perfWorst, &perfAvg);

  //g_log( "Performances: %f %f %f.\n", perfBest, perfWorst, perfAvg );

  // normalizer for proportional selection probabilities
  if (perfAvg - perfWorst) 
    factor = 1.0 / (perfAvg - perfWorst);
  else 
    factor = 1.0;

  // Stochastic universal sampling algorithm by James E. Baker 
  k = 0;
  n = source->numRules();
  sample = new int[_popsize + 1];
  for (i = 0; i < _popsize; i++)
    sample[i] = i % n;

  ptr = rnd.get(1.0);
  for (sum = i = 0; i < n; i++)
    {
      rulePerf = source->get(i)->getPerformance(PerfUtil);
      if ( rulePerf > perfWorst)
        expected = (rulePerf - perfWorst) * factor;
      else 
        expected = 0.0;

      for (sum += expected; (sum > ptr) && (k <= _popsize); ptr++)	
        {
          if ((k < 0) || (k > _popsize))
            g_log.error(1, "Index out of bounds (#6). Limits are (0, %d), index is %d\n", 
                        _popsize, k);

          sample[k++] = i;
        }
    }

  // randomly shuffle pointers to new structures 
  for (i = 0; i < _popsize; i++)
    {
      j = rnd.get (i , _popsize - 1);
      temp = sample[j];
      sample[j] = sample[i];
      sample[i] = temp;
    }

  // finally, form the new population 
  // Gapsize giving the proportion contribution
  // to the new population from the objBest archive set 
  target->clear();
  size = ((double) _popsize) * gapsize;

  for (i = 0; i < size; i++)
    {
      pRuleBeingInserted = source->get(sample[i])->clone();
      pRuleBeingInserted->forceEvaluation();
      if (!target->add(pRuleBeingInserted))
        // target rs is full
        g_log.error(1, "Garp::reproduce(): Target rule set is full");
    }
  delete sample;
}

/****************************************************************/
/***************** mutate ***************************************/

void Garp::mutate(GarpRuleSet * ruleset)
{
  int i, n;

  double temperature = (double) _max_gen / (double) _gen;
  n = ruleset->numRules();
  for (i = 0; i < n; i++)
    ruleset->get(i)->mutate(temperature);
}

/****************************************************************/
/***************** crossover ************************************/

void Garp::crossover(GarpRuleSet * ruleset)
{
  Random rnd;
  int nrules, genes, xcount, last, diff, mom, dad, xpt1, xpt2;

  genes = _samp->numIndependent() * 2;
  nrules = ruleset->numRules();
  last = (int) (_crossover_rate * (double) nrules);

  for (xcount = 0; xcount < last; xcount += 2)
  {
    diff = 0;
    mom = rnd.get(nrules);
    dad = rnd.get(nrules);
    if (dad == mom)
      dad = (dad + 1) % nrules;

    xpt1 = rnd.get(genes);
    xpt2 = rnd.get(genes);

    ruleset->get(mom)->crossover(ruleset->get(dad), xpt1, xpt2);
  }
}


/****************************************************************/
/**** This is a debug function that checks if a rule set is 
 **** correctly sorted. If not it dumps the performance values
 **** for that rule set. 
 **** It was used to debug Garp::select() (replace call bug)
 **** TODO: move this code to the test harness when we have one */

void printPerfs(char * msg, int index, GarpRuleSet * ruleset)
{
  for (int i = 1; i < ruleset->numRules(); i++)
    {
      if (ruleset->get(i - 1)->getPerformance((PerfIndex)8) < 
	  ruleset->get(i)->getPerformance((PerfIndex)8))
	{
	  printf("\nError: rule set out of sort order (Index: %d)\n", index);
	  for (int i = 0; i < ruleset->numRules(); i++)
	    {
	      printf("[%2d]=%6.3f ", i, ruleset->get(i)->getPerformance((PerfIndex)8) );
	      if ((i + 1) % 5 == 0)
		printf("\n");
	    }
	}
    }
}

