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
 * by David Stockwell and modified by Ricardo Scachetti Pereira
 * 
 */

#include "garp.hh"
#include "rules_base.hh"
#include "rules_range.hh"
#include "rules_negrange.hh"
#include "rules_logit.hh"
#include "ruleset.hh"
#include "bioclim_histogram.hh"
#include "regression.hh"
#include <configuration.hh>
#include <random.hh>
#include <Exceptions.hh>

#include <string>
using std::string;

#define NUM_PARAM 4

/****************************************************************/
/*** Algorithm parameter metadata *******************************/

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
    "",

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


/*****************************************************************/
/*** Algorithm's general metadata ********************************/

AlgMetadata metadata = {
  
  "GARP",                                            // Id.
  "GARP - oM implementation", // Name.
  "3.1 beta",                                        // Version.

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
  "Stockwell, D. R. B., modified by Ricardo Scachetti Pereira",  

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
#ifndef DONT_EXPORT_GARP_FACTORY
OM_ALG_DLL_EXPORT 
AlgorithmImpl * 
algorithmFactory()
{
  return new Garp();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}
#endif


/****************************************************************/
/****************** Garp class **********************************/

const PerfIndex defaultPerfIndex = PerfSig;

/****************************************************************/
/****************** Garp constructor ****************************/

Garp::Garp()
  : AlgorithmImpl(& metadata)
{
  // fill in default values for parameters
  _popsize        = 0;
  _resamples      = 0;
  _max_gen        = 0;
  _conv_limit     = 0.0;

  _mortality      = 0.9;
  _gapsize        = 0.1;
  _acc_limit      = 0.0;

  _crossover_rate = 0.1;
  _mutation_rate  = 0.6;

  _significance   = 2.70;

  _maxProgress = 0.0;

  // reset private attributes
  _fittest = _offspring = NULL;

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

/****************************************************************/
/****************** Garp destructor *****************************/
Garp::~Garp()
{
  // debug
  if ( _fittest )
    {
      //g_log.debug( "Resulting rules:\n");
      //_fittest->log();
    }

  if (_offspring)
    delete _offspring;
  
  if (_fittest)
    delete _fittest;
}

// ****************************************************************
// ************* needNormalization ********************************

int Garp::needNormalization( Scalar *min, Scalar *max ) const
{
  *min = -1.0;
  *max = +1.0;

  return 1;
}
  
// ****************************************************************
// ************* initialize ***************************************

int Garp::initialize()
{
  if (!getParameter("MaxGenerations",   &_max_gen))        
      g_log.error(1, "Parameter MaxGenerations not set properly.");

  if (!getParameter("ConvergenceLimit", &_conv_limit))     
      g_log.error(1, "Parameter ConvergenceLimit not set properly.");

  if (!getParameter("PopulationSize",   &_popsize))        
      g_log.error(1, "Parameter PopulationSize not set properly.");

  if (!getParameter("Resamples",        &_resamples))      
      g_log.error(1, "Parameter Resamples not set properly.");

  //g_log.debug("MaxGenerations set to:   %d\n", _max_gen);
  //g_log.debug("ConvergenceLimit set to: %.4f\n", _conv_limit);
  //g_log.debug("PopulationSize set to:   %d\n", _popsize);
  //g_log.debug("Resamples set to:        %d\n", _resamples);

  _offspring  = new GarpRuleSet(2 * _popsize);
  _fittest    = new GarpRuleSet(2 * _popsize);

  cacheSamples(_samp, _cachedOccs, _resamples);
  _bioclimHistogram.initialize(_cachedOccs);
  _regression.calculateParameters(_cachedOccs);

  colonize(_offspring, _popsize);

  return 1;
}
  
/****************************************************************/
/****************************************************************/
void Garp::cacheSamples(const SamplerPtr& sampler, 
			OccurrencesPtr& cachedOccs, 
			int resamples)
{
  OccurrencesImpl * occs = new OccurrencesImpl( "", 
						GeoTransform::cs_default );
  occs->reserve( resamples );
  cachedOccs = ReferenceCountedPointer<OccurrencesImpl>( occs );
  
  for (int i = 0; i < resamples; ++i)
    {
      OccurrencePtr oc = sampler->getOneSample();
      cachedOccs->insert(oc); 
    }
}


/****************************************************************/
/****************** iterate *************************************/

int Garp::iterate()
{
  double perfBest, perfWorst, perfAvg;

  if (done())
    return 1;

  _gen++;
  
  evaluate(_offspring);
  keepFittest(_offspring, _fittest, defaultPerfIndex);
  _fittest->trim(_popsize);
  
  //_fittest->gatherRuleSetStats(_gen);
  //_offspring->gatherRuleSetStats(-_gen);
  
  
  _fittest->performanceSummary(defaultPerfIndex, 
			       &perfBest, &perfWorst, &perfAvg);
  /*  
  // log info about current iteration
  g_log.debug( "%4d] ", _gen );
  g_log.debug( "[%2d] conv=%+7.4f | perfs=%+8.3f, %+8.3f, %+8.3f\n", _fittest->numRules(), 
	 _convergence, perfBest, perfWorst, perfAvg );
  */

  if (done())
    {
      // finalize processing of model
      // by filtering out rules that have low performance 
      _fittest->filter(defaultPerfIndex, _significance);
      return 1;
    }
  
  // algorithm is not done yet
  // select fittest individuals 
  select(_fittest, _offspring, _gapsize);
  
  // create new offspring
  colonize(_offspring, _popsize);
  _offspring->trim(_popsize);
  mutate(_offspring);
  crossover(_offspring);

  return 1;
}
  
/****************************************************************/
/****************** getProgress *********************************/

float Garp::getProgress() const
{
  if (done())
    { return 1.0; }
  else
    { 
      float byIterations  = ( _gen / (float) _max_gen );
      float byConvergence = ( _conv_limit / _convergence );
      float progress = (byIterations > byConvergence) ? byIterations : byConvergence; 
      if (progress > _maxProgress)
	{ _maxProgress = progress; }
      return _maxProgress;
    } 
}

/****************************************************************/
/****************** done ****************************************/

int Garp::done() const
{
  return ( (_gen >= _max_gen) || (_convergence < _conv_limit) );
}

/****************************************************************/
/****************** getValue ************************************/

Scalar Garp::getValue( const Sample& x ) const
{
  return _fittest->getValue(x);
}
  
/****************************************************************/
/****************** getConvergence ******************************/

int Garp::getConvergence( Scalar *val )
{
  *val = _convergence;
  return 0;
}

/****************************************************************/
/****************** configuration *******************************/
void
Garp::_getConfiguration( ConfigurationPtr& config ) const
{
  if ( !done() )
    return;

  ConfigurationPtr model_config ( new ConfigurationImpl("GarpModel") );
  config->addSubsection( model_config );

  model_config->addNameValue( "Generations", _gen );
  model_config->addNameValue( "AccuracyLimit", _acc_limit );
  model_config->addNameValue( "Mortality", _mortality );
  model_config->addNameValue( "Significance", _significance );
  model_config->addNameValue( "FinalCrossoverRate", _crossover_rate );
  model_config->addNameValue( "FinalMutationRate", _mutation_rate );
  model_config->addNameValue( "FinalGapSize", _gapsize );

  if ( _fittest ) {
    int nrules = _fittest->numRules();
 
    ConfigurationPtr rules_config( new ConfigurationImpl("FittestRules") );
    model_config->addSubsection( rules_config );

    rules_config->addNameValue( "Count", nrules );

    for( int i=0; i<nrules; i++ ) {
      GarpRule *rule = _fittest->get(i);
      char type[16];
      sprintf(type, "%c", rule->type() );

      ConfigurationPtr rule_config( new ConfigurationImpl("Rule") );
      rules_config->addSubsection( rule_config );

      rule_config->addNameValue( "Type", type );
      rule_config->addNameValue( "Prediction", rule->getPrediction() );
      rule_config->addNameValue( "Chromosome1", rule->getChrom1());
      rule_config->addNameValue( "Chromosome2", rule->getChrom2());
      rule_config->addNameValue( "Performance", rule->getPerformanceArray(), 10 );
    }
  }

}

void
Garp::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "GarpModel", false );

  if (!model_config)
    return;

  _gen = model_config->getAttributeAsInt( "Generations", 0 );
  _acc_limit = model_config->getAttributeAsDouble( "AccuracyLimit", 0.0 );
  _mortality = model_config->getAttributeAsDouble( "Mortality", 0.0 );
  _significance = model_config->getAttributeAsDouble( "Significance", 0.0 );
  _crossover_rate = model_config->getAttributeAsDouble( "FinalCrossoverRate", 0.0 );
  _mutation_rate = model_config->getAttributeAsDouble( "FinalMutationRate", 0.0 );
  _gapsize = model_config->getAttributeAsDouble( "FinalGapSize", 0.0 );

  _offspring = new GarpRuleSet( 2 * _popsize );
  _fittest = new GarpRuleSet( 2 * _popsize );

  /*
   * This code is commented out for now.  Need to figure out how
   * to get the algorithm primed with its custom sampler after
   * it's deserialized.
   */
  //_bioclimHistogram.initialize( _samp, _resamples );

  ConstConfigurationPtr rules_config = model_config->getSubsection( "FittestRules" );

  int nrules = rules_config->getAttributeAsInt( "Count", 0 );

  Configuration::subsection_list::const_iterator ss;
  for( ss = rules_config->getAllSubsections().begin();
       ss != rules_config->getAllSubsections().end();
       ++ss ) {

    const ConstConfigurationPtr& c(*ss);
    GarpRule * rule = NULL;

    string type = c->getAttribute( "Type" );

    Scalar pred = c->getAttributeAsDouble( "Prediction", 0.0 );

    Sample p_chrom1 = c->getAttributeAsSample( "Chromosome1" ); 

    Sample p_chrom2 = c->getAttributeAsSample( "Chromosome2" );

    Scalar *p_perf;
    int n_perf;
    c->getAttributeAsDoubleArray( "Performance", &p_perf, &n_perf );

    switch( type[0] ) {
    case 'd':
      rule = new RangeRule( pred, p_chrom1.size(), p_chrom1, p_chrom2, p_perf );
      break;

    case 'r':
      rule = new LogitRule( pred, p_chrom1.size(), p_chrom1, p_chrom2, p_perf );
      break;

    case '!':
      rule = new NegatedRangeRule( pred, p_chrom1.size(), p_chrom1, p_chrom2, p_perf );
      break;

    }

    delete [] p_perf;

    _fittest->add(rule);

  }

}

