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
#include <garp_sampler.hh>
#include <test_rules_evaluate_data.cpp>

class ExtRangeRule : public RangeRule
{
public:
  ExtRangeRule() {};
  GarpRule * objFactory()   { return new ExtRangeRule; } 
  Scalar * getGenes()       { return _genes; };
  double * getPerformanceArray() { return _performance; }
  void setPrediction(Scalar newPrediction) { _prediction = newPrediction; }
  void initialize(GarpCustomSampler * sampler) { GarpRule::initialize(sampler); }
  void setGenes(Scalar * genes, int numGenes);
};

void ExtRangeRule::setGenes(Scalar * genes, int numGenes)
{
  int i;

  _numGenes = numGenes;
  if (_genes) { delete _genes; } 

  if (genes)
    {
      _genes = new Scalar[_numGenes * 2];
      
      g_log("Setting genes: ");
      for (i = 0; i < _numGenes * 2; i++)
	{
	  _genes[i] = genes[i]; 
	  g_log("%+8.4f ", _genes[i]);
	}
      g_log("\n");
    }
}


class Sampler;

SampledData::SampledData() {}
SampledData::~SampledData() {}
GarpCustomSampler::GarpCustomSampler() {}
GarpCustomSampler::~GarpCustomSampler() {}
void GarpCustomSampler::initialize(Sampler *, int) {}
int GarpCustomSampler::resamples() {}
Scalar * GarpCustomSampler::getSample(Scalar *) { g_log("#### dummy getSample() called ####\n"); }
int * GarpCustomSampler::getFrequencies(double, int *) {} 
void GarpCustomSampler::createBioclimHistogram(double, double, double) {}
void GarpCustomSampler::getBioclimRange(Scalar, int, Scalar *, Scalar *) {}
int GarpCustomSampler::dim() { g_log("#### dummy dim() called ####\n"); }


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
  // returns first value in sample as point value
  Scalar * values = (Scalar *) &(_sample->samples[_sampleDimension * _nextSample++]);
  *pointValue = values[0];
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


bool TestRule(int sampleIndex, Scalar * ruleGenes, 
	      Scalar rulePred, Scalar * rulePerfs)
{
  int i;
  double * perf;
  bool result;
  ExtRangeRule * rule;
  
  GarpCustomSamplerDummy2 * sampler = new GarpCustomSamplerDummy2;

  rule = new ExtRangeRule;
  sampler->chooseSampleSet(sampleIndex);
  rule->initialize(sampler);
  rule->setPrediction(rulePred);
  rule->setGenes(ruleGenes, sampler->dim());
  rule->evaluate(sampler);
  perf = rule->getPerformanceArray();
  result = checkEqualArray(perf, rulePerfs, 10, eps);

  delete rule;
  delete sampler;

  return result;
}

TEST( evaluate1_1, RangeRule )
{ CHECK(TestRule(0, RuleGenes1_1, RulePred1_1, RulePerfs1_1)); }

TEST( evaluate1_2, RangeRule )
{ CHECK(TestRule(0, RuleGenes1_2, RulePred1_2, RulePerfs1_2)); }

TEST( evaluate2_1, RangeRule )
{ CHECK(TestRule(0, RuleGenes2_1, RulePred2_1, RulePerfs2_1)); }

TEST( evaluate2_2, RangeRule )
{ CHECK(TestRule(0, RuleGenes2_2, RulePred2_2, RulePerfs2_2)); }
