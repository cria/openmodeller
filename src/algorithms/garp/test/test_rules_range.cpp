/**
 * Unit tests for rules_range.cpp
 * 
 * @file   test_rules_range.cpp
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

class ExtRangeRule : public RangeRule
{
public:
  ExtRangeRule();
  GarpRule * objFactory()  { return new ExtRangeRule; } 
  Scalar * getGenes()      { return _genes; };

  void setGenes(Scalar * genes, int numGenes);
};


ExtRangeRule::ExtRangeRule() : RangeRule() 
{
}


void ExtRangeRule::setGenes(Scalar * genes, int numGenes)
{
  int i;

  _numGenes = numGenes;
  if (_genes) { delete _genes; } 
  _genes = new Scalar[_numGenes * 2];

  for (i = 0; i < _numGenes * 2; i++)
    { _genes[i] = genes[i]; }
}

class Sampler;

SampledData::SampledData() {}
SampledData::~SampledData() {}
GarpCustomSampler::GarpCustomSampler() {}
GarpCustomSampler::~GarpCustomSampler() {}
void GarpCustomSampler::initialize(Sampler *, int) {}
int GarpCustomSampler::dim() { printf("## dim() ####\n"); }
int GarpCustomSampler::resamples() {}
Scalar * GarpCustomSampler::getSample(Scalar *) {}
int * GarpCustomSampler::getFrequencies(double, int *) {} 
void GarpCustomSampler::createBioclimHistogram(double, double, double) {}
void GarpCustomSampler::getBioclimRange(Scalar, int, Scalar *, Scalar *) {}


TEST( RangeRule_applies , RangeRule )
{
  ExtRangeRule * rule = new ExtRangeRule;
  
  Scalar genes[8] = {-0.8, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};
  Scalar vals1[8] = {-0.5, +0.5, -0.5, +0.5, -0.5, +0.5, -0.5, +0.5};
  Scalar vals2[8] = {-0.0, +0.0, -0.0, +0.0, -0.0, +0.0, -0.0, +0.0};
  Scalar vals3[8] = {-0.9, +1.0, -1.0, +1.0, -1.0, +1.0, -1.0, +1.0};

  rule->setGenes(genes, 4);

  CHECK_EQUAL(rule->applies(vals1), true);
  CHECK_EQUAL(rule->applies(vals2), true);
  CHECK_EQUAL(rule->applies(vals3), false);
}
