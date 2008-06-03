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

  char *args;
  int option;

  struct options opts[] = 
  {
    { 1, "version",    "Display version info",                          "v", 0 },
    { 2, "xml-req",    "Model creation request file in XML",            "r", 1 },
    { 3, "model-file", "File to store the generated model",             "m", 1 },
    { 4, "log-level",  "Set the log level (debug, warn, info, error)", NULL, 1 },
    { 5, "log-file",   "Log file",                                     NULL, 1 },
    { 6, "prog-file",  "File to store model creation progress",        NULL, 1 },
    { 0, NULL,         NULL,                                           NULL, 0 }
  };

  bool passed_params = false;

  std::string log_level("info");
  std::string request_file;
  std::string model_file;
  std::string log_file;
  std::string progress_file;

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
        printf("om_create 0.2.1\n");
        printf("This is free software; see the source for copying conditions. There is NO\n");
        printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
        exit(0);
        break;
      case 2:
        request_file = args;
        break;
      case 3:
        model_file = args;
        break;
      case 4:
        log_level = args;
        break;
      case 5:
        log_file = args;
        break;
      case 6:
        progress_file = args;
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
    getopts_usage( "om_create", opts );
    exit(0);
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

  // Real work

  try {

    // Load algorithms and instantiate controller class
    AlgorithmFactory::searchDefaultDirs();

    OpenModeller om;

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
      fprintf( stdout, model_output.str().c_str() );

      std::cout << endl << flush;
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
  catch ( runtime_error e ) {

    // If user is tracking progress
    if ( ! progress_file.empty() ) { 

      // -2 means aborted
      progressFileCallback( -2.0, &prog_data );
    }

    printf( "om_create aborted: %s\n", e.what() );
  }
}
