#include <openmodeller/om.hh>
#include <openmodeller/os_specific.hh>

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
  opts.addOption( "o", "model"     , "File with serialized model"                  , true );
  opts.addOption( "t", "threshold" , "Model threshold (default 0.5)"               , true );
  opts.addOption( "" , "geo-unique", "Avoid repeating same coordinates"            , false );
  opts.addOption( "" , "env-unique", "Avoid repeating same environment condition"  , false );

  std::string log_level("info");
  std::string num_points_string;
  std::string label("label");
  std::string sequence_start_string;
  std::string mask_file;
  std::string proportion_string;
  std::string model_file;
  std::string threshold_string;
  bool geo_unique = false;
  bool env_unique = false;

  if ( ! opts.parse( argc, argv ) ) {

    opts.showHelp( argv[0] ); 
    exit(0);
  }

  // Set up any related external resources
  setupExternalResources();

  OpenModeller om;

  while ( ( option = opts.cycle() ) >= 0 ) {

    switch ( option ) {

      case 0:
        log_level = opts.getArgs( option );
        break;
      case 1:
        printf( "om_pseudo %s\n", om.getVersion().c_str() );
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
      case 7:
        model_file = opts.getArgs( option );
        break;
      case 8:
        threshold_string = opts.getArgs( option );
        break;
      case 9:
        geo_unique = true;
        break;
      case 10:
        env_unique = true;
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

  double threshold = 0.5;
  
  if ( ! threshold_string.empty() ) {

    threshold = atof( threshold_string.c_str() );
  }

  if ( threshold <= 0.0 ) {

    printf( "Model threshold must be greater than zero\n");
    exit(-1);
  }

  if ( threshold >= 1.0 ) {

    printf( "Model threshold must be smaller than one\n");
    exit(-1);
  }

  // Log stuff

  Log::Level level_code = getLogLevel( log_level );

  Log::instance()->setLevel( level_code );

  // Real work

  try {

    SamplerPtr samp;

    Model model = 0;
    
    if ( model_file.empty() ) {

      std::vector<std::string> categorical_layers, continuous_layers;

      continuous_layers.push_back( mask_file ); // need to add at least one layer

      EnvironmentPtr env = createEnvironment( categorical_layers, continuous_layers, mask_file );

      if ( ! env ) {

        printf( "Could not create environment object" );
        exit(-1);
      }

      OccurrencesPtr presences( new OccurrencesImpl( label ) );
      OccurrencesPtr absences( new OccurrencesImpl( label ) );

      samp = createSampler( env, presences, absences );
    }
    else {
    
      // Load available algorithms
      AlgorithmFactory::searchDefaultDirs();

      // Load serialized model
      ConfigurationPtr config = Configuration::readXml( model_file.c_str() );

      AlgorithmPtr alg = AlgorithmFactory::newAlgorithm( config->getSubsection( "Algorithm" ) );

      model = alg->getModel();

      // note: alg deserialization doesn't include sampler stuff
      SamplerPtr alg_samp = createSampler( config->getSubsection( "Sampler" ) );

      if ( ! alg_samp ) {

        printf( "Could not find sampler data in the specified model file" );
        exit(-1);
      }

      EnvironmentPtr env = alg_samp->getEnvironment();

      env->changeMask( mask_file );

      // note: no need to change the label in presences & absences, because it 
      // will be ignored when generating the points
      OccurrencesPtr presences = alg_samp->getPresences();
      OccurrencesPtr absences = alg_samp->getAbsences();
 
      samp = createSampler( env, presences, absences );

      // Overwrite sampler, in case masks are different
      alg->setSampler( samp );

      // Normalize environment if necessary
      model->setNormalization( env );
    }

    if ( ! samp ) {

      printf( "Could not create sampler object" );
      exit(-1);
    }

    // Start output

    std::cerr << flush;

    // Header
    cout << "#id\t" << "label\t" << "long\t" << "lat\t" << "abundance" << endl << flush;

    string abundance("0");

    int num_generated_absences = 0;

    int i = 0;

    if ( geo_unique || env_unique ) {

      OccurrencesPtr pseudo = samp->getPseudoAbsences( num_points, model, threshold, geo_unique, env_unique );

      OccurrencesImpl::iterator it = pseudo->begin();
      OccurrencesImpl::iterator end = pseudo->end();

      // Points
      while ( it != end ) {

        // Start with absences. Switch to presence after reaching the number of absences.
        if ( num_generated_absences == num_absences_to_be_generated ) {

          abundance = "1";
        }

        std::cerr << flush;

        cout << sequence_start + i << "\t" << label.c_str() << "\t" << (*it)->x() << "\t" << (*it)->y() << "\t" << abundance.c_str() << endl << flush;

        num_generated_absences++;
        it++;
        i++;
      }
    }
    // No geo or env uniqueness required - can generate one by one
    else {

      // Points
      while ( i < num_points ) {

        const ConstOccurrencePtr occ_ptr = samp->getPseudoAbsence( model, threshold );

        // Start with absences. Switch to presence after reaching the number of absences.
        if ( num_generated_absences == num_absences_to_be_generated ) {

          abundance = "1";
        }

        std::cerr << flush;

        cout << sequence_start + i << "\t" << label.c_str() << "\t" << occ_ptr->x() << "\t" << occ_ptr->y() << "\t" << abundance.c_str() << endl << flush;

        num_generated_absences++;
        i++;

        //Log::instance()->info( "Points: %d \r", i );
      }
    }
  }
  catch ( runtime_error e ) {

    printf( "om_pseudo: %s\n", e.what() );
    exit(-1);
  }

  return 0;
}
