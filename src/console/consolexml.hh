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
#include <string>
using namespace std;
class dllexp ConsoleXml
{
    public:
        ConsoleXml(){} ;
        ~ConsoleXml(){};
        /** Create a model and return the result as a string.
         * We do it like this instead of writing straight to a file
         * so that we can pass the result cleanly to third part apps
         * using this lib, such as the JNI interface. 
         * */
        std::string createModel(const std::string myConfigFile, bool ignoreLog=false);
        /** Project a model into a set of environment layers
         * and put the output image into theOutput */
        bool projectModel(const std::string theModel, const std::string theEnvironment, const std::string theOutput);
        /** Project a model given a single projection XML file as input 
         * and put the output image into mapFile. Third argument indicates 
         * if log should be ignored  */
        bool projectModel(const std::string projectionXmlFile, const std::string mapFile, bool ignoreLog);
         /** Project a model given a single projection XML file as input 
         * and put, the output image into mapFile and a file to store projection 
         * statistics. Fourth argument indicates if log should be ignored */
        bool projectModel(const std::string projectionXmlFile, const std::string mapFile, const std::string statisticsXmlFile, bool ignoreLog);
       /** Get a string containing xml metadata for all algorithms
         */
        std::string getAllAlgorithmMetadataXml();
};
#endif
