#include <openmodeller/om.hh>
#include <openmodeller/om_defs.hh>
#include <openmodeller/Log.hh>

#include <string>
#include <iostream>  // I/O 

#include <stdexcept>
 
using namespace std;

int main( int argc, char **argv ) {
#ifdef BUILD_TERRALIB
  USE_TERRALIB_IO
#endif

  try {

    //Debug, Info, Warn, Error
    Log::instance()->setLevel( Log::Info );

    if (argc != 5) {
      cout << "Usage " << argv[0] << " <label> <mask file> <num points> <sequence start>" << endl;
      return -1;
    }

    std::string label( argv[1] );

    std::string mask_file( argv[2] );

    std::string num_points_string( argv[3] );

    int num_points = atoi( num_points_string.c_str() );

    std::string sequence_start_string( argv[4] );

    int sequence_start = atoi( sequence_start_string.c_str() );

    std::vector<std::string> categorical_layers, continuous_layers;

    continuous_layers.push_back( mask_file ); // need to add at least one layer

    EnvironmentPtr env = createEnvironment( categorical_layers, continuous_layers, mask_file);

    if ( ! env ) {

      cout << "Could not create environment object" << endl;
      return -1;
    }

    OccurrencesPtr presences( new OccurrencesImpl( label ) );
    OccurrencesPtr absences( new OccurrencesImpl( label ) );

    SamplerPtr samp = createSampler( env, presences, absences );

    if ( ! samp ) {

      cout << "Could not create sampler object" << endl;
      return -1;
    }

    for ( int i = 0; i < num_points; ++i ) {

      OccurrencePtr point = samp->getPseudoAbsence();

      cout << sequence_start + i << "\t" << label.c_str() << "\t" << point->x() << "\t" << point->y() << "\t" << "0" << endl;
    }
  }
  catch ( runtime_error e ) {

    cout<<"Exception: " << e.what() << endl;
  }
}
