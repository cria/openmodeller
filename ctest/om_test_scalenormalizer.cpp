/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_HAVE_EH
#define _CXXTEST_ABORT_TEST_ON_FAIL
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/ErrorPrinter.h>

int main() {
 return CxxTest::ErrorPrinter().run();
}
#include "om_test_scalenormalizer.h"

static test_ScaleNormalizer suite_test_ScaleNormalizer;

static CxxTest::List Tests_test_ScaleNormalizer = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_ScaleNormalizer( "om_test_scalenormalizer.h", 45, "test_ScaleNormalizer", suite_test_ScaleNormalizer, Tests_test_ScaleNormalizer );

static class TestDescription_test_ScaleNormalizer_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_ScaleNormalizer_test1() : CxxTest::RealTestDescription( Tests_test_ScaleNormalizer, suiteDescription_test_ScaleNormalizer, 113, "test1" ) {}
 void runTest() { suite_test_ScaleNormalizer.test1(); }
} testDescription_test_ScaleNormalizer_test1;

#include <cxxtest/Root.cpp>
