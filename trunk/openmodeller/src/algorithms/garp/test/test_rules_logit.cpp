/**
 * Unit tests for rules_logit.cpp
 * 
 * @file   test_rules_logit.cpp
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-07-25
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
#include <rules_logit.hh>
#include <test_rules_defs.hh>

typedef ExtendedDummyRule<LogitRule> ExtLogitRule;

Scalar genes1[8] = {-0.8, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};
Scalar genes2[8] = {-0.8, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};


TEST( LogitRule_similar1, LogitRule )
{
  ExtLogitRule * rule1 = new ExtLogitRule;
  ExtLogitRule * rule2 = new ExtLogitRule;
  rule1->setGenes(genes1, 4);
  rule2->setGenes(genes2, 4);
  
  LONGS_EQUAL(true, rule1->similar(rule2));
  
  delete rule1; 
  delete rule2;
}

TEST( LogitRule_similar1b, LogitRule )
{
  ExtLogitRule * rule1 = new ExtLogitRule;
  ExtLogitRule * rule2 = new ExtLogitRule;
  rule1->setGenes(genes1, 4);
  rule2->setGenes(genes2, 4);
  
  LONGS_EQUAL(true, rule1->similar(rule2));
  
  delete rule1; 
  delete rule2;
}

Scalar genes3[8] = {-0.8, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};
Scalar genes4[8] = {-0.8, -0.4, -0.3, +0.8, +0.7, +0.8, -0.8, +0.8};

TEST( LogitRule_similar2, LogitRule )
{
  ExtLogitRule * rule1 = new ExtLogitRule;
  ExtLogitRule * rule2 = new ExtLogitRule;
  rule1->setGenes(genes3, 4);
  rule2->setGenes(genes4, 4);
  
  LONGS_EQUAL(true, rule1->similar(rule2));
  
  delete rule1; 
  delete rule2;
}

Scalar genes5[8] = {-0.8, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};
Scalar genes6[8] = {-0.8, -0.4, -0.3, +0.8, +0.7, -0.4, +0.3, +0.8};

TEST( LogitRule_similar3, LogitRule )
{
  ExtLogitRule * rule1 = new ExtLogitRule;
  ExtLogitRule * rule2 = new ExtLogitRule;
  rule1->setGenes(genes5, 4);
  rule2->setGenes(genes6, 4);
  
  LONGS_EQUAL(true, rule1->similar(rule2));
  
  delete rule1; 
  delete rule2;
}

Scalar genes7[8] = {-0.8, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};
Scalar genes8[8] = {+0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8};

TEST( LogitRule_similar4, LogitRule )
{
  ExtLogitRule * rule1 = new ExtLogitRule;
  ExtLogitRule * rule2 = new ExtLogitRule;
  rule1->setGenes(genes7, 4);
  rule2->setGenes(genes8, 4);
  
  LONGS_EQUAL(true, rule1->similar(rule2));
  
  delete rule1; 
  delete rule2;
}

Scalar genes9[8]  = {+0.02, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8};
Scalar genes10[8] = {-0.04, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8};

TEST( LogitRule_similar5, LogitRule )
{
  ExtLogitRule * rule1 = new ExtLogitRule;
  ExtLogitRule * rule2 = new ExtLogitRule;
  rule1->setGenes(genes9, 4);
  rule2->setGenes(genes10, 4);
  
  LONGS_EQUAL(true, rule1->similar(rule2));
  
  delete rule1; 
  delete rule2;
}

Scalar genes11[8] = {-0.01, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};
Scalar genes12[8] = {-0.50, +0.8, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};

TEST( LogitRule_similar6, LogitRule )
{
  ExtLogitRule * rule1 = new ExtLogitRule;
  ExtLogitRule * rule2 = new ExtLogitRule;
  rule1->setGenes(genes11, 4);
  rule2->setGenes(genes12, 4);
  
  LONGS_EQUAL(false, rule1->similar(rule2));
  
  delete rule1; 
  delete rule2;
}

Scalar genes13[8] = {-0.01, +0.02, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};
Scalar genes14[8] = {-0.50, +0.80, -0.8, +0.8, -0.8, +0.8, -0.8, +0.8};

TEST( LogitRule_similar7, LogitRule )
{
  ExtLogitRule * rule1 = new ExtLogitRule;
  ExtLogitRule * rule2 = new ExtLogitRule;
  rule1->setGenes(genes13, 4);
  rule2->setGenes(genes14, 4);
  
  LONGS_EQUAL(false, rule1->similar(rule2));
  
  delete rule1; 
  delete rule2;
}

// Rule below applies to all points
// TODO: come up with tests for other cases
Scalar genes100[8] = {-1.0, +0.8, -1.0, +0.8, -1.0, +0.8, -1.0, +0.8};
Scalar vals91[8]   = {+0.8,       +0.8,       +0.8,       +0.8      };
Scalar vals92[8]   = {+0.8,       +0.8,       +0.8,       +0.8      };
Scalar vals93[8]   = {+0.8,       +0.8,       +0.8,       +0.8      };

TEST( LogitRule_applies1, LogitRule )
{
  ExtLogitRule * rule = new ExtLogitRule;
  rule->setGenes(genes100, 4);
  Sample sample (4, vals91);
  LONGS_EQUAL(true, rule->applies(sample));
  delete rule; 
}

TEST( LogitRule_applies2, LogitRule )
{
  ExtLogitRule * rule = new ExtLogitRule;
  rule->setGenes(genes100, 4);
  Sample sample (4, vals92);
  LONGS_EQUAL(true, rule->applies(sample));
  delete rule; 
}

TEST( LogitRule_applies3, LogitRule )
{
  ExtLogitRule * rule = new ExtLogitRule;
  rule->setGenes(genes100, 4);
  Sample sample (4, vals93);
  LONGS_EQUAL(true, rule->applies(sample));
  delete rule; 
}

