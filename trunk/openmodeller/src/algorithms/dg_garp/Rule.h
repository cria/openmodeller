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

// Rule.h: interface for the Rule class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __RULE_H_
#define __RULE_H_

#include "EnvCell.h"
#include "Utilities.h"

class EnvCellSet;
class EnvLayerSet;
class RuleSet;

// ====================================================================

/** 
 Abstract class for all rule types supported by Garp
*/
class Rule  
{
	// friend classes
	friend class RuleSet;
	friend class GarpAlgorithm;
	friend class CJobResultValidator;

protected:
	/// BYTE vector containing the genes (representation of the variables in a Genetic Algorithm
	BYTE * Gene;
	/// Number of genes stored by the rule
	int intGenes;

	/// Vector for storing the performance values for the rule
	double dblPerformance[10];
	bool blnNeedsEvaluation;
	int intGens;
	int intTrials;
	int intScreener;
	int intScreen;
	int intLength;
	int intNumber;
	int intConclusion;
	char chrOrigin;
	char chrPad;
	int lId;
	int iOrigGen;

	double _pXYs;
	int _no;
	double _dA;
	double _dSig;

	// global properties coming from RuleSet
	bool * bGeneIsActive;
	int * iGeneIndex;
	int iActiveGenes;

public:
	/// Default constructor
	Rule();
	/// Default destructor
	virtual ~Rule();
	virtual Rule * objFactory() = 0;
	virtual Rule * clone();
	virtual void copy(Rule * fromRule);

	/// Restore Model
	void RestoreRule( double *perf, unsigned char *genes, int arry_len, int *gene_index );

	virtual void log();
	virtual char type() const				{ return 'v'; }

	virtual char * toString();
	virtual void fromString(char * strRule) = 0;
	virtual char * toXML();

	virtual void initialize(EnvCellSet * objEnvCellSet, const RuleSet * objRuleSet, 
				bool * geneIsActivePtr, int * geneIndexPtr, int iActGenes) = 0;
	virtual bool applyToCell(EnvCell * cell) = 0;

	virtual double getCertainty(EnvCell * cell);
	virtual double getError(BYTE pred, EnvCell * cell);
	virtual double getStrength(EnvCell * cell) = 0;

	virtual bool similar(Rule * objOtherRule);

	virtual void mutate(int intTemperature);

	double testWithData(EnvCellSet * objTrainSet);

	bool needsEvaluation()
	{ 
		return blnNeedsEvaluation; 
	}

};

// ====================================================================
class GarpRule : public Rule
{
public:
	GarpRule();
	virtual ~GarpRule();

	virtual char type() const				{ return 'v'; }
};

// ====================================================================
class RangeRule : public GarpRule
{
public:
	RangeRule();
	virtual ~RangeRule();
	virtual Rule * objFactory() { return new RangeRule; }

	virtual char type() const				{ return 'd'; }

	virtual void fromString(char * strRule);
	virtual void initialize(EnvCellSet * objEnvCellSet, const RuleSet * objRuleSet, 
		bool * geneIsActivePtr, int * geneIndexPtr, int iActGenes);
	virtual bool applyToCell(EnvCell * cell);

	virtual double getStrength(EnvCell * cell);

	void bioclimRange(EnvCellSet * objEnvCellSet, BYTE pred, double level, int var);
};

// ====================================================================
class NegatedRangeRule : public RangeRule
{
public:
	NegatedRangeRule();
	virtual ~NegatedRangeRule();
	virtual Rule * objFactory() { return new NegatedRangeRule; }

	virtual char type() const				{ return '!'; }

	virtual void fromString(char * strRule);
	virtual bool applyToCell(EnvCell * cell);

	virtual double getStrength(EnvCell * cell);
};

// ====================================================================
class AtomicRule : public GarpRule
{
public:
	AtomicRule();
	virtual ~AtomicRule();
	virtual Rule * objFactory() { return new AtomicRule; }

	virtual char type() const				{ return 'a'; }

	virtual void fromString(char * strRule);
	virtual void initialize(EnvCellSet * objEnvCellSet, const RuleSet * objRuleSet, 
		bool * geneIsActivePtr, int * geneIndexPtr, int iActGenes);
	virtual bool applyToCell(EnvCell * cell);

	virtual double getStrength(EnvCell * cell);
};

// ====================================================================
class LogitRule : public GarpRule
{
public:
	LogitRule();
	virtual ~LogitRule();
	virtual Rule * objFactory() { return new LogitRule; }

	virtual char type() const				{ return 'r'; }

	virtual void fromString(char * strRule);
	virtual void initialize(EnvCellSet * objEnvCellSet, const RuleSet * objRuleSet, 
		bool * geneIsActivePtr, int * geneIndexPtr, int iActGenes);
	virtual bool applyToCell(EnvCell * cell);

	virtual bool similar(Rule * objOtherRule);

	virtual double getStrength(EnvCell * cell);

	virtual void mutate(int intTemperature);

	int regression(EnvCellSet * objEnvCellSet, int dep, 
		double& constant, double& coef1, double& coef2);
};

// ====================================================================

#endif 
