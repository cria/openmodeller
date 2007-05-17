#include <openmodeller/om.hh>
#include <iostream>



int main( int argc, char **argv ) 
{
  try {
  AreaStats *myAreaStats=new AreaStats();
  myAreaStats->addPrediction(0.33);
  
	
  }
  catch (...)
  {
    return 1; //fail
  }
  return 0; //pass
}

