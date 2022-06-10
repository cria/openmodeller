
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

// RuleSet.cpp : Implementation of CRuleSet

#ifdef WIN32
// avoid warnings caused by problems in VC headers
#define _SCL_SECURE_NO_DEPRECATE
#endif

#include "Rule.h"
#include "RuleSet.h"
#include "EnvCell.h"
#include "EnvCellSet.h"
#include "Utilities.h"

// ==========================================================================
//  RuleSet implementation
// ==========================================================================
RuleSet::RuleSet() 
{
  // rule set starts empty (without rules)
  intRules = 0;

  // reset rule pointers
  for (int i = 0; i < MAX_RULES; i++)
    objRules[i] = NULL;

  // reset area counters
  iTotalArea = iPresenceArea = iAbsenceArea = iNonPredictedArea = 0;
}

// ==========================================================================
RuleSet::~RuleSet() 
{
  clear();
}

// ==========================================================================
void RuleSet::clear()
{
  for (int i = 0; i < intRules; i++)
    if (objRules[i])
    {
      delete objRules[i];
      objRules[i] = NULL;
    }

  // rule set become empty (without rules)
  intRules = 0;

  // reset area counters
  iTotalArea = iPresenceArea = iAbsenceArea = iNonPredictedArea = 0;
}

// ==========================================================================
void RuleSet::setActiveGenes(bool * bGeneIsActivePtr, int * iGeneIndexPtr, int iActiveGenesAux)
{
  for (int i = 0; i < intRules; i++)
  {
    objRules[i]->bGeneIsActive = bGeneIsActivePtr;
    objRules[i]->iGeneIndex = iGeneIndexPtr;
    objRules[i]->iActiveGenes = iActiveGenesAux;
  }
}

// ==========================================================================
char * RuleSet::toXML(char * id)
{
  /* XML Sample

     <RuleSet Id="Best">
     <Rule>...</Rule>
     .			.
     .			.	
     .			.
     <Rule>...</Rule>
     </RuleSet>
     */

  int i;
  int size;
  char * strXML, *strXMLRule;

  size = (1024 * intRules) + 2048;
  strXML = new char[size];


  // rules
  if (!id)
    strcpy(strXML, "<RuleSet>\n");
  else
    sprintf(strXML, "<RuleSet Id=\"%s\">\n", id);

  for (i = 0; i < intRules; i++)
  {
    strXMLRule = objRules[i]->toXML();
    strcat(strXML, strXMLRule);
    strcat(strXML, "\n");
    delete[] strXMLRule;
  }
  strcat(strXML, "</RuleSet>");

  // check string sizes
  if (strlen(strXML) > size_t(size))
    throw GarpException(82, "String size exceeded in RuleSet::toXML()");

  // return result
  return strXML;
}

// ==========================================================================
void RuleSet::saveText(char * strSaveFilename)
{
  FILE * outf;
  int i;

  if (strcmp(strSaveFilename, "") == 0)
    throw GarpException(3, "Cannot save rule set with an empty filename");

  outf = fopen(strSaveFilename, "w");

  if (outf)
  {
    for (i = 0; i < intRules; i++)
      fprintf(outf, "%s\n", objRules[i]->toString());

    fclose(outf);
  }
  else
  {
    char msg[256];
    sprintf(msg, "Rule set file <%s> could not be opened for writing", strSaveFilename);
    throw GarpException(4, msg);
  }
}

// ==========================================================================
Rule * RuleSet::get(int index)
{ return objRules[index]; }

// ==========================================================================
int RuleSet::size()
{ return intRules; }

// ==========================================================================
void RuleSet::set(int index, Rule * objRule)
{ objRules[index] = objRule; }

// ==========================================================================
void RuleSet::add(Rule * objRule)
{
  if (objRule)
  {
    if (intRules < MAX_RULES - 1)
      objRules[intRules++] = objRule;
    else
      throw GarpException(16, "Cannot add rule. Ruleset is full");
  }
  else
    throw GarpException(17, "Cannot add null rule");
}

// ==========================================================================
void RuleSet::trim(int intMaxRules)
{
  int intInitialRules = intRules;
  for (int i = intMaxRules; i < intInitialRules; i++)
  {
    delete objRules[i];
    objRules[i] = NULL;
    intRules--;
  }
}

