/**
 * Test class for Configuration
 * 
 * @author Albert Massayuki Kuniyoshi
 * @date 2007-07-11
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
#include "Configuration.hh"
#include <string>

class test_Configuration : public CxxTest :: TestSuite 
{
	public:
		void setUp (){
				a = new ConfigurationImpl();
				}

		void tearDown (){

				}

		void test1 (){
				std::cout << std::endl;
				std::cout << "Testing Default Constructor..." << std::endl;
				TS_ASSERT(a->getName().empty());
				TS_ASSERT(a->getValue().empty());
				}
	private:
			ConfigurationImpl *a;

};

#endif
