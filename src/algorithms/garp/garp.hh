/**
 * Declaration of Genetic Algorithm for Rule-Set Production (GARP)
 * 
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

#include <openmodeller/om.hh>
#include <openmodeller/Sample.hh>

// required include because of enum PerfIndex and class GarpRule
#include "rules_base.hh" 
#include "bioclim_histogram.hh"
#include "regression.hh"


class GarpRuleSet;

class Random;

/****************************************************************/
/************************* GARP Algorithm ***********************/

/**
  * Implementation of GARP: Genetic Algorithm for Rule-set Production
  */
class Garp : public AlgorithmImpl
{
public:
  Garp();
  virtual ~Garp();

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
  int done() const;

  /** Return progress so far */
  float getProgress() const;

  //
  // Methods used to project the model
  //
  
  /** This method is used when projecting the model.  
    * @note This method is inherited from the Algorithm class
    * @return     
    * @param x a pointer to a vector of openModeller Scalar 
    *        type (currently double). The vector should contain values 
    *        looked up on the environmental variable layers into which 
    *        the mode is being projected. 
    */
  Scalar getValue( const Sample& x ) const;
  
  /** Returns a value that represents the convergence of the algorithm
    * expressed as a number between 0 and 1 where 0 represents model
    * completion. 
    * @return 
    * @param val 
    */
  int getConvergence( Scalar * const val ) const;

  int getGeneration() { return _gen; }

  /** Deletes data structures that are not needed once the model
	* is generated. Implemented to save some memory when clients
	* have to store too many Garp objects.
    */
  void deleteTempDataMembers();

protected:
  virtual void _getConfiguration( ConfigurationPtr& ) const;
  virtual void _setConfiguration( const ConstConfigurationPtr & );

private:

  void cacheSamples(const SamplerPtr&, OccurrencesPtr&, int resamples);

/** Fill a rule set with newly generated rules
  * @param ruleset: rule set where rules will be added to
  * @param numRules number of rules to be added to the rule set.
  */
  void colonize(GarpRuleSet * ruleset, int numRules);

/** Evaluate rules from a rule set based on their performance to predict 
  *  points provided by a sampler object.
  * @param ruleset: rule set to be tested.
  */
  void evaluate(GarpRuleSet * ruleset);

/** Keep fittest individuals from source rule set storing them in target 
  *   rule set in descending order of fitness.     
  * @param source Pointer to the source rule set where rules 
  *        will be selected from (usually the previous generated population)
  * @param target Pointer to the target rule set where 
  *        fittest rules will be transferred to (usually the fittest rule set)
  * @param perfIndex Index of value on performance array that
  *        will be used to select individuals.
  * @note: This method produces a side effect that is to update the
  *        statistics about heuristic operators performance (that will be
  *        used to adjust probabilities of application of those operators on
  *        future generations) and overall rule performance that will be used
  *        to compute the changes to the convergence value.
  */
  void keepFittest(GarpRuleSet * source, GarpRuleSet * target, PerfIndex perfIndex);


/** Select fittest individuals and store them into target ruleset
  * @param source Pointer to the rule set containing parents
  *        (individuals that will provide genes to create new individuals).
  * @param target Pointer to the target rule set where 
  *        offspring will be transferred to.
  * @param gapsize Proportion of contribution from the objBest archive set to the new population.
  */
  void select(GarpRuleSet * source, GarpRuleSet * target, double gapsize);


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

  /** BioclimHistogram to assist the creation of bioclim based rules */
  BioclimHistogram _bioclimHistogram;

  Regression _regression;

  OccurrencesPtr _cachedOccs;

  double _convergence;
  int _improvements;

  int _curr_heur_count[5];
  int _prev_heur_count[5];

  int _gen;

  mutable float _maxProgress;
};


#endif

