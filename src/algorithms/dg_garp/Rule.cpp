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

// Rule.cpp: implementation of the Rule class.
//
//////////////////////////////////////////////////////////////////////

#include "Rule.h"
#include "EnvCellSet.h"
#include "Utilities.h"

#include <openmodeller/om.hh>

#include <math.h> 

// ==========================================================================
//  Rule implelentation
// ==========================================================================
Rule::Rule() 
{
	Gene = NULL;
	intGenes = 0;
	
	for (int i = 0; i < 10; i++)
		dblPerformance[i] = 0.0;

	bGeneIsActive = NULL;
	iGeneIndex = NULL;
	iActiveGenes = 0;

	blnNeedsEvaluation = true;
	intGens = 0;
	intTrials = 0;
	intScreener = 0;
	intScreen = 0;
	intLength = 0;
	intNumber = 0;
	intConclusion = 0;
	chrOrigin = (char) 0;
	chrPad = (char) 0;
	
	lId = 0;
	iOrigGen = 0;

	_pXYs = 0.0;
	_no = 0;
	_dA = 0.0;
	_dSig = 0.0;
}

// ==========================================================================
Rule::~Rule() 
{
	if (Gene)
		delete[] Gene;
}

// ==========================================================================
Rule * Rule::clone()
{
	int i;

	Rule * newRule = objFactory();

	for (i = 0; i < 10; i++) 
		newRule->dblPerformance[i] = dblPerformance[i];
	
	newRule->intGenes = intGenes;
	newRule->intGens = intGens;
	newRule->blnNeedsEvaluation = blnNeedsEvaluation;
	newRule->intTrials = intTrials;
	newRule->intScreener = intScreener;
	newRule->intScreen = intScreen;
	newRule->intLength = intLength;
	newRule->chrOrigin = chrOrigin;
	newRule->chrPad = chrPad;

	if (newRule->Gene) delete[] newRule->Gene;
	newRule->Gene = new BYTE[newRule->intLength];

	for (i = 0;i < intLength; i++)
	  { newRule->Gene[i] = Gene[i]; }

	// parameters related to the algorithm
	newRule->bGeneIsActive = bGeneIsActive;
	newRule->iGeneIndex = iGeneIndex;
	newRule->iActiveGenes = iActiveGenes;

	newRule->lId = lId;
	newRule->iOrigGen = iOrigGen;

	newRule->_pXYs = _pXYs;
	newRule->_no = _no;
	newRule->_dA = _dA;
	newRule->_dSig = _dSig;

	return newRule;
}

// ==========================================================================
void Rule::copy(Rule * fromRule)
{
	int i;

	if (type() != fromRule->type())
		throw GarpException(1, "Cannot copy rules with different types");

	for (i = 0; i < 10; i++) 
		dblPerformance[i] = fromRule->dblPerformance[i];
	
	intGenes			= fromRule->intGenes;
	intGens				= fromRule->intGens;
	blnNeedsEvaluation	= fromRule->blnNeedsEvaluation;
	intTrials			= fromRule->intTrials;
	intScreener			= fromRule->intScreener;
	intScreen			= fromRule->intScreen;
	intLength			= fromRule->intLength;
	chrOrigin			= fromRule->chrOrigin;
	chrPad				= fromRule->chrPad;

	if (Gene) delete[] Gene;
	Gene = new BYTE[intLength];

	for (i = 0; i < intLength; i++)
	  { Gene[i] = fromRule->Gene[i]; }

	// parameters related to the algorithm
	bGeneIsActive = fromRule->bGeneIsActive;
	iGeneIndex    = fromRule->iGeneIndex;
	iActiveGenes  = fromRule->iActiveGenes;

	lId = fromRule->lId;
	iOrigGen = fromRule->iOrigGen;

	_pXYs = fromRule->_pXYs;
	_no = fromRule->_no;
	_dA = fromRule->_dA;
	_dSig = fromRule->_dSig;
}
// ==========================================================================
void
Rule::RestoreRule( double *perf, unsigned char *genes, int arry_len, int *gene_index )
{
  for( int i=0; i<10; i++ ) {

    dblPerformance[i] = perf[i];
  }

  iGeneIndex = gene_index;

  if (Gene)
    delete [] Gene;

  Gene = new BYTE[arry_len];

  intLength = arry_len;
  iActiveGenes = arry_len/2;
  for( int i=0; i<intLength; i++ ) {
    Gene[i] = (BYTE) genes[i];
  }


}

