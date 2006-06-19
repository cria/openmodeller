/***************************************************************************
 *  A simple command line app to create an openModeller model
 *    -------------------
 *       begin                : June 2006
 *       copyright            : (C) 2006 by T.Sutton
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
    if (argc != 2) {
      cout << "Usage: "
          << argv[0]
          << " <outputfilename> a file in which the xml outputs will be stored"
          << endl;
      return -1;
    }
  
    std::string myFileName(argv[1]);
    ConsoleXml myConsoleXml;
    std::string myOutput=myConsoleXml.getAllAlgorithmMetadataXml();
    //write output to file
    ofstream file( myFileName.c_str());
    file << myOutput;
    file.close();
  
}