// ==========================================================================
//  Discard rules which performance (determined by the iPerfIndex) is less
//   than the specified dValue
// ==========================================================================
void RuleSet::discardRules(int iPerfIndex, double dValue)
{
  int i, j;

  i = 0;
  while (i < intRules)
  {
    if (!GarpUtil::equalEps(objRules[i]->dblPerformance[8], objRules[i]->_dSig, 0.0001))
      throw GarpException(1, "The actual and calculated significance values of this rule do not match.");

    if (objRules[i]->dblPerformance[iPerfIndex] < dValue)
    {
      // this rule has not enough performance
      // delete rule 
      delete objRules[i];

      // and shift rules up
      for (j = i; j < intRules - 1; j++)
        objRules[j] = objRules[j + 1];

      // remove the duplicated reference to the last rule
      objRules[intRules - 1] = NULL;

      // update the number of rules in this set
      // no need to increment <i> because the rules were shifted up one position
      intRules--;
    }
    else
    {
      // this rule passed the test
      // go to the next one
      i++;
    }
  }
}

// ==========================================================================
void RuleSet::setEvaluation(bool value)
{
  for (int i = 0; i < intRules; i++)
    objRules[i]->blnNeedsEvaluation = value;
}

// ==========================================================================
void RuleSet::setPad(char pad)
{
  for (int i = 0; i < intRules; i++)
    objRules[i]->chrPad = pad;
}

// ==========================================================================
int RuleSet::countPad(char pad)
{
  int sum = 0;

  for (int i = 0; i < intRules; i++)
    if (objRules[i]->chrPad == pad)
      sum++;

  return sum;
}

// ==========================================================================
void RuleSet::sort(int intPerfIndex)
{
  // sort the entire ruleset using the provided performance index 
  // implemented using bubble sort (argh!!) to be improved
  Rule * temp;
  int i, j;

  double dSum;

  dSum = 0.0;
  for (i = 0; i < this->intRules; i++)
    dSum += objRules[i]->dblPerformance[9];

  for (i = 0; i < intRules - 1; i++)
    for (j = intRules - 1; j > i; j--)
      if (objRules[j]->dblPerformance[intPerfIndex] > objRules[j - 1]->dblPerformance[intPerfIndex])
      {
        temp = objRules[j];
        objRules[j] = objRules[j - 1];
        objRules[j - 1] = temp;
      }
}

// ==========================================================================
void RuleSet::verify(EnvCellSet * objTestDataset, double dAccLimit)
{
  EnvCell * cell;
  int iRuleIndex, iPredictedValue, iActualValue;
  int i, n;

  double dSum;

  dSum = 0.0;
  for (i = 0; i < this->intRules; i++)
    dSum += objRules[i]->dblPerformance[9];

  // init temp structures
  resetConfMatrix(objTestDataset);

  n = objTestDataset->count();
  for (i = 0; i < n; i++)
  {
    // apply rules to each cell in the dataset
    cell = objTestDataset->get(i);
    iRuleIndex = applyRulesToCell(cell, dAccLimit);

    if (iRuleIndex >= 0)
    {
      // rule applies
      iPredictedValue = objRules[iRuleIndex]->Gene[0];
      iActualValue    = cell->values[0];

      addConfMatrix(iPredictedValue, iActualValue);
    }
  }

  dSum = 0.0;
  for (i = 0; i < this->intRules; i++)
    dSum += objRules[i]->dblPerformance[9];
}

// ==========================================================================
void RuleSet::resetConfMatrix(EnvCellSet * objTestDataset)
{
  // init temp structures
  iConfMatrix[0][0] = 0;
  iConfMatrix[0][1] = 0;
  iConfMatrix[1][0] = 0;
  iConfMatrix[1][1] = 0;

  iTotalPoints = objTestDataset->count();
}
// ==========================================================================
int RuleSet::getConfMatrix(int iPredictedValue, int iActualValue)
{
  if ((iActualValue < 0) || (iActualValue > 1) || (iPredictedValue < 0) || (iPredictedValue > 1))
    throw GarpException(1, "Confusion matrix index out of range");

  return iConfMatrix[iActualValue][iPredictedValue];
}

