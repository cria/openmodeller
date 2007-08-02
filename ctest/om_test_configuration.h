/**
 * Test class for Configuration
 * 
 * @authors Ricardo Scachetti Pereira and Albert Massayuki Kuniyoshi
 * @date 2007-07-16
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2003 by CRIA -
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
 * \brief A short description of this test goes here
 */

#ifndef TEST_CONFIGURATION_HH
#define TEST_CONFIGURATION_HH

#include "cxxtest/TestSuite.h"
#include <openmodeller/Configuration.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/Sample.hh>
#include <openmodeller/om.hh>
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
class test_Configuration : public CxxTest :: TestSuite 
{
  public:
    void setUp (){
      A = new ConfigurationImpl();
      B = new ConfigurationImpl("name");
      C = new ConfigurationPtr();
      D = new ConstConfigurationPtr();
      a = new std::string;
      b = new std::string;
      c = new int;
      d = new int;
      e = new double;
      f = new double;
      g = new Sample(5);
    }

    void tearDown (){
      //There is no call for ConfigurationImpl's destructor because it is private.
      delete C;
      delete D;
      delete a;
      delete b;
      delete c;
      delete d;
      delete e;
      delete f;
      delete g;
    }

    void test1 (){
      std::cout << std::endl;
      std::cout << "Testing ConfigurationImpl() ..." << std::endl;
      TS_ASSERT(A->getName().empty());
      TS_ASSERT(A->getValue().empty());
      TS_ASSERT((A->getAllSubsections()).empty());
      TS_ASSERT((A->getAllAttributes()).empty());
    }

    void test2 (){
      std::cout << std::endl;
      std::cout << "Testing ConfigurationImpl( char const * name ) ..." << std::endl;
      TS_ASSERT(B->getName()=="name");
      TS_ASSERT(B->getValue().empty());
      TS_ASSERT((B->getAllSubsections()).empty());
      TS_ASSERT((B->getAllAttributes()).empty());
    }

    void test3 (){
      std::cout << std::endl;
      std::cout << "Testing SetName..." << std::endl;
      A->setName(std::string("Freddy_the_Freeloader"));
      TS_ASSERT(A->getName()==std::string("Freddy_the_Freeloader"));
    }

    void test4 (){
      std::cout << std::endl;
      std::cout << "Testing SetValue with no whitespace..." << std::endl;
      A->setValue(std::string("The Value"));
      TS_ASSERT(A->getValue() == std::string("The Value"));
    }

    void test5 (){
      std::cout << std::endl;
      std::cout << "Testing SetValue with whitespace..." << std::endl;
      *a = "The Value";
      *b = " \t" + *a + "\n\r   \t";
      A->setValue(*b);
      TS_ASSERT(A->getValue() == *a);
    }

    void test6 (){
      std::cout << std::endl;
      std::cout << "Testing GetAttribute that have to return defaultvalue..." << std::endl;
      *a = "defaultvalue";
      TS_ASSERT(A->getAttribute("BadAttr",*a) == *a);
    }

    void test7 (){
      std::cout << std::endl;
      std::cout << "Testing GetAttribute throw if not found (empty attr list)..." << std::endl;
      *a = "BadAttr";
      try{
        *b = A->getAttribute(*a);
        TS_FAIL("No Exception Thrown");
      }
      catch(AttributeNotFound& e){
        TS_ASSERT(*a == e.getName());
      }
      catch(...){
        TS_FAIL("Incorrect Exception Thrown");
      }
    }

    void test8 (){
      std::cout << std::endl;
      std::cout << "Testing AddAttribute string with no whitespace..." << std::endl;
      *a = "String Value";
      A->addNameValue("StringAttr",*a);
      TS_ASSERT(A->getAttribute("StringAttr") == *a);
    }

    void test9 (){
      std::cout << std::endl;
      std::cout << "Testing AddAttribute string with whitespace..." << std::endl;
      *a = "String Value";
      *b = " \t" + *a + "\n\r   \t";
      A->addNameValue("StringAttr",*b);
      TS_ASSERT(A->getAttribute("StringAttr") == *a);
    }

