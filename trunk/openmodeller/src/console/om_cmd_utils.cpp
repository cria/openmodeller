/**
 * Definition of command-line utility functions.
 * 
 * @author Renato De Giovanni (renato at cria . org . br)
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2008 by CRIA -
 * Centro de Referencia em Informacao Ambiental
 *
 * http://www.cria.org.br
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details:
 * 
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <om_cmd_utils.hh>
#include <time.h>

using std::ios_base;

// get Log Level
Log::Level getLogLevel( std::string level ) {

    if ( level == "debug" ) {

      return Log::Debug;
    }
    else if ( level == "info" ) {

      return Log::Info;
    }
    else if ( level == "warn" ) {

      return Log::Warn;
    }
    else if ( level == "error" ) {

      return Log::Error;
    }
    else {

      printf( "Unrecognized log level (%s). Using default value.\n", level.c_str() );
      return Log::Default;
    }
}

// Display progress on screen
void progressDisplayCallback( float progress, void *extraParam )
{
    char buffer[9];

    sprintf( buffer, "%07.4f", 100 * progress );

    std::cout << "Progress: " << buffer << "% \r" << flush;
}

// Progress callback
void progressFileCallback( float progress, void *progressData )
{
    if ( ! progressData ) {

      return;
    }

    progress_data * prog_data = (progress_data *)progressData;

    time_t current_time;
    time( &current_time );

    int my_progress = static_cast<int>(100*progress);

    if ( my_progress < 0 ) {
   
      my_progress /= 100;
    }

    if ( my_progress == -1 || my_progress == -2 || 
         my_progress == 0 || my_progress == 100 ||
         ( progress != prog_data->progress && 
           difftime( current_time, prog_data->timestamp ) > MIN_PROGRESS_INTERVAL ) ) {
    
      FILE *p_file = NULL;
      p_file = fopen( prog_data->file_name.c_str(), "w" );

      if ( p_file == NULL ) {

        // Could not open file
        // todo: send message to cerr but avoid doing this every time the callback is called
        //cerr << "Could not open progress file" << endl;
      }
      else {

        char buffer[4];
        int ret;
        ret = sprintf( buffer, "%d", my_progress );
        fputs( buffer, p_file );
        fclose( p_file );
        prog_data->progress = progress;
        prog_data->timestamp = current_time;
      }
    }
}
