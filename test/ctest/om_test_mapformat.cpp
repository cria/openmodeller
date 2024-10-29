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
#include <cxxtest/TestMain.h>
#include <cxxtest/ErrorPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    CxxTest::ErrorPrinter tmp;
    CxxTest::RealWorldDescription::_worldName = "test_mapformat";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_test_MapFormat_init = false;
#include "om_test_mapformat.h"

static test_MapFormat suite_test_MapFormat;

static CxxTest::List Tests_test_MapFormat = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_MapFormat( "om_test_mapformat.h", 40, "test_MapFormat", suite_test_MapFormat, Tests_test_MapFormat );

static class TestDescription_suite_test_MapFormat_testDefaultConstructor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testDefaultConstructor() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 63, "testDefaultConstructor" ) {}
 void runTest() { suite_test_MapFormat.testDefaultConstructor(); }
} testDescription_suite_test_MapFormat_testDefaultConstructor;

static class TestDescription_suite_test_MapFormat_testConstructor : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testConstructor() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 162, "testConstructor" ) {}
 void runTest() { suite_test_MapFormat.testConstructor(); }
} testDescription_suite_test_MapFormat_testConstructor;

static class TestDescription_suite_test_MapFormat_testSetFormat : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testSetFormat() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 263, "testSetFormat" ) {}
 void runTest() { suite_test_MapFormat.testSetFormat(); }
} testDescription_suite_test_MapFormat_testSetFormat;

static class TestDescription_suite_test_MapFormat_testSetXCel : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testSetXCel() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 280, "testSetXCel" ) {}
 void runTest() { suite_test_MapFormat.testSetXCel(); }
} testDescription_suite_test_MapFormat_testSetXCel;

static class TestDescription_suite_test_MapFormat_testSetYCel : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testSetYCel() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 301, "testSetYCel" ) {}
 void runTest() { suite_test_MapFormat.testSetYCel(); }
} testDescription_suite_test_MapFormat_testSetYCel;

static class TestDescription_suite_test_MapFormat_testSetXMin : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testSetXMin() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 322, "testSetXMin" ) {}
 void runTest() { suite_test_MapFormat.testSetXMin(); }
} testDescription_suite_test_MapFormat_testSetXMin;

static class TestDescription_suite_test_MapFormat_testSetYMin : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testSetYMin() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 343, "testSetYMin" ) {}
 void runTest() { suite_test_MapFormat.testSetYMin(); }
} testDescription_suite_test_MapFormat_testSetYMin;

static class TestDescription_suite_test_MapFormat_testSetXMax : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testSetXMax() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 364, "testSetXMax" ) {}
 void runTest() { suite_test_MapFormat.testSetXMax(); }
} testDescription_suite_test_MapFormat_testSetXMax;

static class TestDescription_suite_test_MapFormat_testSetYMax : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testSetYMax() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 385, "testSetYMax" ) {}
 void runTest() { suite_test_MapFormat.testSetYMax(); }
} testDescription_suite_test_MapFormat_testSetYMax;

static class TestDescription_suite_test_MapFormat_testSetNoDataValue : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testSetNoDataValue() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 406, "testSetNoDataValue" ) {}
 void runTest() { suite_test_MapFormat.testSetNoDataValue(); }
} testDescription_suite_test_MapFormat_testSetNoDataValue;

static class TestDescription_suite_test_MapFormat_testSetProjection : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testSetProjection() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 427, "testSetProjection" ) {}
 void runTest() { suite_test_MapFormat.testSetProjection(); }
} testDescription_suite_test_MapFormat_testSetProjection;

static class TestDescription_suite_test_MapFormat_testUnSetFormat : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testUnSetFormat() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 448, "testUnSetFormat" ) {}
 void runTest() { suite_test_MapFormat.testUnSetFormat(); }
} testDescription_suite_test_MapFormat_testUnSetFormat;

static class TestDescription_suite_test_MapFormat_testUnSetXCel : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testUnSetXCel() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 459, "testUnSetXCel" ) {}
 void runTest() { suite_test_MapFormat.testUnSetXCel(); }
} testDescription_suite_test_MapFormat_testUnSetXCel;

static class TestDescription_suite_test_MapFormat_testUnSetYCel : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testUnSetYCel() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 479, "testUnSetYCel" ) {}
 void runTest() { suite_test_MapFormat.testUnSetYCel(); }
} testDescription_suite_test_MapFormat_testUnSetYCel;

static class TestDescription_suite_test_MapFormat_testUnSetXMin : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testUnSetXMin() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 499, "testUnSetXMin" ) {}
 void runTest() { suite_test_MapFormat.testUnSetXMin(); }
} testDescription_suite_test_MapFormat_testUnSetXMin;

static class TestDescription_suite_test_MapFormat_testUnSetYMin : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testUnSetYMin() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 519, "testUnSetYMin" ) {}
 void runTest() { suite_test_MapFormat.testUnSetYMin(); }
} testDescription_suite_test_MapFormat_testUnSetYMin;

static class TestDescription_suite_test_MapFormat_testUnSetXMax : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testUnSetXMax() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 539, "testUnSetXMax" ) {}
 void runTest() { suite_test_MapFormat.testUnSetXMax(); }
} testDescription_suite_test_MapFormat_testUnSetXMax;

static class TestDescription_suite_test_MapFormat_testUnSetYMax : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testUnSetYMax() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 559, "testUnSetYMax" ) {}
 void runTest() { suite_test_MapFormat.testUnSetYMax(); }
} testDescription_suite_test_MapFormat_testUnSetYMax;

static class TestDescription_suite_test_MapFormat_testUnSetNoDataValue : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testUnSetNoDataValue() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 579, "testUnSetNoDataValue" ) {}
 void runTest() { suite_test_MapFormat.testUnSetNoDataValue(); }
} testDescription_suite_test_MapFormat_testUnSetNoDataValue;

static class TestDescription_suite_test_MapFormat_testUnSetProjection : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testUnSetProjection() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 599, "testUnSetProjection" ) {}
 void runTest() { suite_test_MapFormat.testUnSetProjection(); }
} testDescription_suite_test_MapFormat_testUnSetProjection;

static class TestDescription_suite_test_MapFormat_testGetWidth : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testGetWidth() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 619, "testGetWidth" ) {}
 void runTest() { suite_test_MapFormat.testGetWidth(); }
} testDescription_suite_test_MapFormat_testGetWidth;

static class TestDescription_suite_test_MapFormat_testGetHeight : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_MapFormat_testGetHeight() : CxxTest::RealTestDescription( Tests_test_MapFormat, suiteDescription_test_MapFormat, 630, "testGetHeight" ) {}
 void runTest() { suite_test_MapFormat.testGetHeight(); }
} testDescription_suite_test_MapFormat_testGetHeight;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
