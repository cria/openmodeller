/**
 * Test class for creating test request files
 * 
 * @authors Tim Sutton and Albert Massayuki Kuniyoshi
 * $Id$
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
#include <stdexcept>

std::string prepareTempFile( std::string templateFileName ) {

  try 
  {
    std::string myInFileName(TEST_DATA_DIR);
    myInFileName.append( "/" );
    myInFileName.append( templateFileName );

    //read in the file, replace all instances of token [EXAMPLE_DIR]
    //and [TEST_DATA_DIR] with those provided by the compiler defines
    //then save the file to the /tmp dir and use that as the model 
    //configuration file.
    std::ifstream myInFile( myInFileName.c_str(), std::ios_base::in );

    if ( myInFile.fail() ) {

      std::string msg( "Could not open template file: " );
      msg.append( myInFileName );
      throw std::runtime_error( msg.c_str() );
    }

    std::string myOutFileName( "/tmp/" );
    myOutFileName.append( templateFileName );

    // Check if temporary file already exists
    std::fstream myOutFileTempHandle;
    myOutFileTempHandle.open( myOutFileName.c_str(), std::ios_base::in );

    if ( myOutFileTempHandle.is_open() ) {

      // Do nothing is temp file exists
      myOutFileTempHandle.close();
      return myOutFileName;
    }

    myOutFileTempHandle.close();

    // Create temporary file
    std::ofstream myOutFile( myOutFileName.c_str() );

    if ( myOutFile.fail() ) {

      myInFile.close();
      std::string msg( "Could not open temporary file: " );
      msg.append( myOutFileName );
      throw std::runtime_error( msg.c_str() );
    }

    // Tokens
    std::string myExamplesToken("[EXAMPLE_DIR]");
    std::string myTestDataToken("[TEST_DATA_DIR]");
    std::string myExamplesValue( EXAMPLE_DIR );
    std::string myTestDataValue( TEST_DATA_DIR );

    std::string myLine;

    while ( getline( myInFile, myLine ) ) {

      std::string::size_type myPos = 0;

      while ( ( myPos = myLine.find( myExamplesToken,myPos ) ) != std::string::npos ) {

        myLine.replace( myPos, myExamplesToken.length(), myExamplesValue );
      }

      myPos=0;

      while ( ( myPos = myLine.find( myTestDataToken,myPos ) ) != std::string::npos ) {

        myLine.replace( myPos, myTestDataToken.length(), myTestDataValue );
      }

      myOutFile << myLine.c_str() << std::endl;
    }

    myInFile.close();
    myOutFile.close();

    return myOutFileName;
  }
  catch( std::exception& e ) {

    std::cout << "Exception caught!" << std::endl;
    std::cout << e.what() << std::endl;
    throw e;
  }
}

#endif //TEST_UTILS_HH

