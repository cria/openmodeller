/***************************************************************************
 *  A simple command line app to create an openModeller model
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
#include <fstream>   // file I/O for XML
#include <stdio.h>   // file I/O for log
#include <time.h>    // used to limit the number of times that the progress is written to a file

using namespace std;

/// Constants
#define MIN_INTERVAL 2.0   // in seconds

/// Forward declarations
void modelCallback( float progress, void * theFileName );

/// Globals
time_t gLastTime;
double gLastProgress = -3.0;

int main( int argc, char **argv ) {
    if (argc < 3) {
      cout << "Usage: "
          << argv[0]
          << " xmlinputfile"
          << " xmloutputfile"
          << " [logfile]"
          << " [callbackfile]"
          << endl;
      return -1;
    }
  
    std::string myRequest(argv[1]);
    std::string myFileName(argv[2]);

    bool dontLog = false;

    // Write log to file, if requested
    FILE *flog = NULL;
    if (argc == 4) {
      std::string myLog(argv[3]);
      flog = fopen(myLog.c_str(), "w");

      if (flog == NULL) {
        fprintf(stderr, "Could not open log file!\n");
      }
      else {
        dontLog = true;
        Log::instance()->set(Log::Info, flog, "");
      }
    }

    { // Fake scope to force obj destruction in the end (to catch logs)
      ConsoleXml myConsoleXml;
      std::string myOutput;
      if (argc == 5) { 
        std::string * myProgFile = new std::string(argv[4]);
        time(&gLastTime);
        // Always create initial file with status "queued" (-1)
        modelCallback( -1.0, myProgFile );
        myOutput=myConsoleXml.createModel(myRequest, dontLog, modelCallback, myProgFile);
        // Check if job was completed
        if ( gLastProgress != 1 ) {
          // -2 means aborted
          modelCallback( -2.0, myProgFile );
        }
        delete myProgFile;
      }
      else {
        myOutput=myConsoleXml.createModel(myRequest, dontLog);
      }
     //write output to file
      ofstream file(myFileName.c_str());
      file << myOutput;
      file.close();
    }
    
    // Close log file
    if (flog != NULL) {
      fclose(flog);
    }
}


void modelCallback( float progress, void *theFileName )
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
