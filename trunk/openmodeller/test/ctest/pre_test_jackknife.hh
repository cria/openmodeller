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
#include <openmodeller/Log.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/Sample.hh>
#include <openmodeller/pre/PreParameters.hh>
#include <openmodeller/pre/PreJackknife.hh>
#include <openmodeller/pre/PreAlgorithmFactory.hh>
#include <om_test_utils.h>
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

        AlgorithmFactory::searchDefaultDirs();
        OpenModeller om;

        std::string myInFileName = prepareTempFile( "model_request.xml" );
        ConfigurationPtr c1 = Configuration::readXml( myInFileName.c_str() );
        om.setModelConfiguration( c1 );

        SamplerPtr samp = om.getSampler();

        PreParameters params;
        params.store( "Sampler", samp );
        params.store( "Algorithm", om.getAlgorithm() );
        params.store( "PropTrain", 0.9 );

        PreAlgorithm* preAlgPtr = PreAlgorithmFactory::make("PreJackknife", params);

        TS_ASSERT(preAlgPtr != 0);

        TS_ASSERT( preAlgPtr->apply());

        preAlgPtr->resetState(params);

        double ref_accuracy = 0;
        double mean = 0;
        double variance = 0;
        double std_deviation = 0;
        double jackknife_estimate = 0;
        double jackknife_bias = 0;

        TS_ASSERT( params.retrieve( "Accuracy" , ref_accuracy       ) );
        TS_ASSERT( params.retrieve( "Mean"     , mean               ) );
        TS_ASSERT( params.retrieve( "Variance" , variance           ) );
        TS_ASSERT( params.retrieve( "Deviation", std_deviation      ) );
        TS_ASSERT( params.retrieve( "Estimate" , jackknife_estimate ) );
        TS_ASSERT( params.retrieve( "Bias"     , jackknife_bias     ) );

        //input informations
        typedef std::map<string, string> stringMap;
        stringMap infoIn;
        preAlgPtr->getAcceptedParameters(infoIn);
        std::map<string, string>::const_iterator pos;
        std::cout << std::endl;
        std::cout << "input information "  << std::endl;
        std::cout << std::endl;
        for (pos = infoIn.begin(); pos != infoIn.end(); ++pos)
        {
            std::cout << pos->first << "   ";
            std::cout << pos->second << std::endl;
        }

        //output set informations
        stringMap infoSetOut;
        preAlgPtr->getLayersetResultSpec(infoSetOut);
        std::cout << std::endl;
        std::cout << "output set information "  << std::endl;
        std::cout << std::endl;
        for (pos = infoSetOut.begin(); pos != infoSetOut.end(); ++pos)
        {
            std::cout << pos->first << "   ";
            std::cout << pos->second << std::endl;
        }

        //output information for each layer
        stringMap infoOut;
        preAlgPtr->getLayerResultSpec(infoOut);

        int num_layers = samp->numIndependent();

        for ( int i = 0; i < num_layers; ++i )
        {
            string layer_id = samp->getEnvironment()->getLayerPath(i);

            std::cout << std::endl << layer_id << std::endl;

            PreParameters result;
            preAlgPtr->getLayerResult( layer_id, result );

            for (pos = infoOut.begin(); pos != infoOut.end(); ++pos)
            {
                double accuracy = 0;
                result.retrieve( "Accuracy without layer", accuracy );

                std::cout << pos->first << ": ";
                std::cout << accuracy << std::endl;
            }
        }

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
