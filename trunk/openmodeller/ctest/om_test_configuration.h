/**
 * Test class for Configuration
 * 
 * @authors Ricardo Scachetti Pereira and Albert Massayuki Kuniyoshi
 * @date 2007-07-16
 * $Id: OpenModeller.cpp 3201 2007-06-18 21:08:16Z rdg $
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
#include <sstream>
#include <string>

class test_Configuration : public CxxTest :: TestSuite 
{
	public:
		void setUp (){
				A = new ConfigurationImpl();
				B = new ConfigurationImpl("name");
				C = new ConfigurationPtr();
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
				std::cout << "Testing AddAttribute string..." << std::endl;
				*a = "String Value";
				A->addNameValue("StringAttr",*a);
				TS_ASSERT(A->getAttribute("StringAttr") == *a);
				}

		void test9 (){
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

		void test10 (){
				std::cout << std::endl;
				std::cout << "Testing AddAttribute int..." << std::endl;
				*c = 42;
				A->addNameValue("IntAttr",*c);
				*d = A->getAttributeAsInt("IntAttr" , 0);
				TS_ASSERT(*d == *c);
				}
//Test11 completely modified from test_configuration.cpp
		void test11 (){
				std::cout << std::endl;
				std::cout << "Testing GetAttribute throw if not found (2 attrin list)..." << std::endl;
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
				std::cout << "Testing AddAttribute double..." << std::endl;
				*e = 3.1415;
				A->addNameValue("DoubleAttr",*e);
				*f = A->getAttributeAsDouble("DoubleAttr" , -1.0);
				TS_ASSERT(*e == *f);
				}

		void test13 (){
				std::cout << std::endl;
				std::cout << "Testing AddAttribute Sample..." << std::endl;
				for(unsigned i=0; i<g->size(); i++){
				(*g)[i] = -2 +i;
				}
				A->addNameValue("SampleAttr",*g);
				TS_ASSERT(*g == A->getAttributeAsSample("SampleAttr"));
				}

		void test14 (){
				std::cout << std::endl;
				std::cout << "Testing Simple Xml Serialization check..." << std::endl;
				//Still under development
				/*
				A->setName(std::string("Freddy_the_Freeloader"));
				A->setValue(std::string("The Value"));
				*a = "The Value";
				*b = " \t" + *a + "\n\r   \t";
				A->setValue(*b);
				*a = "String Value";
				A->addNameValue("StringAttr",*a);
				*c = 42;
				A->addNameValue("IntAttr",*c);
				*e = 3.1415;
				A->addNameValue("DoubleAttr",*e);
				for(unsigned i=0; i<g->size(); i++){
									(*g)[i] = -2 +i;
									}
				A->addNameValue("SampleAttr",*g);

				std::stringstream sscheck(std::ios::out);
				Configuration::writeXml(A,sscheck);
				*a = "<Freddy_the_Freeloader\n StringAttr='String Value'\n IntAttr='42'\n DoubleAttr='3.1415000000000002'\n SampleAttr='-2 -1 0 1 2'\n>\nThe Value\n</Freddy_the_Freel
oader>\n";
				std::cout << sscheck.str();
				TS_ASSERT(*a == sscheck.str());
				}
				*/
				}

		void test15 (){
				std::cout << std::endl;
				std::cout << "Testing getSubsection throw if not found (empty subsection list)..." << std::endl;
				*a = "NoSubsection";
				try {
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

		void test16 (){
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

		void test17 (){
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

		void test18 (){
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

		void test19 (){
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

		void test20 (){
				std::cout << std::endl;
				std::cout << "Testing Second Xml Serialization check..." << std::endl;
				//Still under development
				}

		void test21 (){
				std::cout << std::endl;
				std::cout << "Testing Third Xml Serialization check..." << std::endl;
				//Still under development
				}


		private:
			ConfigurationImpl *A;
			ConfigurationImpl *B;
			ConfigurationPtr *C;
			std::string *a;
			std::string *b;
			int *c;
			int *d;
			double *e;
			double *f;
			Sample *g;
};

#endif
