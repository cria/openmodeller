/**
 * Test class for AlgorithmFactory 
 * 
 * @author <Your Name>
 * @date 2007-08-27
 * $Id: om_test_template.h 3609 2007-08-27 14:22:51Z albertmk $
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

#ifndef ALGORITHMFACTORY_HH
#define ALGORITHMFACTORY_HH

#include "cxxtest/TestSuite.h"
#include <openmodeller/AlgorithmFactory.hh>

class test_AlgorithmFactory : public CxxTest :: TestSuite 
{

  public:
    void setUp (){
    }

    void tearDown (){
    }

    void test1 (){
      std::cout << "Testing ..." << std::endl;
      TS_ASSERT(true);
    }

  private:
    
};

#endif
