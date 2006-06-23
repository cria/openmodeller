#include <openmodeller/om.hh>
#include <openmodeller/Log.hh>
#include "request_file.hh"
#include "request_file.hh"
#include "file_parser.hh"

#include <istream>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream.h>  // I/O 
#include <fstream.h>   // file I/O

#include <stdexcept>
 
using namespace std;
 
int main( int argc, char **argv ) {
#ifdef BUILD_TERRALIB
  USE_TERRALIB_IO
#endif

  try {
    //Debug, Info, Warn, Error
    g_log.setLevel( Log::Error );
    AlgorithmFactory::searchDefaultDirs();

    if (argc != 3) {
      cout << "Usage " << argv[0] << " <request file> <output filename>" << endl;
      return -1;
    }
    OpenModeller om;
    // Configure the OpenModeller object from data read from the
    // request file.
    RequestFile request;
    char *request_file = argv[1];
    int resp = request.configure( &om, request_file);

    //cout << "Reading configuration from: " << argv[1] << endl;    
    //ConfigurationPtr c = Configuration::readXml( argv[1] );
    //om.setConfiguration(c);
    //OutputFormat myOutputFormat = MapFormat( format.c_str() );
    // hard coded for now: 8-bit grey tiffs
    //myOutputFormat.setFormat( MapFormat::GreyTiff );
    //om->createMap( argv[1], _outputFormat );

    SamplerPtr s = om.getSampler();
    ConstOccurrencesPtr p = s->getPresences();

    OccurrencesImpl::const_iterator it = p->begin();
    OccurrencesImpl::const_iterator fin = p->end();

    FILE * pFile;
    ofstream fp_out;
    fp_out.open(argv[2], ios::out);

    while ( it != fin ) {
      cout << (*it)->x() << " " << (*it)->y() << " " << (*it)->environment() << endl;
      fp_out<<(*it)->environment()<<endl;
      it++;

    }
    fp_out.close();
    return 0;
  }
  catch (runtime_error e) {
    cout<<"Exception: " << e.what() << endl;
  }
}
