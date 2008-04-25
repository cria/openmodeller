/**
 * Test class for Jackknife
 *
 * @authors Renato De Giovanni, Albert Massayuki Kuniyoshi and Missae
 * @date 2007-07-16
 * $Id: om_test_jackknife.h 4118 2008-03-12 17:18:58Z rdg $
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

#ifndef TEST_PRE_JACKKNIFE_HH
#define TEST_PRE_JACKKNIFE_HH

#include "cxxtest/TestSuite.h"
#include <openmodeller/om.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/Sample.hh>
#include <openmodeller/pre/PreParameters.hh>
#include <openmodeller/pre/Jackknife.hh>
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

      try {
        
        //
        // Go on to do the test now...
        //

        Log::instance()->setLevel( Log::Debug );
        Log::instance()->setCallback( new MyLog() );

        std::ostringstream myOutputStream ;
        AlgorithmFactory::searchDefaultDirs();
        OpenModeller om;

        //createModelRequest();
        std::string myInFileName("/tmp/model_request.xml");
        ConfigurationPtr c1 = Configuration::readXml( myInFileName.c_str() );
        om.setModelConfiguration(c1);

        PreParameters params;
        params.store( "Sampler", om.getSampler() );
        params.store( "Algorithm", om.getAlgorithm() );

        // TODO: pass threshold parameter

        Jackknife jackknife;

        TS_ASSERT( jackknife.Reset( params ) );

        TS_ASSERT( jackknife.Apply() );

        jackknife.ResetState( params);

        double out_param = 0;                  // <------ output 1
        std::multimap<double, int> out_params; // <------ output 2
        double mean = 0;                       // <------ output 3
        double variance = 0;                   // <------ output 4
        double std_deviation = 0;              // <------ output 5
        double jackknife_estimate = 0;         // <------ output 6
        double jackknife_bias = 0;             // <------ output 7

        TS_ASSERT( params.retrive( "out_param"    , out_param          ) );
        TS_ASSERT( params.retrive( "out_params"   , out_params         ) );
        TS_ASSERT( params.retrive( "out_Mean"     , mean               ) );
        TS_ASSERT( params.retrive( "out_Variance" , variance           ) );
        TS_ASSERT( params.retrive( "out_Deviation", std_deviation      ) );
        TS_ASSERT( params.retrive( "out_Estimate" , jackknife_estimate ) );
        TS_ASSERT( params.retrive( "out_Bias"     , jackknife_bias     ) );

        // TODO: add assertions for each returned value

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
};

#endif
