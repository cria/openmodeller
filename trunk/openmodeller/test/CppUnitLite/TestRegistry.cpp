

#include "Test.h"
#include "TestResult.h"
#include "TestRegistry.h"


void TestRegistry::addTest (Test *test) 
{
	instance ().add (test);
}


int TestRegistry::runAllTests (TestResult& result) 
{
	return instance ().run (result);
}


TestRegistry& TestRegistry::instance () 
{
	static TestRegistry registry;
	return registry;
}


void TestRegistry::add (Test *test) 
{
	if (first_test == 0) {
		first_test = test;
		last_test = test;
		return;
	}
	
	last_test->setNext (test);
	last_test = test;
}


int TestRegistry::run (TestResult& result) 
{
	result.testsStarted ();

	for (Test *test = first_test; test != 0; test = test->getNext ())
		test->run (result);
	return result.testsEnded ();
}
