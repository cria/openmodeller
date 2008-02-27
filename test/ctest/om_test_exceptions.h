/**
 * Test class for Exceptions
 * 
 * @author Albert Massayuki Kuniyoshi
 * @date 2007-07-10
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
* \brief Test for Exceptions Class
*/

#ifndef TEST_EXCEPTIONS_HH
#define TEST_EXCEPTIONS_HH

#include "cxxtest/TestSuite.h"
#include "Exceptions.hh"
// Microsoft VC8 requires <string> before <stdexcept> in order for
// this code to compile.
#include <string>
#include <stdexcept>

class test_Exceptions : public CxxTest :: TestSuite 
{

  public:
    void setUp (){

    }

    void tearDown (){

    }

    void test1 (){
      std::cout << std::endl;
      std::cout << "Testing OmException( const std::string& msg ) ..." << std::endl;
      std::string msg = "Testing Exception";
      TS_ASSERT_EQUALS(OmException("Testing Exception").what(),msg);
    }

    void test2 (){
      std::cout << std::endl;
      std::cout << "Testing ConfigurationException( const std::string& msg ) ..." << std::endl;
      std::string msg = "Testing Exception";
      TS_ASSERT_EQUALS(ConfigurationException("Testing Exception").what(),msg);
    }

    void test3 (){
      std::cout << std::endl;
      std::cout << "Testing AttributeNotFound( const std::string& attrName ) ..." << std::endl;
      std::string attr = "SomeAttribute";
      std::string msg = "Attribute " + attr + " not found";
      TS_ASSERT_EQUALS(AttributeNotFound(attr).what(),msg);
      TS_ASSERT_EQUALS(AttributeNotFound(attr).getName(),attr);
    }

    void test4 (){
      std::cout << std::endl;
      std::cout << "Testing SubsectionNotFound( const std::string& attrName ) ..." << std::endl;
      std::string subs = "SomeSubsection";
      std::string msg = "Subsection " + subs + " not found";
      TS_ASSERT_EQUALS(SubsectionNotFound(subs).what(),msg);
      TS_ASSERT_EQUALS(SubsectionNotFound(subs).getName(),subs);
    }

    void test5 (){
      std::cout << std::endl;
      std::cout << "Testing InvalidType( const std::string& msg ) ..." << std::endl;
      std::string msg = "Testing Exception";
      TS_ASSERT_EQUALS(InvalidType("Testing Exception").what(),msg);
    }

    void test6 (){
      std::cout << std::endl;
      std::cout << "Testing XmlParseException( const std::string& msg ) ..." << std::endl;
      std::string msg = "Testing Exception";
      TS_ASSERT_EQUALS(XmlParseException("Testing Exception").what(),msg);
    }

    void test7 (){
      std::cout << std::endl;
      std::cout << "Testing FileIOException( const std::string& msg, const std::string& filename ) ..." << std::endl;
      std::string fileName = "FileName";
      std::string msg = "Testing Exception";
      TS_ASSERT_EQUALS(FileIOException(msg,fileName).what(),msg);
      TS_ASSERT_EQUALS(FileIOException(msg,fileName).getFilename(),fileName);
    }

    void test8 (){
      std::cout << std::endl;
      std::cout << "Testing RasterException( const std::string& msg ) ..." << std::endl;
      std::string msg = "Testing Exception";
      TS_ASSERT_EQUALS(RasterException("Testing Exception").what(),msg);
    }

    void test9 (){
      std::cout << std::endl;
      std::cout << "Testing InvalidParameterException( const std::string& msg ) ..." << std::endl;
      std::string msg = "Testing Exception";
      TS_ASSERT_EQUALS(InvalidParameterException("Testing Exception").what(),msg);
    }

    void test10 (){
      std::cout << std::endl;
      std::cout << "Testing SamplerException( const std::string& msg ) ..." << std::endl;
      std::string msg = "Testing Exception";
      TS_ASSERT_EQUALS(SamplerException("Testing Exception").what(),msg);
    }

    void test11 (){
      std::cout << std::endl;
      std::cout << "Testing AlgorithmException( const std::string& msg ) ..." << std::endl;
      std::string msg = "Testing Exception";
      TS_ASSERT_EQUALS(AlgorithmException("Testing Exception").what(),msg);
    }

  private:

};

#endif

