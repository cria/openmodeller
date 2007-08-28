/**
 * Test class for <Insert Class Name Here> 
 * 
 * @author <Your Name>
 * @date 2007-08-02
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
* \brief A short description of this test goes here
*/

#ifndef <REPLACE_ME>_HH
#define <REPLACE_ME>_HH

#include "cxxtest/TestSuite.h"

class <YourClassName> : public CxxTest :: TestSuite 
{

  public:
    void setUp (){
    }

    void tearDown (){
    }

    void test1 (){
      std::cout << "" << std::endl;
      TS_ASSERT(<yourcriteria> == <yourothercriteria>);
    }

  private:
    <test vars here>
};

#endif
