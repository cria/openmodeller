/**
 * Unit tests for rules_base.cpp
 * 
 * @file   test_rules_base.cpp
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
#include <rules_base.hh>
#include <garp_sampler.hh>

class ExtGarpRule : public GarpRule
{
public:
  ExtGarpRule();
  GarpRule * objFactory()  { return new ExtGarpRule; } 
  void initialize(GarpCustomSampler *) {}
  double getStrength(Scalar *) {}
  bool applies (Scalar *) {}

  Scalar * getGenes()      { return _genes; };
  void setPrediction(Scalar newPrediction) { _prediction = newPrediction; }
  void setGenes(Scalar * genes, int numGenes);
};


ExtGarpRule::ExtGarpRule() : GarpRule() {}

void ExtGarpRule::setGenes(Scalar * genes, int numGenes)
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

#define eps 10e-6

TEST( GarpRule_getCertainty1 , GarpRule )
{
  ExtGarpRule * rule = new ExtGarpRule;
  
  rule->setPrediction(0.0);

  DOUBLES_EQUAL(rule->getCertainty(0.0), 1.0, eps);
  DOUBLES_EQUAL(rule->getCertainty(1.0), 0.0, eps);
  DOUBLES_EQUAL(rule->getCertainty(0.5), 0.0, eps);
}


TEST( GarpRule_getCertainty2 , GarpRule )
{
  ExtGarpRule * rule = new ExtGarpRule;
  
  rule->setPrediction(1.0);

  DOUBLES_EQUAL(rule->getCertainty(0.0), 0.0, eps);
  DOUBLES_EQUAL(rule->getCertainty(1.0), 1.0, eps);
  DOUBLES_EQUAL(rule->getCertainty(0.5), 0.0, eps);
}


TEST( GarpRule_getError , GarpRule )
{
  ExtGarpRule * rule = new ExtGarpRule;
  
  DOUBLES_EQUAL(rule->getError(0.0, 0.0), 0.0, eps);
  DOUBLES_EQUAL(rule->getError(1.0, 0.0), 1.0, eps);
  DOUBLES_EQUAL(rule->getError(1.0, -1.0), 2.0, eps);
  DOUBLES_EQUAL(rule->getError(-1.0, 1.0), 2.0, eps);
  DOUBLES_EQUAL(rule->getError(0.0, 0.5), 0.5, eps);
}
