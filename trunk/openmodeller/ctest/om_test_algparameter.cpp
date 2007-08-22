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
#include "om_test_algparameter.h"

static test_AlgParameter suite_test_AlgParameter;

static CxxTest::List Tests_test_AlgParameter = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_AlgParameter( "om_test_algparameter.h", 37, "test_AlgParameter", suite_test_AlgParameter, Tests_test_AlgParameter );

static class TestDescription_test_AlgParameter_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_AlgParameter_test1() : CxxTest::RealTestDescription( Tests_test_AlgParameter, suiteDescription_test_AlgParameter, 49, "test1" ) {}
 void runTest() { suite_test_AlgParameter.test1(); }
} testDescription_test_AlgParameter_test1;

#include <cxxtest/Root.cpp>