// ==========================================================================
void RuleSet::addConfMatrix(int iPredictedValue, int iActualValue)
{
  if ((iActualValue < 0) || (iActualValue > 1) || (iPredictedValue < 0) || (iPredictedValue > 1))
    throw GarpException(1, "Confusion matrix index out of range");

  iConfMatrix[iActualValue][iPredictedValue]++;
}

// ==========================================================================
int RuleSet::getTotalPoints()
{ 
  return iTotalPoints; 
}
// ==========================================================================
int RuleSet::getPredictedPoints()
{
  return (iConfMatrix[0][0] + iConfMatrix[0][1] + iConfMatrix[1][0] + iConfMatrix[1][1]);
}

// ==========================================================================
int RuleSet::getUnpredictedPoints()
{
  return getTotalPoints() - getPredictedPoints();
}

// ==========================================================================
double RuleSet::getAccuracy()
{
  return (double) (iConfMatrix[0][0] + iConfMatrix[1][1]) / (double) getPredictedPoints();
}
// ==========================================================================
double RuleSet::getOverallAccuracy()
{
  return (double) (iConfMatrix[0][0] + iConfMatrix[1][1]) / (double) getTotalPoints();
}

// ==========================================================================
void RuleSet::updateRuleUsage()
{
  throw GarpException(131, "Method RuleSet::updateRuleUsage not implemented yet");
}

// ==========================================================================
double RuleSet::getOveralPerformance(int iPerfIndex, int iFirstRulesToBeIncluded)
{
  int i;
  double dResult, dWeight, dWSum;

  dWSum   = 0.0;
  dResult = 0.0;
  dWeight = (double) iFirstRulesToBeIncluded;

  for (i = 0; i < iFirstRulesToBeIncluded; i++)
  {
    if (i < intRules)
    {
      dResult += objRules[i]->dblPerformance[iPerfIndex] * dWeight;
      dWSum += dWeight;
      dWeight--;

      //dResult += objRules[i]->dblPerformance[iPerfIndex];
    }
    else
      break;
  }

  dResult /= dWSum;

  return dResult;
}

// ==========================================================================
Scalar RuleSet::getValue(const Sample& sample) const
{
  // convert values to EnvCell
  BYTE bytes[256];
  EnvCell cell(_dim + 2, bytes);

  // first element of bytes is reserved for presence/absence value
  for (int i = 1; i < _dim; i++)
  {
    // Guard against values outside the normalization range
    // due to reprojection to a non-native range
    Scalar value = sample[i - 1];
    if (value > 253.0) value = 253.0;
    if (value < 1.0)   value = 1.0;

    bytes[i] = (BYTE) value;
  }

  int ruleIndex = applyRulesToCell(&cell, 0.0);
  //printf(" Idx=%+2d Pred=%+4.1f\n", ruleIndex, (ruleIndex >= 0)? (Scalar) (objRules[ruleIndex]->Gene[0]) : -1.0);

  if (ruleIndex >= 0)
    return (Scalar) (objRules[ruleIndex]->Gene[0]);

  return 0.0;
}

// ==========================================================================
void RuleSet::log()
{
  for ( int i = 0; i < intRules; i++ )
  {
    printf( "%2d] ", i );
    objRules[i]->log();
  }
}

