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

/// Main code
int main( int argc, char **argv ) {

  Options opts;
  int option;

  // command-line parameters (short name, long name, description, take args)
  opts.addOption( "v", "version"       , "Display version info"                        , false );
  opts.addOption( "r", "xml-req"       , "(option 1) Test request file in XML"         , true );
  opts.addOption( "o", "model"         , "(option 2) Serialized model file"            , true );
  opts.addOption( "p", "points"        , "(option 2) TAB-delimited file with points"   , true );
  opts.addOption( "" , "calc-matrix"   , "Calculate confusion matrix"                  , false );
  opts.addOption( "t", "threshold"     , "Confusion matrix threshold"                  , true );
  opts.addOption( "" , "ignore-abs"    , "Ignore absences for the confusion matrix"    , false );
  opts.addOption( "" , "calc-roc"      , "Calculate ROC curve"                         , false );
  opts.addOption( "n", "resolution"    , "Number of points in the ROC curve"           , true );
  opts.addOption( "b", "num-background", "Number of background points for the ROC curve when there are no absences", true );
  opts.addOption( "e", "max-omission"  , "Calculate ROC partial area ratio given the maximum omission", true );
  opts.addOption( "" , "abs-background", "Use absences as background in ROC curve"   , false );
  opts.addOption( "s", "result"      , "File to store test result in XML"            , true );
  opts.addOption( "", "log-level"    , "Set the log level (debug, warn, info, error)", true );
  opts.addOption( "", "log-file"     , "Log file"                                    , true );
  opts.addOption( "" , "prog-file"   , "File to store test progress"                 , true );

  std::string log_level("info");
  std::string request_file;
  std::string model_file;
  std::string points_file;
  bool calc_matrix = false;
  std::string threshold_string("");
  double threshold = CONF_MATRIX_DEFAULT_THRESHOLD;
  bool ignore_abs = false;
  bool calc_roc = false;
  std::string resolution_string("");
  int resolution = -1;
  std::string num_background_string("");
  int num_background = -1;
  std::string max_omission_string("");
  double max_omission = 1.0;
  bool abs_background = false;
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
        printf( "om_test %s\n", om.getVersion().c_str() );
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
        calc_matrix = true;
        break;
      case 5:
        threshold_string = opts.getArgs( option );
        break;
      case 6:
        ignore_abs = true;
        break;
      case 7:
        calc_roc = true;
        break;
      case 8:
        resolution_string = opts.getArgs( option );
        break;
      case 9:
        num_background_string = opts.getArgs( option );
        break;
      case 10:
        max_omission_string = opts.getArgs( option );
        break;
      case 11:
        abs_background = true;
        break;
      case 12:
        result_file = opts.getArgs( option );
        break;
      case 13:
        log_level = opts.getArgs( option );
        break;
      case 14:
        log_file = opts.getArgs( option );
        break;
      case 15:
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

    prog_data.progress = -3.0;

    // Always create initial file with status "queued" (-1)
    progressFileCallback( -1.0, &prog_data );
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

  if ( ! request_file.empty() ) {

    if ( ! model_file.empty() ) {

      Log::instance()->warn( "Model file parameter will be ignored (using XML request instead)\n" );
    }
    if ( ! points_file.empty() ) {

      printf( "Points file parameter will be ignored (using XML request instead)\n");
    }
    if ( calc_roc ) {

      printf( "Parameter to calculate ROC curve will be ignored (when using XML request you should specify it in the XML)\n");
    }
    if ( calc_matrix ) {

      printf( "Parameter to calculate confusion matrix will be ignored (when using XML request you should specify it in the XML)\n");
    }
  }
  else if ( ( ! model_file.empty() ) && ! points_file.empty() ) {

    // Custom threshold
    if ( ! threshold_string.empty() ) {

      threshold = atof( threshold_string.c_str() );
    }

    // Custom resolution
    if ( ! resolution_string.empty() ) {

      resolution = atoi( resolution_string.c_str() );
    }

    // Custom number of background points
    if ( ! num_background_string.empty() ) {

      num_background = atoi( num_background_string.c_str() );
    }

    // Custom max omission
    if ( ! max_omission_string.empty() ) {

      max_omission = atof( max_omission_string.c_str() );
    }
  }
  else {

    printf( "Please specify either a test request file in XML or a serialized model and a TAB-delimited file with the points to be tested\n");

    // If user is tracking progress
    if ( ! progress_file.empty() ) { 

      // -2 means aborted
      progressFileCallback( -2.0, &prog_data );
    }

    exit(-1);
  }

  if ( ! calc_matrix ) {

    if ( ! threshold_string.empty() ) {

      Log::instance()->warn( "Ignoring threshold - option only available with confusion matrix\n" );
    }
    if ( ignore_abs ) {

      Log::instance()->warn( "Ignoring ignore-abs - option only available with confusion matrix\n" );
    }
  }

  if ( ! calc_roc ) {

    if ( ! resolution_string.empty() ) {

      Log::instance()->warn( "Ignoring resolution - option only available with ROC curve\n" );
    }
    if ( ! max_omission_string.empty() ) {

      Log::instance()->warn( "Ignoring maximum omission - option only available with ROC curve\n" );
    }
    if ( abs_background ) {

      Log::instance()->warn( "Ignoring abs-background - option only available with ROC curve\n" );
    }
    if ( ! num_background_string.empty() ) {

      Log::instance()->warn( "Ignoring number of background points - option only available with ROC curve\n" );
    }
  }

  // Real work

  try {

    // Load algorithms and instantiate controller class
    AlgorithmFactory::searchDefaultDirs();

    SamplerPtr sampler;

    AlgorithmPtr alg;

    if ( ! request_file.empty() ) {

      // Loading input from XML request

      ConfigurationPtr input = Configuration::readXml( request_file.c_str() );

      alg = AlgorithmFactory::newAlgorithm( input->getSubsection( "Algorithm" ) );

      sampler = createSampler( input->getSubsection( "Sampler" ) );

      try {

        ConfigurationPtr statistics_param = input->getSubsection( "Statistics" );

        try {

          ConfigurationPtr matrix_param = statistics_param->getSubsection( "ConfusionMatrix" );

          calc_matrix = true;

          threshold = matrix_param->getAttributeAsDouble( "Threshold", CONF_MATRIX_DEFAULT_THRESHOLD );

          int ignore_absences_int = matrix_param->getAttributeAsInt( "IgnoreAbsences", 0 );

          if ( ignore_absences_int > 0 ) {

              ignore_abs = true;
          }
        }
        catch( SubsectionNotFound& e ) {

          UNUSED(e);
        }

        ConfigurationPtr roc_param;

        try {

          ConfigurationPtr roc_param = statistics_param->getSubsection( "RocCurve" );

          calc_roc = true;

          resolution = roc_param->getAttributeAsInt( "Resolution", -1 );

          num_background = roc_param->getAttributeAsInt( "BackgroundPoints", -1 );

          max_omission = roc_param->getAttributeAsDouble( "MaxOmission", 1.0 );

          int use_absences_as_background_int = roc_param->getAttributeAsInt( "UseAbsencesAsBackground", 0 );

          if ( use_absences_as_background_int > 0 ) {

            abs_background = true;
          }
        }
        catch( SubsectionNotFound& e ) {

          UNUSED(e);
        }
      }
      catch( SubsectionNotFound& e ) {

        // For backwards compatibility, calculate matrix and ROC if 
        // <Statistics> is not present
        calc_matrix = true;
        calc_roc = true;
        UNUSED(e);
      }
    }
    else {

      // Loading input from serialized model + TAB-delimited points file

      ConfigurationPtr input = Configuration::readXml( model_file.c_str() );

      alg = AlgorithmFactory::newAlgorithm( input->getSubsection( "Algorithm" ) );

      Log::instance()->debug( "Loading training sampler to get layers, label and spatial reference\n" );

      SamplerPtr training_sampler = createSampler( input->getSubsection( "Sampler" ) );

      // Get environment from training sampler
      EnvironmentPtr env = training_sampler->getEnvironment();

      // Get label and spatial reference from presence points of the training sampler
      OccurrencesPtr training_presences = training_sampler->getPresences();

      std::string label( training_presences->name() );
      std::string spatial_ref( training_presences->coordSystem() );

      Log::instance()->debug( "Loading test points %s %s\n", label.c_str(), spatial_ref.c_str() );

      OccurrencesReader* oc_reader = OccurrencesFactory::instance().create( points_file.c_str(), spatial_ref.c_str() );

      OccurrencesPtr presences = oc_reader->getPresences( label.c_str() );
      OccurrencesPtr absences = oc_reader->getAbsences( label.c_str() );

      delete oc_reader;

      // Create new sampler for test points
      sampler = createSampler( env, presences, absences );
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

    // Run tests

    Log::instance()->debug( "Starting tests\n" );

    int num_presences = sampler->numPresence();
    int num_absences = sampler->numAbsence();

    ConfusionMatrix matrix;

    // Confusion matrix can only be calculated with presence and/or absence points
    if ( calc_matrix && ( num_presences || num_absences ) ) {

      if ( threshold < 0.0 ) {

        matrix.setLowestTrainingThreshold( alg->getModel(), sampler );

        threshold = matrix.getThreshold();
      }

      matrix.reset( threshold, ignore_abs );

      matrix.calculate( alg->getModel(), sampler );
    }

    RocCurve roc_curve;

    // ROC curve can only be calculated with presence points
    // No absence points will force background points to be generated
    if ( calc_roc && num_presences ) {

      resolution = (resolution <= 0) ? ROC_DEFAULT_RESOLUTION : resolution;

      if ( abs_background ) {

        roc_curve.initialize( resolution, true );
      }
      else {

        if ( num_background > 0 ) {

          roc_curve.initialize( resolution, num_background );
        }	  
        else {

          roc_curve.initialize( resolution );
        }
      }

      roc_curve.calculate( alg->getModel(), sampler );
    }

    if ( calc_matrix && ! num_presences ) {

      Log::instance()->warn( "No presence points - ROC curve and omission error won't be calculated\n" );
    }

    if ( calc_matrix && ! num_absences ) {

      Log::instance()->warn( "No absence points - commission error won't be calculated\n" );
    }


    if ( calc_roc && ! num_presences ) {

      Log::instance()->warn( "No presence points - ROC curve won't be calculated\n" );
    }

    if ( calc_matrix ) {

      if ( num_presences || num_absences ) {

        Log::instance()->info( "\nModel statistics\n" );
        Log::instance()->info( "Accuracy:          %7.2f%%\n", matrix.getAccuracy() * 100 );
      }

      if ( num_presences ) {

        int omissions = matrix.getValue(0.0, 1.0);
        int total     = omissions + matrix.getValue(1.0, 1.0);

        Log::instance()->info( "Omission error:    %7.2f%% (%d/%d)\n", matrix.getOmissionError() * 100, omissions, total );
      }

      if ( num_absences ) {

        int commissions = matrix.getValue(1.0, 0.0);
        int total       = commissions + matrix.getValue(0.0, 0.0);

        Log::instance()->info( "Commission error:  %7.2f%% (%d/%d)\n", matrix.getCommissionError() * 100, commissions, total );
      }
    }

    if ( calc_roc ) {

      if ( num_presences ) {

        Log::instance()->info( "AUC:               %7.2f\n", roc_curve.getTotalArea() );

        if ( max_omission < 1.0 ) {

          Log::instance()->info( "Ratio:             %7.2f\n", roc_curve.getPartialAreaRatio( max_omission ) );
        }
      }
    }

    ConfigurationPtr output( new ConfigurationImpl("Statistics") );

    bool no_statistics = true;

    if ( calc_matrix && matrix.ready() ) {

      ConfigurationPtr cm_config( matrix.getConfiguration() );

      output->addSubsection( cm_config );

      no_statistics = false;
    }

    if ( calc_roc && roc_curve.ready() ) {

      ConfigurationPtr roc_config( roc_curve.getConfiguration() );

      output->addSubsection( roc_config );

      no_statistics = false;
    }

    if ( no_statistics )
    {
      Log::instance()->warn( "No statistics calculated\n" );
    }

    std::ostringstream test_output;

    Configuration::writeXml( output, test_output );

    std::cerr << flush;

    // Write test output to file, if requested
    if ( ! result_file.empty() ) {

      ofstream file( result_file.c_str() );
      file << test_output.str();
      file.close();
    }
    else {

      // Otherwise send it to stdout
      std::cout << test_output.str().c_str() << endl << flush;
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

    printf( "om_test aborted: %s\n", e.what() );
  }
}
