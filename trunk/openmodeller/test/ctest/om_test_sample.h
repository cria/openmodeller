/**
 * Test class for Sample
 * 
 * @authors Ricardo Scachetti Pereira and Albert Massayuki Kuniyoshi
 * @date 2007-07-03
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
* \brief Test for Sample Class
*/

#ifndef TEST_SAMPLE_HH
#define TEST_SAMPLE_HH

#include "cxxtest/TestSuite.h"
#include "Sample.hh"
#include <algorithm>
#include <istream>
#include <sstream>
#include <iostream>

class test_Sample : public CxxTest :: TestSuite 
{

  public:
    void setUp (){
      a = new Sample;
      b = new Sample;
      c = new Sample;
      d = new Sample;
    }

    void tearDown (){
      delete a;
      delete b;
      delete c;
      delete d;
    }

/**
 *Test for Sample default constructor.
 */

    void testDefaultConstructor (){
      std::cout << std::endl;
      std::cout << "Testing Default Constructor..." << std::endl;
      TS_ASSERT_EQUALS(a->size(),(unsigned int) 0);
    }

/**
 *Test for resize function.
 */

    void testResizeFunction (){
      std::cout << std::endl;
      std::cout << "Testing Resize Function..." << std::endl;
      a->resize(10);
      TS_ASSERT_EQUALS(a->size(),(unsigned int) 10);
    }

/**
 *Test for Sample constructor initialized with single Scalar.
 */

    void testConstructorWithSingleScalar (){
      std::cout << std::endl;
      std::cout << "Testing Sample Constructor initialized with single Scalar..." << std::endl;
      *b = Sample (10,-0.1);
      for(unsigned int i=0; i<b->size(); ++i){
      TS_ASSERT_DELTA(-0.1,(*b)[i],1e-10);}
    }

/**
 *Test for Sample assignment through operator[].
 */

    void testDirectAssignmentOperator_1 (){
      std::cout << std::endl;
      std::cout << "Testing Sample assign through operator[]..." << std::endl;
      *a = Sample(10);
      for(unsigned int i=0; i<a->size(); ++i){
        (*a)[i]=i;
      }
      for(unsigned int i=0;i<a->size();++i){
      TS_ASSERT_EQUALS((*a)[i],i);
      }
    }

/**
 *Test for equals function and for operator==().
 */

    void testEqualsFunctionAndEqualToOperator (){
      std::cout << std::endl;
      std::cout << "Testing Sample equals( const Sample& ) and operator==()..." << std::endl;
      *a = Sample(10);
      for(unsigned int i=0;i<a->size();++i){
      (*a)[i]=i;
      }
      *c = Sample (3);
      TS_ASSERT(!c->equals(*a));
      TS_ASSERT(!(*c==*a));
      TS_ASSERT(*c!=*a);
      *d = Sample(a->size());
      TS_ASSERT(!d->equals(*a));
      TS_ASSERT(!(*d==*a));
      TS_ASSERT(*d!=*a);
      for(unsigned int i=0; i<a->size(); ++i){
      (*d)[i] = (*a)[i];
      }
      for(unsigned int i=0; i<a->size(); ++i){
      TS_ASSERT((*a)[i]==i);
      }
      TS_ASSERT(d->equals(*a));
      TS_ASSERT(*d==*a);
      TS_ASSERT(!(*d!=*a));
    }

/**
 *Test for Sample copy constructor.
 */

    void testCopyConstructor (){
      std::cout << std::endl;
      std::cout << "Testing Sample Copy Constructor..." << std::endl;
      *a = Sample(10);
      for(unsigned int i=0;i<a->size();++i){
      (*a)[i]=i;
      }
      *c = Sample(*a);
      TS_ASSERT_EQUALS(*a,*c);
    }

/**
 *Test for Sample operator=(const Sample &).
 */

    void testDirectAssignmentOperator_2() {
      std::cout << std::endl;
      std::cout << "Testing Sample operator=( const Sample & )..." << std::endl;
      *a = Sample(10);
      for(unsigned int i=0;i<a->size();++i){
      (*a)[i]=i;
      }
      *b = *a;
      TS_ASSERT_EQUALS(*b,*a);
    }

/**
 *Test for Sample operator>> and operator<<.
 */

