#include <openmodeller/om.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Sampler.hh>
#include "request_file.hh"
#include "file_parser.hh"

#include <istream>

#include <stdlib.h>
#include <string>

#include <stdexcept>
 
using namespace std;

// This program was created to test a model against a set of independent points (points
// that were not used to create the model).
// It receives as input a serialized model and a request file (similar to the
// one used by om_console) that must contain the followinig keywords:
// WKT coord system = ...
// Occurrences source = (pointing to the source of localities that will test the model)
// Occurrences group = ...
//
// The serialized model is loaded and then the sampler is replaced by a new one related to 
// the test points referenced by the request file.
//
// Model and sampler are then used to calculate the confusion matrix.

int main( int argc, char **argv ) {

  try {

    AlgorithmFactory::searchDefaultDirs();

    if ( argc != 3 ) {

      cout << "Usage " << argv[0] << "<serialized model> <request file with test points>" << endl;

      return -1;
    }

    OpenModeller om;

    g_log.setLevel( Log::Debug );

    g_log.info( "Loading serialized model %s\n", argv[1] );

    // This will load the previous model (including points that were used!!!)
    char* model = argv[1];
    ConfigurationPtr conf = Configuration::readXml( model );
    om.setModelConfiguration( conf );

    g_log.info( "Loading points\n" );

    // This will re-load environment, algorithm and points related to test!
    char *request_file = argv[2];
    FileParser fp( request_file );

    std::string oc_cs = fp.get( "WKT coord system" );
    std::string oc_file = fp.get( "Occurrences source" );
    std::string oc_name = fp.get( "Occurrences group" );

    g_log.info( "WKT = %s\n", oc_cs.c_str() );
    g_log.info( "localities file = %s\n", oc_file.c_str() );
    g_log.info( "localities label = %s\n", oc_name.c_str() );

    if ( oc_file.empty() ) {

      g_log.error( 0, "'Occurrences source' keyword was not specified in the request file!\n" );
      return 0;
    }

    OccurrencesReader* oc_reader = OccurrencesFactory::instance().create( oc_file.c_str(), oc_cs.c_str() );

    OccurrencesPtr presences = oc_reader->getPresences( oc_name.c_str() );

    OccurrencesPtr absences = oc_reader->getAbsences( oc_name.c_str() );

    delete oc_reader;

    g_log.info( "Loaded %u presence(s)\n", presences->numOccurrences() );
    g_log.info( "Loaded %u absence(s)\n", absences->numOccurrences() );

    EnvironmentPtr env = om.getEnvironment();

    SamplerPtr sampler = createSampler( env, presences, absences );

    om.setSampler( sampler );

    ConfusionMatrix* matrix = new ConfusionMatrix();

    matrix->calculate( om.getModel(), sampler );

    g_log( "\nModel statistics\n" );
    g_log( "Accuracy:          %7.2f%%\n", matrix->getAccuracy() * 100 );
    g_log( "Omission error:    %7.2f%%\n", matrix->getOmissionError() * 100 );
    g_log( "Commission error:  %7.2f%%\n", matrix->getCommissionError() * 100 );

    return 0;
  }
  catch (runtime_error e) {

    cout<<"Exception: " << e.what() << endl;
  }
}
