/*************************************************************************************
 * This code simply loads a previously created model, loads a reprojection 
 * environment, and projects the model into that environment,
 * then writes the model out.
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

int main( int argc, char **argv ) {

  Options opts;
  int option;

  // command-line parameters (short name, long name, description, take args)
  opts.addOption( "v", "version"  , "Display version info"                        , false );
  opts.addOption( "r", "xml-req"  , "Projection request file in XML"              , true );
  opts.addOption( "m", "dist-map" , "File to store the generated model"           , true );
  opts.addOption( "" , "log-level", "Set the log level (debug, warn, info, error)", true );
  opts.addOption( "" , "log-file" , "Log file"                                    , true );
  opts.addOption( "" , "prog-file", "File to store projection progress"           , true );
  opts.addOption( "" , "stat-file", "File to store projection statistics"         , true );

  std::string log_level("info");
  std::string request_file;
  std::string map_file;
  std::string log_file;
  std::string progress_file;
  std::string statistics_file;

  if ( ! opts.parse( argc, argv ) ) {

    opts.showHelp( argv[0] ); 
    exit(0);
  }

  while ( ( option = opts.cycle() ) >= 0 ) {

    switch ( option ) {

      case 0:
        printf("om_project 0.2.2\n");
        printf("This is free software; see the source for copying conditions. There is NO\n");
        printf("warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
        exit(0);
        break;
      case 1:
        request_file = opts.getArgs( option );
        break;
      case 2:
        map_file = opts.getArgs( option );
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
      case 8:
        statistics_file = opts.getArgs( option );
        break;
      default:
        break;
    }
  }

  // Check parameters

  if ( request_file.empty() ) {

    printf( "Please specify a projection request file in XML\n");
    exit(-1);
  }

  if ( map_file.empty() ) {

    printf( "Please specify a file to store the distribution map\n");
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
      om.setMapCallback( progressFileCallback, &prog_data );
    }
    else if ( ! statistics_file.empty() ) {

      // Default callback will display progress on screen when a statistics file was specified
      // (which means statistics won't be sent to stdout)
      om.setMapCallback( progressDisplayCallback );
    }

    std::ostringstream model_output;

    ConfigurationPtr input = Configuration::readXml( request_file.c_str() );
    om.setProjectionConfiguration( input );

    om.createMap( map_file.c_str() );

    AreaStats * stats = om.getActualAreaStats();

    ConfigurationPtr stats_cfg = stats->getConfiguration();

    std::ostringstream statistics_output;

    Configuration::writeXml( stats_cfg, statistics_output );

    std::cerr << flush;

    // Write statistics output to file, if requested
    if ( ! statistics_file.empty() ) {

      ofstream file( statistics_file.c_str() );
      file << statistics_output.str();
      file.close();
    }
    else {

      // Otherwise send it to stdout
      fprintf( stdout, statistics_output.str().c_str() );

      std::cout << endl << flush;
    }

    delete stats;

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

    printf( "om_project aborted: %s\n", e.what() );
  }
}
