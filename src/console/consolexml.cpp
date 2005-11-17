/***************************************************************************
 * This file contains suporting functions for creating and projecting models 
 * using xml request files.
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

#ifndef CONSOLEXML_CPP
#define CONSOLEXML_CPP
#include <openmodeller/om.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/MapFormat.hh>
#include <istream>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <iostream.h>  // I/O 
#include <fstream.h>   // file I/O
#include <sstream>     // treat string as a stream

#include <stdexcept>
using namespace std;

class MyLog : public Log::LogCallback 
{
  void operator()( Log::Level l, const std::string& msg ) 
  {
    cout << msg;
  }
};

void helloWorld()
{

  std::cout << "Hello world";
}

/** Create a model and return the result as a string.
 * We do it like this instead of writing straight to a file
 * so that we can pass the result cleanly to third part apps
 * using this lib, such as the JNI interface. 
 * */
std::string createModel(const std::string myConfigFile)
{
  try 
  {
    g_log.setLevel( Log::Debug );
    g_log.setCallback( new MyLog() );

    ostringstream myOutputStream ;

    AlgorithmFactory::searchDefaultDirs();

    OpenModeller om;
    {
      cout << "Loading Test file " << myConfigFile << endl;
      ConfigurationPtr c = Configuration::readXml( myConfigFile.c_str() );
      om.setConfiguration(c);
    }
    om.createModel();
    {
      ConfigurationPtr c = om.getConfiguration();
      Configuration::writeXml( c, myOutputStream);
    }
    return myOutputStream.str();
  }
  catch( exception& e ) {
    std::string myError("Exception caught!\n");
    myError.insert(myError.length(),e.what());
    return myError;
  }
}

int projectModel(const std::string theModel, const  std::string theEnvironment, const std::string theOutput)
{
  try {

    g_log.setLevel( Log::Debug );

    AlgorithmFactory::searchDefaultDirs();

    OpenModeller om;
    cout << "Loading Model file " << theModel << endl;

    ConfigurationPtr c = Configuration::readXml( theModel.c_str() );

    om.setConfiguration(c);


    cout << "Create Model" << endl;
    om.createModel();

    cout << "Loading Projection Environment " << theEnvironment << endl;

    c = Configuration::readXml( theEnvironment.c_str() );

    EnvironmentPtr e = createEnvironment( c );

    cout << "Projecting to file " << theOutput << endl;

    om.createMap( e, theOutput.c_str() );

    return 0;
  }
  catch( exception& e ) {
    cerr << "Exception Caught" << endl;
    cerr << e.what() << endl;
    return 0;
  }
}
#endif
