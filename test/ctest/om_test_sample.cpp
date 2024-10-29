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
    CxxTest::RealWorldDescription::_worldName = "test_sample";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_test_Sample_init = false;
#include "om_test_sample.h"

static test_Sample suite_test_Sample;

static CxxTest::List Tests_test_Sample = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_Sample( "om_test_sample.h", 42, "test_Sample", suite_test_Sample, Tests_test_Sample );

static class TestDescription_suite_test_Sample_testDefaultConstructor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testDefaultConstructor() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 64, "testDefaultConstructor" ) {}
 void runTest() { suite_test_Sample.testDefaultConstructor(); }
} testDescription_suite_test_Sample_testDefaultConstructor;

static class TestDescription_suite_test_Sample_testResizeFunction : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testResizeFunction() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 74, "testResizeFunction" ) {}
 void runTest() { suite_test_Sample.testResizeFunction(); }
} testDescription_suite_test_Sample_testResizeFunction;

static class TestDescription_suite_test_Sample_testConstructorWithSingleScalar : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testConstructorWithSingleScalar() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 85, "testConstructorWithSingleScalar" ) {}
 void runTest() { suite_test_Sample.testConstructorWithSingleScalar(); }
} testDescription_suite_test_Sample_testConstructorWithSingleScalar;

static class TestDescription_suite_test_Sample_testDirectAssignmentOperator_1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testDirectAssignmentOperator_1() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 97, "testDirectAssignmentOperator_1" ) {}
 void runTest() { suite_test_Sample.testDirectAssignmentOperator_1(); }
} testDescription_suite_test_Sample_testDirectAssignmentOperator_1;

static class TestDescription_suite_test_Sample_testEqualsFunctionAndEqualToOperator : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testEqualsFunctionAndEqualToOperator() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 113, "testEqualsFunctionAndEqualToOperator" ) {}
 void runTest() { suite_test_Sample.testEqualsFunctionAndEqualToOperator(); }
} testDescription_suite_test_Sample_testEqualsFunctionAndEqualToOperator;

static class TestDescription_suite_test_Sample_testCopyConstructor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testCopyConstructor() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 143, "testCopyConstructor" ) {}
 void runTest() { suite_test_Sample.testCopyConstructor(); }
} testDescription_suite_test_Sample_testCopyConstructor;

static class TestDescription_suite_test_Sample_testDirectAssignmentOperator_2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testDirectAssignmentOperator_2() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 158, "testDirectAssignmentOperator_2" ) {}
 void runTest() { suite_test_Sample.testDirectAssignmentOperator_2(); }
} testDescription_suite_test_Sample_testDirectAssignmentOperator_2;

static class TestDescription_suite_test_Sample_testBitwiseOperators : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testBitwiseOperators() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 173, "testBitwiseOperators" ) {}
 void runTest() { suite_test_Sample.testBitwiseOperators(); }
} testDescription_suite_test_Sample_testBitwiseOperators;

static class TestDescription_suite_test_Sample_testBitwiseOperatorsPrecision : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testBitwiseOperatorsPrecision() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 192, "testBitwiseOperatorsPrecision" ) {}
 void runTest() { suite_test_Sample.testBitwiseOperatorsPrecision(); }
} testDescription_suite_test_Sample_testBitwiseOperatorsPrecision;

static class TestDescription_suite_test_Sample_testOperatorAssignmentBySum_1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testOperatorAssignmentBySum_1() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 211, "testOperatorAssignmentBySum_1" ) {}
 void runTest() { suite_test_Sample.testOperatorAssignmentBySum_1(); }
} testDescription_suite_test_Sample_testOperatorAssignmentBySum_1;

static class TestDescription_suite_test_Sample_testOperatorAssignmentBySum_2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testOperatorAssignmentBySum_2() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 229, "testOperatorAssignmentBySum_2" ) {}
 void runTest() { suite_test_Sample.testOperatorAssignmentBySum_2(); }
} testDescription_suite_test_Sample_testOperatorAssignmentBySum_2;

