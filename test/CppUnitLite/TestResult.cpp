
#include "TestResult.h"
#include "Failure.h"

#include <stdio.h>


TestResult::TestResult ()
	: failureCount (0)
{
}


void TestResult::testsStarted () 
{
}


void TestResult::addFailure (const Failure& failure) 
{
  fprintf(stdout, 
  "###############################################################################\n");
  fprintf (stdout, "%s%s%s%s%s%s%ld%s%s\n",
	   "Test \"", 
	   failure.testName.asCharString(),
	   "\" failed:  \"",
	   failure.message.asCharString (),
	   "\" " ,
	   "line ",
	   failure.lineNumber,
	   " in ",
	   failure.fileName.asCharString ());
  fprintf(stdout, 
  "###############################################################################\n");
  
  failureCount++;
}


int TestResult::testsEnded () 
{
	if (failureCount > 0)
	  {
	    fprintf (stdout, "There were %u failures\n", failureCount);
	    return 1;
	  }
	else
	  {
	    fprintf (stdout, "There were no test failures\n");
	    return 0;
	  }
}