    void testBitwiseOperators (){
      std::cout << std::endl;
      std::cout << "Testing Sample operator>> and operator<<..." << std::endl;
      *a = Sample(10);
      for(unsigned int i=0;i<a->size();++i){
      (*a)[i]=i;
      }
      std::cerr << "A= " << *a << std::endl;
      std::stringstream ss( std::ios::in | std::ios::out );
      ss << *a;
      ss >> *b;
      std::cerr << "B= " << *b << std::endl;
      TS_ASSERT_EQUALS(*a,*b);
    }

/**
 *Test for Sample operator>> and operator<< precision.
 */

    void testBitwiseOperatorsPrecision (){
      std::cout << std::endl;
      std::cout << "Testing Sample operator>> and operator<< precision..." << std::endl;
      std::string testString="1.0 1.01 1.001 1.0001 1.00001 1.000001 1.0000001 1.00000001 1.000000001";
      std::stringstream ss(testString, std::ios::in);
      *b = Sample();
      ss >> *b;
      std::cerr << "B= " << *b << std::endl;
      std::stringstream s2(std::ios::in | std::ios::out);
      s2 << *b;
      *c = Sample();
      s2 >> *c;
      TS_ASSERT_EQUALS(*b,*c);
    }

/**
 *Test for Sample operator+=(Sample).
 */

    void testOperatorAssignmentBySum_1(){
      std::cout << std::endl;
      std::cout << "Testing Sample operator+=(Sample)..." << std::endl;
      *a = Sample(10);
      for(unsigned int i=0;i<a->size();++i){
      (*a)[i]=i;
      }
      *c = Sample(*a);
      *c += *a;
      for(unsigned int i=0;i<c->size();++i){
      TS_ASSERT_EQUALS((*c)[i],2*(*a)[i]);
      }
    }

/**
 *Test for Sample operator+=(Scalar).
 */

    void testOperatorAssignmentBySum_2 (){
      std::cout << std::endl;
      std::cout << "Testing Sample operator+=(Scalar)..." << std::endl;
      *a = Sample(10);
      for(unsigned int i=0;i<a->size();++i){
      (*a)[i]=i;
      }
      *c = Sample(*a);
      *c += 35;
      for(unsigned int i=0;i<c->size();++i){
      TS_ASSERT_EQUALS((*c)[i],35+(*a)[i]);
      }
    }

/**
 *Test for Sample operator-=(Sample).
 */

    void testOperatorAssignmentByDifference_1 (){
      std::cout << std::endl;
      std::cout << "Testing Sample operator-=(Sample)..." << std::endl;
      *a = Sample(10);
      for(unsigned int i=0;i<a->size();++i){
      (*a)[i]=i;
      }
      *c = Sample(*a);
      *c -= *a;
      for(unsigned int i=0;i<c->size();i++){
      TS_ASSERT_EQUALS((*c)[i],0);
      }
    }

/**
 *Test for Sample operator-=(Scalar)
 */

    void testOperatorAssignmentByDifference_2 (){
      std::cout << std::endl;
      std::cout << "Testing Sample operator-=(Scalar)..." << std::endl;
      *a = Sample(10);
      for(unsigned int i=0;i<a->size();++i){
      (*a)[i]=i;
      }
      *c = Sample(*a);
      *c -= 35;
      for(unsigned int i=0;i<c->size();++i){
      TS_ASSERT_EQUALS((*c)[i],(*a)[i]-35);
      }
    }

/**
 *Test for Sample operator*=(Sample).
 */

    void testOperatorAssignmentByProduct_1 (){
      std::cout << std::endl;
      std::cout << "Testing Sample operator*=(Sample)..." << std::endl;
      *a = Sample(10);
      for(unsigned int i=0;i<a->size();++i){
      (*a)[i]=i;
      }
      *c = Sample(*a);
      *c *= *a;
      for(unsigned int i=0;i<c->size();++i){
      TS_ASSERT_EQUALS((*c)[i],(*a)[i]*(*a)[i]);
      }
    }

/**
 *Test for Sample operator*=(Scalar).
 */

    void testOperatorAssignmentByProduct_2 (){
      std::cout << std::endl;
      std::cout << "Testing Sample operator*=(Scalar)..." << std::endl;
      *a = Sample(10);
      for(unsigned int i=0;i<a->size();++i){
      (*a)[i]=i;
      }
      *c = Sample(*a);
      *c *= 35;
      for(unsigned int i=0;i<c->size();++i){
      TS_ASSERT_EQUALS((*c)[i],35*(*a)[i]);
      }
    }

/**
 *Test for operator/=(Sample).
 */

