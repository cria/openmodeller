/**
 * Test class for SampleExprVar
 * 
 * @author Albert Massayuki Kuniyoshi
 * @date 2007-08-20
 * $Id:
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
* \brief Test for SampleExprVar Class
*/

#ifndef TEST_SAMPLEEXPRVAR_HH
#define TEST_SAMPLEEXPRVAR_HH
#define SAMPLESIZE 10
#include "cxxtest/TestSuite.h"
#include <openmodeller/SampleExprVar.hh>
#include <cmath>
#include <algorithm>
SExprType< SQR< TIMES< _2<Sample>, NEG< SQRT < PLUS< _1<Sample>, _3<Sample> > > > > > >::type expr;
class test_SampleExprVar : public CxxTest :: TestSuite 
{

  public:
    void setUp (){
      a = new Sample(SAMPLESIZE);
      b = new Sample(SAMPLESIZE);
      c = new Sample(SAMPLESIZE);
      for( unsigned int i=0; i<SAMPLESIZE; ++i ){
      (*a)[i] = i+1;
      (*b)[i] = SAMPLESIZE-i;
      (*c)[i] = SAMPLESIZE - 2*i;
      }
    }

    void tearDown (){
      delete a;
      delete b;
      delete c;
    }

    void test1 (){
      std::cout << "Testing SampleExprVar 1 ..." << std::endl;
      Sample d = expr(*a)(*b)(*c);
      for( unsigned int i=0; i<a->size(); ++i ) {
      Scalar val = (*b)[i] * std::sqrt( (*a)[i] + (*c)[i] );
      val *= val;
      TS_ASSERT_DELTA( d[i], val, 1e-10 );
      }
    }

    void test2 (){
      std::cout << "Testing SampleExprVar 2 ..." << std::endl;
      Sample d = expr(*b)(*c)(*a);
      for( unsigned int i=0; i<a->size(); ++i ) {
      Scalar val = (*c)[i] * std::sqrt( (*b)[i] + (*a)[i] );
      val *= val;
      TS_ASSERT_DELTA( d[i], val, 1e-10 );
      }
    }

    void test3 (){
      std::cout << "Testing SampleExprVar 3 ..." << std::endl;
      Sample d = expr(*a)(*b)(*c);
      Sample e = expr(*a)(*b)(*c);
      for( unsigned int i=0; i<a->size(); ++i ) {
      Scalar val = (*b)[i] * std::sqrt( (*a)[i] + (*c)[i] );
      val *= val;
      TS_ASSERT_DELTA( d[i], val, 1e-10 );
      TS_ASSERT_DELTA( e[i], val, 1e-10 );
      }
    }

  private:
    Sample *a;
    Sample *b;
    Sample *c;
};

#endif

