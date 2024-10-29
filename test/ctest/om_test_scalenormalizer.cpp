/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/ErrorPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    CxxTest::ErrorPrinter tmp;
    CxxTest::RealWorldDescription::_worldName = "test_scalenormalizer";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_test_ScaleNormalizer_init = false;
#include "om_test_scalenormalizer.h"

static test_ScaleNormalizer suite_test_ScaleNormalizer;

static CxxTest::List Tests_test_ScaleNormalizer = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_ScaleNormalizer( "om_test_scalenormalizer.h", 49, "test_ScaleNormalizer", suite_test_ScaleNormalizer, Tests_test_ScaleNormalizer );

static class TestDescription_suite_test_ScaleNormalizer_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_ScaleNormalizer_test1() : CxxTest::RealTestDescription( Tests_test_ScaleNormalizer, suiteDescription_test_ScaleNormalizer, 141, "test1" ) {}
 void runTest() { suite_test_ScaleNormalizer.test1(); }
} testDescription_suite_test_ScaleNormalizer_test1;

static class TestDescription_suite_test_ScaleNormalizer_test2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_ScaleNormalizer_test2() : CxxTest::RealTestDescription( Tests_test_ScaleNormalizer, suiteDescription_test_ScaleNormalizer, 182, "test2" ) {}
 void runTest() { suite_test_ScaleNormalizer.test2(); }
} testDescription_suite_test_ScaleNormalizer_test2;

static class TestDescription_suite_test_ScaleNormalizer_test3 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_ScaleNormalizer_test3() : CxxTest::RealTestDescription( Tests_test_ScaleNormalizer, suiteDescription_test_ScaleNormalizer, 227, "test3" ) {}
 void runTest() { suite_test_ScaleNormalizer.test3(); }
} testDescription_suite_test_ScaleNormalizer_test3;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