/****************************************************************/
/***************** GARP Algorithm private methods ***************/
/****************************************************************/


/****************************************************************/
/***************** keepFittest **********************************/

void Garp::keepFittest(GarpRuleSet * source, GarpRuleSet * target, 
		     PerfIndex perfIndex)
{
  int i, n, accepted, converged, similarIndex, insertIndex;
  GarpRule * candidateRule, * similarRule;

  converged = 0;

  // step through source rule-set trying to insert rules into target
  n = source->numRules();
  for (i = 0; i < n; i++)
    {
      insertIndex = -1;
      accepted = 0;
      candidateRule = source->get(i);
      similarIndex = target->findSimilar(candidateRule);
      //if ((similarIndex < -1) || (similarIndex >= target->numRules()))
      //  g_log.error(1, "Index out of bounds (#8). Limits are (-1, %d), index is %d\n", target->numRules(), similarIndex);

      if (similarIndex >= 0)
        {
          converged++;

	  // similar rule found replace it if better
          similarRule = target->get(similarIndex);
          if (candidateRule->getPerformance(perfIndex) > 
              similarRule->getPerformance(perfIndex))
            {
              // first create a clone of the rule, then replace the old one
              candidateRule = candidateRule->clone();
	      //target->replace(similarIndex, candidateRule);
	      target->remove(similarIndex);
	      insertIndex = target->insert(perfIndex, candidateRule);
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

      //printf("InsertIndex=%+3d | converged=%3d\n", insertIndex, converged);
    }

  // update convergence value
  _improvements += converged;
  if (_improvements)
    { _convergence = ( _convergence + ( (double) converged ) / _improvements ) / 2.0; }
  else
    { _convergence = 1.0; }

  /*
  printf("Convergence: %+7.4f at generation %5d (%3d; %6d; %+7.4f) similar=%d\n", _convergence, 
  	 _gen, converged, _improvements, ( (double) converged ) / _improvements, similarIndex);
  */

  // TODO: update heuristic rates based on who entered the target rule-set
}

/****************************************************************/
/***************** evaluate *************************************/

void Garp::evaluate(GarpRuleSet * ruleset)
{
  int i, n;
  
  n = ruleset->numRules();
  for (i = 0; i < n; i++)
  { 
    ruleset->get(i)->evaluate(_cachedOccs);
  }

  return;
}

/****************************************************************/
/***************** colonize *************************************/

void Garp::colonize(GarpRuleSet * ruleset, int numRules)
{
  int i, p, dim;
  GarpRule * rule;
  Random rnd;
  
  dim = _samp->numIndependent();

  for (i = ruleset->numRules(); i < numRules; i++)
    {
      // pick the next rule to be generated
      p = rnd(3);

      switch (p)
	{
      case 0: 
        rule = new RangeRule(dim);
	rule->setPrediction(1.0);
        ((RangeRule *) rule)->initialize(_bioclimHistogram);
        break;

      case 1: 
        rule = new NegatedRangeRule(dim); 
	rule->setPrediction(0.0);
        ((NegatedRangeRule *) rule)->initialize(_bioclimHistogram);
        break;

      case 2: 
        rule = new LogitRule(dim); 
	Scalar pred = (rnd.get(0.0, 1.0) > 0.5) ? 1.0 : 0.0;
	rule->setPrediction(pred);
        ((LogitRule *) rule)->initialize(_regression);
        break;
	}

      //g_log.debug("[%c] ", rule->type());
      ruleset->add(rule);
    }
}

/****************************************************************/
/***************** select ***************************************/

void Garp::select(GarpRuleSet * source, GarpRuleSet * target, 
                     double gapsize)
{
  Random rnd;
  int * sample;
  int i, j, k, n, temp;
  double perfBest, perfWorst, perfAvg;
  double sum, ptr, factor, expected, rulePerf, size;
  perfBest = perfWorst = perfAvg = 0.0;
  GarpRule * pRuleBeingInserted;
  
  source->performanceSummary(defaultPerfIndex, &perfBest, &perfWorst, &perfAvg);

  //g_log.debug( "Performances: %f %f %f.\n", perfBest, perfWorst, perfAvg );

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
  sum = 0.0;
  for (i = 0; i < n; i++)
    {
      rulePerf = source->get(i)->getPerformance(defaultPerfIndex);
      expected = (rulePerf - perfWorst) * factor;
      for (sum += expected; (sum > ptr) && (k <= _popsize); ptr++)	
        {
          if ((k < 0) || (k > _popsize))
            g_log.error(1, "Index out of bounds (#6). Limits are (0, %d), index is %d\n", 
                        _popsize, k);

          sample[k++] = i;
        }
    }

  /*
  FILE * f = stdout;
  fprintf(f, "Generation: %4d\n", _gen);
  for (i = 0; i < _popsize; i++)
    fprintf(f, "%+9.4f %3d\n", source->get(sample[i])->getPerformance(defaultPerfIndex), sample[i]);
  */

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
  delete[] sample;
}

/****************************************************************/
/***************** mutate ***************************************/

void Garp::mutate(GarpRuleSet * ruleset)
{
  int i, n;

  double temperature = 2.0 / (double) _gen;
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

  genes = _samp->numIndependent();
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


// *****************
void Garp::deleteTempDataMembers()
{
  if (_offspring)
    delete _offspring;
  _offspring = NULL;
}

/****************************************************************/
/**** This is a debug function that checks if a rule set is 
 **** correctly sorted. If not it dumps the performance values
 **** for that rule set. 
 **** It was used to debug Garp::keepFittest() (replace call bug)
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

