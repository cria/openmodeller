/**
 * Unit tests for method GarpRule::evaluate()
 * 
 * @file   test_rules_evaluate.cpp
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-04-07
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
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
 */

#include <CppUnitLite/TestHarness.h>
#include <om_occurrences.hh>
#include <Sample.hh>

#include <rules_range.hh>
#include <rules_negrange.hh>
#include <rules_logit.hh>
#include <test_rules_defs.hh>
#include <test_rules_evaluate_data.cpp>

typedef ExtendedDummyRule<RangeRule> ExtRangeRule;
typedef ExtendedDummyRule<NegatedRangeRule> ExtNegatedRangeRule;
typedef ExtendedDummyRule<LogitRule> ExtLogitRule;


#define eps 10e-6

OccurrencesPtr getSampleSet(int hardcodedSamplesIndex, int *dim)
{
  HardcodedSample * data = SamplesSet + hardcodedSamplesIndex;
  OccurrencesPtr occs ( new OccurrencesImpl("") );

  *dim = data->dimension;

  int valDim = data->dimension + 1;
 
  for (int i = 0; i < data->resamples; ++i)
    {
      Scalar pred = *(data->values + (valDim * i));
      Sample s(data->dimension, data->values + (valDim * i) + 1);
      OccurrencePtr oc( new OccurrenceImpl(0.0, 0.0, -1.0, pred) );
      oc->setEnvironment(s);
      occs->insert(oc);
    }

  return occs;
}



// helper function
bool checkEqualArray(Scalar * array1, Scalar * array2, int size, double veps)
{
  int i;
  bool result = true;

  if (!array1 || !array2)
    return false;

  for (i = 0; i < size; i++)
    {
      //printf("Comparing (%2d): %+12.8f == %+12.8f => ", i, array1[i], array2[i]);
      if ((array1[i] != -1000) && (array2[i] != -1000))
	{ result = result && (fabs(array1[i] - array2[i]) <= veps); }
     
      //printf("%d\n", result);
    }

  //printf("\n");

  return result;
}

template <class T>
bool testEvaluate(int sampleIndex, Scalar * ruleGenes,
		  Scalar rulePred, Scalar * rulePerfs)
{
  int i, dim;
  double * perf;
  bool result;

  T * rule = new T();
  OccurrencesPtr occs = getSampleSet(sampleIndex, &dim);
  rule->setPrediction(rulePred);
  rule->setGenes(ruleGenes, dim);
  rule->evaluate(occs);
  perf = rule->getPerformanceArray();

  /*
  OccurrencesImpl::const_iterator it = occs->begin();
  OccurrencesImpl::const_iterator end = occs->end();

  int k = 0;
  while (it != end)
    {
      Scalar y = (*it)->abundance();
      Sample s = (*it)->environment();
      
      printf("occs[%2d]: (occ=%-7.3f) ", k, y);
      for (int i = 0; i < dim; ++i)
	  printf("%-7.3f | ", s[i]); 
      printf("\n");

      ++it; ++k;
    }
    
  printf("Rule (pred=%7.3f): ", rule->getPrediction());
  for (int i = 0; i < dim; ++i)
    printf("%-7.3f %-7.3f | ", rule->getChrom1()[i], rule->getChrom2()[i]); 
  printf("\n");
  */

  result = checkEqualArray(perf, rulePerfs, 10, eps);
  delete rule;
  return result;
}


// SampleSet 1
// ===========
TEST( evaluate1_1, RangeRule )
{ CHECK(testEvaluate<ExtRangeRule>(1, RuleGenes1_1, RulePred1_1, RulePerfs1_1)); }

TEST( evaluate1_2, RangeRule )
{ CHECK(testEvaluate<ExtRangeRule>(1, RuleGenes1_2, RulePred1_2, RulePerfs1_2)); }

TEST( evaluate1_3, NegatedRangeRule )
{ CHECK(testEvaluate<ExtNegatedRangeRule>(1, RuleGenes1_3, RulePred1_3, RulePerfs1_3)); }

TEST( evaluate1_4, NegatedRangeRule )
{ CHECK(testEvaluate<ExtNegatedRangeRule>(1, RuleGenes1_4, RulePred1_4, RulePerfs1_4)); }


// SampleSet 2
// ===========
TEST( evaluate2_1, RangeRule )
{ CHECK(testEvaluate<ExtRangeRule>(2, RuleGenes2_1, RulePred2_1, RulePerfs2_1)); }

TEST( evaluate2_2, RangeRule )
{ CHECK(testEvaluate<ExtRangeRule>(2, RuleGenes2_2, RulePred2_2, RulePerfs2_2)); }
 
TEST( evaluate2_3, RangeRule )
{ CHECK(testEvaluate<ExtRangeRule>(2, RuleGenes2_3, RulePred2_3, RulePerfs2_3)); }

TEST( evaluate2_4, RangeRule )
{ CHECK(testEvaluate<ExtRangeRule>(2, RuleGenes2_4, RulePred2_4, RulePerfs2_4)); }


// SampleSet 3
// ===========
TEST( evaluate3_1, RangeRule )
{ CHECK(testEvaluate<ExtRangeRule>(3, RuleGenes3_1, RulePred3_1, RulePerfs3_1)); }

TEST( evaluate3_2, RangeRule )
{ CHECK(testEvaluate<ExtRangeRule>(3, RuleGenes3_2, RulePred3_2, RulePerfs3_2)); }


// SampleSet 4
// ===========
TEST( evaluate4_1, RangeRule )
{ CHECK(testEvaluate<ExtRangeRule>(4, RuleGenes4_1, RulePred4_1, RulePerfs4_1)); }

TEST( evaluate4_2, RangeRule )
{ CHECK(testEvaluate<ExtRangeRule>(4, RuleGenes4_2, RulePred4_2, RulePerfs4_2)); }

TEST( evaluate4_3, RangeRule )
{ CHECK(testEvaluate<ExtRangeRule>(4, RuleGenes4_3, RulePred4_3, RulePerfs4_3)); }

TEST( evaluate4_4, NegatedRangeRule )
{ CHECK(testEvaluate<ExtNegatedRangeRule>(4, RuleGenes4_4, RulePred4_4, RulePerfs4_4)); }

TEST( evaluate4_5, NegatedRangeRule )
{ CHECK(testEvaluate<ExtNegatedRangeRule>(4, RuleGenes4_5, RulePred4_5, RulePerfs4_5)); }

TEST( evaluate4_6, NegatedRangeRule )
{ CHECK(testEvaluate<ExtNegatedRangeRule>(4, RuleGenes4_6, RulePred4_6, RulePerfs4_6)); }



// Logit regression tests
TEST( regression4_1, LogitRule )
{
  int dim;
  Regression reg;

  OccurrencesPtr occs = getSampleSet(4, &dim);
  reg.calculateParameters(occs);

  DOUBLES_EQUAL(reg.getA()[0], +0.336735, eps);
  DOUBLES_EQUAL(reg.getB()[0], -0.459183, eps);
  DOUBLES_EQUAL(reg.getC()[0], -0.445368, eps);

  DOUBLES_EQUAL(reg.getA()[1], +0.378012, eps);
  DOUBLES_EQUAL(reg.getB()[1], -0.353916, eps);
  DOUBLES_EQUAL(reg.getC()[1], -0.596326, eps);
}
