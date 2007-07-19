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

static class TestDescription_test_refcount_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_refcount_test1() : CxxTest::RealTestDescription( Tests_test_refcount, suiteDescription_test_refcount, 53, "test1" ) {}
 void runTest() { suite_test_refcount.test1(); }
} testDescription_test_refcount_test1;

static class TestDescription_test_refcount_test2 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_refcount_test2() : CxxTest::RealTestDescription( Tests_test_refcount, suiteDescription_test_refcount, 59, "test2" ) {}
 void runTest() { suite_test_refcount.test2(); }
} testDescription_test_refcount_test2;

static class TestDescription_test_refcount_test3 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_refcount_test3() : CxxTest::RealTestDescription( Tests_test_refcount, suiteDescription_test_refcount, 65, "test3" ) {}
 void runTest() { suite_test_refcount.test3(); }
} testDescription_test_refcount_test3;

#include <cxxtest/Root.cpp>
