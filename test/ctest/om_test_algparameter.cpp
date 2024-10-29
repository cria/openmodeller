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
    CxxTest::RealWorldDescription::_worldName = "cxxtest";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_test_AlgParameter_init = false;
#include "om_test_algparameter.h"

static test_AlgParameter suite_test_AlgParameter;

static CxxTest::List Tests_test_AlgParameter = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_AlgParameter( "om_test_algparameter.h", 40, "test_AlgParameter", suite_test_AlgParameter, Tests_test_AlgParameter );

static class TestDescription_suite_test_AlgParameter_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AlgParameter_test1() : CxxTest::RealTestDescription( Tests_test_AlgParameter, suiteDescription_test_AlgParameter, 61, "test1" ) {}
 void runTest() { suite_test_AlgParameter.test1(); }
} testDescription_suite_test_AlgParameter_test1;

static class TestDescription_suite_test_AlgParameter_test2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AlgParameter_test2() : CxxTest::RealTestDescription( Tests_test_AlgParameter, suiteDescription_test_AlgParameter, 67, "test2" ) {}
 void runTest() { suite_test_AlgParameter.test2(); }
} testDescription_suite_test_AlgParameter_test2;

static class TestDescription_suite_test_AlgParameter_test3 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AlgParameter_test3() : CxxTest::RealTestDescription( Tests_test_AlgParameter, suiteDescription_test_AlgParameter, 73, "test3" ) {}
 void runTest() { suite_test_AlgParameter.test3(); }
} testDescription_suite_test_AlgParameter_test3;

static class TestDescription_suite_test_AlgParameter_test4 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AlgParameter_test4() : CxxTest::RealTestDescription( Tests_test_AlgParameter, suiteDescription_test_AlgParameter, 79, "test4" ) {}
 void runTest() { suite_test_AlgParameter.test4(); }
} testDescription_suite_test_AlgParameter_test4;

static class TestDescription_suite_test_AlgParameter_test5 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AlgParameter_test5() : CxxTest::RealTestDescription( Tests_test_AlgParameter, suiteDescription_test_AlgParameter, 86, "test5" ) {}
 void runTest() { suite_test_AlgParameter.test5(); }
} testDescription_suite_test_AlgParameter_test5;

static class TestDescription_suite_test_AlgParameter_test6 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AlgParameter_test6() : CxxTest::RealTestDescription( Tests_test_AlgParameter, suiteDescription_test_AlgParameter, 94, "test6" ) {}
 void runTest() { suite_test_AlgParameter.test6(); }
} testDescription_suite_test_AlgParameter_test6;

static class TestDescription_suite_test_AlgParameter_test7 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AlgParameter_test7() : CxxTest::RealTestDescription( Tests_test_AlgParameter, suiteDescription_test_AlgParameter, 102, "test7" ) {}
 void runTest() { suite_test_AlgParameter.test7(); }
} testDescription_suite_test_AlgParameter_test7;

static class TestDescription_suite_test_AlgParameter_test8 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AlgParameter_test8() : CxxTest::RealTestDescription( Tests_test_AlgParameter, suiteDescription_test_AlgParameter, 110, "test8" ) {}
 void runTest() { suite_test_AlgParameter.test8(); }
} testDescription_suite_test_AlgParameter_test8;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