// ==========================================================================
void Rule::initialize(EnvCellSet * objEnvCellSet, const RuleSet * objRuleSet, 
					  bool * bGeneIsActivePtr, int * iGeneIndexPtr, int iActGenes)
{
	int i, p;

	// parameters related to the algorithm
	bGeneIsActive = bGeneIsActivePtr;
	iGeneIndex = iGeneIndexPtr;
	iActiveGenes = iActGenes;

	if (!iGeneIndex)
		i = 0;

	intTrials = 0;
	blnNeedsEvaluation = true;
	intGens = 0;
	chrOrigin = type();
	intScreen = 1;
	chrPad = ' ';

	// number of genes is equal to the number of values in a cell
	// dont take the mask into account
	intGenes = objEnvCellSet->get(0)->size() - 1;
	intLength = intGenes * 2;

	//printf("intGenes=%3d intLength=%3d ActGenes=%3d\n", intGenes, intLength, iActGenes);

	if (Gene) delete[] Gene;
	Gene = new BYTE[intLength];

	for (i = 1; i < intGenes; i++)
	{
		Gene[i * 2]     = 0;
		Gene[i * 2 + 1] = 255;
	}

	p = GarpUtil::randint(0, objEnvCellSet->count() - 1);
	Gene[0] = objEnvCellSet->get(p)->values[0];
    Gene[1] = 0;

	intConclusion = Gene[0];
}

// ==========================================================================
char * Rule::toString()
{
	int i;
	char typ = (char) 0;
	char * strText = new char[1024];
	char strAux[65];

	// rule type
	typ = this->type();
	strcpy(strText, "");
	sprintf(strText, "%c ", typ);

	// genes
	for (i = 0; i < intLength; i++)
	{
		sprintf(strAux, " %3d ", Gene[i]);
		strcat(strText, strAux);
	}

	// performance
	for (i = 0; i < 10; i++)
	{
		sprintf(strAux, " %-5.3f ", dblPerformance[i]);
		strcat(strText, strAux); 
	}

	// other values

	// check string sizes
	if (strlen(strText) > 1024)
		throw GarpException(82, "String size exceeded in Rule::toString()");

	// return result
	return strText;
}

// ==========================================================================
char * Rule::toXML()
{
	/* XML Sample

		<Rule Type="r">
			<Genes> 1 0 64 128 12 15 0 255 </Genes>
			<Performance> 0.0000 1.2500 ... </Performance>
		</Rule>
	*/

	int i;
	char strAux[64];
	char * strXML;

	strXML = new char[1024];

	// rule type
	sprintf(strXML, "<Rule Type=\"%c\" Id=\"%d\" OrigGen=\"%d\">\n", this->type(), lId, iOrigGen);

	// genes
	strcat(strXML, "  <Genes>");
	for (i = 0; i < intLength; i++)
	{
		sprintf(strAux, " %3d", Gene[i]);
		strcat(strXML, strAux); 
	}
	strcat(strXML, "</Genes>\n");

	// performance
	strcat(strXML, "  <Performance>");
	for (i = 0; i < 10; i++)
	{
		sprintf(strAux, " %-5.3f", dblPerformance[i]);
		strcat(strXML, strAux); 
	}

	strcat(strXML, "</Performance>");

	strcat(strXML, "</Rule>");

	// other values

	// check string sizes
	if (strlen(strXML) > 1024)
		throw GarpException(82, "String size exceeded in Rule::toXML()");

	// return result
	return strXML;
}

// ==========================================================================
double Rule::getCertainty(EnvCell * cell)
{ return (Gene[0] == cell->values[0]); }

// ==========================================================================
double Rule::getError(BYTE pred, EnvCell * cell)
{ return (double) abs( pred - cell->values[0] ); }
 
// ==========================================================================
void Rule::mutate(int intTemperature)
{
	BYTE aux1, aux2, aux_g1, aux_g2;
	int rnd1, rnd2;
	int j, k;

    j = GarpUtil::randint(1, iActiveGenes);
	k = 2 * iGeneIndex[j];

	rnd1 = GarpUtil::randint(-intTemperature, intTemperature);
	rnd2 = GarpUtil::randint(-intTemperature, intTemperature);

	if ((k + 1 > intLength) || (k < 2))
		throw GarpException(100, "Array read out of bounds (Rule::mutate)");

	aux1 = Gene[k];
	aux2 = Gene[k + 1];
	
	Gene[k]     = (BYTE) (aux1 - rnd1);
	Gene[k + 1] = (BYTE) (aux2 + rnd2);

	aux_g1 = MIN(Gene[k], Gene[k + 1]);
	aux_g2 = MAX(Gene[k], Gene[k + 1]);

	Gene[k]     = aux_g1;
	Gene[k + 1] = aux_g2;

	if (type() == 'a')
		Gene[k + 1] = Gene[k];

	blnNeedsEvaluation = true;
	intGens = 0;
	chrOrigin = 'm';
}