    void test10 (){
      std::cout << std::endl;
      std::cout << "Testing GetAttribute throw if not found (1 attrin list)..." << std::endl;
      *a = "String Value";
      A->addNameValue("StringAttr",*a);
      *a = "BadAttr";
      try{
        *b = A->getAttribute(*a);
        TS_FAIL("No Exception Thrown");
      }
      catch(AttributeNotFound& e){
        TS_ASSERT(*a == e.getName());
      }
      catch(...){
        TS_FAIL("Incorrect Exception Thrown");
      }
    }

    void test11 (){
      std::cout << std::endl;
      std::cout << "Testing GetAttribute throw if not found (2 attrin list)..." << std::endl;
      *a = "String Value1";
      A->addNameValue("StringAttr1",*a);
      *a = "String Value2";
      A->addNameValue("StringAttr2",*a);
      *a = "BadAttr";
      try{
        throw A->getAttribute(*a,"Default Value");
      }
      catch(std::string & e){
        TS_ASSERT("Default Value" == e);
      }
      catch(...){
        TS_FAIL("Incorrect Exception Thrown");
      }
    }

    void test12 (){
      std::cout << std::endl;
      std::cout << "Testing AddAttribute int..." << std::endl;
      *c = 42;
      A->addNameValue("IntAttr",*c);
      *d = A->getAttributeAsInt("IntAttr" , 0);
      TS_ASSERT(*d == *c);
    }

    void test13 (){
      std::cout << std::endl;
      std::cout << "Testing AddAttribute double..." << std::endl;
      *e = 3.1415;
      A->addNameValue("DoubleAttr",*e);
      *f = A->getAttributeAsDouble("DoubleAttr" , -1.0);
      TS_ASSERT(*e == *f);
    }

    void test14 (){
      std::cout << std::endl;
      std::cout << "Testing AddAttribute Sample..." << std::endl;
      for(unsigned i=0; i<g->size(); i++){
        (*g)[i] = -2 +i;
      }
      A->addNameValue("SampleAttr",*g);
      TS_ASSERT(*g == A->getAttributeAsSample("SampleAttr"));
    }

    void test15 (){
      std::cout << std::endl;
      std::cout << "Testing Simple Xml Serialization check..." << std::endl;
      //Still under development
      /*
         A->setName(std::string("Freddy_the_Freeloader"));
         A->setValue(std::string("The Value"));
         for(unsigned i=0; i<g->size(); i++){
         (*g)[i] = -2 +i;
         }
         A->addNameValue("SampleAttr",*g);
       *e = 3.1415;
       A->addNameValue("DoubleAttr",*e);
       *c = 42;
       A->addNameValue("IntAttr",*c);
       *a = "String Value";
       A->addNameValue("StringAttr",*a);
       std::stringstream sscheck(std::ios::out);
       Configuration::writeXml(A,sscheck);
       *a = "<StringAttr='String Value'\n IntAttr='42'\n DoubleAttr='3.1415000000000002'\n SampleAttr='-2 -1 0 1 2'\n>\nThe Value\n</Freddy_the_Freeloader>\n";
       std::cout << sscheck.str();
       TS_ASSERT(*a == sscheck.str());
       */
    }

    void test16 (){
      std::cout << std::endl;
      std::cout << "Testing getSubsection throw if not found (empty subsection list)..." << std::endl;
      *a = "NoSubsection";
      try {
        *C = A->getSubsection(*a,true);
        TS_FAIL("No Exception Thrown");
      }
      catch(SubsectionNotFound& e){
        TS_ASSERT(*a == e.getName());				
      }
      catch(...){
        TS_FAIL("Incorrect Exception Thrown");
      }
    }

    void test17 (){
      std::cout << std::endl;
      std::cout << "Testing getSubsection nothrow if not found (empty subsection list)..." << std::endl;
      *a = "NoSubsection";				
      try {
        *C = A->getSubsection(*a,false);
        TS_ASSERT(!*C);
      }
      catch(...){
        TS_FAIL("Exception Thrown");			
      }
    }

    void test18 (){
      /*
         std::cout << std::endl;
         std::cout << "Testing Add subsection..." << std::endl;
         A->addSubsection(*C);
       *C=A->getSubsection();
       TS_ASSERT();
       */
    }

