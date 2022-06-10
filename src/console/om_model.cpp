/*************************************************************************************
 *  A simple command line app to create an openModeller model
 *    -------------------
 *       begin                : November 2005
 *       copyright            : (C) 2005 by T.Sutton, Kevin Ruland, Renato De Giovanni
 *       email                : tim@linfiniti.com
 *************************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <openmodeller/om.hh>
#include <openmodeller/Log.hh>
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
  opts.addOption( "v", "version"     , "Display version info"                        , false );
  opts.addOption( "r", "xml-req"     , "Model creation request file in XML"          , true );
  opts.addOption( "m", "model-file"  , "File to store the generated model"           , true );
  opts.addOption( "" , "log-level"   , "Set the log level (debug, warn, info, error)", true );
  opts.addOption( "" , "log-file"    , "Log file"                                    , true );
  opts.addOption( "" , "prog-file"   , "File to store model creation progress"       , true );
  opts.addOption( "c", "config-file" , "Configuration file for openModeller"         , true );

  std::string log_level("info");
  std::string request_file;
  std::string model_file;
  std::string log_file;
  std::string progress_file;
  std::string config_file;

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
        printf( "om_model %s\n", om.getVersion().c_str() );
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
        log_level = opts.getArgs( option );
        break;
      case 4:
        log_file = opts.getArgs( option );
        break;
      case 5:
        progress_file = opts.getArgs( option );
        break;
      case 6:
        config_file = opts.getArgs( option );
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

  // om configuration
  if ( ! config_file.empty() ) { 

    Settings::loadConfig( config_file );
  }

  // Check parameters

  if ( request_file.empty() ) {

    printf( "Please specify a model creation request file in XML\n");
    exit(-1);
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

  // Real work

  try {

    // Load algorithms and instantiate controller class
    AlgorithmFactory::searchDefaultDirs();

    // If user wants to track progress
    if ( ! progress_file.empty() ) { 

      // Set callback to write to a file
      om.setModelCallback( progressFileCallback, &prog_data );
    }
    else if ( ! model_file.empty() ) {

      // Default callback will display progress on screen when a model file was specified
      // (which means the model won't be sent to stdout)
      om.setModelCallback( progressDisplayCallback );
    }

    ConfigurationPtr input = Configuration::readXml( request_file.c_str() );
    om.setModelConfiguration( input );

    om.createModel();

    om.calculateModelStatistics( input );

    ConfigurationPtr output = om.getModelConfiguration();

    std::ostringstream model_output;

    Configuration::writeXml( output, model_output );

    std::cerr << flush;

    // Write model output to file, if requested
    if ( ! model_file.empty() ) {

      ofstream file( model_file.c_str() );
      file << model_output.str();
      file.close();
    }
    else {

      // Otherwise send it to stdout
      std::cout << model_output.str().c_str() << endl << flush;
    }

    // If user wants to track progress
    if ( ! progress_file.empty() ) { 

      // Check if job was completed
      if ( prog_data.progress != 1 ) {

        // -2 means aborted
        progressFileCallback( -2.0, &prog_data );
      }
    }
  }
  catch ( runtime_error &e ) {

    // If user is tracking progress
    if ( ! progress_file.empty() ) { 

      // -2 means aborted
      progressFileCallback( -2.0, &prog_data );
    }

    printf( "om_model aborted: %s\n", e.what() );
  }
}
