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

// RuleSet.h : Declaration of the CRuleSet

#ifndef __RULESET_H_
#define __RULESET_H_

#include "Utilities.h"
#include "Rule.h"

#include <om.hh>

class EnvCell;
class EnvCellSet;
class Rule;

// ========================================================================
// RuleSet
// ========================================================================
class RuleSet
{
	// friend classes
	friend class GarpAlgorithm;

private:
	Rule * objRules[MAX_RULES];
	int intRules;

	int _dim;

	// temp data structures to hold verify results
	int iConfMatrix[2][2];
	int iTotalPoints;

	// counter for the number of cells
	int iTotalArea;			// total cells not masked
	int iPresenceArea;      // total cells predicted present
	int iAbsenceArea;       // total cells predicted absent
	int iNonPredictedArea;  // total cells non-predicted

	void resetConfMatrix(EnvCellSet * objTestDataset);
	void addConfMatrix(int iPredictedValue, int iActualValue);

public:
	RuleSet();
	virtual ~RuleSet();

	void log();

	void setDimension(int dim) { _dim = dim; }

	void loadText(char * strFilename);
	void saveText(char * strFilename);

	char * toXML(char * id);

	// verify related methods
	void verify(EnvCellSet * objTestDataset, double dAccLimit);
	int getConfMatrix(int iPredictedValue, int iActualValue);
	int getTotalPoints();
	int getPredictedPoints();
	int getUnpredictedPoints();
	double getAccuracy();
	double getOverallAccuracy();

	void setActiveGenes(bool * bGeneIsActivePtr, int * iGeneIndexPtr, int iActiveGenesAux);

	//void predict(EnvLayer * objPredictionLayer, EnvLayerSet * objPredictionArea, double Accuracylimit);
	//void predictRuleCoverage(EnvLayer * objPredictionLayer, EnvLayerSet * objPredictionArea, double Accuracylimit);
	int applyRulesToCell(EnvCell * cell, double Accuracylimit) const;

	//bool projectRuleSet(EnvLayerSet * oToLayerSet, bool bConservative, RuleSet * oToRuleSet);

	void updateRuleUsage();

	int size();
	void add(Rule * objRule);
	void set(int index, Rule * objRule);
	Rule * get(int index);

	Scalar getValue(const Sample& sample) const;

	void clear();
	void sort(int intPerfIndex);
	void trim(int intMaxRules);
	void discardRules(int iPerfIndex, double dValue);

	void setEvaluation(bool value);
	void setPad(char pad);
	int countPad(char pad);

	double getOveralPerformance(int iPerfIndex, int iFirstRulesToBeIncluded);

	void gatherRuleSetStats(int gen);
	
};

// ========================================================================

#endif //__RULESET_H_
