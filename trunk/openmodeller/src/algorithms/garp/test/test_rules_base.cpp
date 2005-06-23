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

#include <TestHarness.h>
#include <openmodeller/Sample.hh>
#include <rules_base.hh>

class ExtGarpRule : public GarpRule
{
public:
  ExtGarpRule(int numGenes = 0) : GarpRule(numGenes) {};

  ExtGarpRule(Scalar pred, int numGenes, 
	      Sample& ch1, Sample& ch2, double * perf) :
    GarpRule(pred, numGenes, ch1, ch2, perf) {};

  GarpRule * objFactory() const { return new ExtGarpRule(); } 
  int getStrength(const Sample&) const {}
  bool applies (const Sample&) const {}

  void setPrediction(Scalar newPrediction) { _prediction = newPrediction; }
  void setGenes(Scalar * genes, int numGenes);
};


void ExtGarpRule::setGenes(Scalar * genes, int numGenes)
{
  int i;

  _numGenes = numGenes;

  if (genes)
    {
      Scalar values1[_numGenes];
      Scalar values2[_numGenes];
      
      for (i = 0; i < _numGenes; i++)
	{
	  values1[i] = genes[i * 2];
	  values2[i] = genes[i * 2 + 1];
	}

      _chrom1 = Sample(_numGenes, values1);
      _chrom2 = Sample(_numGenes, values2);
    }
}


#define eps 10e-6

TEST( initialize, GarpRule )
{
  int i, n = 5;

  ExtGarpRule * rule = new ExtGarpRule(n);

  Sample chrom1 = rule->getChrom1();
  Sample chrom2 = rule->getChrom2();

  for (i = 0; i < n; ++i)
    { 
      DOUBLES_EQUAL((double) chrom1[i], -1.0, eps);
      DOUBLES_EQUAL((double) chrom2[i], +1.0, eps);
    }

  delete rule;
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
  double perfs[10];
  Sample c1(4, -0.5);
  Sample c2(4, +0.5);
  Sample blank(4);

  ExtGarpRule * src = new ExtGarpRule(1.0, 4, c1, c2, perfs);
  ExtGarpRule * dst = new ExtGarpRule(1.0, 4, blank, blank, perfs);
  
  LONGS_EQUAL(1, dst->copy(src));

  for (int i = 0; i < 4; ++i)
    {
      DOUBLES_EQUAL(-0.5, dst->getChrom1()[i], eps);
      DOUBLES_EQUAL(+0.5, dst->getChrom2()[i], eps);
    }
  
  delete src;
  delete dst;
}


TEST( clone , GarpRule )
{
  double perfs[10];
  Sample c1(4, -0.5);
  Sample c2(4, +0.5);

  ExtGarpRule * src = new ExtGarpRule(1.0, 4, c1, c2, perfs);
  ExtGarpRule * dst = (ExtGarpRule *) src->clone();
  
  for (int i = 0; i < 4; ++i)
    {
      DOUBLES_EQUAL(-0.5, dst->getChrom1()[i], eps);
      DOUBLES_EQUAL(+0.5, dst->getChrom2()[i], eps);
    }
  
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

Scalar genes3[8] = {-0.2, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};
Scalar genes4[8] = {+0.7, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};

TEST( Similar2, GarpRule )
{
  ExtGarpRule * rule1 = new ExtGarpRule;
  ExtGarpRule * rule2 = new ExtGarpRule;
  rule1->setGenes(genes3, 4);
  rule2->setGenes(genes4, 4);
  
  LONGS_EQUAL(rule1->similar(rule2), true);
  
  delete rule1; 
  delete rule2;
}

Scalar genes5[8] = {-0.8, +0.8, -1.0, +1.0, -0.8, +0.8, -0.8, +0.8};
Scalar genes6[8] = {-0.8, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};

TEST( Similar3, GarpRule )
{
  ExtGarpRule * rule1 = new ExtGarpRule;
  ExtGarpRule * rule2 = new ExtGarpRule;
  rule1->setGenes(genes5, 4);
  rule2->setGenes(genes6, 4);
  
  LONGS_EQUAL(rule1->similar(rule2), false);
  
  delete rule1; 
  delete rule2;
}

Scalar genes7[8] = {-0.8, +0.8, -1.0, +0.9, -0.8, +0.8, -0.8, +0.8};
Scalar genes8[8] = {-0.8, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};

TEST( Similar4, GarpRule )
{
  ExtGarpRule * rule1 = new ExtGarpRule;
  ExtGarpRule * rule2 = new ExtGarpRule;
  rule1->setGenes(genes7, 4);
  rule2->setGenes(genes8, 4);
  
  LONGS_EQUAL(rule1->similar(rule2), true);
  
  delete rule1; 
  delete rule2;
}

