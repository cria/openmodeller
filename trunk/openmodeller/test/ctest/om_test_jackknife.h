/**
 * Test class for Configuration
 * 
 * @authors Ricardo Scachetti Pereira and Albert Massayuki Kuniyoshi
 * @date 2007-07-16
 * $Id: om_test_configuration.h 3614 2007-08-27 18:21:00Z albertmk $
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2007 by CRIA -
 * Centro de Referencia em Informacao Ambiental
 *
 * http://www.cria.org.br
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details:
 * 
 * http://www.gnu.org/copyleft/gpl.html
 */

/** \ingroup test
 * \brief Test for Configuration Class
 */

#ifndef TEST_JACKKNIFE_HH
#define TEST_JACKKNIFE_HH

#include "cxxtest/TestSuite.h"
#include <openmodeller/Configuration.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/Sample.hh>
#include <openmodeller/om.hh>
#include <om_test_utils.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

class MyLog : public Log::LogCallback 
{
  void operator()( Log::Level l, const std::string& msg ) 
  {
    std::cout << msg;
  }
};

class test_Jackknife : public CxxTest :: TestSuite 
{
  public:
    void setUp (){
    }

    void tearDown (){
    }

    void test1 (){
      std::cout << std::endl;
      std::cout << "Testing jackknife..." << std::endl;
      try 
      {
        
        //
        // Go on to do the test now...
        //

        Log::instance()->setLevel( Log::Debug );
        Log::instance()->setCallback( new MyLog() );

        std::ostringstream myOutputStream ;
        AlgorithmFactory::searchDefaultDirs();
        OpenModeller om;

        //createModelRequest();
        //std::string myInFileName("/tmp/model_request.xml");
        std::string myInFileName("/tmp/model_request_chi.xml");
        ConfigurationPtr c1 = Configuration::readXml( myInFileName.c_str() );
        om.setModelConfiguration(c1);
        //om.jackknife( om.getSampler(), om.getAlgorithm(), 0.90 );
        om.mychisquare( om.getSampler() );
        return ;
      }
      catch( std::exception& e ) {
        std::string myError("Exception caught!\n");
        std::cout << "Exception caught!" << std::endl;
        std::cout << e.what() << std::endl;
        myError.insert(myError.length(),e.what());
        return ;
      }
    }


  private:
};

#endif

