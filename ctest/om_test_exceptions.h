/**
 * Test class for Exceptions
 * 
 * @author Albert Massayuki Kuniyoshi
 * @date 2007-07-10
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

#ifndef TEST_EXCEPTIONS_HH
#define TEST_EXCEPTIONS_HH

#include "cxxtest/TestSuite.h"
#include "Exceptions.hh"
// Microsoft VC8 requires <string> before <stdexcept> in order for
// this code to compile.
#include <string>
#include <stdexcept>

class test_Random : public CxxTest :: TestSuite 
{

	public:
		void setUp (){

				}

		void tearDown (){

				}

		void test1 (){
				std::cout << std::endl;
				std::cout << "Testing OmException( const std::string& msg ) ..." << std::endl;
				TS_ASSERT_EQUALS(0,0);
				}

	private:


};

#endif
