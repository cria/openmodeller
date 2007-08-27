/**
 * Test class for AlgParameter
 *
 * @author Albert Massayuki Kuniyoshi
 * @date 2007-08-22
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
* \brief Test for AlgParameter Class
*/

#ifndef TEST_ALGPARAMETER_HH
#define TEST_ALGPARAMETER_HH
#include "cxxtest/TestSuite.h"
#include <openmodeller/AlgParameter.hh>
#include <string.h>

class test_AlgParameter : public CxxTest :: TestSuite 
{

  public:
    void setUp (){
      a[0] = new char;
      a[1] = new char;
      a[2] = new char;

      b[0] = new char;
      b[1] = new char;
      b[2] = new char;
      b[3] = new char;
      b[4] = new char;
      b[5] = new char;

      (*a)[0] = 'i';
      (*a)[1] = 'd';

      (*b)[0] = 'v';
      (*b)[1] = 'a';
      (*b)[2] = 'l';
      (*b)[3] = 'u';
      (*b)[4] = 'e';

      A = new AlgParameter();
      B = new AlgParameter(*a,*b);
      C = new AlgParameter(*A);
    }

    void tearDown (){
      delete a[0];
      delete a[1];
      delete a[2];

      delete b[0];
      delete b[1];
      delete b[2];
      delete b[3];
      delete b[4];
      delete b[5];

      delete A;
      delete B;
      delete C;
    }

    void test1 (){
      std::cout << "Testing AlgParameter default constructor ..." << std::endl;
      TS_ASSERT(A->id()==0);
      TS_ASSERT(A->value()==0);
    }

    void test2 (){
      std::cout << "Testing AlgParameter constructor ..." << std::endl;
      TS_ASSERT(strcmp(B->id(),*a)==0);
      TS_ASSERT(strcmp(B->value(),*b)==0);
    }

    void test3 (){
      std::cout << "Testing AlgParameter constructor ..." << std::endl;
      TS_ASSERT(C->id()==0);
      TS_ASSERT(C->value()==0);
    }

    void test4 (){
      std::cout << "Testing Algparameter operator=(const AlgParameter &) ..." << std::endl;
      B->operator=(*A);
      TS_ASSERT(A->id()==0);
      TS_ASSERT(A->value()==0);
      TS_ASSERT(B->id()==0);
      TS_ASSERT(B->value()==0);
    }

    void test5 (){
      std::cout << "Testing setId(char const *id) ..." << std::endl;
      (*a)[2] = '_';
      A->setId(*a);
      TS_ASSERT(strcmp(A->id(),*a)==0);
      TS_ASSERT(A->value()==0);
    }

    void test6 (){
      std::cout << "Testing setValue(char const *val) ..." << std::endl;
      (*b)[5] = '_';
      A->setValue(*b);
      TS_ASSERT(A->id()==0);
      TS_ASSERT(strcmp(A->value(),*b)==0);
    }

    void test7 (){
      std::cout << "Testing setValue(double val) ..." << std::endl;
      char buf[32];
      sprintf(buf,"%-32.8f",2.0);
      A->setValue(2.0);
      TS_ASSERT(strcmp(A->value(),buf)==0);
    }

    void test8 (){
      std::cout << "Testing valueReal() ..." << std::endl;
      TS_ASSERT(atof(B->value())==B->valueReal());
      TS_ASSERT(A->valueReal()==0.0);
    }

  private:
    char *a[10];
    char *b[10];
    AlgParameter *A;
    AlgParameter *B;
    AlgParameter *C;
};

#endif
