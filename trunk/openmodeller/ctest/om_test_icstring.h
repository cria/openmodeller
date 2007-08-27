/**
 * Test class for icstring
 * 
 * @authors Ricardo Scachetti Pereira and Albert Massayuki Kuniyoshi
 * @date 2007-07-03
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
* \brief Test for icstring Class
*/

#ifndef TEST_ICSTRING_HH
#define TEST_ICSTRING_HH

#include "cxxtest/TestSuite.h"
#include "ignorecase_traits.hh"
#include <string>

class test_icstring : public CxxTest :: TestSuite 
{

  public:
    void setUp (){
      a = new icstring("AAAA");
      b = new icstring("aaaa");
      c = new icstring("AaAa");
      d = new icstring("aAaA");
    }

    void tearDown (){
      delete a;
      delete b;
      delete c;
      delete d;
    }

    void test1 (){
      std::cout << std::endl;
      std::cout << "Testing Compare 1..." << std::endl;
      TS_ASSERT(*a==*b);
    }

    void test2 (){
      std::cout << std::endl;
      std::cout << "Testing Compare 2..." << std::endl;
      TS_ASSERT(*c==*d);
    }

    void test3 (){
      std::cout << std::endl;
      std::cout << "Testing Compare 3..." << std::endl;
      TS_ASSERT(*d==*d);
    }

    void test4 (){
      std::cout << std::endl;
      std::cout << "Testing Compare 4..." << std::endl;
      *a = icstring("bbbb");
      TS_ASSERT(!(*a==*b));
    }

    void test5 (){
      std::cout << std::endl;
      std::cout << "Testing Compare 5..." << std::endl;
      *a = icstring("a");
      TS_ASSERT(*a==*a);
    }

    void test6 (){
      std::cout << std::endl;
      std::cout << "Testing Compare 6..." << std::endl;
      *a = icstring("a");
      *b = icstring("b");
      TS_ASSERT(!(*a==*b));
    }

  private:
    icstring *a;
    icstring *b;
    icstring *c;
    icstring *d;
};

#endif