    void test19 (){
      std::cout << std::endl;
      std::cout << "Testing Add subsection..." << std::endl;
      /*
       *C = ConfigurationPtr(new ConfigurationImpl(std::string("Point")));
       C->addNameValue("X",1);
       C->addNameValue("Y",2);
       A->addSubsection(*C);
       TS_ASSERT(A->getAllSubsections().size() == 1);
       */
    }

    void test20 (){
      std::cout << std::endl;
      std::cout << "Testing getSubsection throw if not found (1 subsection in list)..." << std::endl;
      *a = "NoSubsection";
      try{
        *C = A->getSubsection(*a);
        TS_FAIL("No Exception Thrown");
      }
      catch(SubsectionNotFound& e){
        TS_ASSERT(*a == e.getName());
      }
      catch(...){
        TS_FAIL("Incorrect Exception Thrown");
      }
    }

    void test21 (){
      std::cout << std::endl;
      std::cout << "Testing getSubsection nothrow if not found (1 subsection in list)..." << std::endl;
      *a = "NoSubsection";
      try{
        *C = A->getSubsection(*a,false);
        TS_ASSERT(!*C);
      }
      catch(...){
        TS_FAIL("Exception Thrown");
      }
    }

    void test22 (){
      std::cout << std::endl;
      std::cout << "Testing Second Xml Serialization check..." << std::endl;
      std::cout << "(Testing model creation xml)" << std::endl;
      //Still under development
      try 
      {
        {
          Log::instance()->setLevel( Log::Debug );
          Log::instance()->setCallback( new MyLog() );
        }

        std::ostringstream myOutputStream ;

        AlgorithmFactory::searchDefaultDirs();

        OpenModeller om;
        {
          //Note that TEST_DATA_DIR is a compiler define created by CMakeLists.txt

          std::string myConfigFile(TEST_DATA_DIR);
          myConfigFile.append("/model_request.xml");
          std::cout << "Loading Test file : " << myConfigFile << std::endl;
          ConfigurationPtr c = Configuration::readXml( myConfigFile.c_str() );
          om.setModelConfiguration(c);
        }
        om.createModel();
        {
          ConfigurationPtr c = om.getModelConfiguration();
          Configuration::writeXml( c, myOutputStream);
          std::string myFileName("/tmp/configuration_test22_out.xml");
          std::ofstream file(myFileName.c_str());
          file << myOutputStream.str();;
          file.close();
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

    void test23 (){
      std::cout << std::endl;
      std::cout << "Testing Third Xml Serialization check..." << std::endl;
      std::cout << "(Testing model project xml)" << std::endl;
      //Still under development
      try 
      {
        {
          Log::instance()->setLevel( Log::Debug );
          Log::instance()->setCallback( new MyLog() );
        }

        std::ostringstream myOutputStream ;

        AlgorithmFactory::searchDefaultDirs();

        OpenModeller om;
        {
          //Note that TEST_DATA_DIR is a compiler define created by CMakeLists.txt
          std::string projectionXmlFile(TEST_DATA_DIR);
          projectionXmlFile.append("/projection_request.xml");
          std::cout << "Loading Test file : " << projectionXmlFile << std::endl;
          ConfigurationPtr c = Configuration::readXml( projectionXmlFile.c_str() );
          std::cout << "XML loaded" << std::endl;
          om.setProjectionConfiguration(c);
          std::string myFileName("/tmp/configuration_test23_out.tif");
          std::cout << "Projecting to file " << myFileName << std::endl;
          om.createMap( myFileName.c_str() );

          return ;
        }
      }
      catch( std::exception& e ) {
        std::string myError("Exception caught!\n");
        std::cout << "Exception caught!" << std::endl;
        std::cout << e.what() << std::endl;
        myError.insert(myError.length(),e.what());
        return;
      }
    }


  private:
    ConfigurationImpl *A;
    ConfigurationImpl *B;
    ConfigurationPtr *C;
    ConstConfigurationPtr *D;
    std::string *a;
    std::string *b;
    int *c;
    int *d;
    double *e;
    double *f;
    Sample *g;
};

#endif
