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
				subs = new Configuration::subsection_list;
				attrs = new Configuration::attribute_list;
				a = new std::string;
				b = new std::string;
				}

		void tearDown (){
				//There is no call for ConfigurationImpl's destructor because it is private.
				delete subs;
				delete attrs;
				delete a;
				delete b;
				}

		void test1 (){
				std::cout << std::endl;
				std::cout << "Testing ConfigurationImpl() ..." << std::endl;
				TS_ASSERT(A->getName().empty());
				TS_ASSERT(A->getValue().empty());
				*subs = A->getAllSubsections();
				TS_ASSERT(subs->empty());
				*attrs = A->getAllAttributes();
				TS_ASSERT(attrs->empty());
				}

		void test2 (){
				std::cout << std::endl;
				std::cout << "Testing ConfigurationImpl( char const * name ) ..." << std::endl;
				TS_ASSERT(B->getName()=="name");
				TS_ASSERT(B->getValue().empty());
				*subs = B->getAllSubsections();
				TS_ASSERT(subs->empty());
				*attrs = B->getAllAttributes();
				TS_ASSERT(attrs->empty());
				}

		void test3 (){
				std::cout << std::endl;
				std::cout << "Testing SetName..." << std::endl;
				A->setName(std::string("name"));
				TS_ASSERT(A->getName()==std::string("name"));
				}

		void test4 (){
				std::cout << std::endl;
				std::cout << "Testing SetValue with no whitespace..." << std::endl;
				A->setValue(std::string("value"));
				TS_ASSERT(A->getValue() == std::string("value"));
				}

		void test5 (){
				std::cout << std::endl;
				std::cout << "Testing SetValue with whitespace..." << std::endl;
				*a = "value";
				*b = " \t" + *a + "\n\r   \t";
				A->setValue(*b);
				TS_ASSERT(A->getValue() == *a);
				}
//--------------------------------------------------------------------------------------------------------------------------------------------------------//
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

		private:
			ConfigurationImpl *A;
			ConfigurationImpl *B;
			Configuration::subsection_list *subs;
			Configuration::attribute_list *attrs;
			std::string *a;
			std::string *b;
};

#endif