// ==========================================================================
bool Rule::similar(Rule * objOtherRule)
{
	bool found;
	int j, k;
	int ng_2k, ng_2k_1, bg_2k, bg_2k_1;

	if (type() == objOtherRule->type())
	{
		// check rule value (presence/absence)
		if (Gene[0] != objOtherRule->Gene[0]) 
			return 0;

		for (j = 1, found = true; (j < iActiveGenes) && (found); j += 1)
		{
			k = iGeneIndex[j];

			if ((k * 2 + 1 > intLength) || (k * 2 < 2))
				throw GarpException(100, "Array read out of bounds (Rule::similar)");

			ng_2k_1 = Gene[k * 2 + 1];
			ng_2k   = Gene[k * 2];

			bg_2k_1 = objOtherRule->Gene[k * 2 + 1];
			bg_2k   = objOtherRule->Gene[k * 2];

			found = !( ((ng_2k_1 - ng_2k) == 255 && 
						(bg_2k_1 - bg_2k) != 255 ) || 
					   ((ng_2k_1 - ng_2k) != 255 && 
					    (bg_2k_1 - bg_2k) == 255 ) );
		}

		return found;
	}

	return false;
}

// ==========================================================================
double Rule::testWithData(EnvCellSet * objTrainSet)
{
	const int MAX_UTILS = 10;
	double Utility[MAX_UTILS];

	EnvCell * cell;
	int n, i, rnd, no=0;
	double prediction, certainty, strength;
	double pXs=0,pYs=0,pXYs=0,pYcXs=0,pXSs=0,pYcs=0;
	
	for (i = 1; i < MAX_UTILS; i++) 
		Utility[i] = 0.0;

	Utility[0] = 1.0;

	n = objTrainSet->size();
	for(i=0; i < n; i++)
    {	
		// Get an in range data point
		rnd = GarpUtil::randint(0, n - 1);
		cell = objTrainSet->get(rnd);
	
		strength = getStrength(cell);
		certainty = getCertainty(cell);
		prediction = getError(128, cell);

		pXs  += strength;
		pYs  += certainty;
		pYcs += prediction;

		if (strength > 0)
		{
			pXSs  += strength;
			pYcXs += getError((BYTE)prediction, cell);
			pXYs  += (MIN(certainty, strength)) / strength;
			no++;
		}
    }

	// Priors
	Utility[1] = pXs/n;		// proportion 
	Utility[2] = pYs/n;		// Prior probability
	Utility[3] = pYcs/n;
	
	// Posteriors
	if (no > 0) 
		Utility[4] = pXSs/no;
	else 
		Utility[4] = 0;
	
	if (no > 0)					// Posterior probability
		Utility[5] = pXYs/no; 
	else 
		Utility[5] = 0;
	
	if (no>0) 
		Utility[6] = (pYcXs/no);
	else 
		Utility[6] = 0;
	
	Utility[7] = ((double)no)/n;

	// Crisp Significance
	if (no >= MIN_SIG_NO && Utility[2] > 0 && Utility[2] < 1.0)	
		Utility[8]= (pXYs-Utility[2] * no) / sqrt(no * Utility[2] * (1 - Utility[2]));
	else Utility[8] = 0;

	Utility[9] = 0.0;

	//flags!!! not implemented yet
	//if (Postflag)   
		Utility[0] *= Utility[5];

	//if (Compflag) 
	//	Utility[0] *= Utility[7];
  
	//if (Sigflag)	
		Utility[0] *= Utility[8];
  
		//printf("%c] u0=%+7.3f u1=%+7.3f u2=%+7.3f u8=%+7.3f pXYs=%+7.3f no=%4d n=%4d\n",
		//type(), Utility[0], Utility[1], Utility[2], Utility[8], pXYs, no, n);

	//if (Ecoflag)
	//	Utility[0] *= ecoSpace();
  
	//if (Lengthflag) 
	//	Utility[0] /= length();
	//*/
  
	// Record performance in rule

	for (i = 0; i < 10; i++) 
		dblPerformance[i] = Utility[i];
	
	_pXYs = pXYs;
	_no = no;
	_dA = pYs / n;
	_dSig = Utility[8];

	return (Utility[0]);
}

