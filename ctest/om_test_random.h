/**
 * Test class for Random 
 * 
 * @author Albert Massayuki Kuniyoshi
 * @date 2007-07-05
 * $Id$
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
* \brief Test for Random Class
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
      r = new float;
      dim = new float;
      std::srand(0);
      *num = rand()/(RAND_MAX+1.0);
    }

    void tearDown (){
      delete a;
      delete num;
      delete r;
      delete dim;
    }

    void test1 (){
      std::cout << std::endl;
      std::cout << "Testing get( double min, double max ) ..." << std::endl;
      std::srand(0);
      TS_ASSERT_EQUALS((101.55-1.55)*(*num)+1.55,a->get( 1.55 , 101.55 ));
      TS_ASSERT(a->get(1.55,101.55)>=1.55&&a->get(1.55,101.55)<101.55);	
    }

    void test2 (){
      std::cout << std::endl;
      std::cout << "Testing get( double max ) ..." << std::endl;
      std::srand(0);
      TS_ASSERT_EQUALS(155.6432*(*num),a->get(155.6432));
      TS_ASSERT(a->get(155.6432)>=0&&a->get(155.6432)<155.6432);
    }

    void test3 (){
      std::cout << std::endl;
      std::cout << "Testing get() ..." << std::endl;
      std::srand(0);
      TS_ASSERT_EQUALS(*num,a->get());
    }

    void test4 (){
      std::cout << std::endl;
      std::cout << "Testing operator()( double min, double max ) ..." << std::endl;
      std::srand(0);
      TS_ASSERT_EQUALS((101.55-1.55)*(*num)+1.55,a->operator()( 1.55 , 101.55 ));
      TS_ASSERT(a->operator()(1.55,101.55)>=1.55&&a->operator()(1.55,101.55)<101.55);
    }

    void test5 (){
      std::cout << std::endl;
      std::cout << "Testing operator()( double max ) ..." << std::endl;
      std::srand(0);
      TS_ASSERT_EQUALS(155.6432*(*num),a->operator()(155.6432));
      TS_ASSERT(a->operator()(155.6432)>=0&&a->operator()(155.6432)<155.6432);
    }

    void test7 (){
      std::cout << std::endl;
      std::cout << "Testing get( int min, int max )..." << std::endl;
      std::srand(0);
      TS_ASSERT_EQUALS(int((101-1)*(*num)+1),a->get( 1 , 101 ));
      TS_ASSERT(a->get(1,101)>=1&&a->get(1,101)<101);
    }

    void test8 (){
      std::cout << std::endl;
      std::cout << "Testing get( int max ) ..." << std::endl;
      std::srand(0);
      TS_ASSERT_EQUALS(int(155*(*num)),a->get(155));
      TS_ASSERT(a->get(155)>=0&&a->get(155)<155);
    }

    void test9 (){
      std::cout << std::endl;
      std::cout << "Testing operator()( int min, int max ) ..." << std::endl;
      std::srand(0);
      TS_ASSERT_EQUALS(int((101-1)*(*num)+1),a->operator()( 1 , 101 ));
      TS_ASSERT(a->operator()(1,101)>=1&&a->operator()(1,101)<101);
    }

    void test10 (){
      std::cout << std::endl;
      std::cout << "Testing operator()( int max ) ..." << std::endl;
      std::srand(0);
      TS_ASSERT_EQUALS(int(155*(*num)),a->operator()(155));
      TS_ASSERT(a->operator()(155)>=0&&a->operator()(155)<155);
    }

    void test11 (){
      std::cout << std::endl;
      std::cout << "Testing get( long min, long max )..." << std::endl;
      std::srand(0);
      TS_ASSERT_EQUALS(long((101-1)*(*num)+1),a->get(long(1) ,long(101) ));
      TS_ASSERT(a->get(long(1),long(101))>=long(1)&&a->get(long(1),long(101))<long(101));
    }

    void test12 (){
      std::cout << std::endl;
      std::cout << "Testing get( long max ) ..." << std::endl;
      std::srand(0);
      TS_ASSERT_EQUALS(long(155*(*num)),a->get(long(155)));
      TS_ASSERT(a->get(long(155))>=long(0)&&a->get(long(155))<long(155));
    }

    void test13 (){
      std::cout << std::endl;
      std::cout << "Testing operator()( long min, long max ) ..." << std::endl;
      std::srand(0);
      TS_ASSERT_EQUALS(long((101-1)*(*num)+1),a->operator()(long(1) ,long(101) ));
      TS_ASSERT(a->operator()(long(1),long(101))>=long(1)&&a->operator()(long(1),long(101))<long(101));
    }

    void test14 (){
      std::cout << std::endl;
      std::cout << "Testing operator()( long max ) ..." << std::endl;
      std::srand(0);
      TS_ASSERT_EQUALS(long(155*(*num)),a->operator()(long(155)));
      TS_ASSERT(a->operator()(long(155))>=long(0)&&a->operator()(long(155))<long(155));
    }

    void test15 (){
      std::cout << std::endl;
      std::cout << "Testing discrete( float range, float dim_interv ) ..." << std::endl;
      *r = 4.0;
      *dim = 1.0;
      std::srand(0);
      TS_ASSERT_EQUALS(int((double(2.0*(*r)/(*dim))+1)*(*num))*(*dim)-(*r),a->discrete(*r,*dim));
      TS_ASSERT(a->operator()(long(155))>=long(0)&&a->operator()(long(155))<long(155));
    }

  private:
    Random *a;
    double *num;
    float *r;
    float *dim;
};

#endif

