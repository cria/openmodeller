/**
 * Test class for Random 
 * 
 * @author Albert Massayuki Kuniyoshi
 * @date 2007-07-05
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

#ifndef TEST_RANDOM_HH
#define TEST_RANDOM_HH

#include "cxxtest/TestSuite.h"
#include "Random.hh"
#include <cstdlib>

class test_Random : public CxxTest :: TestSuite 
{

	public:
		void setUp (){
				a = new Random();
				num = new double;
				std::srand(0);
				*num = rand()/(RAND_MAX+1.0);
				}

		void tearDown (){
				delete a;
				delete num;
				}

		void test1 (){
				std::cout << std::endl;
				std::cout << "Testing get( double min, double max )..." << std::endl;
				std::srand(0);
				TS_ASSERT_EQUALS((101.55-1.55)*(*num)+1.55,a->get( 1.55 , 101.55 ));
				TS_ASSERT(a->get(1.55,101.55)>=1.55&&a->get(1.55,101.55)<101.55);	
				}

		void test2 (){
				std::cout << std::endl;
				std::cout << "Testing get( double max )..." << std::endl;
				std::srand(0);
				TS_ASSERT_EQUALS(155.6432*(*num),a->get(155.6432));
				TS_ASSERT(a->get(155.6432)>=0&&a->get(155.6432)<155.6432);
				}

	private:
				Random *a;
				double *num;
};

#endif
