/**
 * Test class for creating test request files
 * 
 * @authors Tim Sutton and Albert Massayuki Kuniyoshi
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
 * \brief Utility methods for creating test data.
 * @NOTE this is not a test - just an include file to put into 
 *       tests that need a model configuration.
 */

#ifndef TEST_UTILS_HH
#define TEST_UTILS_HH

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

bool createModelRequest(){
	try 
	{
		//Note that EXAMPLE_DIR and TEST_DATA_DIR is a compiler define 
		//created by CMakeLists.txt
		std::string myConfigFile(TEST_DATA_DIR);
		myConfigFile.append("/model_request.xml");
		//read in the file, replace all instances of token [EXAMPLE_DIR]
		//and [TEST_DATA_DIR] with those provided by the compiler defines
		//then save the file to the /tmp dir and use that as the model 
		//configuration file.
		std::string myExamplesToken("[EXAMPLE_DIR]");
		std::string myTestDataToken("[TEST_DATA_DIR]");
		std::string myExamplesValue(EXAMPLE_DIR);
		myExamplesValue.append("/");
		std::string myTestDataValue(TEST_DATA_DIR);
		myTestDataValue.append("/");
		std::ifstream myInFile (myConfigFile.c_str(), std::ios_base::in);
		std::string myInFileName("/tmp/model_request.xml");
		std::ofstream myOutFile(myInFileName.c_str());
		std::string myLine;
		while (getline(myInFile,myLine,'\n'))
		{
			std::string::size_type myPos=0;
			while ( (myPos = myLine.find(myExamplesToken,myPos)) != std::string::npos)
			{
				std::cout << "Replacing examples token at : " << myPos << "\n";
				myLine.replace( myPos, myExamplesToken.length(), myExamplesValue );
			}
			myPos=0;
			while ( (myPos = myLine.find(myTestDataToken,myPos)) != std::string::npos)
			{
				std::cout << "Replacing test data token at : " << myPos << "\n";
				myLine.replace( myPos, myTestDataToken.length(), myTestDataValue );
			}
			myOutFile << myLine.c_str();;
		}
		myInFile.close();
		myOutFile.close();
                return true;
	}
	catch( std::exception& e ) {
		std::string myError("Exception caught!\n");
		std::cout << "Exception caught!" << std::endl;
		std::cout << e.what() << std::endl;
		myError.insert(myError.length(),e.what());
		return false;
	}
}

#endif //TEST_UTILS_HH

