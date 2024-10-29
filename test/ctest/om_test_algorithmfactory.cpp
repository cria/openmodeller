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
    CxxTest::RealWorldDescription::_worldName = "test_algorithmfactory";
    status = CxxTest::Main< CxxTest::ErrorPrinter >( tmp, argc, argv );
    return status;
}
bool suite_test_AlgorithmFactory_init = false;
#include "om_test_algorithmfactory.h"

static test_AlgorithmFactory suite_test_AlgorithmFactory;

static CxxTest::List Tests_test_AlgorithmFactory = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_test_AlgorithmFactory( "om_test_algorithmfactory.h", 38, "test_AlgorithmFactory", suite_test_AlgorithmFactory, Tests_test_AlgorithmFactory );

static class TestDescription_suite_test_AlgorithmFactory_test1 : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_test_AlgorithmFactory_test1() : CxxTest::RealTestDescription( Tests_test_AlgorithmFactory, suiteDescription_test_AlgorithmFactory, 48, "test1" ) {}
 void runTest() { suite_test_AlgorithmFactory.test1(); }
} testDescription_suite_test_AlgorithmFactory_test1;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
