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
    CxxTest::RealWorldDescription::_worldName = "test_sampleexprvar";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_test_SampleExprVar_init = false;
#include "om_test_sampleexprvar.h"

static test_SampleExprVar suite_test_SampleExprVar;

static CxxTest::List Tests_test_SampleExprVar = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_SampleExprVar( "om_test_sampleexprvar.h", 40, "test_SampleExprVar", suite_test_SampleExprVar, Tests_test_SampleExprVar );

static class TestDescription_suite_test_SampleExprVar_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_SampleExprVar_test1() : CxxTest::RealTestDescription( Tests_test_SampleExprVar, suiteDescription_test_SampleExprVar, 61, "test1" ) {}
 void runTest() { suite_test_SampleExprVar.test1(); }
} testDescription_suite_test_SampleExprVar_test1;

static class TestDescription_suite_test_SampleExprVar_test2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_SampleExprVar_test2() : CxxTest::RealTestDescription( Tests_test_SampleExprVar, suiteDescription_test_SampleExprVar, 71, "test2" ) {}
 void runTest() { suite_test_SampleExprVar.test2(); }
} testDescription_suite_test_SampleExprVar_test2;

static class TestDescription_suite_test_SampleExprVar_test3 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_SampleExprVar_test3() : CxxTest::RealTestDescription( Tests_test_SampleExprVar, suiteDescription_test_SampleExprVar, 81, "test3" ) {}
 void runTest() { suite_test_SampleExprVar.test3(); }
} testDescription_suite_test_SampleExprVar_test3;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
