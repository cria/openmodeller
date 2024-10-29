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
#include <cxxtest/TestMain.h>
#include <cxxtest/ErrorPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    CxxTest::ErrorPrinter tmp;
    CxxTest::RealWorldDescription::_worldName = "test_chisquare";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_test_ChiSquare_init = false;
#include "pre_test_chisquare.hh"

static test_ChiSquare suite_test_ChiSquare;

static CxxTest::List Tests_test_ChiSquare = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_ChiSquare( "pre_test_chisquare.hh", 55, "test_ChiSquare", suite_test_ChiSquare, Tests_test_ChiSquare );

static class TestDescription_suite_test_ChiSquare_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_ChiSquare_test1() : CxxTest::RealTestDescription( Tests_test_ChiSquare, suiteDescription_test_ChiSquare, 65, "test1" ) {}
 void runTest() { suite_test_ChiSquare.test1(); }
} testDescription_suite_test_ChiSquare_test1;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
