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
#include "om_test_exceptions.h"

static test_Exceptions suite_test_Exceptions;

static CxxTest::List Tests_test_Exceptions = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_Exceptions( "om_test_exceptions.h", 42, "test_Exceptions", suite_test_Exceptions, Tests_test_Exceptions );

static class TestDescription_test_Exceptions_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_Exceptions_test1() : CxxTest::RealTestDescription( Tests_test_Exceptions, suiteDescription_test_Exceptions, 54, "test1" ) {}
 void runTest() { suite_test_Exceptions.test1(); }
} testDescription_test_Exceptions_test1;

#include <cxxtest/Root.cpp>
