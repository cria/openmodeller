/**
 * Unit tests for rules_negrange.cpp
 * 
 * @file   test_rules_negrange.cpp
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-07-22
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
#include <Sample.hh>
#include <rules_negrange.hh>
#include <test_rules_defs.hh>

typedef ExtendedDummyRule<NegatedRangeRule> ExtNegatedRangeRule;


Scalar genes1[8] = {-0.8, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};
Scalar vals11[4] = {-0.5,       -0.5,       -0.5,       -0.5      };
Scalar vals12[4] = {-0.0,       -0.0,       -0.0,       -0.0      };
Scalar vals13[4] = {-0.9,       -1.0,       -1.0,       -1.0      };

TEST( NegatedRangeRule_applies11 , NegatedRangeRule )
{
  ExtNegatedRangeRule * rule = new ExtNegatedRangeRule;
  rule->setGenes(genes1, 4);
  Sample sample(4, vals11);
  LONGS_EQUAL(rule->applies(sample), false);
}

TEST( NegatedRangeRule_applies12 , NegatedRangeRule )
{
  ExtNegatedRangeRule * rule = new ExtNegatedRangeRule;
  rule->setGenes(genes1, 4);
  Sample sample(4, vals12);
  LONGS_EQUAL(rule->applies(sample), false);
}

TEST( NegatedRangeRule_applies13 , NegatedRangeRule )
{
  ExtNegatedRangeRule * rule = new ExtNegatedRangeRule;
  rule->setGenes(genes1, 4);
  Sample sample(4, vals13);
  LONGS_EQUAL(rule->applies(sample), true);
}


Scalar genes2[8] = {-1.0, +1.0, -1.0, +1.0, -1.0, +1.0, -1.0, +1.0};
Scalar vals21[8] = {-0.5,       -0.5,       -0.5,       -0.5      };
Scalar vals22[8] = {-0.0,       -0.0,       -0.0,       -0.0      };
Scalar vals23[8] = {-2.9,       -1.0,       -1.0,       -1.0      };

TEST( NegatedRangeRule_applies21 , NegatedRangeRule )
{
  ExtNegatedRangeRule * rule = new ExtNegatedRangeRule;
  rule->setGenes(genes2, 4);
  Sample sample(4, vals21);
  LONGS_EQUAL(rule->applies(sample), true);
}

TEST( NegatedRangeRule_applies22 , NegatedRangeRule )
{
  ExtNegatedRangeRule * rule = new ExtNegatedRangeRule;
  rule->setGenes(genes2, 4);
  Sample sample(4, vals22);
  LONGS_EQUAL(rule->applies(sample), true);
}

TEST( NegatedRangeRule_applies23 , NegatedRangeRule )
{
  ExtNegatedRangeRule * rule = new ExtNegatedRangeRule;
  rule->setGenes(genes2, 4);
  Sample sample(4, vals23);
  LONGS_EQUAL(rule->applies(sample), true);
}

Scalar genes3[8] = {-1.0, +1.0, -0.1, +0.1, -1.0, +1.0, -1.0, +1.0};
Scalar vals31[8] = {-0.5        -0.5,       -0.5,       -0.5      };
Scalar vals32[8] = {-0.0        -0.0,       -0.0,       -0.0      };
Scalar vals33[8] = {-2.9        -1.0,       -1.0,       -1.0      };

TEST( NegatedRangeRule_applies31 , NegatedRangeRule )
{
  ExtNegatedRangeRule * rule = new ExtNegatedRangeRule;
  rule->setGenes(genes3, 4);
  Sample sample(4, vals31);
  LONGS_EQUAL(rule->applies(sample), true);
}

TEST( NegatedRangeRule_applies32 , NegatedRangeRule )
{
  ExtNegatedRangeRule * rule = new ExtNegatedRangeRule;
  rule->setGenes(genes3, 4);
  Sample sample(4, vals32);
  LONGS_EQUAL(rule->applies(sample), false);
}

TEST( NegatedRangeRule_applies33 , NegatedRangeRule )
{
  ExtNegatedRangeRule * rule = new ExtNegatedRangeRule;
  rule->setGenes(genes3, 4);
  Sample sample(4, vals33);
  LONGS_EQUAL(rule->applies(sample), true);
}
