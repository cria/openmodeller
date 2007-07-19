/**
 * Test class for refcount
 * 
 * @author Albert Massayuki Kuniyoshi
 * @date 2007-07-19
 * $Id: OpenModeller.cpp 3201 2007-06-18 21:08:16Z rdg $
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
* \brief A short description of this test goes here
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
			b = new ConfigurationPtr();
    }

    void tearDown (){
			delete a;
			delete b;
    }
    void test1 (){
			std::cout << "Testing operator->()..." << std::endl;
			TS_ASSERT(a->operator->() == 0);
    }

  private:
			ConfigurationPtr *a;
			ConfigurationPtr *b;
};


#endif

