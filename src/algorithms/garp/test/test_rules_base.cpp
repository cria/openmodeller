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
  int getStrength(Scalar *) {}
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

  if (genes)
    {
      _genes = new Scalar[_numGenes * 2];
      
      for (i = 0; i < _numGenes * 2; i++)
	{ _genes[i] = genes[i]; }
    }
}


#define eps 10e-6

class GarpCustomSamplerDummy1 : public GarpCustomSampler
{
public: 
  GarpCustomSamplerDummy1(int dim) { _dim = dim; }
  int dim() { g_log("**** 2nd dummy dim() ****\n"); return _dim; }
private: 
  int _dim;
};

TEST( initialize, GarpRule )
{
  int i, n = 5;

  ExtGarpRule * rule = new ExtGarpRule;
  GarpCustomSamplerDummy1 * sampler = new GarpCustomSamplerDummy1(n);

  // make sure sampler is returning the correct value
  LONGS_EQUAL(n, sampler->dim());

  rule->initialize(sampler);
  Scalar * genes = rule->getGenes();

  for (i = 0; i < n * 2; i += 2)
    { 
      DOUBLES_EQUAL((double) genes[i],     -1.0, eps);
      DOUBLES_EQUAL((double) genes[i + 1], +1.0, eps);
    }

  delete rule;
  delete sampler;
}


// helper function
bool checkEqualArray(Scalar * array1, Scalar * array2, int size, double veps)
{
  int i;

  if (!array1 || !array2)
    return false;

  for (i = 0; i < size; i++)
    if (fabs(array1[i] - array2[i]) > veps)
      { return false; }

  return true;
}


TEST( copy , GarpRule )
{
  ExtGarpRule * src = new ExtGarpRule;
  ExtGarpRule * dst = new ExtGarpRule;
  
  Scalar genes[8] = {-0.9, +1.0, -1.0, +1.0, -1.0, +1.0, -1.0, +1.0};
  Scalar blank[8] = { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0};

  src->setGenes(genes, 4);
  dst->setGenes(blank, 4);
  CHECK(dst->copy(src) == 1); 
  CHECK(src->getGenes() != NULL);
  CHECK(dst->getGenes() != NULL);
  CHECK(checkEqualArray(src->getGenes(), dst->getGenes(), 8, eps));
  
  delete src;
  delete dst;
}


TEST( clone , GarpRule )
{
  ExtGarpRule * src = new ExtGarpRule;
  ExtGarpRule * dst;
  
  Scalar genes[8] = {-0.9, +1.0, -1.0, +1.0, -1.0, +1.0, -1.0, +1.0};

  src->setGenes(genes, 4);
  dst = (ExtGarpRule *) src->clone();

  CHECK(src->type() == dst->type());  // check if factory works
  CHECK(src->getGenes() != NULL);
  CHECK(dst->getGenes() != NULL);
  CHECK(checkEqualArray(src->getGenes(), dst->getGenes(), 8, eps));
  
  delete src;
  delete dst;
}


TEST( getCertainty1 , GarpRule )
{
  ExtGarpRule * rule = new ExtGarpRule;
  
  rule->setPrediction(0.0);

  DOUBLES_EQUAL(rule->getCertainty(0.0), 1.0, eps);
  DOUBLES_EQUAL(rule->getCertainty(1.0), 0.0, eps);
  DOUBLES_EQUAL(rule->getCertainty(0.5), 0.0, eps);

  delete rule;
}


TEST( getCertainty2 , GarpRule )
{
  ExtGarpRule * rule = new ExtGarpRule;
  
  rule->setPrediction(1.0);

  DOUBLES_EQUAL(rule->getCertainty(0.0), 0.0, eps);
  DOUBLES_EQUAL(rule->getCertainty(1.0), 1.0, eps);
  DOUBLES_EQUAL(rule->getCertainty(0.5), 0.0, eps);

  delete rule;
}


TEST( getError , GarpRule )
{
  ExtGarpRule * rule = new ExtGarpRule;
  
  DOUBLES_EQUAL(rule->getError(0.0, 0.0), 0.0, eps);
  DOUBLES_EQUAL(rule->getError(1.0, 0.0), 1.0, eps);
  DOUBLES_EQUAL(rule->getError(1.0, -1.0), 2.0, eps);
  DOUBLES_EQUAL(rule->getError(-1.0, 1.0), 2.0, eps);
  DOUBLES_EQUAL(rule->getError(0.0, 0.5), 0.5, eps);

  delete rule;
}


Scalar genes1[8] = {-0.8, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};
Scalar genes2[8] = {-0.8, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};

TEST( Similar1, GarpRule )
{
  ExtGarpRule * rule1 = new ExtGarpRule;
  ExtGarpRule * rule2 = new ExtGarpRule;
  rule1->setGenes(genes1, 4);
  rule2->setGenes(genes2, 4);
  
  LONGS_EQUAL(rule1->similar(rule2), true);
  
  delete rule1; 
  delete rule2;
}

