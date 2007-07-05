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
#include "om_test_random.h"

static test_Random suite_test_Random;

static CxxTest::List Tests_test_Random = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_Random( "om_test_random.h", 39, "test_Random", suite_test_Random, Tests_test_Random );

static class TestDescription_test_Random_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_Random_test1() : CxxTest::RealTestDescription( Tests_test_Random, suiteDescription_test_Random, 55, "test1" ) {}
 void runTest() { suite_test_Random.test1(); }
} testDescription_test_Random_test1;

#include <cxxtest/Root.cpp>
