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
#include <fstream>   // file I/O

using namespace std;

int main( int argc, char **argv ) {
    if (argc != 3) {
      cout << "Usage: "
          << argv[0]
          << " <configfile-xml> a openmodeller xml request file "
          << " <outputfilename> a file in which the xml outputs will be stored"
          << endl;
      return -1;
    }
  
    std::string myRequest(argv[1]);
    std::string myFileName(argv[2]);
    ConsoleXml myConsoleXml;
    std::string myOutput=myConsoleXml.createModel(myRequest);
    //write output to file
    ofstream file( myFileName.c_str());
    file << myOutput;
    file.close();
  
}
