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
#include "om_test_sampleexpr.h"

static test_sampleexpr suite_test_sampleexpr;

static CxxTest::List Tests_test_sampleexpr = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_sampleexpr( "om_test_sampleexpr.h", 43, "test_sampleexpr", suite_test_sampleexpr, Tests_test_sampleexpr );

static class TestDescription_test_sampleexpr_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_sampleexpr_test1() : CxxTest::RealTestDescription( Tests_test_sampleexpr, suiteDescription_test_sampleexpr, 62, "test1" ) {}
 void runTest() { suite_test_sampleexpr.test1(); }
} testDescription_test_sampleexpr_test1;

static class TestDescription_test_sampleexpr_test2 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_sampleexpr_test2() : CxxTest::RealTestDescription( Tests_test_sampleexpr, suiteDescription_test_sampleexpr, 70, "test2" ) {}
 void runTest() { suite_test_sampleexpr.test2(); }
} testDescription_test_sampleexpr_test2;

static class TestDescription_test_sampleexpr_test3 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_sampleexpr_test3() : CxxTest::RealTestDescription( Tests_test_sampleexpr, suiteDescription_test_sampleexpr, 78, "test3" ) {}
 void runTest() { suite_test_sampleexpr.test3(); }
} testDescription_test_sampleexpr_test3;

#include <cxxtest/Root.cpp>
