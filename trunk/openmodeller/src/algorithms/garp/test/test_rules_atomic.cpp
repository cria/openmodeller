/**
 * Unit tests for rules_atomic.cpp
 * 
 * @file   test_rules_atomic.cpp
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
#include <rules_atomic.hh>
#include <garp_sampler.hh>
#include <test_rules_defs.hh>

EXTENDED_DUMMY_RULE( AtomicRule );


Scalar genes1[8] = {-0.8, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};
Scalar vals11[8] = {-0.5,       -0.5,       -0.5,       -0.5      };
Scalar vals12[8] = {-0.0,       -0.0,       -0.0,       -0.0      };
Scalar vals13[8] = {-0.8,       -0.8,       -0.8,       -0.8      };

TEST( AtomicRule_applies11 , AtomicRule )
{ 
  ExtAtomicRule * rule = new ExtAtomicRule;
  rule->setGenes(genes1, 4);
  LONGS_EQUAL(rule->applies(vals11), false);
}

TEST( AtomicRule_applies12 , AtomicRule )
{ 
  ExtAtomicRule * rule = new ExtAtomicRule;
  rule->setGenes(genes1, 4);
  LONGS_EQUAL(rule->applies(vals12), false);
}

TEST( AtomicRule_applies13 , AtomicRule )
{ 
  ExtAtomicRule * rule = new ExtAtomicRule;
  rule->setGenes(genes1, 4);
  LONGS_EQUAL(rule->applies(vals13), true);
}

Scalar genes2[8] = {-1.0, +1.0, -1.0, +1.0, -1.0, +1.0, -1.0, +1.0};
Scalar vals21[8] = {-0.5,       -0.5,       -0.5,       -0.5      };
Scalar vals22[8] = {-0.0,       -0.0,       -0.0,       -0.0      };
Scalar vals23[8] = {-2.9,       -1.0,       -1.0,       -1.0      };

TEST( AtomicRule_applies21 , AtomicRule )
{
  ExtAtomicRule * rule = new ExtAtomicRule;
  rule->setGenes(genes2, 4);
  LONGS_EQUAL(rule->applies(vals21), true);
}

TEST( AtomicRule_applies22 , AtomicRule )
{
  ExtAtomicRule * rule = new ExtAtomicRule;
  rule->setGenes(genes2, 4);
  LONGS_EQUAL(rule->applies(vals22), true);
}

TEST( AtomicRule_applies23 , AtomicRule )
{
  ExtAtomicRule * rule = new ExtAtomicRule;
  rule->setGenes(genes2, 4);
  LONGS_EQUAL(rule->applies(vals23), true);
}


Scalar genes3[8] = {-1.0, +1.0, -0.1, +0.1, -1.0, +1.0, -1.0, +1.0};
Scalar vals31[8] = {-0.5,       -0.5,       -0.5,       -0.5      };
Scalar vals32[8] = {-0.0,       -0.0,       -0.0,       -0.0      };
Scalar vals33[8] = {-2.9,       -0.1,       -1.0,       -1.0      };

TEST( AtomicRule_applies31 , AtomicRule )
{
  ExtAtomicRule * rule = new ExtAtomicRule;
  rule->setGenes(genes3, 4);
  LONGS_EQUAL(rule->applies(vals31), false);
}

TEST( AtomicRule_applies32 , AtomicRule )
{
  ExtAtomicRule * rule = new ExtAtomicRule;
  rule->setGenes(genes3, 4);
  LONGS_EQUAL(rule->applies(vals32), false);
}

TEST( AtomicRule_applies33 , AtomicRule )
{
  ExtAtomicRule * rule = new ExtAtomicRule;
  rule->setGenes(genes3, 4);
  LONGS_EQUAL(rule->applies(vals33), true);
}


Scalar genes4[8] = {-0.8, +0.8, -1.0, +1.0, -0.8, +0.0, -0.0, +1.0};
Scalar vals41[8] = {-0.8,       -0.5,       -0.8,       -0.5      };
Scalar vals42[8] = {-0.8,       -0.0,       -0.8,       -0.0      };
Scalar vals43[8] = {-0.8,       -9.0,       -0.8,       -0.0      };

TEST( AtomicRule_applies41 , AtomicRule )
{
  ExtAtomicRule * rule = new ExtAtomicRule;
  rule->setGenes(genes4, 4);
  LONGS_EQUAL(rule->applies(vals41), false);
}

TEST( AtomicRule_applies42 , AtomicRule )
{
  ExtAtomicRule * rule = new ExtAtomicRule;
  rule->setGenes(genes4, 4);
  LONGS_EQUAL(rule->applies(vals42), true);
}

TEST( AtomicRule_applies43 , AtomicRule )
{
  ExtAtomicRule * rule = new ExtAtomicRule;
  rule->setGenes(genes4, 4);
  LONGS_EQUAL(rule->applies(vals43), true);
}


Scalar genes5[8] = {-0.5, -0.5, -0.5, -0.5};
Scalar vals51[8] = {-0.5,       -0.5      };

TEST( AtomicRule_applies51 , AtomicRule )
{
  ExtAtomicRule * rule = new ExtAtomicRule;
  rule->setGenes(genes5, 2);
  LONGS_EQUAL(rule->applies(vals51), true);
}

