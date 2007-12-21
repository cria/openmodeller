#include <openmodeller/om.hh>
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

      cout << "Usage " << argv[0] << " <serialized model> <request file with test points>" << endl;

      return -1;
    }

    OpenModeller om;

    Log::instance()->setLevel( Log::Debug );

    Log::instance()->info( "Loading serialized model %s\n", argv[1] );

    // This will load the previous model (including points that were used!!!)
    char* model = argv[1];
    ConfigurationPtr conf = Configuration::readXml( model );
    om.setModelConfiguration( conf );

    Log::instance()->info( "Loading points\n" );

    // This will re-load environment, algorithm and points related to test!
    char *request_file = argv[2];
    FileParser fp( request_file );

    std::string oc_cs = fp.get( "WKT coord system" );
    std::string oc_file = fp.get( "Occurrences source" );
    std::string oc_name = fp.get( "Occurrences group" );

    Log::instance()->info( "WKT = %s\n", oc_cs.c_str() );
    Log::instance()->info( "localities file = %s\n", oc_file.c_str() );
    Log::instance()->info( "localities label = %s\n", oc_name.c_str() );

    if ( oc_file.empty() ) {

      Log::instance()->error( 0, "'Occurrences source' keyword was not specified in the request file!\n" );
      return 0;
    }

    OccurrencesReader* oc_reader = OccurrencesFactory::instance().create( oc_file.c_str(), oc_cs.c_str() );

    OccurrencesPtr presences = oc_reader->getPresences( oc_name.c_str() );

    int num_presences = presences->numOccurrences();

    Log::instance()->info( "Loaded %u presence(s)\n", num_presences );

    OccurrencesPtr absences = oc_reader->getAbsences( oc_name.c_str() );

    if ( absences ) {

      Log::instance()->info( "Loaded %u absence(s)\n", absences->numOccurrences() );
    }
    else {

      Log::instance()->debug( "Sampling pseudo-absences\n" );

      absences = new OccurrencesImpl( 0.0 );

      SamplerPtr original_sampler = om.getSampler();

      for ( int i = 0; i < num_presences; ++i ) {

        OccurrencePtr oc = original_sampler->getPseudoAbsence();
        absences->insert( oc );

        Log::instance()->debug( "(%f,%f)\n", oc->x(), oc->y() );
      }

      Log::instance()->info( "Created %u pseudo-absence(s)\n", num_presences );
    }

    delete oc_reader;

    EnvironmentPtr env = om.getEnvironment();

    SamplerPtr sampler = createSampler( env, presences, absences );

    om.setSampler( sampler );

    ConfusionMatrix* matrix = new ConfusionMatrix();

    matrix->calculate( om.getModel(), sampler );

    RocCurve* roc_curve = new RocCurve();

    roc_curve->calculate( om.getModel(), sampler );

    Log::instance()->info( "\nModel statistics\n" );
    Log::instance()->info( "Accuracy:          %7.2f%%\n", matrix->getAccuracy() * 100 );

    int omissions = matrix->getValue(0.0, 1.0);
    int total     = omissions + matrix->getValue(1.0, 1.0);

    Log::instance()->info( "Omission error:    %7.2f%% (%d/%d)\n", matrix->getOmissionError() * 100, omissions, total );

    int commissions = matrix->getValue(1.0, 0.0);
    total           = commissions + matrix->getValue(0.0, 0.0);

    Log::instance()->info( "Commission error:  %7.2f%% (%d/%d)\n", matrix->getCommissionError() * 100, commissions, total );

    Log::instance()->info( "AUC:               %7.2f\n", roc_curve->getArea() );

    return 0;
  }
  catch (runtime_error e) {

    cout<<"Exception: " << e.what() << endl;
  }
}
