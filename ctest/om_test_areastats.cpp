/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/ErrorPrinter.h>

int main() {
 return CxxTest::ErrorPrinter().run();
}
#include "om_test_areastats.h"

static test_AreaStats suite_test_AreaStats;

static CxxTest::List Tests_test_AreaStats = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_AreaStats( "om_test_areastats.h", 38, "test_AreaStats", suite_test_AreaStats, Tests_test_AreaStats );

static class TestDescription_test_AreaStats_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_AreaStats_test1() : CxxTest::RealTestDescription( Tests_test_AreaStats, suiteDescription_test_AreaStats, 51, "test1" ) {}
 void runTest() { suite_test_AreaStats.test1(); }
} testDescription_test_AreaStats_test1;

#include <cxxtest/Root.cpp>
