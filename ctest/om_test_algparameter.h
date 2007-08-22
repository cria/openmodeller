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

class test_AlgParameter : public CxxTest :: TestSuite 
{

  public:
    void setUp (){
                 A = new AlgParameter();
                 B = new AlgParameter("id","value");
                 C = new AlgParameter(*A);
    }

    void tearDown (){
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
                TS_WARN("Need to fix characters!");
/*
                TS_ASSERT_EQUALS(B->id(),"id");
                TS_ASSERT_EQUALS(B->value(),"value");
*/
    }

    void test3 (){
      std::cout << "Testing AlgParameter constructor ..." << std::endl;
                TS_ASSERT(C->id()==0);
                TS_ASSERT(C->value()==0);
    }

    void test4 (){
      std::cout << "Testing Algparameter operator=(const AlgParameter &) ..." << std::endl;
                B->operator=(*A);
                TS_ASSERT_EQUALS(B->id(),A->id());
                TS_ASSERT_EQUALS(B->value(),A->value());
    }

    void test5 (){
      std::cout << "Testing setId(char const *id) ..." << std::endl;
                A->setId("id_");
                TS_ASSERT(A->id()=="id_");
                TS_ASSERT(A->value()=="value");
    }

    void test6 (){
      std::cout << "Testing setValue(char const *val) ..." << std::endl;
                A->setValue("value_");
                TS_ASSERT(A->id()=="id_");
                TS_ASSERT(A->value()=="value_");
    }

    void test7 (){
      std::cout << "Testing setValue(double val) ..." << std::endl;

    }

  private:
             AlgParameter *A;
             AlgParameter *B;
             AlgParameter *C;
};

#endif
