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

#include "consolexml.cpp"


int main( int argc, char **argv ) {

 
    if (argc != 4) {
      cout << "Usage: "
          << argv[0]
          << " <configfile-xml> as produced by openmodeller serialised model output "
          << " <projection-xml> as in environment.xml"
          << " <outputfilename>"
          << endl;
      return -1;
    }
    std::string myModel(argv[1]);
    std::string myEnvironment(argv[2]);
    std::string myOutput(argv[3]);
    projectModel(myModel,myEnvironment,myOutput);
}
