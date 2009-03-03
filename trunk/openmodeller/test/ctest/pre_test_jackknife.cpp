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
#include "pre_test_jackknife.hh"

static test_Jackknife suite_test_Jackknife;

static CxxTest::List Tests_test_Jackknife = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_Jackknife( "pre_test_jackknife.hh", 51, "test_Jackknife", suite_test_Jackknife, Tests_test_Jackknife );

static class TestDescription_test_Jackknife_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_Jackknife_test1() : CxxTest::RealTestDescription( Tests_test_Jackknife, suiteDescription_test_Jackknife, 61, "test1" ) {}
 void runTest() { suite_test_Jackknife.test1(); }
} testDescription_test_Jackknife_test1;

#include <cxxtest/Root.cpp>
