#include <openmodeller/om.hh>
#include <openmodeller/os_specific.hh>
#include <openmodeller/Exceptions.hh>

#include "getopts/getopts.h"

#include "om_cmd_utils.hh"

#include <string>
#include <stdexcept>
#include <iomanip>   // std::setprecision
#include <sstream>

using namespace std;

int main( int argc, char **argv ) {

  Options opts;
  int option;

  // command-line parameters (short name, long name, description, take args)
  opts.addOption( "v", "version"   , "Display the version info"                             , false );
  opts.addOption( "r", "xml-req"   , "(option 1) Model evaluation request file in XML"      , true );
  opts.addOption( "n", "num-points", "(option 2) Number of points to be generated"          , true );
  opts.addOption( "l", "label"     , "(option 2) Label for the points"                      , true );
  opts.addOption( "q", "seq-start" , "(option 2) Sequence start for points id"              , true );
  opts.addOption( "m", "mask"      , "(option 2) Mask file"                                 , true );
  opts.addOption( "p", "proportion", "(option 2) Proportion of absence points (decimals)"   , true );
  opts.addOption( "o", "model"     , "(option 2) File with serialized model"                , true );
  opts.addOption( "t", "threshold" , "(option 2) Model threshold (default 0.5)"             , true );
  opts.addOption( "" , "geo-unique", "(option 2) Avoid repeating same coordinates"          , false );
  opts.addOption( "" , "env-unique", "(option 2) Avoid repeating same environment condition", false );
  opts.addOption( "s", "result"    , "File to store result"                                 , true );
  opts.addOption( "" , "log-level" , "Set the log level (debug, warn, info, error)"         , true );
  opts.addOption( "" , "log-file"  , "Log file"                                             , true );
  opts.addOption( "" , "prog-file" , "File to store progress"                               , true );

  std::string log_level("info");
  std::string request_file;
  std::string num_points_string;
  int num_points = 0;
  std::string label("label");
  std::string sequence_start_string;
  int sequence_start = 1;
  std::string mask_file;
  std::string proportion_string;
  double proportion = 1.0;
  int num_absences_to_be_generated;
  std::string model_file;
  std::string threshold_string;
  double threshold = 0.5;
  bool geo_unique = false;
  bool env_unique = false;
  std::string result_file;
  std::string log_file;
  std::string progress_file;

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
        printf( "om_pseudo %s\n", om.getVersion().c_str() );
        printf("This is free software; see the source for copying conditions. There is NO\n");
        printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
        exit(0);
        break;
      case 1:
        request_file = opts.getArgs( option );
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
      case 11:
        result_file = opts.getArgs( option );
        break;
      case 12:
        log_level = opts.getArgs( option );
        break;
      case 13:
        log_file = opts.getArgs( option );
        break;
      case 14:
        progress_file = opts.getArgs( option );
        break;
      default:
        break;
    }
  }

  // Log stuff

  Log::Level level_code = getLogLevel( log_level );

  if ( ! log_file.empty() ) {

    Log::instance()->set( level_code, log_file, "" );
  }
  else {
 
    // Just set the level - things will go to stderr
    Log::instance()->setLevel( level_code );
  }

  // Check parameters

  if ( request_file.empty() ) {

    if ( num_points_string.empty() ) {

      printf( "Please specify the number of points to be generated\n");
      exit(1);
    }

    num_points = atoi( num_points_string.c_str() );

    if ( num_points <= 0 ) {

      printf( "Please specify a valid (> 0) number of points to be generated\n");
      exit(1);
    }

    if ( mask_file.empty() ) {

      printf( "Please specify a mask file\n");
      exit(1);
    }
  
    if ( ! sequence_start_string.empty() ) {

      sequence_start = atoi( sequence_start_string.c_str() );
    }

    if ( proportion_string.empty() ) {

      proportion_string = "1";
    }

    proportion = atof( proportion_string.c_str() );

    if ( proportion > 1.0 ) {

      proportion = 1.0;
    }
    else if ( proportion < 0.0 ) {

      proportion = 0.0;
    }

    if ( ! threshold_string.empty() ) {

      threshold = atof( threshold_string.c_str() );
    }

    if ( threshold <= 0.0 ) {

      printf( "Model threshold must be greater than zero\n");
      exit(1);
    }

    if ( threshold >= 1.0 ) {

      printf( "Model threshold must be smaller than one\n");
      exit(1);
    }
  }
  else {

    if ( ! num_points_string.empty() ) {

      Log::instance()->warn( "num-points parameter will be ignored (using XML request instead)\n" );
    }
    if ( label.compare("label") != 0 ) {

      Log::instance()->warn( "label parameter will be ignored (using XML request instead)\n" );
    }
    if ( ! sequence_start_string.empty() ) {

      Log::instance()->warn( "seq-start parameter will be ignored (using XML request instead)\n" );
    }
    if ( ! mask_file.empty() ) {

      Log::instance()->warn( "mask parameter will be ignored (using XML request instead)\n" );
    }
    if ( ! proportion_string.empty() ) {

      Log::instance()->warn( "proportion parameter will be ignored (using XML request instead)\n" );
    }
    if ( ! model_file.empty() ) {

      Log::instance()->warn( "model parameter will be ignored (using XML request instead)\n" );
    }
    if ( ! threshold_string.empty() ) {

      Log::instance()->warn( "threshold parameter will be ignored (using XML request instead)\n" );
    }
    if ( geo_unique ) {

      Log::instance()->warn( "geo-unique parameter will be ignored (using XML request instead)\n" );
    }
    if ( env_unique ) {

      Log::instance()->warn( "env-unique parameter will be ignored (using XML request instead)\n" );
    }
  }

  // Initialize progress data if user wants to track progress
  progress_data prog_data;

  if ( ! progress_file.empty() ) { 

    prog_data.file_name = progress_file;

    time( &prog_data.timestamp );

    prog_data.progress = -3.0;

    // Always create initial file with status 0.0
    progressFileCallback( 0.0, &prog_data );
  }
  
  // Real work

  try {

    SamplerPtr samp;

    Model model = 0;
    
    if ( request_file.empty() ) {

      if ( model_file.empty() ) {

        std::vector<std::string> categorical_layers, continuous_layers;

        continuous_layers.push_back( mask_file ); // need to add at least one layer

        EnvironmentPtr env = createEnvironment( categorical_layers, continuous_layers );

        if ( ! env ) {

          Log::instance()->error( "Could not create environment object. Aborting.\n");

          // If user is tracking progress
          if ( ! progress_file.empty() ) { 

            // -2 means aborted
            progressFileCallback( -2.0, &prog_data );
          }

          exit(1);
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

          Log::instance()->error( "Could not find sampler data in the specified model file. Aborting.\n");

          // If user is tracking progress
          if ( ! progress_file.empty() ) { 

            // -2 means aborted
            progressFileCallback( -2.0, &prog_data );
          }

          exit(1);
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
    }
    else {

      ConfigurationPtr config = Configuration::readXml( request_file.c_str() );

      EnvironmentPtr env = createEnvironment( config->getSubsection( "Environment" ) );

      if ( ! env ) {

        Log::instance()->error( "Could not create environment object. Aborting.\n");

        // If user is tracking progress
        if ( ! progress_file.empty() ) { 

          // -2 means aborted
          progressFileCallback( -2.0, &prog_data );
        }

        exit(1);
      }

      ConfigurationPtr options_config = config->getSubsection( "Options" );

      num_points = options_config->getAttributeAsInt( "NumPoints", 0 );

      if ( num_points <= 0 ) {

        Log::instance()->error( "Please specify a valid (> 0) number of points to be generated. Aborting.\n");

        // If user is tracking progress
        if ( ! progress_file.empty() ) { 

          // -2 means aborted
          progressFileCallback( -2.0, &prog_data );
        }

        exit(1);
      }

      try {

        label = options_config->getAttribute( "Label" );
      }
      catch ( AttributeNotFound& e ) { 

        // optional attribute
        UNUSED(e);
      }

      try {

        proportion = options_config->getAttributeAsDouble( "ProportionOfAbsences", 1.0 );

        if ( proportion > 1.0 ) {

          proportion = 1.0;
        }
        else if ( proportion < 0.0 ) {

          proportion = 0.0;
        }
      }
      catch ( AttributeNotFound& e ) { 

        // optional attribute
        UNUSED(e);
      }

      OccurrencesPtr presences( new OccurrencesImpl( label ) );
      OccurrencesPtr absences( new OccurrencesImpl( label ) );

      samp = createSampler( env, presences, absences );
      
      ConstConfigurationPtr occ_filter_config = options_config->getSubsection( "OccurrencesFilter", false );

      if ( occ_filter_config ) {

        ConstConfigurationPtr su_config = occ_filter_config->getSubsection( "SpatiallyUnique", false );

        if ( su_config ) {

          samp->spatiallyUnique();
        }

        ConstConfigurationPtr eu_config = occ_filter_config->getSubsection( "EnvironmentallyUnique", false );

        if ( eu_config ) {

          samp->environmentallyUnique();
        }
      }
    }
    
    if ( ! samp ) {

      Log::instance()->error( "Could not create sampler object. Aborting.\n");

      // If user is tracking progress
      if ( ! progress_file.empty() ) { 

        // -2 means aborted
        progressFileCallback( -2.0, &prog_data );
      }

      exit(1);
    }

    num_absences_to_be_generated = (int)(num_points * proportion);

    SamplerPtr new_samp;

    OccurrencesPtr new_presences( new OccurrencesImpl( 1.0 ) );

    if ( num_absences_to_be_generated < num_points ) {

      new_presences = samp->getPseudoPresences( (num_points-num_absences_to_be_generated), model, threshold, geo_unique, env_unique, sequence_start );
      new_presences->setLabel( label );
    }

    OccurrencesPtr new_absences( new OccurrencesImpl( 0.0 ) );

    if ( num_absences_to_be_generated > 0 ) {

      new_absences = samp->getPseudoAbsences( num_absences_to_be_generated, model, threshold, geo_unique, env_unique, sequence_start+num_points-num_absences_to_be_generated );
      new_absences->setLabel( label );
    }

    new_samp = createSampler( samp->getEnvironment(), new_presences, new_absences );

    // Output
    std::cerr << flush;
    if ( request_file.empty() ) {

      // No XML request = TXT output
      std::streambuf * buf;
      std::ofstream of;

      if ( ! result_file.empty() ) {

        of.open( result_file.c_str() );
        buf = of.rdbuf();
      }
      else {

        buf = std::cout.rdbuf();
      }

      std::ostream out(buf);
      std::cerr << flush;
      out << "#id\t" << "label\t" << "long\t" << "lat\t" << "abundance" << endl << flush;

      OccurrencesImpl::iterator it = new_presences->begin();
      OccurrencesImpl::iterator end = new_presences->end();

      // Presences
      int i = 0;
      while ( it != end ) {

        std::cerr << flush;
        out << sequence_start + i << "\t" << label.c_str() << "\t" << std::setprecision(9) << (*it)->x() << "\t" << (*it)->y() << "\t1" << endl << flush;
        it++;
        i++;
      }
      
      it = new_absences->begin();
      end = new_absences->end();

      // Absences
      while ( it != end ) {

        std::cerr << flush;
        out << sequence_start + i << "\t" << label.c_str() << "\t" << std::setprecision(9) << (*it)->x() << "\t" << (*it)->y() << "\t0" << endl << flush;
        it++;
        i++;
      }
    }
    else {

      // XML output

      std::ostringstream output;

      Configuration::writeXml( new_samp->getConfiguration(), output );

      // Write output to file, if requested
      if ( ! result_file.empty() ) {

        ofstream file( result_file.c_str() );
        file << output.str();
        file.close();
      }
      else {

        // Otherwise send it to stdout
        std::cout << output.str().c_str() << endl << flush;
      }
    }

    // If user wants to track progress
    if ( ! progress_file.empty() ) { 

      // Indicate that the job is finished
      progressFileCallback( 1.0, &prog_data );
    }
  }
  catch ( runtime_error e ) {

    // If user is tracking progress
    if ( ! progress_file.empty() ) { 

      // -2 means aborted
      progressFileCallback( -2.0, &prog_data );
    }

    printf( "om_pseudo: %s\n", e.what() );
    exit(1);
  }

  return 0;
}
