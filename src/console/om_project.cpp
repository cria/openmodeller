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

    if (argc < 3 ) {
      cout << "Usage: "
          << argv[0]
          << " xmlinputfile"
          << " mapfile"
          << " [logfile]"
          << endl;
      return -1;
    }

    std::string myProjectionXmlFile(argv[1]);
    std::string myMapFile(argv[2]);

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
        g_log.set(Log::Info, flog, "");
      }
    }

    ConsoleXml myConsoleXml;

    myConsoleXml.projectModel(myProjectionXmlFile,myMapFile,dontLog);
}
