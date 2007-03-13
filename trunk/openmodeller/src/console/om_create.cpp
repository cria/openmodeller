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

using namespace std;

int main( int argc, char **argv ) {
    if (argc < 3) {
      cout << "Usage: "
          << argv[0]
          << " xmlinputfile"
          << " xmloutputfile"
          << " [logfile]"
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
      std::string myOutput=myConsoleXml.createModel(myRequest, dontLog);
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
