#include <openmodeller/om.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/os_specific.hh>

#include "getopts/getopts.h"

#include "om_cmd_utils.hh"

#include <fstream>   // file I/O for XML
#include <sstream>   // ostringstream datatype
#include <stdio.h>   // file I/O for log
#include <time.h>    // used to limit the number of times that the progress is written to a file
#include <string>    // string library
#include <stdexcept> // try/catch

using namespace std;

int get_values(AlgorithmPtr alg, EnvironmentPtr env, OccurrencesPtr occs, Scalar * values, int cnt);

/// Main code
int main( int argc, char **argv ) {

  Options opts;
  int option;

  // command-line parameters (short name, long name, description, take args)
  opts.addOption( "v", "version"     , "Display version info"                           , false );
  opts.addOption( "r", "xml-req"     , "(option 1) Model evaluation request file in XML", true );
  opts.addOption( "o", "model"       , "(option 2) Serialized model file"               , true );
  opts.addOption( "p", "points"      , "(option 2) TAB-delimited file with points"      , true );
  opts.addOption( "s", "result"      , "File to store evaluation result in XML"         , true );
  opts.addOption( "", "log-level"    , "Set the log level (debug, warn, info, error)"   , true );
  opts.addOption( "", "log-file"     , "Log file"                                       , true );
  opts.addOption( "" , "prog-file"   , "File to store job progress"                     , true );

  std::string log_level("info");
  std::string request_file;
  std::string model_file;
  std::string points_file;
  std::string result_file;
  std::string log_file;
  std::string progress_file;

  if ( ! opts.parse( argc, argv ) ) {

    opts.showHelp( argv[0] );
  }

  // Set up any related external resources
  setupExternalResources();

  OpenModeller om;

  while ( ( option = opts.cycle() ) >= 0 ) {

    switch ( option ) {

      case 0:
        printf( "om_evaluate %s\n", om.getVersion().c_str() );
        printf("This is free software; see the source for copying conditions. There is NO\n");
        printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
        exit(0);
        break;
      case 1:
        request_file = opts.getArgs( option );
        break;
      case 2:
        model_file = opts.getArgs( option );
        break;
      case 3:
        points_file = opts.getArgs( option );
        break;
      case 4:
        result_file = opts.getArgs( option );
        break;
      case 5:
        log_level = opts.getArgs( option );
        break;
      case 6:
        log_file = opts.getArgs( option );
        break;
      case 7:
        progress_file = opts.getArgs( option );
        break;
      default:
        break;
    }
  }

  // Initialize progress data if user wants to track progress
  progress_data prog_data;

  if ( ! progress_file.empty() ) { 

    prog_data.file_name = progress_file;

    time( &prog_data.timestamp );

    prog_data.progress = -1.0; // queued

    // Always create initial file with progress 0
    progressFileCallback( 0.0, &prog_data );
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

    if ( model_file.empty() || points_file.empty() ) {

      printf( "Please specify either a test request file in XML or a serialized model and a TAB-delimited file with the points to be tested\n");

      // If user is tracking progress
      if ( ! progress_file.empty() ) { 

        // -2 means aborted
        progressFileCallback( -2.0, &prog_data );
      }

      exit(-1);
    }
  }
  else {

    if ( ! model_file.empty() ) {

      Log::instance()->warn( "Model file parameter will be ignored (using XML request instead)\n" );
    }
    if ( ! points_file.empty() ) {

      Log::instance()->warn( "Points file parameter will be ignored (using XML request instead)\n");
    }
  }

  // Real work

  try {

    // Load algorithms and instantiate controller class
    AlgorithmFactory::searchDefaultDirs();

    // IMPORTANT: data is not deserialized through Sampler objects, which would be much simpler.
    //            The reason is that some of the input points may be masked out so they 
    //            would be discarded when the sampler cross references occurrences and environment. 
    //            The number of evaluations MUST match the number of input points.
    AlgorithmPtr alg;
    OccurrencesPtr presences;
    OccurrencesPtr absences;
    EnvironmentPtr env;

    if ( ! request_file.empty() ) {

      // Loading input from XML request

      Log::instance()->debug( "Loading input from XML\n" );

      ConfigurationPtr input = Configuration::readXml( request_file.c_str() );

      alg = AlgorithmFactory::newAlgorithm( input->getSubsection( "Algorithm" ) );

      // Load environment and occurrences separately
      ConfigurationPtr sampler_config = input->getSubsection( "Sampler" );

      if ( ConstConfigurationPtr env_config = sampler_config->getSubsection( "Environment", false ) ) {

        env = createEnvironment();
        env->setConfiguration( env_config );
      }

      if ( ConstConfigurationPtr presences_config = sampler_config->getSubsection( "Presence", false ) ) {

        presences = new OccurrencesImpl(1.0);
        presences->setConfiguration( presences_config );
      }

      if ( ConstConfigurationPtr absences_config = sampler_config->getSubsection( "Absence", false ) ) {

        absences = new OccurrencesImpl(0.0);
        absences->setConfiguration( absences_config );
      }
    }
    else {

      // Loading input from serialized model + TAB-delimited points file

      ConfigurationPtr input = Configuration::readXml( model_file.c_str() );

      alg = AlgorithmFactory::newAlgorithm( input->getSubsection( "Algorithm" ) );

      Log::instance()->debug( "Loading training sampler to get layers, label and spatial reference\n" );

      SamplerPtr training_sampler = createSampler( input->getSubsection( "Sampler" ) );

      // IMPORTANT: environmental scenario is taken from training sampler!
      env = training_sampler->getEnvironment();

      // IMPORTANT: label and spatial reference are taken from presence points of the training sampler!
      OccurrencesPtr training_presences = training_sampler->getPresences();

      std::string label( training_presences->label() );
      std::string spatial_ref( training_presences->coordSystem() );

      Log::instance()->debug( "Loading points %s %s\n", label.c_str(), spatial_ref.c_str() );

      OccurrencesReader* oc_reader = OccurrencesFactory::instance().create( points_file.c_str(), spatial_ref.c_str() );

      presences = oc_reader->getPresences( label.c_str() );
      absences = oc_reader->getAbsences( label.c_str() );

      delete oc_reader;
    }

    if ( ! alg->done() ) {

      Log::instance()->error( "No model could be found as part of the specified algorithm. Aborting.\n");

      // If user is tracking progress
      if ( ! progress_file.empty() ) { 

        // -2 means aborted
        progressFileCallback( -2.0, &prog_data );
      }

      exit(-1);
    }

    if ( alg->needNormalization() ) {

      env->normalize( alg->getNormalizer() );
    }
    
    ConfigurationPtr output( new ConfigurationImpl("Values") );

    int num_presences = 0;
    int num_absences = 0;

    if ( presences ) {

      num_presences = presences->numOccurrences();
    }
    
    if ( absences ) {

      num_absences = absences->numOccurrences();
    }

    Log::instance()->debug( "Loaded %d presences and %d absences\n", num_presences, num_absences );
    
    // Evaluate model
    Log::instance()->debug( "Starting evaluation\n" );
    
    Scalar * vs = new (nothrow) Scalar[num_presences + num_absences];
    if ( ! vs ) {

      Log::instance()->error( "Not enough memory to allocate model values. Aborting.\n");

      // If user is tracking progress
      if ( ! progress_file.empty() ) { 

        // -2 means aborted
        progressFileCallback( -2.0, &prog_data );
      }

      exit(-1);
    }
    int cnt = 0;

    if ( presences ) {

      cnt = get_values(alg, env, presences, vs, cnt);
    }
    if ( absences ) {

      cnt = get_values(alg, env, absences, vs, cnt);
    }
    
    int precision = 5;
    
    output->addNameValue( "V", vs, num_presences + num_absences, precision );

    std::ostringstream evaluation_output;

    Configuration::writeXml( output, evaluation_output );

    std::cerr << flush;

    // Write test output to file, if requested
    if ( ! result_file.empty() ) {

      ofstream file( result_file.c_str() );
      file << evaluation_output.str();
      file.close();
    }
    else {

      // Otherwise send it to stdout
      std::cout << evaluation_output.str().c_str() << endl << flush;
    }

    // If user wants to track progress
    if ( ! progress_file.empty() ) { 

      // Indicate that the job is finished
      progressFileCallback( 1.0, &prog_data );
    }
    
    delete[] vs;
  }
  catch ( runtime_error e ) {

    // If user is tracking progress
    if ( ! progress_file.empty() ) { 

      // -2 means aborted
      progressFileCallback( -2.0, &prog_data );
    }

    printf( "om_evaluate aborted: %s\n", e.what() );
  }
}

/******************/
/*** get values ***/
int 
get_values(AlgorithmPtr alg, EnvironmentPtr env, OccurrencesPtr occs, Scalar * values, int cnt) {

  OccurrencesImpl::const_iterator oc;
  OccurrencesImpl::const_iterator end;
  Scalar val;

  if ( occs->numOccurrences() > 0 ) {

    oc = occs->begin();
    end = occs->end();
    Sample sample;

    while ( oc != end ) {
      
      // Use environmental data already provided by the point, if present
      // (in this case it will always be unnormalized, so we need to check normalization)
      if ( (*oc)->hasEnvironment() ) {

        if ( alg->needNormalization() ) {

          (*oc)->normalize( alg->getNormalizer(), env->numCategoricalLayers() );
        }
        
	sample = (*oc)->environment();
      }
      else {

        sample = env->get( (*oc)->x(), (*oc)->y() );
      }

      if ( sample.size() > 0 ) {

        //sample.dump();
        val = alg->getValue( sample );
      }
      else {
        val= -1.0;
      }

      //printf( "val=%0.5f\n", val );
      values[cnt] = val;
      ++cnt;
      ++oc;
    }
  }
  
  return cnt;
}