// ==========================================================================
//  GarpRule implelentation
// ==========================================================================
GarpRule::GarpRule() { }

// ==========================================================================
GarpRule::~GarpRule() { }

// ==========================================================================
//  RangeRule implelentation
// ==========================================================================
RangeRule::RangeRule() { }

// ==========================================================================
RangeRule::~RangeRule() { }

// ==========================================================================
void RangeRule::fromString(char * strRule)
{
}

// ==========================================================================
void RangeRule::initialize(EnvCellSet * objEnvCellSet, const RuleSet * objRuleSet, 
						   bool * geneIsActivePtr, int * geneIndexPtr, int iActGenes)
{
	int i, j, k;

	// call parent initialize
	GarpRule::initialize(objEnvCellSet, objRuleSet, geneIsActivePtr, geneIndexPtr, iActGenes);

	// loop iterates through variables
	for(k = 1; k < iActiveGenes; k++)
	{
		i = GarpUtil::randint(1, iActiveGenes);
	
		j = iGeneIndex[i];
		
		if ((j * 2 + 1 > intLength) || (j * 2 < 2))
			throw GarpException(100, "Array out of bounds (RangeRule::initialize)");

		bioclimRange(objEnvCellSet, Gene[0], GarpUtil::random() * 0.1, j);
	}
}

// ==========================================================================
void RangeRule::bioclimRange(EnvCellSet * objEnvCellSet, BYTE pred, double level, int var)
{
	BioclimHistogram * histogram;
	int sum, n;
  int UL = 0;
  int LL = 0;
    
	histogram = objEnvCellSet->getBioclimHistogram();

	sum = 0;

    for (n = 0; n < 256; n++)
    {
		sum += histogram->matrix[pred][var][n];
		if (sum>(level * histogram->matrix[pred][0][pred])) 
		{
			// printf("%d %d %f",var,sum,level);
			LL = n;
			break;
		}
	}

    sum = 0;    
    
	for (n = 255; n >= 0; n--)
    {
		sum += histogram->matrix[pred][var][n];
		if (sum > (level * histogram->matrix[pred][0][pred])) 
		{
			// printf("%d %d %f",var,sum,level);
			UL = n;
			break;
		}	
	}

    Gene[var * 2 + 1] = (BYTE) UL;
    Gene[var * 2]     = (BYTE) LL;
}

// ==========================================================================
bool RangeRule::applyToCell(EnvCell * cell)
{
	// visit each of the genes
	for (int i = 1; i < iActiveGenes; i++)
	{
		if (!((Gene[iGeneIndex[i] * 2] == 0) && (Gene[iGeneIndex[i] * 2 + 1] == 255)))
			if (GarpUtil::notBetween(cell->values[iGeneIndex[i]], Gene[iGeneIndex[i] * 2], Gene[iGeneIndex[i] * 2 + 1]))
				return false;
	}

	return true;
}

// ==========================================================================
double RangeRule::getStrength(EnvCell * cell)
{
	int a, b, c;
	int i, k;	

	//printf("GetStrength(%2d)\n", Gene[0]);
	for (k = 1; k < iActiveGenes; k++)
    {
		// get the index of the kth active gene
		i = iGeneIndex[k];

		if ((i * 2 + 1 > intLength) || (i * 2 < 2))
			throw GarpException(100, "Array out of bounds (RangeRule::getStrength)");

		a = Gene[i * 2];
		b = Gene[i * 2 + 1];
		c = cell->values[i];

		if (!GarpUtil::membership(a, b, c)) 
		  { 
		    //printf("Strength = 0\n"); 
		    return 0; 
		  }
    } 

	//printf("Strength = 1\n");
  return 1.0;
}

// ==========================================================================
//  NegatedRangeRule implelentation
// ==========================================================================
NegatedRangeRule::NegatedRangeRule() {} 
// ==========================================================================
NegatedRangeRule::~NegatedRangeRule() {} 
// ==========================================================================
void NegatedRangeRule::fromString(char * strRule)
{
}

