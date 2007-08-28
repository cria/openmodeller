#include <openmodeller/om.hh>
#include <iostream>
int main( int argc, char **argv ) 
{
  AlgorithmFactory::searchDefaultDirs();
  OpenModeller * mypOpenModeller = new OpenModeller();
  const AlgMetadata **mypAlgorithmMetadataArray = mypOpenModeller->availableAlgorithms();
  const AlgMetadata *mypAlgorithmMetadata = *mypAlgorithmMetadataArray;

  unsigned int myCount=0;
  while ( mypAlgorithmMetadata  )
  {
    std::cout << mypAlgorithmMetadata->name << std::endl;
    *mypAlgorithmMetadataArray++;
    mypAlgorithmMetadata = *mypAlgorithmMetadataArray;
    ++myCount;
  }
  unsigned int myResult = 1; // assume fail
  if (myCount > 0)
  {
    myResult = 0; //pass
  }
  return myResult;
}

