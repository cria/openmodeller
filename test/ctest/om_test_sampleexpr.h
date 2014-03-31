/**
 * Test class for SampleExpr
 * 
 * @authors Ricardo Scachetti Pereira and Albert Massayuki Kuniyoshi
 * @date 2007-07-04
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
* \brief Test for SampleExpr Class
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
      (*b)[i] = SAMPLESIZE-i;
      }
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
      TS_ASSERT((*c)[i]==-(*a)[i]);
      }
    }

    void test2 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Unary Sqr ..." << std::endl;
      *c = sqr(*a);
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i] == (*a)[i]*(*a)[i]);
      }
    }

    void test3 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Unary Sqrt ..." << std::endl;
      *c = sqrt(*a);
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT_DELTA((*c)[i],std::sqrt((*a)[i]),1e-15);
      }
    }

    void test4 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nested Unary Test 1 ..." << std::endl;
      *c = - - *a;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]==(*a)[i]);
      }
    }

    void test5 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nested Unary Test 2 ..." << std::endl;
      *c = sqr(sqrt(*a));
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT_DELTA((*c)[i],std::sqrt((*a)[i])*std::sqrt((*a)[i]),1e-4);
      }
    }

    void test6 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nested Unary Test 3 ..." << std::endl;
      *c = sqrt(sqr(*a));
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT_DELTA((*c)[i],std::sqrt((*a)[i]*(*a)[i]),1e-15);
      }
    }

    void test7 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nested Unary Test 4 ..." << std::endl;
      *c = sqr( - sqrt(*a) );
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT_DELTA((*c)[i],std::sqrt((*a)[i])*std::sqrt((*a)[i]),1e-4);
      }
    }

    void test8 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Sample+Sample ..." << std::endl;
      *c = *a + *b;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]==(*a)[i]+(*b)[i]);
      }
    }

    void test9 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Sample-Sample ..." << std::endl;
      *c = *a - *b;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]==(*a)[i]-(*b)[i]);
      }
    }

    void test10 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Sample*Sample ..." << std::endl;
      *c = (*a) * (*b);
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]==(*a)[i]*(*b)[i]);}
      }

    void test11 (){
       std::cout << std::endl;
       std::cout << "Testing SampleExpr Sample/Sample ..." << std::endl;
       *c = (*a)/(*b);
       for( unsigned int i=0; i<a->size(); ++i){
       TS_ASSERT_DELTA((*c)[i],(*a)[i]/(*b)[i],1e-15);}
       }

    void test12 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr min(Sample,Sample) ..." << std::endl;
      *c = min(*a,*b);
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]==std::min((*a)[i],(*b)[i]));
      }
    }

    void test13 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr max(Sample,Sample) ..." << std::endl;
      *c = max(*a,*b);
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]==std::max((*a)[i],(*b)[i]));}
      }

    void test14 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 1 (sqr(a) + b) ..." << std::endl;
      *c = sqr(*a)+*b;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]==(*a)[i]*(*a)[i]+(*b)[i]);
      }
    }

    void test15 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 2 ( -a - b)..." << std::endl;
      *c = - *a - *b;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]== - (*a)[i] - (*b)[i]);
      }
    }

    void test16 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 3 ( -a * b) ..." << std::endl;
      *c = - (*a)*(*b);
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]== - (*a)[i]*(*b)[i]);
      }
    }

    void test17 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 4 ( sqr(a) / b ) ..." << std::endl;
      *c = sqr(*a)/(*b);
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT_DELTA((*c)[i],(*a)[i]*(*a)[i]/(*b)[i],1e-10);
      }
    }

    void test18 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 5 ( min(sqr(a),b) ) ..." << std::endl;
      *c = min(sqr(*a),*b);
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]==std::min((*a)[i]*(*a)[i],(*b)[i]));
      }
    }

    void test19 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 6 ( max(sqr(a),b) ) ..." << std::endl;
      *c = max(sqr(*a),*b);
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]==std::max((*a)[i]*(*a)[i],(*b)[i]));
      }
    }

    void test20 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 7 ( a + -b) ..." << std::endl;
      *c = *a + -*b;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i] == (*a)[i] + -(*b)[i]);}
      }

    void test21 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 8 ( a - sqrt(b)) ..." << std::endl;
      *c = *a - sqrt(*b);
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT_DELTA((*c)[i],(*a)[i]-std::sqrt((*b)[i]),1e-15);
      }
    }

    void test22 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 9 ( a * -b) ..." << std::endl;
      *c = *a * - *b;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]==-(*a)[i]*(*b)[i]);}
      }

    void test23 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 10 ( a / -b ) ..." << std::endl;
      *c = *a/-*b;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT_DELTA((*c)[i],-(*a)[i]/(*b)[i],1e-15);
      }
    }

    void test24 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 11 ( min(a,sqr(b)) ) ..." << std::endl;
      *c = min(*a,sqr(*b));
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]==std::min((*a)[i],(*b)[i]*(*b)[i]));
      }
    }

    void test25 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 12 ( max(a,sqr(b)) ) ..." << std::endl;
      *c = max(*a,sqr(*b));
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]==std::max((*a)[i],(*b)[i]*(*b)[i]));
      }
    }

    void test26 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 13 ( -a + -b) ..." << std::endl;
      *c = - *a + - *b;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]== -((*a)[i]+(*b)[i]));
      }
    }

    void test27 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 14 ( -a - -b) ..." << std::endl;
      *c = - *a - - *b;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]== - (*a)[i]+(*b)[i]);
      }
    }

    void test28 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 15 ( sqr(a) * sqr(b)) ..." << std::endl;
      *c = sqr(*a) * sqr(*b);
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i]==(*a)[i]*(*a)[i]*(*b)[i]*(*b)[i]);
      }
    }

    void test29 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Nest Binary Test 16 ( sqr(a) / -b ) ..." << std::endl;
      *c = sqr(*a)/-*b;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT_DELTA((*c)[i],(*a)[i]*(*a)[i]/-(*b)[i],1e-10);
      }
    }

    void test30 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Scalar Add (a+1) ..." << std::endl;
      *c = *a + 1;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i] == (*a)[i]+1);
      }
    }

    void test31 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Scalar Add (a+2.0) ..." << std::endl;
      *c = *a + 2.0;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i] == (*a)[i]+2.0);
      }
    }

    void test32 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Scalar Add (5+a+1) ..." << std::endl;
      *c = 5 + *a + 1;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i] == (*a)[i] + 6);
      }
    }

    void test33 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Scalar Add (30.7+a-2.0) ..." << std::endl;
      *c = 30.7 + *a - 2.0;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT_DELTA((*c)[i],(*a)[i]+28.7,1e-10);
      }
    }

    void test34 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Scalar Mult (a*3) ..." << std::endl;
      *c = *a * 3;
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i] == (*a)[i]*3);
      }
    }

    void test35 (){
      std::cout << std::endl;
      std::cout << "Testing SampleExpr Scalar Mult (6.0*a) ..." << std::endl;
      *c = 6.0*(*a); 
      for( unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*c)[i] == (*a)[i]*6.0);
      }
    }

  private:
    Sample *a;
    Sample *b;
    Sample *c;
};

#endif