// ==========================================================================
bool NegatedRangeRule::applyToCell(EnvCell * cell)
{
	int i, j;

	// visit each of the genes
	for (i = 1; i < iActiveGenes; i++)
	{
		j = iGeneIndex[i];

		if ((j * 2 + 1 > intLength) || (j * 2 < 2))
			throw GarpException(100, "Array out of bounds (NegatedRangeRule::applyToCell)");

		if (!((Gene[j * 2] == 0) && (Gene[j * 2 + 1] == 255)))
			if (GarpUtil::notBetween(cell->values[j], Gene[j * 2], Gene[j * 2 + 1]))
				return true;
	}

	return false;
}

// ==========================================================================
double NegatedRangeRule::getStrength(EnvCell * cell)
{
	double strength, neg_strength;
	
	strength     = RangeRule::getStrength(cell);
	neg_strength = 1 - strength;
	
	return neg_strength;
}

// ==========================================================================
//  AtomicRule implelentation
// ==========================================================================
AtomicRule::AtomicRule() { }

// ==========================================================================
AtomicRule::~AtomicRule() { }

// ==========================================================================
void AtomicRule::initialize(EnvCellSet * objEnvCellSet, const RuleSet * objRuleSet, 
							bool * geneIsActivePtr, int * geneIndexPtr, int iActGenes)
{
	int i, j, k, p;

	// call inherited initialize
	GarpRule::initialize(objEnvCellSet, objRuleSet, geneIsActivePtr, geneIndexPtr, iActGenes);

	p = GarpUtil::randint(0, objEnvCellSet->size());
	EnvCell * objEnvCell = objEnvCellSet->get(p);

    for (i = 1; i < iActiveGenes; i++)
	{
		j = GarpUtil::randint(1, iActiveGenes);
		k = iGeneIndex[j];

		if ((k * 2 + 1 > intLength) || (k * 2 < 2))
			throw GarpException(100, "Array out of bounds (AtomicRule::initialize)");

		Gene[2 * k] = Gene[2 * k + 1] = objEnvCell->values[k]; 
    }  
}

// ==========================================================================
void AtomicRule::fromString(char * strRule)
{
}

// ==========================================================================
bool AtomicRule::applyToCell(EnvCell * cell)
{
	int i;

	// visit each of the genes
	for (i = 1; i < iActiveGenes; i++)
	{
		if (!((Gene[iGeneIndex[i] * 2] == 0) && (Gene[iGeneIndex[i] * 2 + 1] == 255)))
			if (!(cell->values[iGeneIndex[i]] == Gene[iGeneIndex[i] * 2]))
				return false;
	}

	return true;
}

// ==========================================================================
double AtomicRule::getStrength(EnvCell * cell)
{
	for (int i = 1; i < iActiveGenes; i++)
	{
		if (!GarpUtil::membership(Gene[iGeneIndex[i] * 2], Gene[iGeneIndex[i] * 2 + 1], cell->values[iGeneIndex[i]])) 
			return 0;
	}

	return 1;
}

// ==========================================================================
//  LogitRule implelentation
// ==========================================================================
LogitRule::LogitRule()
{
}

// ==========================================================================
LogitRule::~LogitRule()
{
}

// ==========================================================================
void LogitRule::fromString(char * strRule)
{
}

// ==========================================================================
void LogitRule::initialize(EnvCellSet * objEnvCellSet, const RuleSet * objRuleSet, 
						   bool * geneIsActivePtr, int * geneIndexPtr, int iActGenes)
{
	int i, j, k;
	double constant, coef[2];

	// call inherited initialize
	GarpRule::initialize(objEnvCellSet, objRuleSet, geneIsActivePtr, geneIndexPtr, iActGenes);

	// TO DO: must take into account the variables in use (may use some, or all)
    for (i = 0; i < iActiveGenes; i++)
	{
		k = GarpUtil::randint(1, iActiveGenes);
		j = iGeneIndex[k];

		regression(objEnvCellSet, j, constant, coef[0], coef[1]);

		Gene[1]     = (int) constant;
		Gene[j * 2]   = (int) coef[0]; 
		Gene[j * 2 + 1] = (int) coef[1];
    }  
}

