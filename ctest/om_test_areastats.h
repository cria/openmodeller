/**
 * Test class for AreaStats 
 * 
 * @author Albert Massayuki Kuniyoshi
 * @date 2007-07-30
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

/** \ingroup test_AreaStats
* \brief Test for AreaStats
*/

#ifndef TEST_AREASTATS_HH
#define TEST_AREASTATS_HH

#include "cxxtest/TestSuite.h"
#include "AreaStats.hh"

class test_AreaStats : public CxxTest :: TestSuite 
{
	public:
		void setUp (){
				A = new AreaStats(Scalar(1.00));
				B = new AreaStats(*A);
				}

		void tearDown (){
				delete A;
				delete B;
				}

		void test1 (){
				std::cout << std::endl;
				std::cout << "Testing reset(Scalar predictiontThreshold) ..." << std::endl;
				A->reset(2.00);
				TS_ASSERT(A->getTotalArea()==0);
				TS_ASSERT(A->getAreaPredictedPresent()==0);
				TS_ASSERT(A->getAreaPredictedAbsent()==0);
				TS_ASSERT(A->getAreaNotPredicted()==0);
				TS_ASSERT(A->getPredictionThreshold()==Scalar(2.00));
				}

		private:
			AreaStats *A;
			AreaStats *B;
};
#endif
