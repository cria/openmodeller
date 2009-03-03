/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_HAVE_EH
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/ErrorPrinter.h>

int main() {
 return CxxTest::ErrorPrinter().run();
}
#include "pre_test_chisquare.hh"

static test_ChiSquare suite_test_ChiSquare;

static CxxTest::List Tests_test_ChiSquare = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_ChiSquare( "pre_test_chisquare.hh", 55, "test_ChiSquare", suite_test_ChiSquare, Tests_test_ChiSquare );

static class TestDescription_test_ChiSquare_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_ChiSquare_test1() : CxxTest::RealTestDescription( Tests_test_ChiSquare, suiteDescription_test_ChiSquare, 65, "test1" ) {}
 void runTest() { suite_test_ChiSquare.test1(); }
} testDescription_test_ChiSquare_test1;

#include <cxxtest/Root.cpp>
