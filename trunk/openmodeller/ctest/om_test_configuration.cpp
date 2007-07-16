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
#include "om_test_configuration.h"

static test_Configuration suite_test_Configuration;

static CxxTest::List Tests_test_Configuration = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_Configuration( "om_test_configuration.h", 42, "test_Configuration", suite_test_Configuration, Tests_test_Configuration );

static class TestDescription_test_Configuration_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_Configuration_test1() : CxxTest::RealTestDescription( Tests_test_Configuration, suiteDescription_test_Configuration, 56, "test1" ) {}
 void runTest() { suite_test_Configuration.test1(); }
} testDescription_test_Configuration_test1;

static class TestDescription_test_Configuration_test2 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_Configuration_test2() : CxxTest::RealTestDescription( Tests_test_Configuration, suiteDescription_test_Configuration, 67, "test2" ) {}
 void runTest() { suite_test_Configuration.test2(); }
} testDescription_test_Configuration_test2;

static class TestDescription_test_Configuration_test3 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_Configuration_test3() : CxxTest::RealTestDescription( Tests_test_Configuration, suiteDescription_test_Configuration, 75, "test3" ) {}
 void runTest() { suite_test_Configuration.test3(); }
} testDescription_test_Configuration_test3;

#include <cxxtest/Root.cpp>
