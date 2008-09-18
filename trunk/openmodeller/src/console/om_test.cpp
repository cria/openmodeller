#include <openmodeller/om.hh>

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
  opts.addOption( "v", "version"   , "Display version info"                        , false );
  opts.addOption( "r", "xml-req"   , "(option 1) Test request file in XML"         , true );
  opts.addOption( "", "model-file" , "(option 2) Serialized model file"            , true );
  opts.addOption( "", "points-file", "(option 2) TAB-delimited file with points"   , true );
  opts.addOption( "", "result-file", "File to store test result in XML"            , true );
  opts.addOption( "", "log-level"  , "Set the log level (debug, warn, info, error)", true );
  opts.addOption( "", "log-file"   , "Log file"                                    , true );

  std::string log_level("info");
  std::string request_file;
  std::string model_file;
  std::string points_file;
  std::string result_file;
  std::string log_file;

  if ( ! opts.parse( argc, argv ) ) {

    opts.showHelp( argv[0] );
  }

  while ( ( option = opts.cycle() ) >= 0 ) {

    switch ( option ) {

      case 0:
        printf("om_test 0.1\n");
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

  if ( ! request_file.empty() ) {

    if ( ! model_file.empty() ) {

      Log::instance()->warn( "Model file parameter will be ignored (using XML request instead)\n" );
    }
    if ( ! points_file.empty() ) {

      printf( "Points file parameter will be ignored (using XML request instead)\n");
    }
  }
  else if ( ( ! model_file.empty() ) && ! points_file.empty() ) {

    // OK here. Condition used just to allow the else below.
  }
  else {

    printf( "Please specify either a test request file in XML or a serialized model and a TAB-delimited file with the points to be tested\n");
    exit(-1);
  }

  // Real work

  try {

    // Load algorithms and instantiate controller class
    AlgorithmFactory::searchDefaultDirs();

    OpenModeller om;

    SamplerPtr sampler;

    AlgorithmPtr alg;

    if ( ! request_file.empty() ) {

      // Loading input from XML request

      ConfigurationPtr input = Configuration::readXml( request_file.c_str() );

      alg = AlgorithmFactory::newAlgorithm( input->getSubsection( "Algorithm" ) );

      sampler = createSampler( input->getSubsection( "Sampler" ) );
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

    // Run tests

    int num_presences = sampler->numPresence();
    int num_absences = sampler->numAbsence();

    ConfusionMatrix matrix;

    // Confusion matrix can only be calculated with presence and/or absence points
    if ( num_presences || num_absences ) {

      matrix.calculate( alg->getModel(), sampler );
    }

    RocCurve roc_curve;

    // ROC curve can only be calculated with presence and absence points
    if ( num_presences && num_absences ) {

      roc_curve.calculate( alg->getModel(), sampler );
    }

    if ( ! num_presences ) {

      Log::instance()->warn( "No presence points - omission error won't be calculated\n" );
    }

    if ( ! num_absences ) {

      Log::instance()->warn( "No absence points - ROC curve and commission error won't be calculated\n" );
    }

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

      Log::instance()->info( "AUC:               %7.2f\n", roc_curve.getArea() );
    }

    ConfigurationPtr output( new ConfigurationImpl("Statistics") );

    if ( matrix.ready() ) {

      ConfigurationPtr cm_config( matrix.getConfiguration() );

      output->addSubsection( cm_config );
    }

    if ( num_absences && roc_curve.ready() ) {

      ConfigurationPtr roc_config( roc_curve.getConfiguration() );

      output->addSubsection( roc_config );
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
      fprintf( stdout, test_output.str().c_str() );

      std::cout << endl << flush;
    }
  }
  catch ( runtime_error e ) {

    printf( "om_test aborted: %s\n", e.what() );
  }
}
