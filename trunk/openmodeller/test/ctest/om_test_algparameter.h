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
#include <stdlib.h>

class test_AlgParameter : public CxxTest :: TestSuite 
{

  public:
    void setUp (){

      a = "id";
      b = "value";

      A = new AlgParameter();
      B = new AlgParameter(a,b);
      C = new AlgParameter(*A);
    }

    void tearDown (){

      delete A;
      delete B;
      delete C;
    }

    void test1 (){
      std::cout << "Testing AlgParameter default constructor..." << std::endl;
      TS_ASSERT(A->id()=="");
      TS_ASSERT(A->value()=="");
    }

    void test2 (){
      std::cout << "Testing AlgParameter constructor with id/value parameters..." << std::endl;
      TS_ASSERT(B->id()==a);
      TS_ASSERT(B->value()==b);
    }

    void test3 (){
      std::cout << "Testing AlgParameter constructor with AlgParameter parameter..." << std::endl;
      TS_ASSERT(C->id()=="");
      TS_ASSERT(C->value()=="");
    }

    void test4 (){
      std::cout << "Testing Algparameter operator=(const AlgParameter &)..." << std::endl;
      B->operator=(*A);
      TS_ASSERT(A->id()==B->id());
      TS_ASSERT(A->value()==B->value());
    }

    void test5 (){
      std::cout << "Testing setId(std::string const id)..." << std::endl;
      a = "_id_";
      A->setId(a);
      TS_ASSERT(A->id()==a);
      TS_ASSERT(A->value()=="");
    }

    void test6 (){
      std::cout << "Testing setValue(std::string const val)..." << std::endl;
      b = "test";
      A->setValue(b);
      TS_ASSERT(A->id()=="");
      TS_ASSERT(A->value()==b);
    }

    void test7 (){
      std::cout << "Testing setValue(double val) ..." << std::endl;
      char buf[32];
      sprintf(buf,"%-32.8f",2.0);
      A->setValue(2.0);
      TS_ASSERT(A->value()==buf);
    }

    void test8 (){
      std::cout << "Testing valueReal() ..." << std::endl;
      TS_ASSERT(atof(B->value().c_str())==B->valueReal());
      TS_ASSERT(A->valueReal()==0.0);
    }

  private:
    std::string a;
    std::string b;
    AlgParameter *A;
    AlgParameter *B;
    AlgParameter *C;
};

#endif
