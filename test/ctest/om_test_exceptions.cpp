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
    CxxTest::RealWorldDescription::_worldName = "test_exceptions";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_test_Exceptions_init = false;
#include "om_test_exceptions.h"

static test_Exceptions suite_test_Exceptions;

static CxxTest::List Tests_test_Exceptions = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_Exceptions( "om_test_exceptions.h", 42, "test_Exceptions", suite_test_Exceptions, Tests_test_Exceptions );

static class TestDescription_suite_test_Exceptions_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Exceptions_test1() : CxxTest::RealTestDescription( Tests_test_Exceptions, suiteDescription_test_Exceptions, 54, "test1" ) {}
 void runTest() { suite_test_Exceptions.test1(); }
} testDescription_suite_test_Exceptions_test1;

static class TestDescription_suite_test_Exceptions_test2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Exceptions_test2() : CxxTest::RealTestDescription( Tests_test_Exceptions, suiteDescription_test_Exceptions, 61, "test2" ) {}
 void runTest() { suite_test_Exceptions.test2(); }
} testDescription_suite_test_Exceptions_test2;

static class TestDescription_suite_test_Exceptions_test3 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Exceptions_test3() : CxxTest::RealTestDescription( Tests_test_Exceptions, suiteDescription_test_Exceptions, 68, "test3" ) {}
 void runTest() { suite_test_Exceptions.test3(); }
} testDescription_suite_test_Exceptions_test3;

static class TestDescription_suite_test_Exceptions_test4 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Exceptions_test4() : CxxTest::RealTestDescription( Tests_test_Exceptions, suiteDescription_test_Exceptions, 77, "test4" ) {}
 void runTest() { suite_test_Exceptions.test4(); }
} testDescription_suite_test_Exceptions_test4;

static class TestDescription_suite_test_Exceptions_test5 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Exceptions_test5() : CxxTest::RealTestDescription( Tests_test_Exceptions, suiteDescription_test_Exceptions, 86, "test5" ) {}
 void runTest() { suite_test_Exceptions.test5(); }
} testDescription_suite_test_Exceptions_test5;

static class TestDescription_suite_test_Exceptions_test6 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Exceptions_test6() : CxxTest::RealTestDescription( Tests_test_Exceptions, suiteDescription_test_Exceptions, 93, "test6" ) {}
 void runTest() { suite_test_Exceptions.test6(); }
} testDescription_suite_test_Exceptions_test6;

static class TestDescription_suite_test_Exceptions_test7 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Exceptions_test7() : CxxTest::RealTestDescription( Tests_test_Exceptions, suiteDescription_test_Exceptions, 100, "test7" ) {}
 void runTest() { suite_test_Exceptions.test7(); }
} testDescription_suite_test_Exceptions_test7;

static class TestDescription_suite_test_Exceptions_test8 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Exceptions_test8() : CxxTest::RealTestDescription( Tests_test_Exceptions, suiteDescription_test_Exceptions, 109, "test8" ) {}
 void runTest() { suite_test_Exceptions.test8(); }
} testDescription_suite_test_Exceptions_test8;

static class TestDescription_suite_test_Exceptions_test9 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Exceptions_test9() : CxxTest::RealTestDescription( Tests_test_Exceptions, suiteDescription_test_Exceptions, 116, "test9" ) {}
 void runTest() { suite_test_Exceptions.test9(); }
} testDescription_suite_test_Exceptions_test9;

static class TestDescription_suite_test_Exceptions_test10 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Exceptions_test10() : CxxTest::RealTestDescription( Tests_test_Exceptions, suiteDescription_test_Exceptions, 123, "test10" ) {}
 void runTest() { suite_test_Exceptions.test10(); }
} testDescription_suite_test_Exceptions_test10;

static class TestDescription_suite_test_Exceptions_test11 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Exceptions_test11() : CxxTest::RealTestDescription( Tests_test_Exceptions, suiteDescription_test_Exceptions, 130, "test11" ) {}
 void runTest() { suite_test_Exceptions.test11(); }
} testDescription_suite_test_Exceptions_test11;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
