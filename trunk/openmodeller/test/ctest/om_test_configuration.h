/**
 * Test class for Configuration
 * 
 * @authors Ricardo Scachetti Pereira and Albert Massayuki Kuniyoshi
 * @date 2007-07-16
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
 * \brief Test for Configuration Class
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
      std::cout << "Testing Simple Xml Serialization check (no subsection)..." << std::endl;
      A->setName(std::string("Freddy_the_Freeloader"));
      A->setValue(std::string("The Value"));
      for(unsigned i=0; i<g->size(); i++){
      (*g)[i] = -2 +i;
      }
      A->addNameValue("SampleAttr",*g);
       *e = 3.1415;
       A->addNameValue("DoubleAttr",*e,5);
       *c = 42;
       A->addNameValue("IntAttr",*c);
       *a = "String Value";
       A->addNameValue("StringAttr",*a);
       std::stringstream sscheck(std::ios::out);
       Configuration::writeXml(A,sscheck);
       *a = "\n<Freddy_the_Freeloader SampleAttr=\"4294967294 4294967295 0 1 2\" DoubleAttr=\"3.1415\" IntAttr=\"42\" StringAttr=\"String Value\">The Value</Freddy_the_Freeloader>";
       std::cout << "Expected:" << std::endl << *a;
       std::cout << std::endl << "Found:" << std::endl << sscheck.str();
       TS_ASSERT(*a == sscheck.str());
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
       std::cout << std::endl;
       std::cout << "Testing Add subsection..." << std::endl;
/*
       C->addNameValue("X",1);
       C->addNameValue("Y",2);
       A->addSubsection(*C);
       TS_ASSERT(A->getAllSubsections().size() == 1);
*/
    }

    void test19 (){
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

    void test20 (){
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

    void test21 (){
      std::cout << std::endl;
      std::cout << "Testing Second Xml Serialization check..." << std::endl;
      std::cout << "(Testing model creation xml)" << std::endl;
      try 
      {
        //Note that EXAMPLE_DIR and TEST_DATA_DIR is a compiler define 
        //created by CMakeLists.txt
        std::string myConfigFile(TEST_DATA_DIR);
        myConfigFile.append("/model_request.xml");
        std::cout << "Loading Test file : " << myConfigFile << std::endl;
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
        std::string myInFileName("/tmp/configuration_test22_in.xml");
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
        
        //
        // Go on to do the test now...
        //

        Log::instance()->setLevel( Log::Debug );
        Log::instance()->setCallback( new MyLog() );

        std::ostringstream myOutputStream ;
        AlgorithmFactory::searchDefaultDirs();
        OpenModeller om;

        ConfigurationPtr c1 = Configuration::readXml( myInFileName.c_str() );
        om.setModelConfiguration(c1);
        om.createModel();
        ConfigurationPtr c2 = om.getModelConfiguration();
        Configuration::writeXml( c2, myOutputStream);
        std::string myOutFileName("/tmp/configuration_test22_out.xml");
        std::ofstream myOutFile2(myOutFileName.c_str());
        myOutFile2 << myOutputStream.str();;
        myOutFile2.close();
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

    void test22 (){
      std::cout << std::endl;
      std::cout << "Testing Third Xml Serialization check..." << std::endl;
      std::cout << "(Testing model project xml)" << std::endl;
      //Note that EXAMPLE_DIR and TEST_DATA_DIR is a compiler define 
      //created by CMakeLists.txt
      std::string myConfigFile(TEST_DATA_DIR);
      myConfigFile.append("/projection_request.xml");
      std::cout << "Loading Test file : " << myConfigFile << std::endl;
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
      std::string myInFileName("/tmp/configuration_test23_in.xml");
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

      //
      // Go on to do the test now...
      //
      try 
      {
        Log::instance()->setLevel( Log::Debug );
        Log::instance()->setCallback( new MyLog() );

        std::ostringstream myOutputStream ;

        AlgorithmFactory::searchDefaultDirs();

        OpenModeller om;
        {
          ConfigurationPtr c = Configuration::readXml( myInFileName.c_str() );
          std::cout << "XML loaded" << std::endl;
          om.setProjectionConfiguration(c);
          std::string myOutFileName("/tmp/configuration_test23_out.tif");
          std::cout << "Projecting to file " << myOutFileName << std::endl;
          om.createMap( myOutFileName.c_str() );

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

    void test23 (){
      std::cout << std::endl;
      std::cout << "Testing Parse Xml check ..." << std::endl;
      std::string testVal="\n<Freddy_the_Freeloader SampleAttr=\"4294967294 4294967295 0 1 2\" DoubleAttr=\"3.141500000000000181188398\" IntAttr=\"42\" StringAttr=\"String Value\">The Value</Freddy_the_Freeloader>";
      std::stringstream ss( testVal, std::ios::in );
      *C = Configuration::readXml(ss);
      std::stringstream sscheck( std::ios::out );
      Configuration::writeXml( *C, sscheck );
      TS_ASSERT( testVal == sscheck.str() );
    }

    void test24 (){
      std::cout << std::endl;
      std::cout << "Testing Parse Xml exception check 1 ..." << std::endl;
      std::string badxml = "blablabla";
      std::stringstream ss( badxml, std::ios::in );
      try {
      *C = Configuration::readXml( ss );
      TS_FAIL( "No Exception Thrown" );
      }
      catch( XmlParseException& e ) {
      std::cout << "Correct exception" << std::endl;
      TS_ASSERT(true);
      }
      catch( ... ) {
      TS_FAIL( "Wrong Exception Thrown" );
      }
    }

    void test25 (){
      std::cout << std::endl;
      std::cout << "Testing Parse Xml exception check 2 ..." << std::endl;
      std::string badxml = "<Starttag></EndTag>";
      std::stringstream ss( badxml, std::ios::in );
      try {
      *C = Configuration::readXml( ss );
      TS_FAIL( "No Exception Thrown" );
      }
      catch( XmlParseException& e ) {
      std::cout << "Correct exception" << std::endl;
      TS_ASSERT(true);
      }
      catch( ... ) {
      TS_FAIL( "Wrong Exception Thrown" );
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

