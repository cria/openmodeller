// Code kindly donated by Kevin Ruland
// This code simply loads a problem,
// creates a model,
// The loads a reprojection environment,
// and projects.
// then writes the model out.
//

#include <openmodeller/om.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/MapFormat.hh>
#include <istream>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream.h>  // I/O 
#include <fstream.h>   // file I/O

#include <stdexcept>
using namespace std;

static int project(std::string theModel, std::string theEnvironment, std::string theOutput)
{
 try {

    g_log.setLevel( Log::Debug );

    AlgorithmFactory::searchDefaultDirs();

    OpenModeller om;
    cout << "Loading Model file " << theModel << endl;

    ConfigurationPtr c = Configuration::readXml( theModel.c_str() );

    om.setConfiguration(c);


    cout << "Create Model" << endl;
    om.createModel();

    cout << "Loading Projection Environment " << theEnvironment << endl;

    c = Configuration::readXml( theEnvironment.c_str() );

    EnvironmentPtr e = createEnvironment( c );

    cout << "Projecting to file " << theOutput << endl;

    om.createMap( e, theOutput.c_str() );

    return 0;
  }
  catch( exception& e ) {
    cerr << "Exception Caught" << endl;
    cerr << e.what() << endl;
    return 0;
  }
}
int main( int argc, char **argv ) {

 
    if (argc != 4) {
      cout << "Usage: "
          << argv[0]
          << " <configfile-xml> as produced by openmodeller serialised model output "
          << " <projection-xml> as in environment.xml"
          << " <outputfilename>"
          << endl;
      return -1;
    }
    std::string myModel(argv[1]);
    std::string myEnvironment(argv[2]);
    std::string myOutput(argv[3]);
    project(myModel,myEnvironment,myOutput);
}
