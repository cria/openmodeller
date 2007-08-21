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
#include "om_test_sampleexprvar.h"

static test_SampleExprVar suite_test_SampleExprVar;

static CxxTest::List Tests_test_SampleExprVar = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_SampleExprVar( "om_test_sampleexprvar.h", 38, "test_SampleExprVar", suite_test_SampleExprVar, Tests_test_SampleExprVar );

static class TestDescription_test_SampleExprVar_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_SampleExprVar_test1() : CxxTest::RealTestDescription( Tests_test_SampleExprVar, suiteDescription_test_SampleExprVar, 48, "test1" ) {}
 void runTest() { suite_test_SampleExprVar.test1(); }
} testDescription_test_SampleExprVar_test1;

#include <cxxtest/Root.cpp>
