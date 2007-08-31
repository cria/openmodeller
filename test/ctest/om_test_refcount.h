/**
 * Test class for refcount
 * 
 * @author Albert Massayuki Kuniyoshi
 * @date 2007-07-19
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
* \brief Test for refcount Class
*/

#ifndef REFCOUNT_HH
#define REFCOUNT_HH

#include "cxxtest/TestSuite.h"
#include "refcount.hh"
#include "Configuration.hh"

class test_refcount : public CxxTest :: TestSuite 
{

  public:
    void setUp (){
      a = new ConfigurationPtr();
      b = new ConfigurationPtr(*a);
      c = new ConstConfigurationPtr();
      d = new ConstConfigurationPtr(*c);
    }

    void tearDown (){
      delete a;
      delete b;
      delete c;
      delete d;
    }

/**
 *Test for operator->().
 */

    void test1 (){
      std::cout << "Testing operator->()..." << std::endl;
      TS_ASSERT(a->operator->()==0);
      TS_ASSERT_EQUALS(a->operator->(),b->operator->());
      TS_ASSERT(c->operator->()==0);
      TS_ASSERT_EQUALS(c->operator->(),d->operator->());
    }

/**
 *Test for operator!().
 */

    void test2 (){
      std::cout << "Testing operator!()..." << std::endl;
      TS_ASSERT(a->operator!());
      TS_ASSERT(b->operator!());
      TS_ASSERT(c->operator!());
      TS_ASSERT(d->operator!());
    }

/**
 *Test for operator bool().
 */

    void test3 (){
      std::cout << "Testing operator bool()..." << std::endl;
      TS_ASSERT(!a->operator bool());
      TS_ASSERT(!b->operator bool());
      TS_ASSERT(!c->operator bool());
      TS_ASSERT(!d->operator bool());
    }

//Obs:Assignment operators were not tested.

  private:
    ConfigurationPtr *a;
    ConfigurationPtr *b;
    ConstConfigurationPtr *c;
    ConstConfigurationPtr *d;
};

#endif