// ==========================================================================
int LogitRule::regression(EnvCellSet * objEnvCellSet, int dep, double& constant, double& coef1, double& coef2)
{
	double a, b, x, y, xi, yi, xiyi, xi2, xb, xx, xxi, xxiyi, xxi2;
	int i, n, pred;
	BYTE * values;

	n = objEnvCellSet->count();
	pred = 0;

	a = b = x = y = xi = yi = xiyi = xi2 = xb = xx = xxi = xxiyi = xxi2 = 0.0;

	for (i = 0; i < n; i++) 
	{
		values = objEnvCellSet->get(i)->values;

		y = (double) values[pred];
		x = (double) values[dep];
		
		xx =  x * x;
		xi += x;
		yi += y;
		xiyi += x * y;
		xi2 += x * x;

		xxi += xx;
		xxiyi += xx * y;
		xxi2 += xx * xx;
	}

	b = (n * xiyi - xi * yi) / (n * xi2 - (xi * xi));
	coef1 = ((b * 255) + 128);

	a = yi / n - b * xi / n;
	constant = a;
	xb = (n * xxiyi - xxi * yi) / (n * xxi2 - (xxi * xxi));
	coef2 = ((xb * 255) + 128);

	//printf("\nb= %f %f %f %f %f %f %d",b,a,xb,coef[0],coef[1],coef[2],n);
	
	return n;
}

// ==========================================================================
bool LogitRule::applyToCell(EnvCell * cell)
{ return (getStrength(cell) == 1.0); }

// ==========================================================================
double LogitRule::getStrength(EnvCell * cell)
{
	BYTE * Data;
	int	i, k;
	double	Sum, prob, r;

	Sum = 0.0;
	prob = 0.0;

	Data = cell->values;

	for (k = 1; k < iActiveGenes; k++)
	{
		i = iGeneIndex[k];

		if ((i * 2 + 1 > intLength) || (i * 2 < 2))
			throw GarpException(100, "Array read out of bounds (LogitRule::getStrength)");

		if (GarpUtil::membership(Gene[i * 2], Gene[i * 2 + 1], 1) != 255) 
		{
			r = (double) (Data[i] / 254.0);

			Sum += ((double) (Gene[i * 2]     - 128)) * r;
			Sum += ((double) (Gene[i * 2 + 1] - 128)) * r * r;
			
			//printf("\ni:%d %d C:%f %d %f", i,Rule->Gene[i*2],v,Data[i],Sum);
		}

		prob = 1.0 / (1.0 + (double) exp(-Sum));

		//printf("\nprob %f",prob);
	}

	if (prob > 0.5)
		return 1.0;
	else
		return 0.0;
}

// ==========================================================================
void LogitRule::mutate(int intTemperature)
{ 
	int j, k;

    k = GarpUtil::randint(1, iActiveGenes);
	j = 2 * iGeneIndex[k];

	if ((j > intLength) || (j < 0))
		throw GarpException(100, "Array read out of bounds (LogitRule::mutate)");

	Gene[j]     = Gene[j]     + GarpUtil::randint(-intTemperature, intTemperature); 
	Gene[j + 1] = Gene[j + 1] + GarpUtil::randint(-intTemperature, intTemperature); 

	blnNeedsEvaluation = true;
	intGens = 0;
	chrOrigin = 'm';
}

// ==========================================================================
bool LogitRule::similar(Rule * objRule)
{
	bool found;
	int k;

	LogitRule * objOtherRule = (LogitRule *) objRule;

	if (type() == objOtherRule->type())
	{
		// check rule value (presence/absence)
		if (Gene[0] != objOtherRule->Gene[0]) 
			return 0;

		for (k = 2, found = true; (k < intLength) && (found); k ++)
		{
			if ((k > intLength) || (k < 2))
				throw GarpException(100, "Array read out of bounds (LogitRule::similar)");

			found = !( ((abs(Gene[k] - 128) < 10) && 
						(abs(objOtherRule->Gene[k] - 128) > 10)) || 
					   ((abs(Gene[k] - 128)>10) && 
					    (abs(objOtherRule->Gene[k] - 128)<10)) );
		}

		return found;
	}

	return false;
}

// ==========================================================================
void Rule::log()
{
  printf("<%c> ", type());
  for (int i = 0; i < intGenes * 2; i += 2)
    {
      //if (fabs(Genes[i] - Gene[i + 1]) >= 2.0)
      //Log::instance()->info( "******** ******** ");
      //else
      printf( "%3d %3d ", Gene[i], Gene[i + 1] );
    }

  printf( "- (%2d) : %f\n", Gene[0], dblPerformance[0]);
}

// ==========================================================================

