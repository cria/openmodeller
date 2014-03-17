/* **************************************
 *  GARP Modeling Package
 *
 * **************************************
 *
 * Copyright (c), The Center for Research, University of Kansas, 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 * Copyright (C), David R.B. Stockwell of Symbiotik Pty. Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the license that is distributed with the software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * license.txt file included with this software for more details.
 */

// GarpAlgorithm.h : Declaration of the Algorithm class

#ifndef __GARPALGORITHM_H_
#define __GARPALGORITHM_H_

#include "RuleSet.h"
#include "Utilities.h"

#include <openmodeller/om.hh>

class EnvCellSet;

// ========================================================================
// GarpAlgorithm
// ========================================================================
class GarpAlgorithm : public AlgorithmImpl
{
private:
	// current ruleset
	RuleSet objNew;

	// best model so far
	RuleSet objBest;

	// data points used for training
	EnvCellSet * objTrainSet;


public:
	// OM Algorithm virtual methods
	int initialize();
	int iterate();
	int done() const;
	float getProgress() const;
	Scalar getValue( const Sample& x ) const;
	int getConvergence( Scalar * const val ) const;
	int getGeneration() { return Gen; }


	GarpAlgorithm();
	virtual ~GarpAlgorithm();

	RuleSet * getBestSet() { return &objBest; }
	void getInitialModel(int intSize, EnvCellSet * objTrainSet);
	void updateRuleUsage();

	int ruleSetSize()
	{ return objBest.size(); }

	char * getParameter2(char * sParamName);
	void setParameter(char * sParamName, char * sParamValue);

	char * getSelectedLayersAsString();
	void setSelectedLayers(char * strParamValue);

 protected:
	void _getConfiguration( ConfigurationPtr& ) const;
	void _setConfiguration( const ConstConfigurationPtr& );

private:
	// private methods
	void initializeProperties();

	// Garp algorithm
	void DisplayStatus();
	void colonize(RuleSet * objRules, EnvCellSet * objTrainSet, int intNewRules);
	void evaluate(RuleSet * objRules, EnvCellSet * objTrainSet);
	int saveRule(int iIndex);
	void measure();
	double converge();
	void select();

	void mutate();
	void crossover();
	void join();

	void updateHeuOpPerformance(char chrType);

	void generate(EnvCellSet * objTestDataset);

	// deprecated methods
	void saveBestRules(RuleSet * toRuleSet, RuleSet * fromRuleSet);
	void concatenateRuleSets(RuleSet * toRuleSet, RuleSet * fromRuleSet);


private:

	long lVersion;

	// flags
	int Sigflag;
	int Postflag;
	int Compflag;
	int	Adjustflag;

	int BioclimOnlyFlag;
	int LogitOnlyFlag;  
	int RangeRuleFlag;  
	int NegatedRuleFlag;
	int AtomicRuleFlag; 
	int LogitRuleFlag;  

	bool bGeneIsActive[MAX_ENV_LAYERS];
	int iGeneIndex[MAX_ENV_LAYERS];
	int iActiveGenes;

	// timming
	int iCPUTime;         // accumulated CPU time used for processing the model (in miliseconds) 

	// algorithm variables
	int Resamples;

	double Accuracylimit; // The minimium post prob of a rule
	double MinUsage;
	double Mortality;
	
	int Totalgens;		// generations per experiment
	int Totaltrials;			// trials per experiment               
	int Popsize;				// population size
	double C_rate;       // crossover rate                      
	double M_rate;       // mutation rate                       
	double J_rate;       // join rate
	double I_rate;
	double Gapsize;		// fraction of pop generated from archive
	int Trials;          // trial counter
	int Maxspin;	        // max gens without evals
	double Resampling_f;
	double Significance;   // the minimum level for inc in best
	double Conv_limit;    // the fractional addition of rules
	double Cutval;


	// data collection and loop control variables
	double  Ave_current_perf;/* ave perf in current generation       */
	double  Best;            /* best performance seen so far         */
	double  Best_current_perf;/* best perf in current generation     */
	int    Best_guy;        /* index of best_current_perf           */
	int    Conv;            /* number of partially coverged genes   */
	mutable bool   Doneflag;        /* set when termination conditions hold */
	int    Experiment;      /* experiment counter                   */
	int    Gen;             /* generation counter                   */
	int    Lost;            /* number of totally coverged positions */
	int    Spin;            /* number of gens since eval occurred   */
	double  Worst;           /* worst performance seen so far        */
	double  Worst_current_perf;/* worst perf in current generation   */
	int     Heuristic[2][5];        /* successes of heuristic operators */
	double  Convergence;        /* the stability of rule set */
	int     Improvements;   /* the number of times the best set has been 
								improved by addition or alteration */
	int Resample;

	mutable float _maxProgress;

};

// ========================================================================

#endif //__GarpAlgorithm_H_
