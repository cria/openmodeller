#include <openmodeller/om.hh>
#include <openmodeller/om_defs.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Random.hh>

#include "getopts/getopts.h"

#include "om_cmd_utils.hh"

#include <string>
#include <iostream>  // I/O 

#include <stdexcept>
 
using namespace std;

int main( int argc, char **argv ) {

#ifdef BUILD_TERRALIB
  USE_TERRALIB_IO
#endif

  char *args;
  int option;

  struct options opts[] = 
  {
    { 1, "log-level", "Set the log level (debug, warn, info, error)", NULL, 1 },
    { 2, "version",   "Display the version info",                      "v", 0 },
    { 3, "num-points","Number of points to be generated",              "n", 1 },
    { 4, "label",     "Label for the points",                          "l", 1 },
    { 5, "seq-start", "Sequence start for points id",                  "s", 1 },
    { 6, "mask",      "Mask file",                                     "m", 1 },
    { 7, "proportion","Proportion of absence points (in %)",           "p", 1 },
    { 0, NULL,        NULL,                                           NULL, 0 }
  };

  bool passed_params = false;

  std::string log_level("info");
  std::string num_points_string;
  std::string label("label");
  std::string sequence_start_string;
  std::string mask_file;
  std::string proportion_string("100");

  while ( ( option = getopts( argc, argv, opts, &args ) ) != 0 ) {

    passed_params = true;

    switch ( option ) {

      // Special Case: Recognize options that we didn't set above.
      case -2: 
        printf( "Unknown option: %s\n", args );
        break;
      // Special Case: getopts() can't allocate memory.
      case -1:
        printf( "Unable to allocate memory from getopts().\n" );
        exit(-1);
        break;
      case 1:
        log_level = args;
        break;
      case 2:
        printf("om_pseudo 0.3.0\n");
        exit(0);
        break;
      case 3:
        num_points_string = args;
        break;
      case 4:
        label = args;
        break;
      case 5:
        sequence_start_string = args;
        break;
      case 6:
        mask_file = args;
        break;
      case 7:
        proportion_string = args;
        break;
      default:
        break;
    }

    // This free() is required since getopts() automagically allocates space 
    // for "args" everytime it's called. */
    free( args );
  }

  if ( ! passed_params ) {

    // Display usage
    getopts_usage( "om_pseudo", opts );
    exit(0);
  }

  // Check parameters

  if ( num_points_string.empty() ) {

    printf( "Please specify the number of points to be generated\n");
    exit(-1);
  }

  int num_points = atoi( num_points_string.c_str() );

  if ( num_points <= 0 ) {

    printf( "Please specify a valid (> 0) number of points to be generated\n");
    exit(-1);
  }

  if ( mask_file.empty() ) {

    printf( "Please specify a mask file\n");
    exit(-1);
  }
  else {

    FILE * fh = fopen( mask_file.c_str(), "r" );

    if ( fh == NULL ) {

      printf( "Could not open the specified mask file (check parameter and permissions)\n");
      exit(-1);
    }

    fclose( fh );
  }

  int sequence_start = 1; // default
  
  if ( ! sequence_start_string.empty() ) {

    sequence_start = atoi( sequence_start_string.c_str() );
  }

  if ( proportion_string.empty() ) {

    proportion_string = "100";
  }

  double proportion = atof( proportion_string.c_str() );

  if ( proportion > 100 ) {

    proportion = 100;
  }
  else if ( proportion < 0 ) {

    proportion = 0;
  }

  proportion /= 100;

  int num_absences_to_be_generated = (int)(num_points * proportion);

  // Log stuff

  Log::Level level_code = getLogLevel( log_level );

  Log::instance()->setLevel( level_code );

  // Real work

  try {

    std::vector<std::string> categorical_layers, continuous_layers;

    continuous_layers.push_back( mask_file ); // need to add at least one layer

    EnvironmentPtr env = createEnvironment( categorical_layers, continuous_layers, mask_file );

    if ( ! env ) {

      printf( "Could not create environment object" );
      exit(-1);
    }

    OccurrencesPtr presences( new OccurrencesImpl( label ) );
    OccurrencesPtr absences( new OccurrencesImpl( label ) );

    SamplerPtr samp = createSampler( env, presences, absences );

    if ( ! samp ) {

      printf( "Could not create sampler object" );
      exit(-1);
    }

    std::cerr << flush;

    // Header
    cout << "#id\t" << "label\t" << "long\t" << "lat\t" << "abundance" << endl << flush;

    string abundance;

    int num_generated_absences = 0;

    for ( int i = 0; i < num_points; ++i ) {

      OccurrencePtr point = samp->getPseudoAbsence();

      Random rnd;

      if ( num_generated_absences == num_absences_to_be_generated ) {

        // If we already have all absence points, generate a presence point
        abundance = "1";
      }
      else {

        // If we can still generate both categories (presence/absence), use rand
        if ( rnd() <= proportion ) {

          abundance = "0";

          ++num_generated_absences;
        }
        else {

          abundance = "1";
        }
      }

      std::cerr << flush;

      cout << sequence_start + i << "\t" << label.c_str() << "\t" << point->x() << "\t" << point->y() << "\t" << abundance.c_str() << endl << flush;
    }
  }
  catch ( runtime_error e ) {

    printf( "om_pseudo: %s\n", e.what() );
    exit(-1);
  }

  return 0;
}