static class TestDescription_suite_test_Sample_testOperatorAssignmentByDifference_1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testOperatorAssignmentByDifference_1() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 247, "testOperatorAssignmentByDifference_1" ) {}
 void runTest() { suite_test_Sample.testOperatorAssignmentByDifference_1(); }
} testDescription_suite_test_Sample_testOperatorAssignmentByDifference_1;

static class TestDescription_suite_test_Sample_testOperatorAssignmentByDifference_2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testOperatorAssignmentByDifference_2() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 265, "testOperatorAssignmentByDifference_2" ) {}
 void runTest() { suite_test_Sample.testOperatorAssignmentByDifference_2(); }
} testDescription_suite_test_Sample_testOperatorAssignmentByDifference_2;

static class TestDescription_suite_test_Sample_testOperatorAssignmentByProduct_1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testOperatorAssignmentByProduct_1() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 283, "testOperatorAssignmentByProduct_1" ) {}
 void runTest() { suite_test_Sample.testOperatorAssignmentByProduct_1(); }
} testDescription_suite_test_Sample_testOperatorAssignmentByProduct_1;

static class TestDescription_suite_test_Sample_testOperatorAssignmentByProduct_2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testOperatorAssignmentByProduct_2() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 301, "testOperatorAssignmentByProduct_2" ) {}
 void runTest() { suite_test_Sample.testOperatorAssignmentByProduct_2(); }
} testDescription_suite_test_Sample_testOperatorAssignmentByProduct_2;

static class TestDescription_suite_test_Sample_testOperatorAssignmentByDividend_1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testOperatorAssignmentByDividend_1() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 319, "testOperatorAssignmentByDividend_1" ) {}
 void runTest() { suite_test_Sample.testOperatorAssignmentByDividend_1(); }
} testDescription_suite_test_Sample_testOperatorAssignmentByDividend_1;

static class TestDescription_suite_test_Sample_testOperatorAssignmentByDividend_2 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testOperatorAssignmentByDividend_2() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 339, "testOperatorAssignmentByDividend_2" ) {}
 void runTest() { suite_test_Sample.testOperatorAssignmentByDividend_2(); }
} testDescription_suite_test_Sample_testOperatorAssignmentByDividend_2;

static class TestDescription_suite_test_Sample_testOperatorAssignmentByBitwiseAND : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testOperatorAssignmentByBitwiseAND() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 356, "testOperatorAssignmentByBitwiseAND" ) {}
 void runTest() { suite_test_Sample.testOperatorAssignmentByBitwiseAND(); }
} testDescription_suite_test_Sample_testOperatorAssignmentByBitwiseAND;

static class TestDescription_suite_test_Sample_testOperatorAssignmentByBitwiseOR : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testOperatorAssignmentByBitwiseOR() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 378, "testOperatorAssignmentByBitwiseOR" ) {}
 void runTest() { suite_test_Sample.testOperatorAssignmentByBitwiseOR(); }
} testDescription_suite_test_Sample_testOperatorAssignmentByBitwiseOR;

static class TestDescription_suite_test_Sample_testSqrFunction : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testSqrFunction() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 399, "testSqrFunction" ) {}
 void runTest() { suite_test_Sample.testSqrFunction(); }
} testDescription_suite_test_Sample_testSqrFunction;

static class TestDescription_suite_test_Sample_testSqrtFunction : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testSqrtFunction() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 416, "testSqrtFunction" ) {}
 void runTest() { suite_test_Sample.testSqrtFunction(); }
} testDescription_suite_test_Sample_testSqrtFunction;

static class TestDescription_suite_test_Sample_testNormFunction : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testNormFunction() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 433, "testNormFunction" ) {}
 void runTest() { suite_test_Sample.testNormFunction(); }
} testDescription_suite_test_Sample_testNormFunction;

static class TestDescription_suite_test_Sample_testdotProductFunction : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_Sample_testdotProductFunction() : CxxTest::RealTestDescription( Tests_test_Sample, suiteDescription_test_Sample, 447, "testdotProductFunction" ) {}
 void runTest() { suite_test_Sample.testdotProductFunction(); }
} testDescription_suite_test_Sample_testdotProductFunction;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
