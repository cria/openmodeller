/**
 * Test class for MapFormat
 * 
 * @author Albert Massayuki Kuniyoshi
 * @date 2007-07-30
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

/** \ingroup test_MapFormat
* \brief Test for MapFormat
*/

#ifndef TEST_MAPFORMAT_HH
#define TEST_MAPFORMAT_HH

#include "cxxtest/TestSuite.h"

#include <openmodeller/MapFormat.hh>
#include <openmodeller/Exceptions.hh>


class test_MapFormat: public CxxTest :: TestSuite 
{
	public:
		void setUp (){
				A = new MapFormat();
				a = new std::string;
				}

		void tearDown (){
				delete A;
				delete a;
				}

		void test1 (){
				std::cout << std::endl;
				std::cout << "Testing MapFormat() ..." << std::endl;
				*a="Cell width not set";
				try{
				A->getXCel();
				TS_FAIL("No Exception Thrown");
				}
				catch(InvalidParameterException& e){
				TS_ASSERT(*a==e.what());
				}
				catch(...){
				TS_FAIL("Incorrect Exception Thrown");
				}
				*a="Cell height not set";
				try{
				A->getYCel();
				TS_FAIL("No Exception Thrown");
				}
				catch(InvalidParameterException& e){
				TS_ASSERT(*a==e.what());
				}
				catch(...){
				TS_FAIL("Incorrect Exception Thrown");
				}
				*a="XMin not set";
				try{
				A->getXMin();
				TS_FAIL("No Exception Thrown");
				}
				catch(InvalidParameterException& e){
				TS_ASSERT(*a==e.what());
				}
				catch(...){
				TS_FAIL("Incorrect Exception Thrown");
				}
				*a="Ymin not set";
				try{
				A->getYMin();
				TS_FAIL("No Exception Thrown");
				}
				catch(InvalidParameterException& e){
				TS_ASSERT(*a==e.what());
				}
				catch(...){
				TS_FAIL("Incorrect Exception Thrown");
				}
				*a="Xmax not set";
				try{
				A->getXMax();
				TS_FAIL("No Exception Thrown");
				}
				catch(InvalidParameterException& e){
				TS_ASSERT(*a==e.what());
				}
				catch(...){
				TS_FAIL("Incorrect Exception Thrown");
				}
				*a="Ymax not set";
				try{
				A->getYMax();
				TS_FAIL("No Exception Thrown");
				}
				catch(InvalidParameterException& e){
				TS_ASSERT(*a==e.what());
				}
				catch(...){
				TS_FAIL("Incorrect Exception Thrown");
				}
				*a="NoDataValue not set";
				try{
				A->getNoDataValue();
				TS_FAIL("No Exception Thrown");
				}
				catch(InvalidParameterException& e){
				TS_ASSERT(*a==e.what());
				}
				catch(...){
				TS_FAIL("Incorrect Exception Thrown");
				}
				}

		private:
				MapFormat *A;
				std::string *a;
};
#endif
