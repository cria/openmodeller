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
#include <rules_range.hh>
#include <rules_negrange.hh>
#include <rules_logit.hh>
#include <rules_atomic.hh>
#include <garp_sampler.hh>
#include <test_rules_defs.hh>
#include <test_rules_evaluate_data.cpp>

class GarpCustomSamplerDummy2;

EXTENDED_DUMMY_RULE ( RangeRule );
EXTENDED_DUMMY_RULE ( NegatedRangeRule );
EXTENDED_DUMMY_RULE ( LogitRule );
EXTENDED_DUMMY_RULE ( AtomicRule );

#define eps 10e-6

class GarpCustomSamplerDummy2 : public GarpCustomSampler
{
public: 
  GarpCustomSamplerDummy2() {};
  void chooseSampleSet(int hardcodedSamplesIndex);
  Scalar * getSample(Scalar * values);
  int resamples() { g_log("Resamples:  %d\n", _sample->resamples); return _sample->resamples; }
  int dim()       { g_log("Dimensions: %d\n", _sample->dimension); return _sample->dimension; }

private:
  int _nextSample;
  int _sampleDimension;
  HardcodedSample * _sample;
};

void GarpCustomSamplerDummy2::chooseSampleSet(int hardcodedSamplesIndex)
{
  _sample = SamplesSet + hardcodedSamplesIndex;
  _sampleDimension = _sample->dimension + 1;
  _nextSample = 0;
}

Scalar * GarpCustomSamplerDummy2::getSample(Scalar * pointValue)
{
  //g_log("Getting sample #%d\n", _nextSample);
  // returns first value in sample as point value
  Scalar * values = (Scalar *) &(_sample->samples[_sampleDimension * _nextSample]);
  *pointValue = values[0];
  _nextSample++;
  return (Scalar *) &(values[1]);
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
      g_log("Comparing (%2d): %+12.8f == %+12.8f => ", i, array1[i], array2[i]);

      if ((array1[i] != -1000) && (array2[i] != -1000))
	{ 
	  result = result && (fabs(array1[i] - array2[i]) <= veps); 
	}
      
      g_log("%d\n", result);
    }

  return result;
}

#define TEST_DUMMY_EVALUATE( name ) \
bool test##name##Evaluate(int sampleIndex, Scalar * ruleGenes, \
                          Scalar rulePred, Scalar * rulePerfs) \
{ \
  int i; \
  double * perf; \
  bool result; \
  Ext##name * rule = new Ext##name; \
  GarpCustomSamplerDummy2 * sampler = new GarpCustomSamplerDummy2; \
  sampler->chooseSampleSet(sampleIndex); \
  rule->setPrediction(rulePred);\
  rule->setGenes(ruleGenes, sampler->dim());\
  rule->evaluate(sampler);\
  perf = rule->getPerformanceArray();\
  result = checkEqualArray(perf, rulePerfs, 10, eps);\
  delete rule;\
  delete sampler;\
  return result;\
}


TEST_DUMMY_EVALUATE ( RangeRule );
TEST_DUMMY_EVALUATE ( NegatedRangeRule );
TEST_DUMMY_EVALUATE ( LogitRule );
TEST_DUMMY_EVALUATE ( AtomicRule );

// SampleSet 1
// ===========
TEST( evaluate1_1, RangeRule )
{ CHECK(testRangeRuleEvaluate(1, RuleGenes1_1, RulePred1_1, RulePerfs1_1)); }

TEST( evaluate1_2, RangeRule )
{ CHECK(testRangeRuleEvaluate(1, RuleGenes1_2, RulePred1_2, RulePerfs1_2)); }

TEST( evaluate1_3, NegatedRangeRule )
{ CHECK(testNegatedRangeRuleEvaluate(1, RuleGenes1_3, RulePred1_3, RulePerfs1_3)); }

TEST( evaluate1_4, NegatedRangeRule )
{ CHECK(testNegatedRangeRuleEvaluate(1, RuleGenes1_4, RulePred1_4, RulePerfs1_4)); }

TEST( evaluate1_5, RangeRule )
{ CHECK(testAtomicRuleEvaluate(1, RuleGenes1_5, RulePred1_5, RulePerfs1_5)); }

TEST( evaluate1_6, RangeRule )
{ CHECK(testAtomicRuleEvaluate(1, RuleGenes1_6, RulePred1_6, RulePerfs1_6)); }

// SampleSet 2
// ===========
TEST( evaluate2_1, RangeRule )
{ CHECK(testRangeRuleEvaluate(2, RuleGenes2_1, RulePred2_1, RulePerfs2_1)); }

TEST( evaluate2_2, RangeRule )
{ CHECK(testRangeRuleEvaluate(2, RuleGenes2_2, RulePred2_2, RulePerfs2_2)); }
 
TEST( evaluate2_3, RangeRule )
{ CHECK(testRangeRuleEvaluate(2, RuleGenes2_3, RulePred2_3, RulePerfs2_3)); }

TEST( evaluate2_4, RangeRule )
{ CHECK(testRangeRuleEvaluate(2, RuleGenes2_4, RulePred2_4, RulePerfs2_4)); }


// SampleSet 3
// ===========
TEST( evaluate3_1, RangeRule )
{ CHECK(testRangeRuleEvaluate(3, RuleGenes3_1, RulePred3_1, RulePerfs3_1)); }

TEST( evaluate3_2, RangeRule )
{ CHECK(testRangeRuleEvaluate(3, RuleGenes3_2, RulePred3_2, RulePerfs3_2)); }


// SampleSet 4
// ===========
TEST( evaluate4_1, RangeRule )
{ CHECK(testRangeRuleEvaluate(4, RuleGenes4_1, RulePred4_1, RulePerfs4_1)); }

TEST( evaluate4_2, RangeRule )
{ CHECK(testRangeRuleEvaluate(4, RuleGenes4_2, RulePred4_2, RulePerfs4_2)); }

TEST( evaluate4_3, RangeRule )
{ CHECK(testRangeRuleEvaluate(4, RuleGenes4_3, RulePred4_3, RulePerfs4_3)); }

TEST( evaluate4_4, NegatedRangeRule )
{ CHECK(testNegatedRangeRuleEvaluate(4, RuleGenes4_4, RulePred4_4, RulePerfs4_4)); }

TEST( evaluate4_5, NegatedRangeRule )
{ CHECK(testNegatedRangeRuleEvaluate(4, RuleGenes4_5, RulePred4_5, RulePerfs4_5)); }

TEST( evaluate4_6, NegatedRangeRule )
{ CHECK(testNegatedRangeRuleEvaluate(4, RuleGenes4_6, RulePred4_6, RulePerfs4_6)); }
