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
#include "om_test_mapformat.h"

static test_MapFormat suite_test_MapFormat;

static CxxTest::List Tests_test_MapFormat = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_MapFormat( "om_test_mapformat.h", 41, "test_MapFormat", suite_test_MapFormat, Tests_test_MapFormat );

static class TestDescription_test_MapFormat_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_MapFormat_test1() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 54, "test1" ) {}
 void runTest() { suite_test_MapFormat.test1(); }
} testDescription_test_MapFormat_test1;

#include <cxxtest/Root.cpp>