    void testOperatorAssignmentByDividend_1 (){
      std::cout << std::endl;
      std::cout << "Testing operator/=(Sample)..." << std::endl;
      *a = Sample(10);
      for(unsigned int i=0;i<a->size();++i){
      (*a)[i]=i;
      }
      *c = Sample(*a);
      *d = Sample(*a);
      *d += 1;
      *c /= *d;
      for(unsigned int i=0;i<c->size();++i){
      TS_ASSERT_DELTA((*c)[i],(*a)[i]/(*d)[i],1e-10);
      }
    }

/**
 *Test for Sample operator/=(Scalar).
 */

    void testOperatorAssignmentByDividend_2 (){
      std::cout << std::endl;
      std::cout << "Testing Sample operator/=(Scalar)..." << std::endl;
      *a = Sample(10);
      for(unsigned int i=0;i<a->size();++i){
      (*a)[i]=i;}
      *c = Sample(*a);
      *c /= 35;
      for(unsigned int i=0;i<c->size();++i){
      TS_ASSERT_DELTA((*c)[i],(*a)[i]/35,1e-10);
      }
    }

/**
 *Test for Sample operator&=(Scalar).
 */

    void testOperatorAssignmentByBitwiseAND (){
      std::cout << std::endl;
      std::cout << "Testing Sample operator&=(Scalar)..." << std::endl;
      *a = Sample(10);
      for(unsigned int i=0;i<a->size();++i){
      (*a)[i]=i;
      }
      *b = Sample(*a);
      for(unsigned int i=0;i<b->size();++i){
      (*b)[i] = i-(*b)[i];
      }
      *c = Sample(*a);
      *c &= *b;
      for(unsigned int i=0;i<c->size();++i){
      TS_ASSERT_EQUALS( (*c)[i],std::min((*a)[i],(*b)[i]) );
      }
    }

/**
 *Test for Sample operator|=(Scalar).
 */

    void testOperatorAssignmentByBitwiseOR (){
      std::cout << std::endl;
      std::cout << "Testing Sample operator|=(Scalar)..." << std::endl;
      *a = Sample(10);
      for(unsigned int i=0;i<a->size();++i){
      (*a)[i]=i;
      }
      *b = Sample(*a);
      for(unsigned int i=0;i<b->size();++i){
      (*b)[i] = i-(*b)[i];
      }
      *c |= *b;
      for(unsigned int i=0;i<c->size();++i){
      TS_ASSERT_EQUALS((*c)[i],std::max((*a)[i],(*b)[i]));
      }
    }

/**
 *Test for sqr().
 */

    void testSqrFunction (){
      std::cout << std::endl;
      std::cout << "Testing Sample sqr()..." << std::endl;
      *c = Sample(3);
      (*c)[0] = -5;
      (*c)[1] = -0;
      (*c)[2] = -0.5;
      c->sqr();
      TS_ASSERT_EQUALS((*c)[0],25.0);
      TS_ASSERT_EQUALS((*c)[1],0.0);
      TS_ASSERT_EQUALS((*c)[2],0.25);
    }

/**
 *Test for sqrt().
 */

    void testSqrtFunction (){
      std::cout << std::endl;
      std::cout << "Testing Sample sqrt()..." << std::endl;
      *c = Sample(3);
      (*c)[0] = 0;
      (*c)[1] = 25;
      (*c)[2] = 625;
      c->sqrt();
      TS_ASSERT_EQUALS((*c)[0],0.0);
      TS_ASSERT_EQUALS((*c)[1],5.0);
      TS_ASSERT_EQUALS((*c)[2],25.0);
    }

/**
 *Test for norm().
 */

    void testNormFunction (){
      std::cout << std::endl;
      std::cout << "Testing Sample norm()..." << std::endl;
      *c = Sample(3);
      (*c)[0] = 0;
      (*c)[1] = 3;
      (*c)[2] = 4;
      TS_ASSERT_EQUALS(c->norm(),5.0);
    }

/**
 *Test for dotProduct.
 */

    void testdotProductFunction (){
      std::cout << std::endl;
      std::cout << "Testing Sample doProduct()..." << std::endl;
      *b = Sample(3);
      *c = Sample(3);
      (*b)[0] = 0;
      (*b)[1] = 3;
      (*b)[2] = 2;
      (*c)[0] = 10;
      (*c)[1] = 3;
      (*c)[2] = 8;
      TS_ASSERT_EQUALS(b->dotProduct(*c),5.0);
    }

  private:
    Sample *a;
    Sample *b;
    Sample *c;
    Sample *d;
};

#endif

