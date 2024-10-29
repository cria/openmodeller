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
    CxxTest::RealWorldDescription::_worldName = "test_icstring";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_test_icstring_init = false;
#include "om_test_icstring.h"

static test_icstring suite_test_icstring;

static CxxTest::List Tests_test_icstring = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_icstring( "om_test_icstring.h", 39, "test_icstring", suite_test_icstring, Tests_test_icstring );

static class TestDescription_suite_test_icstring_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_icstring_test1() : CxxTest::RealTestDescription( Tests_test_icstring, suiteDescription_test_icstring, 57, "test1" ) {}
 void runTest() { suite_test_icstring.test1(); }
} testDescription_suite_test_icstring_test1;

static class TestDescription_suite_test_icstring_test2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_icstring_test2() : CxxTest::RealTestDescription( Tests_test_icstring, suiteDescription_test_icstring, 63, "test2" ) {}
 void runTest() { suite_test_icstring.test2(); }
} testDescription_suite_test_icstring_test2;

static class TestDescription_suite_test_icstring_test3 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_icstring_test3() : CxxTest::RealTestDescription( Tests_test_icstring, suiteDescription_test_icstring, 69, "test3" ) {}
 void runTest() { suite_test_icstring.test3(); }
} testDescription_suite_test_icstring_test3;

static class TestDescription_suite_test_icstring_test4 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_icstring_test4() : CxxTest::RealTestDescription( Tests_test_icstring, suiteDescription_test_icstring, 75, "test4" ) {}
 void runTest() { suite_test_icstring.test4(); }
} testDescription_suite_test_icstring_test4;

static class TestDescription_suite_test_icstring_test5 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_icstring_test5() : CxxTest::RealTestDescription( Tests_test_icstring, suiteDescription_test_icstring, 82, "test5" ) {}
 void runTest() { suite_test_icstring.test5(); }
} testDescription_suite_test_icstring_test5;

static class TestDescription_suite_test_icstring_test6 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_icstring_test6() : CxxTest::RealTestDescription( Tests_test_icstring, suiteDescription_test_icstring, 89, "test6" ) {}
 void runTest() { suite_test_icstring.test6(); }
} testDescription_suite_test_icstring_test6;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
