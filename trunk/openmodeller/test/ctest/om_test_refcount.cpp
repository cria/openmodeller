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
#include "om_test_refcount.h"

static test_refcount suite_test_refcount;

static CxxTest::List Tests_test_refcount = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_refcount( "om_test_refcount.h", 39, "test_refcount", suite_test_refcount, Tests_test_refcount );

static class TestDescription_test_refcount_testOperator_1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_refcount_testOperator_1() : CxxTest::RealTestDescription( Tests_test_refcount, suiteDescription_test_refcount, 61, "testOperator_1" ) {}
 void runTest() { suite_test_refcount.testOperator_1(); }
} testDescription_test_refcount_testOperator_1;

static class TestDescription_test_refcount_testOperator_2 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_refcount_testOperator_2() : CxxTest::RealTestDescription( Tests_test_refcount, suiteDescription_test_refcount, 73, "testOperator_2" ) {}
 void runTest() { suite_test_refcount.testOperator_2(); }
} testDescription_test_refcount_testOperator_2;

static class TestDescription_test_refcount_testBoolFunction : public CxxTest::RealTestDescription {
public:
 TestDescription_test_refcount_testBoolFunction() : CxxTest::RealTestDescription( Tests_test_refcount, suiteDescription_test_refcount, 85, "testBoolFunction" ) {}
 void runTest() { suite_test_refcount.testBoolFunction(); }
} testDescription_test_refcount_testBoolFunction;

#include <cxxtest/Root.cpp>
