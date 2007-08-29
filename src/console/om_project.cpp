/***************************************************************************
 * This code simply loads a previously created model, loads a reprojection 
 * environment, and projects the model into that environment,
 * then writes the model out.
 *    -------------------
 *       begin                : November 2005
 *       copyright            : (C) 2005 by T.Sutton, Kevin Ruland
 *       email                : tim@linfiniti.com
 ****************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "consolexml.hh"

#include <time.h>    // used to limit the number of times that the progress is written to a file

/// Constants
#define MIN_INTERVAL 2.0   // in seconds

/// Forward declarations
void mapCallback( float progress, void * theFileName );

/// Globals
time_t gLastTime;
double gLastProgress = -3.0;

int main( int argc, char **argv ) {

  //  Original code:
  //
  //     if (argc != 4) {
  //       cout << "Usage: "
  //           << argv[0]
  //           << " <configfile-xml> as produced by openmodeller serialised model output "
  //           << " <projection-xml> as in environment.xml"
  //           << " <outputfilename>"
  //           << endl;
  //       return -1;
  //     }
  //     std::string myModel(argv[1]);
  //     std::string myEnvironment(argv[2]);
  //     std::string myOutput(argv[3]);
  //     ConsoleXml myConsoleXml;
  //     myConsoleXml.projectModel(myModel,myEnvironment,myOutput);

  if (argc < 4 ) {
    cout << "Usage: "
      << argv[0]
      << " xmlinputfile"
      << " mapfile"
      << " (logfile | statisticsfile logfile (progressfile) )"
      << endl;
    return -1;
  }

  std::string myProjectionXmlFile(argv[1]);
  std::string myMapFile(argv[2]);

  bool dontLog = false;

  Log::instance()->setLevel(Log::Info);
  Log::instance()->setPrefix("");

  // Write log to file, if requested
  FILE *flog = NULL;
  if (argc >= 4) {
    std::string myLog("");
    if (argc == 4) {
      // to keep backwards compatibility, if 3 arguments are passed then 
      // the last one will be considered the log!
      myLog.append(argv[3]);
    }
    else {
      // if more than 3 arguments are passed, log is the fourth one (last)
      myLog.append(argv[4]);
    }

    flog = fopen(myLog.c_str(), "w");

    if (flog == NULL) {
      fprintf(stderr, "Could not open log file!\n");
    }
    else {
      dontLog = true;
      Log::instance()->set(Log::Info, flog, "");
    }
  }

  { // Fake scope to destroy object in the end (to catch all logs)
    ConsoleXml myConsoleXml;

    if (argc == 4) {
      myConsoleXml.projectModel(myProjectionXmlFile,myMapFile,dontLog);
    }
    else if (argc == 5) {
      std::string myStatsFile(argv[3]);
      myConsoleXml.projectModel(myProjectionXmlFile,myMapFile,myStatsFile,dontLog);
    }
    else {
      std::string myStatsFile(argv[3]);
      std::string * myProgFile = new std::string(argv[5]);
      time(&gLastTime);
      // Always create initial file with status "queued" (-1)
      mapCallback( -1.0, myProgFile );
      myConsoleXml.projectModel(myProjectionXmlFile,myMapFile,myStatsFile,dontLog,mapCallback,myProgFile);
      // Check if job was completed
      if ( gLastProgress != 1 ) {
        // -2 means aborted
        mapCallback( -2.0, myProgFile );
      }
      delete myProgFile;
    }
  }

  // Close log file
  if (flog != NULL) {
    fclose(flog);
  }
}


void mapCallback( float progress, void *theFileName )
{
    if ( ! theFileName ) {
      return;
    }

    time_t currentTime;
    time(&currentTime);

    int myProgress = static_cast<int>(100*progress);

    if ( myProgress < 0 ) {
   
      myProgress /= 100;
    }

    if ( myProgress == -1 || myProgress == -2 || 
         myProgress == 0 || myProgress == 100 ||
         ( progress != gLastProgress && 
           difftime( currentTime, gLastTime ) > MIN_INTERVAL ) ) {
    
      std::string * fileName = (std::string *)theFileName;

      FILE *pFile = NULL;
      pFile = fopen(fileName->c_str(), "w");

      if (pFile == NULL) {
        // Could not open file...
      }
      else {
        char buffer[3];
        int ret;
        ret = sprintf(buffer, "%d", myProgress);
        fputs(buffer,pFile);
        fclose(pFile);
        gLastProgress = progress;
        gLastTime = currentTime;
      }
    }
}
