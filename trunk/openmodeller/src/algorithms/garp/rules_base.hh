/**
 * Declaration of GarpRule classe used in GARP
 * 
 * @file   rules_base.hh
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


#ifndef _RULES_BASE_HH_
#define _RULES_BASE_HH_

#include <om.hh>

class GarpCustomSampler;

enum PerfIndex
{
  /** Utility: main performance value. Default is significance. */
  PerfUtil = 0,

  /** pXs/n. This is the proportion of data points the rule applies to */
  PerfPrStr = 1,

  /** Prior probability = pYs/n This is the proportion of the predicted class in the training set.*/
  PerfPrProb = 2,

  /** Prior distance = pYcs/n */
  PerfPrDist = 3,

  /** Posterior strength = pXSs/no */
  PerfPostStr = 4,

  /** Posterior probability = pXYs/no */
  PerfPostProb = 5,

  /** Posterior distance = pYcXs/no */
  PerfPostDist = 6,

  /** Coverage = no/n*/
  PerfCov = 7,

  /** Significance = (pXYs-no*pYs/n)/sqrt(no*pYs*(1-pYs/n)/n) */
  PerfSig = 8,

  /** Error = Significance*sqrt( (pXYs/no)*(1-pXYs/no) )/no */
  PerfErr = 9

  /** Where: 
      pXs - sum of the applicability of the rule, or rule strength. 
            If a data point satisfies a precondition of a rule the 
            strength is one, if not the strength is zero. 
      pYs - sum of the certainty of the rule. If the predicted 
            variable of the data satisfies the conclusion of a rule, 
            (i.e. the rule predicts correctly) the certainty is one, 
            else zero. 
      no -  number of data points selected by the rule 
      pXSs - sum of strength after application (equals no) 
      pXYs - sum of certainty after application of rule
  */
};


enum RuleOrigin
{
  /** */
  OriginColonization = 0,
  OriginMutation = 1,
  OriginJoin = 2,
  OriginCrossover = 3
};


enum RuleType
{
  /** */
  VirtualRuleType = -1,
  RangeRuleType = 0,
  NegatedRuleType = 1,
  LogitRuleType = 2,
  AtomicRuleType = 3
};

bool equalEps(double v1, double v2);
bool between(double value, double min, double max);
int membership(double value1, double value2, double value);

/****************************************************************/
/****************** GarpRule class hierarchy ********************/

class GarpRule  
{
public:
  /// Default constructor
  GarpRule();

  /// Constructor with setters 
  GarpRule(Scalar prediction, int numGenes, 
	   Scalar * genes, double * performances);
  
  /// Default destructor
  virtual ~GarpRule();


/** Returns a copy of the rule. Caller is responsible for deallocating
  * memory for this object when it is done using it.
  */
  virtual GarpRule * clone();

  virtual GarpRule * objFactory() = 0;

  virtual int copy(GarpRule * fromRule);
  
  virtual void initialize(GarpCustomSampler * sampler);

  int numGenes() { return _numGenes; }

  virtual char type() const				{ return 'v'; }
  bool needsEvaluation() { return _needsEvaluation; } 
  void forceEvaluation() { _needsEvaluation = true; } 
  void evaluated()       { _needsEvaluation = false; } 

  Scalar getPrediction()       { return _prediction; }
  Scalar * getGenes()          { return _genes; };
  double * getPerformanceArray() { return _performance; }
  double getPerformance(PerfIndex perfIndex);

  virtual int getStrength(Scalar * values) = 0;
  virtual int getCertainty(Scalar pred);
  virtual double getError(Scalar predefinedValue, Scalar prediction);
  
  virtual bool similar(GarpRule * compareToRule);
  virtual void mutate(double temperature);
  virtual void crossover(GarpRule * rule, int xpt1, int xpt2);

  void adjustRange(Scalar * v1, Scalar * v2);
  virtual bool applies(Scalar * values) = 0;
  double evaluate(GarpCustomSampler * sampler);
  
  virtual void log();

protected:
  /// BYTE vector containing the genes (representation of the variables in a Genetic Algorithm
  Scalar * _genes;
  Scalar _prediction;
  
  /// Number of genes stored by the rule
  int _numGenes;
  
  /// Vector for storing the performance values for the rule
  double _performance[10];
  bool _needsEvaluation;
  char _origin;
};

// ====================================================================

#endif 
