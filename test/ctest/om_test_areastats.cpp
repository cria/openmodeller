/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/ErrorPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    CxxTest::ErrorPrinter tmp;
    CxxTest::RealWorldDescription::_worldName = "test_areastats";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_test_AreaStats_init = false;
#include "om_test_areastats.h"

static test_AreaStats suite_test_AreaStats;

static CxxTest::List Tests_test_AreaStats = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_AreaStats( "om_test_areastats.h", 41, "test_AreaStats", suite_test_AreaStats, Tests_test_AreaStats );

static class TestDescription_suite_test_AreaStats_testAreaStatsConstructor_1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AreaStats_testAreaStatsConstructor_1() : CxxTest::RealTestDescription( Tests_test_AreaStats, suiteDescription_test_AreaStats, 60, "testAreaStatsConstructor_1" ) {}
 void runTest() { suite_test_AreaStats.testAreaStatsConstructor_1(); }
} testDescription_suite_test_AreaStats_testAreaStatsConstructor_1;

static class TestDescription_suite_test_AreaStats_testAreaStatsConstructor_2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AreaStats_testAreaStatsConstructor_2() : CxxTest::RealTestDescription( Tests_test_AreaStats, suiteDescription_test_AreaStats, 74, "testAreaStatsConstructor_2" ) {}
 void runTest() { suite_test_AreaStats.testAreaStatsConstructor_2(); }
} testDescription_suite_test_AreaStats_testAreaStatsConstructor_2;

static class TestDescription_suite_test_AreaStats_testResetFunction : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AreaStats_testResetFunction() : CxxTest::RealTestDescription( Tests_test_AreaStats, suiteDescription_test_AreaStats, 88, "testResetFunction" ) {}
 void runTest() { suite_test_AreaStats.testResetFunction(); }
} testDescription_suite_test_AreaStats_testResetFunction;

static class TestDescription_suite_test_AreaStats_testAddPrefictionFunction_1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AreaStats_testAddPrefictionFunction_1() : CxxTest::RealTestDescription( Tests_test_AreaStats, suiteDescription_test_AreaStats, 103, "testAddPrefictionFunction_1" ) {}
 void runTest() { suite_test_AreaStats.testAddPrefictionFunction_1(); }
} testDescription_suite_test_AreaStats_testAddPrefictionFunction_1;

static class TestDescription_suite_test_AreaStats_testAddPrefictionFunction_2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AreaStats_testAddPrefictionFunction_2() : CxxTest::RealTestDescription( Tests_test_AreaStats, suiteDescription_test_AreaStats, 118, "testAddPrefictionFunction_2" ) {}
 void runTest() { suite_test_AreaStats.testAddPrefictionFunction_2(); }
} testDescription_suite_test_AreaStats_testAddPrefictionFunction_2;

static class TestDescription_suite_test_AreaStats_testAddPrediction_3 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AreaStats_testAddPrediction_3() : CxxTest::RealTestDescription( Tests_test_AreaStats, suiteDescription_test_AreaStats, 133, "testAddPrediction_3" ) {}
 void runTest() { suite_test_AreaStats.testAddPrediction_3(); }
} testDescription_suite_test_AreaStats_testAddPrediction_3;

static class TestDescription_suite_test_AreaStats_testAddNonPrediction : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AreaStats_testAddNonPrediction() : CxxTest::RealTestDescription( Tests_test_AreaStats, suiteDescription_test_AreaStats, 148, "testAddNonPrediction" ) {}
 void runTest() { suite_test_AreaStats.testAddNonPrediction(); }
} testDescription_suite_test_AreaStats_testAddNonPrediction;

static class TestDescription_suite_test_AreaStats_testGetConfiguration : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AreaStats_testGetConfiguration() : CxxTest::RealTestDescription( Tests_test_AreaStats, suiteDescription_test_AreaStats, 163, "testGetConfiguration" ) {}
 void runTest() { suite_test_AreaStats.testGetConfiguration(); }
} testDescription_suite_test_AreaStats_testGetConfiguration;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
