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
				a = new std::string;
				b = new double;
				A = new MapFormat();
				B = new MapFormat(0.01,0.02,0.03,0.04,0.05,0.06,0.07,"name");
				}

		void tearDown (){
				delete a;
				delete b;
				delete A;
				delete B;
				}

		void test1 (){
				std::cout << std::endl;
				std::cout << "Testing MapFormat() ..." << std::endl;
				//Need to test format
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
				*a="Projection not set";
				try{
				A->getProjection();
				TS_FAIL("No Exception Thrown");
				}
				catch(InvalidParameterException& e){
				TS_ASSERT(*a==e.what());
				}
				catch(...){
				TS_FAIL("Incorrect Exception Thrown");
				}
				}

		void test2 (){
				std::cout << std::endl;
				std::cout << "Testing MapFormat::MapFormat( Coord xcel, Coord ycel, Coord xmin, Coord ymin, Coord xmax, Coord ymax, Scalar noval, char const * proj ) ..." << std::endl;
				//Need to test format
				*b=0.01;
				try{
				B->getXCel();
				TS_ASSERT(*b==B->getXCel());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("Cell width not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				*b=0.02;
				try{
				B->getYCel();
				TS_ASSERT(*b==B->getYCel());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("Cell height not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				*b=0.03;
				try{
				B->getXMin();
				TS_ASSERT(*b==B->getXMin());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("XMin not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				*b=0.04;
				try{
				B->getYMin();
				TS_ASSERT(*b==B->getYMin());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("YMin not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				*b=0.05;
				try{
				B->getXMax();
				TS_ASSERT(*b==B->getXMax());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("XMax not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				*b=0.06;
				try{
				B->getYMax();
				TS_ASSERT(*b==B->getYMax());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("YMax not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				*b=0.07;
				try{
				B->getNoDataValue();
				TS_ASSERT(*b==B->getNoDataValue());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("NoDataValue not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				*a="name";
				try{
				B->getProjection();
				TS_ASSERT(*a==B->getProjection());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("Projection not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				}
//Need to test setFormat
		void test3 (){
				std::cout << std::endl;
				std::cout << "Testing setXCel(Coord v) ..." << std::endl;
				A->setXCel(0.01);
				*b=0.01;
				try{
				A->getXCel();
				TS_ASSERT(*b==A->getXCel());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("Cell width not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				}

		void test4 (){
				std::cout << std::endl;
				std::cout << "Testing setYCel(Coord v) ..." << std::endl;
				A->setYCel(0.02);
				*b=0.02;
				try{
				A->getYCel();
				TS_ASSERT(*b==A->getYCel());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("Cell height not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				}

		void test5 (){
				A->setXMin(0.03);
				*b=0.03;
				try{
				A->getXMin();
				TS_ASSERT(*b==A->getXMin());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("XMin not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				}

		void test6 (){
				A->setYMin(0.04);
				*b=0.04;
				try{
				A->getYMin();
				TS_ASSERT(*b==A->getYMin());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("YMin not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				}

		void test7 (){
				A->setXMax(0.05);
				*b=0.05;
				try{
				A->getXMax();
				TS_ASSERT(*b==A->getXMax());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("XMax not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				}

		void test8 (){
				A->setYMax(0.06);
				*b=0.06;
				try{
				A->getYMax();
				TS_ASSERT(*b==A->getYMax());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("YMax not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				}

		void test9 (){
				A->setNoDataValue(0.07);
				*b=0.07;
				try{
				A->getNoDataValue();
				TS_ASSERT(*b==A->getNoDataValue());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("NoDataValue not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				}

		void test10 (){
				A->setProjection("name");
				*a="name";
				try{
				A->getProjection();
				TS_ASSERT(*a==A->getProjection());
				}
				catch(InvalidParameterException& e){
				TS_FAIL("Projection not set");
				}
				catch(...){
				TS_FAIL("Invalid Exception Thrown");
				}
				}

		private:
				std::string *a;
				double *b;
				MapFormat *A;
				MapFormat *B;
};
#endif
