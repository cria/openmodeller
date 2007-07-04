/**
 * Test class for SampleExpr
 * 
 * @authors Ricardo Scachetti Pereira and Albert Massayuki Kuniyoshi
 * @date 2007-07-03
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

#ifndef TEST_SAMPLEEXPR_HH
#define TEST_SAMPLEEXPR_HH

#include "cxxtest/TestSuite.h"
#include "Sample.hh"
#include "SampleExpr.hh"
#include <cmath>
#include <algorithm>

#define SAMPLESIZE 10

class test_sampleexpr : public CxxTest :: TestSuite 
{

	public:
		void setUp (){
				a = new Sample(SAMPLESIZE);
				b = new Sample(SAMPLESIZE);
				c = new Sample();
				for( unsigned int i=0; i<SAMPLESIZE; ++i ){
				(*a)[i] = i+1;
				(*b)[i] = SAMPLESIZE-i;}
				}

		void tearDown (){
				delete a;
				delete b;
				delete c;
				}

		void test1 (){
				std::cout << std::endl;
				std::cout << "Testing SampleExpr Unary Neg ..." << std::endl;
				*c = -*a;
				for( unsigned int i=0; i<a->size(); ++i){
				TS_ASSERT((*c)[i]==-(*a)[i]);}
				}

  		void test2 (){
				std::cout << std::endl;
				std::cout << "Testing SampleExpr Unary Sqr ..." << std::endl;
				*c = sqr(*a);
				for( unsigned int i=0; i<a->size(); ++i){
				TS_ASSERT((*c)[i] == (*a)[i]*(*a)[i]);}
				}

		void test3 (){
				std::cout << std::endl;
				std::cout << "Testing SampleExpr Unary Sqrt ..." << std::endl;
				*c = sqrt(*a);
				for( unsigned int i=0; i<a->size(); ++i){
				TS_ASSERT_DELTA((*c)[i],std::sqrt((*a)[i]),1e-15);}
				}

	private:
				Sample *a;
				Sample *b;
				Sample *c;
				Sample *d;

};

#endif
