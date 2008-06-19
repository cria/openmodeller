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

  Options opts;
  int option;

  // command-line parameters (short name, long name, description, take args)
  opts.addOption( "" , "log-level" , "Set the log level (debug, warn, info, error)", true );
  opts.addOption( "v", "version"   , "Display the version info"                    , false );
  opts.addOption( "n", "num-points", "Number of points to be generated"            , true );
  opts.addOption( "l", "label"     , "Label for the points"                        , true );
  opts.addOption( "s", "seq-start" , "Sequence start for points id"                , true );
  opts.addOption( "m", "mask"      , "Mask file"                                   , true );
  opts.addOption( "p", "proportion", "Proportion of absence points (in %)"         , true );

  std::string log_level("info");
  std::string num_points_string;
  std::string label("label");
  std::string sequence_start_string;
  std::string mask_file;
  std::string proportion_string("100");

  if ( ! opts.parse( argc, argv ) ) {

    opts.showHelp( argv[0] ); 
    exit(0);
  }

  while ( ( option = opts.cycle() ) >= 0 ) {

    switch ( option ) {

      case 0:
        log_level = opts.getArgs( option );
        break;
      case 1:
        printf("om_pseudo 0.3.1\n");
        printf("This is free software; see the source for copying conditions. There is NO\n");
        printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
        exit(0);
        break;
      case 2:
        num_points_string = opts.getArgs( option );
        break;
      case 3:
        label = opts.getArgs( option );
        break;
      case 4:
        sequence_start_string = opts.getArgs( option );
        break;
      case 5:
        mask_file = opts.getArgs( option );
        break;
      case 6:
        proportion_string = opts.getArgs( option );
        break;
      default:
        break;
    }
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
