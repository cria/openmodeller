/**
 * Test class for ChiSquare
 *
 * @authors Renato De Giovanni, Albert Massayuki Kuniyoshi and Missae
 * $Id: om_test_chisquare.h 4118 2008-03-12 17:18:58Z rdg $
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


#ifndef TEST_PRE_CHISQUARE_HH
#define TEST_PRE_CHISQUARE_HH

#include "cxxtest/TestSuite.h"
#include <openmodeller/Configuration.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/Sample.hh>
#include <openmodeller/om.hh>
#include <openmodeller/pre/PreParameters.hh>
#include <openmodeller/pre/PreChiSquare.hh>
#include <openmodeller/pre/PreAlgorithmFactory.hh>
#include <om_test_utils.h>
#include <string>
#include <map>

class MyLog : public Log::LogCallback 
{
  void operator()( Log::Level l, const std::string& msg ) 
  {
    std::cout << msg;
  }
};

class test_ChiSquare : public CxxTest :: TestSuite 
{
  public:

    void setUp (){
    }

    void tearDown (){
    }

    void test1 (){

      std::cout << std::endl;
      std::cout << "Testing chi-square..." << std::endl;

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

        PreParameters params;
        params.store( "Sampler", om.getSampler() );

		PreAlgorithm* preAlgPtr = PreAlgorithmFactory::make("PreChiSquare", params);

		TS_ASSERT(preAlgPtr != 0);

		TS_ASSERT( preAlgPtr->apply());

		preAlgPtr->resetState(params);

        std::vector< size_t > statistic;
		TS_ASSERT( params.retrive( "statistic", statistic ) );

		std::cout << std::endl;
		std::cout << "*************** chisquare results ****************** "  << std::endl;
		std::cout << std::endl; 
		std::cout << "layer number         statistic" << std::endl;
		std::cout << std::endl; 
		for (size_t i = 0; i < statistic.size(); ++i)
		{
			std::cout << "    " << i << "                    ";
			std::cout << statistic[i] << std::endl;
		}

		//input informations
		typedef std::map<string, string> stringMap;
		stringMap infoIn;
		preAlgPtr->getAcceptedParameters( infoIn ); 
		std::map<string, string>::const_iterator pos;
        std::cout << std::endl;
		std::cout << "input information "  << std::endl;
        std::cout << std::endl;
		for (pos = infoIn.begin(); pos != infoIn.end(); ++pos)
		{
			std::cout << pos->first << "   ";
		    std::cout << pos->second << std::endl;
		}

		//output informations for each layer
		stringMap infoOut;
 		preAlgPtr->getLayerResultSpec( infoOut );
        std::cout << std::endl;
		std::cout << "output information "  << std::endl;
        std::cout << std::endl;
		for (pos = infoOut.begin(); pos != infoOut.end(); ++pos)
		{
			std::cout << pos->first << "   ";
		    std::cout << pos->second << std::endl;
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

