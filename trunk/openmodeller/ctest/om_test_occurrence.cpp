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
#include "om_test_occurrence.h"

static test_Occurrence suite_test_Occurrence;

static CxxTest::List Tests_test_Occurrence = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_Occurrence( "om_test_occurrence.h", 40, "test_Occurrence", suite_test_Occurrence, Tests_test_Occurrence );

static class TestDescription_test_Occurrence_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_Occurrence_test1() : CxxTest::RealTestDescription( Tests_test_Occurrence, suiteDescription_test_Occurrence, 57, "test1" ) {}
 void runTest() { suite_test_Occurrence.test1(); }
} testDescription_test_Occurrence_test1;

static class TestDescription_test_Occurrence_test2 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_Occurrence_test2() : CxxTest::RealTestDescription( Tests_test_Occurrence, suiteDescription_test_Occurrence, 69, "test2" ) {}
 void runTest() { suite_test_Occurrence.test2(); }
} testDescription_test_Occurrence_test2;

static class TestDescription_test_Occurrence_test3 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_Occurrence_test3() : CxxTest::RealTestDescription( Tests_test_Occurrence, suiteDescription_test_Occurrence, 81, "test3" ) {}
 void runTest() { suite_test_Occurrence.test3(); }
} testDescription_test_Occurrence_test3;

static class TestDescription_test_Occurrence_test4 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_Occurrence_test4() : CxxTest::RealTestDescription( Tests_test_Occurrence, suiteDescription_test_Occurrence, 86, "test4" ) {}
 void runTest() { suite_test_Occurrence.test4(); }
} testDescription_test_Occurrence_test4;

static class TestDescription_test_Occurrence_test5 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_Occurrence_test5() : CxxTest::RealTestDescription( Tests_test_Occurrence, suiteDescription_test_Occurrence, 91, "test5" ) {}
 void runTest() { suite_test_Occurrence.test5(); }
} testDescription_test_Occurrence_test5;

static class TestDescription_test_Occurrence_test6 : public CxxTest::RealTestDescription {
public:
 TestDescription_test_Occurrence_test6() : CxxTest::RealTestDescription( Tests_test_Occurrence, suiteDescription_test_Occurrence, 96, "test6" ) {}
 void runTest() { suite_test_Occurrence.test6(); }
} testDescription_test_Occurrence_test6;

#include <cxxtest/Root.cpp>
