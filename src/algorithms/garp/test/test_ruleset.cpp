/**
 * Unit tests for ruleset.cpp
 * 
 * @file   test_ruleset.cpp
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
#include <ruleset.hh>
#include <rules_base.hh>

class DummyRule : public GarpRule
{
public:
  DummyRule(int id = -1);
  GarpRule * objFactory() const { return new DummyRule(); } 
  int getStrength(const Sample&) const {}
  double getValue(const Sample&) const {}
  bool applies(const Sample&) const {}

  int getId() { return _id; }
  void setPerformance(PerfIndex index, double newPerf) { _performance[index] = newPerf; }

private:
  int _id;
};

DummyRule::DummyRule(int id) : GarpRule()
{ 
  _id = id; 
}

// dummy implementation of GarpRule methods
GarpRule::GarpRule() :
  _numGenes(0),
  _prediction(0),
  _needsEvaluation(true),
  _chrom1(), 
  _chrom2()
{
  _origin = type();
  for (int i = 0; i < 10; i++)
    _performance[i] = 0.0;
}

double GarpRule::getPerformance(PerfIndex perfIndex) const
{
  return _performance[perfIndex];
}

GarpRule::~GarpRule() {};
GarpRule * GarpRule::clone() const { g_log("Cl"); };
int GarpRule::copy(const GarpRule * fromRule) { g_log("Cp"); };
int GarpRule::getCertainty(Scalar pred) const { g_log("Ct"); };
double GarpRule::getError(Scalar predefinedValue, Scalar pred) const { g_log("Er"); };
void GarpRule::adjustRange(Scalar& v1, Scalar& v2) const { g_log("Ad"); };
void GarpRule::crossover(GarpRule * rule, int xpt1, int xpt2) { g_log("Co"); };
void GarpRule::mutate(double temperature) { g_log("Mu"); };
bool GarpRule::similar(const GarpRule * objOtherRule) const { g_log("Si"); };
double GarpRule::evaluate(const OccurrencesPtr& occs) { g_log("Ev"); };
void GarpRule::log() {};

void testHelperAddRules(GarpRuleSet * ruleset, int from, int to)
{
  int i;
  DummyRule * r;

  for (i = from; i <= to; i++)
    {
      r = new DummyRule(i);
      ruleset->add(r);
    }
}

// check function testHelperAddRules
TEST( testHelperAddRules , Global )
{
  DummyRule * rule;
  GarpRuleSet ruleset(100);
  testHelperAddRules(&ruleset, 0, 99);

  // check number of rules and if there is a rule in there
  LONGS_EQUAL(100, ruleset.numRules());
  rule = (DummyRule *) ruleset.get(49);
  LONGS_EQUAL(49, rule->getId());
}

// tests default constructor
TEST( GarpRuleSet_Constructor , GarpRuleSet )
{
  GarpRuleSet ruleset(10);
  LONGS_EQUAL(10, ruleset.size());
  LONGS_EQUAL(0, ruleset.numRules());
}

// test addition and deletion of rules from ruleset
TEST( GarpRuleSet_add , Garp)
{
  DummyRule * r0, * r1, * r2, * r3;

  r0 = new DummyRule(0);
  r1 = new DummyRule(1);
  r2 = new DummyRule(2);
  r3 = new DummyRule(3);

  GarpRuleSet ruleset(3);

  // doesn't fail until r4 is added, then it runs out of space
  LONGS_EQUAL(1, ruleset.add(r0));
  LONGS_EQUAL(2, ruleset.add(r1));
  LONGS_EQUAL(3, ruleset.add(r2));
  LONGS_EQUAL(0, ruleset.add(r3));

  // test if rules inserted remain in the same order
  // rules in ruleset is 0-based
  LONGS_EQUAL(0, ((DummyRule *) (ruleset.get(0)))->getId());
  LONGS_EQUAL(1, ((DummyRule *) (ruleset.get(1)))->getId());
  LONGS_EQUAL(2, ((DummyRule *) (ruleset.get(2)))->getId());
}

// test method trim
TEST( GarpRuleSet_trim , GarpRuleSet )
{
  DummyRule * rule;
  GarpRuleSet ruleset(100);

  testHelperAddRules(&ruleset, 0, 99);

  // keep first 50 rules (0-49) and delete the rest
  ruleset.trim(50);

  // check if num of rules is correct
  LONGS_EQUAL(50, ruleset.numRules());

  // now check if last rule is id 49
  LONGS_EQUAL(49, ((DummyRule *) (ruleset.get(49)))->getId());

  // check if rule 50 has been deleted
  rule = (DummyRule *) ruleset.get(50); CHECK(rule == NULL);
  rule = (DummyRule *) ruleset.get(75); CHECK(rule == NULL);
  rule = (DummyRule *) ruleset.get(99); CHECK(rule == NULL);
}

// test method clear
TEST( GarpRuleSet_clear , GarpRuleSet )
{
  DummyRule * rule;
  GarpRuleSet ruleset(100);

  testHelperAddRules(&ruleset, 0, 99);

  // test method clear()
  ruleset.clear();

  // check if num of rules is correct
  LONGS_EQUAL(0, ruleset.numRules());

  // check if rules have been deleted
  rule = (DummyRule *) ruleset.get(0); CHECK(rule == NULL);
  rule = (DummyRule *) ruleset.get(49); CHECK(rule == NULL);
  rule = (DummyRule *) ruleset.get(50); CHECK(rule == NULL);
  rule = (DummyRule *) ruleset.get(75); CHECK(rule == NULL);
  rule = (DummyRule *) ruleset.get(99); CHECK(rule == NULL);
}

// test method filter
TEST( GarpRuleSet_filter , GarpRuleSet )
{
  DummyRule * rule;
  GarpRuleSet ruleset(100);

  testHelperAddRules(&ruleset, 0, 99);

  // change some performance values. other (uninitialized) values should be 0.0
  rule = (DummyRule *) (ruleset.get(10)); rule->setPerformance(PerfSig, 100.0);
  rule = (DummyRule *) (ruleset.get(20)); rule->setPerformance(PerfSig, 100.0);
  rule = (DummyRule *) (ruleset.get(25)); rule->setPerformance(PerfSig, 100.0);
  rule = (DummyRule *) (ruleset.get(37)); rule->setPerformance(PerfSig, 100.0);
  rule = (DummyRule *) (ruleset.get(77)); rule->setPerformance(PerfSig, 100.0);

  // this one will be filtered out too
  rule = (DummyRule *) (ruleset.get(30)); rule->setPerformance(PerfSig, 35.0);  

  // filters out rules with significance lower than 50.0
  ruleset.filter(PerfSig, 50.0);

  // check how many rules were left in ruleset
  LONGS_EQUAL(5, ruleset.numRules());

  // check what rules remained
  // note that order of rules is important
  rule = (DummyRule *) (ruleset.get(0)); LONGS_EQUAL(10, rule->getId()); 
  rule = (DummyRule *) (ruleset.get(1)); LONGS_EQUAL(20, rule->getId()); 
  rule = (DummyRule *) (ruleset.get(2)); LONGS_EQUAL(25, rule->getId()); 
  rule = (DummyRule *) (ruleset.get(3)); LONGS_EQUAL(37, rule->getId()); 
  rule = (DummyRule *) (ruleset.get(4)); LONGS_EQUAL(77, rule->getId());
  rule = (DummyRule *) (ruleset.get(5)); CHECK(rule == NULL);
}

// test method replace
TEST( GarpRuleSet_replace , GarpRuleSet )
{
  DummyRule * rule;
  GarpRuleSet ruleset(100);

  testHelperAddRules(&ruleset, 0, 49);

  // create brand new rule 
  rule = new DummyRule(1250);

  // try to replace something that is out of bounds
  LONGS_EQUAL(0, ruleset.replace(-1, rule)); // rule is outside of range
  
  LONGS_EQUAL(0, ruleset.replace(75, rule));  // there is no rule at index 75
  // 
  LONGS_EQUAL(0, ruleset.replace(25, NULL));  // rule is NULL

  // finally replace rule correctly
  LONGS_EQUAL(1, ruleset.replace(25, rule));

  // check if it is really there
  rule = (DummyRule *) (ruleset.get(25)); 
  LONGS_EQUAL(1250, rule->getId());
}

// test performanceSummary method
TEST ( GarpRuleSet_performanceSummary , GarpRuleSet )
{
  double best, worst, avg;
  DummyRule * rule;
  GarpRuleSet ruleset(100);

  testHelperAddRules(&ruleset, 0, 99);

  // change some performance values. other (uninitialized) values should be 0.0
  rule = (DummyRule *) (ruleset.get(10)); rule->setPerformance(PerfSig, -100.0);
  rule = (DummyRule *) (ruleset.get(20)); rule->setPerformance(PerfSig, 50.0);
  rule = (DummyRule *) (ruleset.get(25)); rule->setPerformance(PerfSig, 100.0);
  rule = (DummyRule *) (ruleset.get(37)); rule->setPerformance(PerfSig, 50.0);
  rule = (DummyRule *) (ruleset.get(77)); rule->setPerformance(PerfSig, 180.0);

  ruleset.performanceSummary(PerfSig, &best, &worst, &avg);
  DOUBLES_EQUAL(2.80, avg, 0.0);
  DOUBLES_EQUAL(-100.0, worst, 0.0);
  DOUBLES_EQUAL(180.0, best, 0.0);
}

/*
// TODO: add test for method findSimilar()
TEST ( GarpRuleSet_findSimilar , GarpRuleSet )
{
  FAIL("Test for method findSimilar() not implemented yet");
}

TEST ( DummyTest3 , dummy )
{
  FAIL("At least not while you are playing around with the test framework!!");
}

TEST ( DummyTest2 , dummy )
{
  FAIL("Running a set of tests and not having a failure is not cool.");
}

TEST ( DummyTest1 , dummy )
{
  FAIL("This is just a test for the testing harness.");
}
*/

// ==============================================================
// ====================== GarpRule hierarchy tests ==============

// TODO: implement tests for all GarpRule classes
