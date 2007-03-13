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

#include <consolexml.hh>
#include <istream>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>  // I/O 
#include <fstream>   // file I/O
#include <sstream>     // treat string as a stream

#include <stdexcept>

class MyLog : public Log::LogCallback 
{
  void operator()( Log::Level l, const std::string& msg ) 
  {
    cout << msg;
  }
};

/** Create a model and return the result as a string.
 * We do it like this instead of writing straight to a file
 * so that we can pass the result cleanly to third part apps
 * using this lib, such as the JNI interface. 
 * */
std::string ConsoleXml::createModel(const std::string myConfigFile, bool ignoreLog)
{
  try 
  {
    if (!ignoreLog)
    {
      Log::instance()->setLevel( Log::Debug );
      Log::instance()->setCallback( new MyLog() );
    }

    ostringstream myOutputStream ;

    AlgorithmFactory::searchDefaultDirs();

    OpenModeller om;
    {
      cout << "Loading Test file " << myConfigFile << endl;
      ConfigurationPtr c = Configuration::readXml( myConfigFile.c_str() );
      om.setModelConfiguration(c);
    }
    om.createModel();
    {
      ConfigurationPtr c = om.getModelConfiguration();
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

bool ConsoleXml::projectModel(const std::string theModel, const std::string theEnvironment, const std::string theOutput)
{  
  try {

    Log::instance()->setLevel( Log::Debug );
    Log::instance()->setCallback( new MyLog() );

    AlgorithmFactory::searchDefaultDirs();

    OpenModeller om;
    cout << "Loading Model file " << theModel << endl;

    ConfigurationPtr c = Configuration::readXml( theModel.c_str() );

    om.setModelConfiguration(c);


    cout << "Create Model" << endl;
    om.createModel();

    cout << "Loading Projection Environment " << theEnvironment << endl;

    c = Configuration::readXml( theEnvironment.c_str() );

    EnvironmentPtr e = createEnvironment( c );

    cout << "Projecting to file " << theOutput << endl;

    om.createMap( e, theOutput.c_str() );

    return true;
  }
  catch( exception& e ) {
    cerr << "Exception Caught" << endl;
    cerr << e.what() << endl;
    return false;
  }
}

bool ConsoleXml::projectModel(const std::string projectionXmlFile, const std::string mapFile, bool ignoreLog)
{  
  try {

    if (!ignoreLog)
    {
      Log::instance()->setLevel( Log::Debug );
      Log::instance()->setCallback( new MyLog() );
    }

    AlgorithmFactory::searchDefaultDirs();

    OpenModeller om;
    cout << "Loading XML Input file " << projectionXmlFile << endl;

    ConfigurationPtr c = Configuration::readXml( projectionXmlFile.c_str() );

    om.setProjectionConfiguration(c);

    cout << "Projecting to file " << mapFile << endl;

    om.createMap( mapFile.c_str() );

    return true;
  }
  catch( exception& e ) {
    cerr << "Exception Caught" << endl;
    cerr << e.what() << endl;
    return false;
  }
}

bool ConsoleXml::projectModel(const std::string projectionXmlFile, const std::string mapFile, const std::string statisticsXmlFile, bool ignoreLog)
{  
  try {

    if (!ignoreLog)
    {
      Log::instance()->setLevel( Log::Debug );
      Log::instance()->setCallback( new MyLog() );
    }

    AlgorithmFactory::searchDefaultDirs();

    OpenModeller om;
    cout << "Loading XML Input file " << projectionXmlFile << endl;

    ConfigurationPtr c = Configuration::readXml( projectionXmlFile.c_str() );

    om.setProjectionConfiguration(c);

    cout << "Projecting to file " << mapFile << endl;

    om.createMap( mapFile.c_str() );

    cout << "Saving statistics into " << statisticsXmlFile << endl;

    AreaStats * stats = om.getActualAreaStats();

    ConfigurationPtr areaStatsCfg = stats->getConfiguration();

    Configuration::writeXml( areaStatsCfg, statisticsXmlFile.c_str() );

    delete stats;

    return true;
  }
  catch( exception& e ) {
    cerr << "Exception Caught" << endl;
    cerr << e.what() << endl;
    return false;
  }
}

std::string ConsoleXml::getAllAlgorithmMetadataXml()
{
  try {

    Log::instance()->setLevel( Log::Debug );
    Log::instance()->setCallback( new MyLog() );

    AlgorithmFactory::searchDefaultDirs();
    ostringstream myOutputStream ;
    ConfigurationPtr cfg = AlgorithmFactory::getConfiguration();
    Configuration::writeXml( cfg, myOutputStream);
    return myOutputStream.str();
  }
  catch( exception& e ) {
    cerr << "Exception Caught" << endl;
    cerr << e.what() << endl;
    return false;
  }
}