/*
// ==========================================================================
void RuleSet::predict(EnvLayer * objPredictionLayer, EnvLayerSet * objPredictionArea, double Accuracylimit)
{
int row, col, pos;
int intRows, intColumns, intSize;
int ruleIndex;
bool status;
BYTE bytPred;
EnvCell cell;
BYTE values[MAX_ENV_LAYERS];

intRows    = objPredictionArea->rows();
intColumns = objPredictionArea->columns();

// number of values in each cell (including species and mask values (+2)
intSize    = objPredictionArea->size() + 2;

// reset position counter
pos = 0;

// set cell parameters
cell.setSize(intSize);
cell.setValues(values);

// reset area counters
iTotalArea = iPresenceArea = iAbsenceArea = iNonPredictedArea = 0;

// new prediction algorithm
// for each cell do
// visit each row
for (row = 0; row < intRows; row++)
{
// visit each column within the current row
for (col = 0; col < intColumns; col++)
{
// get current position
//pos = (row * intColumns) + col;

// get current cell value
status = objPredictionArea->getValue(pos, &cell);

// check if cell is not masked
if (status) 
{
// NOTE: the element Item[0] should never be used in prediction!!!
// we don't need the datapoints to project the model back to the env. layers

// cell is not masked: add one cell to the total area counter
iTotalArea++;

// check which (if any) rule apply to this cell
ruleIndex = applyRulesToCell(&cell, Accuracylimit);

// if there is a rule that applies to this cell, write the rule value to file
if (ruleIndex >= 0)
{
// rule <ruleIndex> applies: write its value to output layer
bytPred = (BYTE) objRules[ruleIndex]->Gene[0];
objPredictionLayer->set(pos, bytPred);

// increment area counter (depending on the prediction)
if (bytPred == PRESENCE)
iPresenceArea++;
else if (bytPred == ABSENCE)
iAbsenceArea++;
}

else
{
// no rule applies: write NODATA
objPredictionLayer->set(pos, (BYTE) MISSING_VALUE);
iNonPredictedArea++;
}

}
else
{
  // if it is masked then write NODATA to cell
  objPredictionLayer->set(pos, (BYTE) MASK_VALUE);
}

// visit next cell
pos++;
}


//GarpProgressEvents::fireProcessEvent((int) (row * 100 / intRows), 0, 0);
}

//GarpProgressEvents::fireProcessEvent(100, 0, 0);
}

// ==========================================================================
void RuleSet::predictRuleCoverage(EnvLayer * objPredictionLayer, EnvLayerSet * objPredictionArea, double Accuracylimit)
{
  int row, col, pos;
  int intRows, intColumns, intSize;
  int ruleIndex;
  bool status;
  EnvCell cell;
  BYTE values[MAX_ENV_LAYERS];

  intRows    = objPredictionArea->rows();
  intColumns = objPredictionArea->columns();

  // number of values in each cell (including species and mask values (+2)
  intSize    = objPredictionArea->size() + 2;

  // reset position counter
  pos = 0;

  // set cell parameters
  cell.setSize(intSize);
  cell.setValues(values);

  // new prediction algorithm
  // for each cell do
  // visit each row
  for (row = 0; row < intRows; row++)
  {
    // visit each column within the current row
    for (col = 0; col < intColumns; col++)
    {
      // get current position
      //pos = (row * intColumns) + col;

      // get current cell value
      status = objPredictionArea->getValue(pos, &cell);

      // check if cell is not masked
      if (status) 
      {
        // NOTE: the element Item[0] should never be used in prediction!!!
        // we don't need the datapoints to project the model back to the env. layers

        // check which (if any) rule apply to this cell
        ruleIndex = applyRulesToCell(&cell, Accuracylimit);

        // if there is a rule that applies to this cell, write the rule value to file
        if (ruleIndex >= 0)
        {
          // rule <ruleIndex> applies: write its value to output layer
          //objPredictionLayer->set(pos, (BYTE) objRules[ruleIndex]->Gene[0]);
          objPredictionLayer->set(pos, (BYTE) ruleIndex + 1);
        }

        else
        {
          // no rule applies: write NODATA
          objPredictionLayer->set(pos, (BYTE) MISSING_VALUE);
        }

      }
      else
      {
        // if it is masked then write NODATA to cell
        objPredictionLayer->set(pos, (BYTE) MASK_VALUE);
      }

      // visit next cell
      pos++;
    }


    //GarpProgressEvents::fireProcessEvent((int) (row * 100 / intRows), 0, 0);
  }

  //GarpProgressEvents::fireProcessEvent(100, 0, 0);
}

*/

// ==========================================================================
int RuleSet::applyRulesToCell(EnvCell * cell, double Accuracylimit) const
{
  int i, maxi, ruleIndex;
  double u, max;
  int b_membership[256]; 

  for (i = 0; i < 256; i++)
    b_membership[i] = 0;

  max = maxi = 0;

  ruleIndex = -1;

  // check every rule
  i = 0;
  while ((i < intRules) && (ruleIndex == -1))
  {
    // get the 5th performance value
    u = (int)(255.0 * objRules[i]->dblPerformance[5]);

    if (objRules[i]->dblPerformance[5] >= Accuracylimit)
    {
      if (objRules[i]->applyToCell(cell))
      {
        ruleIndex = i;

        // update value for 
        if (objRules[i]->intScreen)
        {
          if (b_membership[objRules[i]->Gene[0]] < u) 
            b_membership[objRules[i]->Gene[0]] = (int) u;

          if (max < u) 
          {
            max  = u;
            maxi = i;
          }
        }
      }
    }

    // check next rule
    i++;
  }

  if (max) 
    objRules[maxi]->dblPerformance[9]++;

  return ruleIndex;
}

