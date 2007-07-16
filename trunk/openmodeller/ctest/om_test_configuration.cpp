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
CxxTest::StaticSuiteDescription suiteDescription_test_Configuration( "om_test_configuration.h", 39, "test_Configuration", suite_test_Configuration, Tests_test_Configuration );

static class TestDescription_test_Configuration_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_Configuration_test1() : CxxTest::RealTestDescription( Tests_test_Configuration, suiteDescription_test_Configuration, 50, "test1" ) {}
 void runTest() { suite_test_Configuration.test1(); }
} testDescription_test_Configuration_test1;

#include <cxxtest/Root.cpp>
