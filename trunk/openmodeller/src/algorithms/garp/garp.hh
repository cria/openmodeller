/**
 * Declaration of Genetic Algorithm for Rule-Set Production (GARP)
 * 
 * @file   garp.hh
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


#ifndef _GARP_HH_
#define _GARP_HH_

#include <om.hh>

// required include because of enum PerfIndex and class GarpRule
#include <rules_base.hh> 
#include <serialization/serializable.hh>

class GarpRuleSet;
class GarpCustomSampler;

class Sampler;
class SampledData;
class Random;


/****************************************************************/
/************************* GARP Algorithm ***********************/

class Garp : public Algorithm
{
public:
  Garp();
  virtual ~Garp();

  /** This method is used when you want to ensure that all variables in all
    * environmental layers are scaled to the same value range. 
    * GARP requires values to be normalized between -1.0 and 1.0.
    * @param Scalar pointer min that the post normalised will be fitted to
	*        Always set to -1.0 in GARP
    * @param Scalar pointer max that the post normalised will be fitted to
	*        Always set to +1.0 in GARP
    * @return Always return 1 for GARP.
   */
  int needNormalization( Scalar *min, Scalar *max );
  
  /** Initialize model by colonizing it with new rules.
    * @note This method is inherited from the Algorithm class
    * @return 0 on error
    */
  int initialize();
  
  /** Start model execution (build the model). This method is the main body of the Genetic Algorithm.
    * The steps during the algorithm are:
    *  1) Evaluate current population testing the rules against data points
    *  2) Select fittest individuals from population based on significance
    *  3) If reached any stop condition (# of iterations or convergence limit) then output best result and stop
    *  4) Else perform reproduction, generating a new offspring from current population
    *  5) Repeat from step 1.
    * @return 0 on error 
    */
  int iterate();
  
  /** Check if the model generation is completed (e.g. convergence
    * point has been met. 
    * @return Implementation specific but usually 1 for completion.
    */
  int done();

  //
  // Methods used to project the model
  //
  
  /** This method is used when projecting the model.  
    * @note This method is inherited from the Algorithm class
    * @return     
    * @param Scalar *x a pointer to a vector of openModeller Scalar 
    *        type (currently double). The vector should contain values 
    *        looked up on the environmental variable layers into which 
    *        the mode is being projected. 
    */
  Scalar getValue( Scalar *x );
  
  /** Returns a value that represents the convergence of the algorithm
    * expressed as a number between 0 and 1 where 0 represents model
    * completion. 
    * @return 
    * @param Scalar *val 
    */
  int getConvergence( Scalar *val );

  /*
   */
  int serialize(Serializer * serializer);
  int deserialize(Deserializer * deserializer);

private:

/** Fill a rule set with newly generated rules
  * @param GarpRuleSet * ruleset: rule set where rules will be added to
  * @param GarpCustomSampler * sampler: sampler that will provide
  *        data points and other data used during rule creation.
  * @param numRules number of rules to be added to the rule set.
  */
  void colonize(GarpRuleSet * ruleset, GarpCustomSampler * sampler, int numRules);

/** Evaluate rules from a rule set based on their performance to predict 
  *  points provided by a sampler object.
  * @param GarpRuleSet * ruleset: rule set to be tested.
  * @param GarpCustomSampler * sampler: sampler that will provide
  *        data points for testing.
  */
  void evaluate(GarpRuleSet * ruleset, GarpCustomSampler * sampler);

/** Select fittest individuals from source rule set storing them in target 
  *   rule set in descending order of fitness.     
  * @param GarpRuleSet * source: pointer to the source rule set where rules 
  *        will be selected from (usually the previous generated population)
  * @param GarpRuleSet * target: pointer to the target rule set where 
  *        fittest rules will be transferred to (usually the fittest rule set)
  * @param PerfIndex perfIndex: Index of value on performance array that
  *        will be used to select individuals.
  * @note: This method produces a side effect that is to update the
  *        statistics about heuristic operators performance (that will be
  *        used to adjust probabilities of application of those operators on
  *        future generations) and overall rule performance that will be used
  *        to compute the changes to the convergence value.
  */
  void select(GarpRuleSet * source, GarpRuleSet * target, PerfIndex perfIndex);


/** Perform reproduction, generating a new offspring from current population
  * @param GarpRuleSet * source: pointer to the rule set containing parents
  *        (individuals that will provide genes to create new individuals).
  * @param GarpRuleSet * target: pointer to the target rule set where 
  *        offspring will be transferred to.
  */
  void reproduce(GarpRuleSet * source, GarpRuleSet * target, double gapsize);


/** Mutate rules in a rule set
  * @param ruleset Rule set containing rules to be mutated
  */
  void mutate(GarpRuleSet * ruleset);


/** Perform crossing over with rules in a rule set
  * @param ruleset Rule set containing rules to be crossed over
  */
  void crossover(GarpRuleSet * ruleset);

  //
  // Algorithm parameters
  //

  /** Maximum number of iterations to be executed (if convergence is not reached) */
  int _max_gen;

  /** Number of points to be considered after resampling (with replacement) */
  int _popsize;

  /** Number of points to be considered after resampling (with replacement) */
  int _resamples;

  /** Minimum posterior probability of a rule */
  double _acc_limit;

  /** Minimum value for convergence that triggers termination of execution */
  double _conv_limit;

  /** Percentage of rules that are killed at the end of a iteration */
  double _mortality;

  /** */
  double _significance; 
  double _crossover_rate;
  double _mutation_rate;
  double _gapsize;

  //
  // Internal variables
  //

  /** Rule-set that stores the best rules found so far */
  GarpRuleSet * _fittest;

  /** Rule-set that stores the next generation of rules (offspring) */
  GarpRuleSet * _offspring;

  /** Custom sampler to wrap basic sampler */
  GarpCustomSampler * _custom_sampler;

  double _convergence;
  int _improvements;

  int _curr_heur_count[5];
  int _prev_heur_count[5];

  int _gen;
};


#endif

