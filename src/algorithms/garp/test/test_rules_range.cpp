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
#include <test_rules_defs.hh>

EXTENDED_DUMMY_RULE( RangeRule );


TEST( RangeRule_applies1 , RangeRule )
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


TEST( RangeRule_applies2 , RangeRule )
{
  ExtRangeRule * rule = new ExtRangeRule;
  
  Scalar genes[8] = {-1.0, +1.0, -1.0, +1.0, -1.0, +1.0, -1.0, +1.0};
  Scalar vals1[8] = {-0.5, +0.5, -0.5, +0.5, -0.5, +0.5, -0.5, +0.5};
  Scalar vals2[8] = {-0.0, +0.0, -0.0, +0.0, -0.0, +0.0, -0.0, +0.0};
  Scalar vals3[8] = {-2.9, +3.0, -1.0, +1.0, -1.0, +1.0, -1.0, +1.0};

  rule->setGenes(genes, 4);

  CHECK_EQUAL(rule->applies(vals1), true);
  CHECK_EQUAL(rule->applies(vals2), true);
  CHECK_EQUAL(rule->applies(vals3), true);
}

TEST( RangeRule_applies3 , RangeRule )
{
  ExtRangeRule * rule = new ExtRangeRule;
  
  Scalar genes[8] = {-1.0, +1.0, -0.1, +0.1, -1.0, +1.0, -1.0, +1.0};
  Scalar vals1[8] = {-0.5, +0.5, -0.5, +0.5, -0.5, +0.5, -0.5, +0.5};
  Scalar vals2[8] = {-0.0, +0.0, -0.0, +0.0, -0.0, +0.0, -0.0, +0.0};
  Scalar vals3[8] = {-2.9, +3.0, -1.0, +1.0, -1.0, +1.0, -1.0, +1.0};

  rule->setGenes(genes, 4);

  CHECK_EQUAL(rule->applies(vals1), false);
  CHECK_EQUAL(rule->applies(vals2), true);
  CHECK_EQUAL(rule->applies(vals3), false);
}