/*
// ==========================================================================
bool RuleSet::projectRuleSet(EnvLayerSet * oToLayerSet, bool bConservative, RuleSet * oToRuleSet)
{
// check if rule set is not null
if ((intRules <= 0) || (objModelLayerSet->size() == 0) || (oToLayerSet->size() == 0) ||
(objModelLayerSet->size() != oToLayerSet->size()))
return false;

// this code does not work right, I guess. See the other implementation below it
int i, j, iLayers;
int iNatMin[MAX_ENV_LAYERS];
int iNatMax[MAX_ENV_LAYERS];
int iProjMin[MAX_ENV_LAYERS];
int iProjMax[MAX_ENV_LAYERS];
double dFactor[MAX_ENV_LAYERS];

EnvLayer * oNat, *oProj;

// find out how many genes a rule have
iLayers = objModelLayerSet->size();

// now calculate the factor, min and max values	for each gene
// those values will be used to modify the gene values in the
// result dataset
for (i = 0; i < iLayers; i++)
{
// get one layer from native range
// and other from projection area
oNat  = objModelLayerSet->getLayerByIndex(i);
oProj = oToLayerSet->getLayerByIndex(i);

// get projection values for each gene
// min values
if (oNat->dblMinValue > oProj->dblMinValue)
{
iNatMin[i] = MIN_SCALED_VALUE;
iProjMin[i] = oProj->scale(oNat->dblMinValue);
}
else
{
iNatMin[i] = oNat->scale(oProj->dblMinValue);
iProjMin[i] = MIN_SCALED_VALUE;
}

// now max values
if (oNat->dblMaxValue > oProj->dblMaxValue)
{
iNatMax[i] = MAX_SCALED_VALUE;
iProjMax[i] = oProj->scale(oNat->dblMaxValue);
}
else
{
iNatMax[i] = oNat->scale(oProj->dblMaxValue);
iProjMax[i] = MAX_SCALED_VALUE;
}

dFactor[i] = (((double) iProjMax[i] - (double) iProjMin[i]) / 
((double) iNatMax[i]  - (double) iNatMin[i]));
}

// first clear the rule set
oToRuleSet->trim(0);

// copy the rules to the result rule set and modify the rules
for (j = 0; j < intRules; j++)
{
Rule * oAuxRule;

// get a clone of the ith rule in the current 
oAuxRule = objRules[j]->clone();

// modify the gene values
// projecting the values from the Native Dataset to the projection area
// jump first gene (rule value for the species - 
// - presence or absence - cannot project these values)
for (i = 0; i < iLayers; i++)
{
int iOldValue1, iOldValue2;
int iNewValue1, iNewValue2;

iOldValue1 = (int) oAuxRule->Gene[(i + 1) * 2];
iOldValue2 = (int) oAuxRule->Gene[(i + 1) * 2 + 1];

iNewValue1 = (int) ((iOldValue1 - iNatMin[i]) * dFactor[i]) + iProjMin[i];
iNewValue2 = (int) ((iOldValue2 - iNatMin[i]) * dFactor[i]) + iProjMin[i];

// correct the values to avoid values out of range
if (iNewValue1 < 0)
  iNewValue1 = 0;
else if (iNewValue1 > 255)
  iNewValue1 = 255;

if (iNewValue2 < 0)
  iNewValue2 = 0;
else if (iNewValue2 > 255)
  iNewValue2 = 255;

  // decide if it is an conservative or liberal projection
  // In conservative projections, if the value of a gene
  // is in one of the bounds of the native range layer value,
  // this value is mapped to the correspondent value to the
  // projection area. In liberal projections, a value in one
  // of the bounds of the layer values means that the rule 
  // applies for all values above (or below) that gene value

if (iOldValue1 == 0 && !bConservative)
  iNewValue1 = 0;

if (iOldValue2 == 0 && !bConservative)
  iNewValue2 = 0;

  oAuxRule->Gene[(i + 1) * 2]     = (BYTE) iNewValue1;
  oAuxRule->Gene[(i + 1) * 2 + 1] = (BYTE) iNewValue2;
  }

// add rule to the rule set
oToRuleSet->add(oAuxRule);
}	

//

int i, j, iLayers;
int iOldValue1, iOldValue2;
int iNewValue1, iNewValue2;
EnvLayer * oNat, *oProj;
Rule * oAuxRule;

// find out how many genes a rule have
iLayers = objModelLayerSet->size();

// first clear the rule set
oToRuleSet->trim(0);

// copy the rules to the result rule set and modify the rules
for (j = 0; j < intRules; j++)
{
  // get a clone of the ith rule in the current 
  oAuxRule = objRules[j]->clone();

  // logit rules cannot be projected using this approach
  // so, for now, let's just copy them and hope for the best
  // that means, for logit rules, projection will disconsider the difference in the range of values
  if (oAuxRule->type() != 'r')
  {
    // modify the gene values
    // projecting the values from the Native Dataset to the projection area
    // jump first gene (rule value for the species - 
    // - presence or absence - cannot project these values)
    for (i = 0; i < iLayers; i++)
    {
      oNat  = objModelLayerSet->getLayerByIndex(i);
      oProj = oToLayerSet->getLayerByIndex(i);

      // assertion
      //if ((oAuxRule->Gene[(i + 1) * 2]     != objRules[j]->Gene[(i + 1) * 2]) || 
      //	(oAuxRule->Gene[(i + 1) * 2 + 1] != objRules[j]->Gene[(i + 1) * 2 + 1]))
      //	return false;
      //

      iOldValue1 = (int) oAuxRule->Gene[(i + 1) * 2];
      iOldValue2 = (int) oAuxRule->Gene[(i + 1) * 2 + 1];

      iNewValue1 = (int) oProj->scale(oNat->unscale(iOldValue1) + 0.000000000001);
      iNewValue2 = (int) oProj->scale(oNat->unscale(iOldValue2) + 0.000000000001);

      // correct the values to avoid values out of range
      if (iNewValue1 < 0)
        iNewValue1 = 1;
      else if (iNewValue1 > 255)
        iNewValue1 = 254;

      if (iNewValue2 < 0)
        iNewValue2 = 1;
      else if (iNewValue2 > 255)
        iNewValue2 = 254;

      // decide if it is an conservative or liberal projection
      // In conservative projections, if the value of a gene
      // is in one of the bounds of the native range layer value,
      // this value is mapped to the correspondent value to the
      // projection area. In liberal projections, a value in one
      // of the bounds of the layer values means that the rule 
      // applies for all values above (or below) that gene value

      if (iOldValue1 == 0 && !bConservative)
        iNewValue1 = 0;

      if (iOldValue2 == 0 && !bConservative)
        iNewValue2 = 0;

      if (iOldValue1 == 255 && !bConservative)
        iNewValue1 = 255;

      if (iOldValue2 == 255 && !bConservative)
        iNewValue2 = 255;

      oAuxRule->Gene[(i + 1) * 2]     = (BYTE) iNewValue1;
      oAuxRule->Gene[(i + 1) * 2 + 1] = (BYTE) iNewValue2;
    }
  } // end if oAuxRule->type() != 'r'

  // add rule to the rule set
  oToRuleSet->add(oAuxRule);
}

oToRuleSet->objModelLayerSet = oToLayerSet;

return true;
}
*/
// ==========================================================================

// ==============================================================

// gather rule set statistics
void RuleSet::gatherRuleSetStats(int gen)
{
  char type='0'; //initialise to something invalid

  printf("%4d]", gen);

  for (int i = 0; i < 3; i++)
  {
    switch (i)
    {
      case 0: type ='d'; break;
      case 1: type ='!'; break;
      case 2: type ='r'; break;
              //case 3: type ='a'; break;
    }

    double max = -10000;
    double sum = 0;
    int ct = 0;
    int pres = 0;
    for (int j = 0; j < intRules; j++)
    {
      Rule * rule = objRules[j];
      if (rule->type() == type)
      {
        ct++;
        sum += rule->dblPerformance[0];
        pres += (int) rule->Gene[0];
        if (max < rule->dblPerformance[0])
          max = rule->dblPerformance[0];
      }
    }

    if (max == -10000)
      max = 0;

    printf("%c %2d %+7.2f %+7.2f %2d|", type, ct, max, sum / ct, pres);
  }

  printf("\n");
}
