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
#include "om_test_occurrence.h"

static test_Occurrence suite_test_Occurrence;

static CxxTest::List Tests_test_Occurrence = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_Occurrence( "om_test_occurrence.h", 37, "test_Occurrence", suite_test_Occurrence, Tests_test_Occurrence );

static class TestDescription_test_Occurrence_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_Occurrence_test1() : CxxTest::RealTestDescription( Tests_test_Occurrence, suiteDescription_test_Occurrence, 49, "test1" ) {}
 void runTest() { suite_test_Occurrence.test1(); }
} testDescription_test_Occurrence_test1;

#include <cxxtest/Root.cpp>
