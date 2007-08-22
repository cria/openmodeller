/**
 * Test class for Occurrence
 * 
 * @author Albert Massayuki Kuniyoshi
 * @date 2007-08-21
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
* \brief Test for OccurrenceImpl Class
*/

#ifndef TEST_OCCURRENCE_HH
#define TEST_OCCURRENCE_HH
#include "cxxtest/TestSuite.h"
#include <openmodeller/Occurrence.hh>
#include <openmodeller/Sample.hh>
#include <openmodeller/om_defs.hh>
#include <string>
#include <vector>

class test_Occurrence : public CxxTest :: TestSuite 
{

  public:
    void setUp (){
      name = new std::string;
      *name = "id";
      attr = new std::vector<Scalar>;
      env = new std::vector<Scalar>;
      Scalar num(1.0);
      for(int i=0; i<10; i++){
      attr->push_back(num);
      env->push_back(num);
      num = num + Scalar(1.0);
      }
      A = new OccurrenceImpl();
      B = new OccurrenceImpl(*name,Coord(1.0),Coord(2.0),Scalar(3.0),Scalar(4.0));
      C = new OccurrenceImpl(*name,Coord(1.0),Coord(2.0),Scalar(3.0),Scalar(4.0),*attr,*env);
      D = new OccurrenceImpl(*name,Coord(1.0),Coord(2.0),Scalar(3.0),Scalar(4.0),Sample(1),Sample(2));
    }

    void tearDown (){
      delete name;
      delete attr;
      delete env;
      delete A;
      delete B;
      delete C;
      delete D;
    }

    void test1 (){
      std::cout << "Testing Occurrence default constructor ..." << std::endl;
      TS_ASSERT((A->id()).empty());
      TS_ASSERT(A->x()==Coord(0.0));
      TS_ASSERT(A->y()==Coord(0.0));
      TS_ASSERT(A->error()==Scalar(0.0));
      TS_ASSERT(A->abundance()==Scalar(0.0));
      TS_ASSERT(A->attributes()==Sample());
      TS_ASSERT(A->environment()==Sample());
      TS_ASSERT(A->originalEnvironment()==Sample());
    }

    void test2 (){
      std::cout << "Testing Occurrence constructor with uncertanty I ..." << std::endl;
      TS_ASSERT(B->id()==*name);
      TS_ASSERT(B->x()==Coord(1.0));
      TS_ASSERT(B->y()==Coord(2.0));
      TS_ASSERT(B->error()==Scalar(3.0));
      TS_ASSERT(B->abundance()==Scalar(4.0));
      TS_ASSERT(B->attributes()==Sample());
      TS_ASSERT(B->environment()==Sample());
      TS_ASSERT(B->originalEnvironment()==Sample());
    }

    void test3 (){
      std::cout << "Testing Occurrence constructor with uncertanty using std::vector ..." << std::endl;
      TS_ASSERT(C->id()==*name);
      TS_ASSERT(C->x()==Coord(1.0));
      TS_ASSERT(C->y()==Coord(2.0));
      TS_ASSERT(C->error()==Scalar(3.0));
      TS_ASSERT(C->abundance()==Scalar(4.0));
      TS_ASSERT(C->attributes()==*attr);
      TS_ASSERT(C->environment()==C->originalEnvironment());
      TS_ASSERT(C->originalEnvironment()==*env);
    }

    void test4 (){
      std::cout << "Testing Occurrence constructor with uncertanty II ..." << std::endl;
      TS_ASSERT(D->id()==*name);
      TS_ASSERT(D->x()==Coord(1.0));
      TS_ASSERT(D->y()==Coord(2.0));
      TS_ASSERT(D->error()==Scalar(3.0));
      TS_ASSERT(D->abundance()==Scalar(4.0));
      TS_ASSERT(D->attributes()==Sample(1));
      TS_ASSERT(D->environment()==D->originalEnvironment());
      TS_ASSERT(D->originalEnvironment()==Sample(2));
    }

    void test5 (){
      std::cout << "Testing Occurrence constructor without uncertanty..." << std::endl;
      TS_ASSERT(true);
    }

    void test6 (){
      std::cout << "Testing Occurrence constructor by OccurrenceImpl reference ..." << std::endl;
      TS_ASSERT(true);
    }

  private:
      std::string *name;
      std::vector<Scalar> *attr;
      std::vector<Scalar> *env;
      OccurrenceImpl *A;
      OccurrenceImpl *B;
      OccurrenceImpl *C;
      OccurrenceImpl *D;
};

#endif
