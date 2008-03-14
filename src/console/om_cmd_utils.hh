/**
 * Declaration of command-line utility functions.
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

#ifndef OM_CMD_UTILS_HH
#define OM_CMD_UTILS_HH

#include <openmodeller/Log.hh>

#include <string>

using namespace std;

// Minimum delay interval in seconds between two progress callbacks 
// before any progress-related action
#define MIN_PROGRESS_INTERVAL 2.0

struct progress_data {

  std::string file_name;
  time_t timestamp;
  float progress;

};

// Return the corresponding Log::Level from a human friendly string representation 
// (debug, warn, info, error)
Log::Level getLogLevel( std::string level );

// Custom callback to display job progress on the screen
void progressDisplayCallback( float progress, void *extraParam );

// Custom callback to track job progress by writing it into a file
void progressFileCallback( float progress, void *progressFile );

#endif
