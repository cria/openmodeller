/**
 * Test class for AreaStats 
 * 
 * @author Albert Massayuki Kuniyoshi
 * @date 2007-07-30
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
* \brief Test for AreaStats Class
*/

#ifndef TEST_AREASTATS_HH
#define TEST_AREASTATS_HH

#include "cxxtest/TestSuite.h"
#include "AreaStats.hh"
#include "Configuration.hh"
#include <float.h>
#include <string>

class test_AreaStats : public CxxTest :: TestSuite 
{
  public:
    void setUp (){
      A = new AreaStats(Scalar(1.00));
      B = new AreaStats(*A);
      C = new ConfigurationPtr();
    }

    void tearDown (){
      delete A;
      delete B;
      delete C;
    }

/**
 *Test for AreaStats(Scalar predictionThreshold).
 */

    void testAreaStatsConstructor_1(){
      std::cout << std::endl;
      std::cout << "Testing AreaStats(Scalar predictionThreshold) ..." << std::endl;
      TS_ASSERT_EQUALS(A->getTotalArea(),0);
      TS_ASSERT_EQUALS(A->getAreaPredictedPresent(),0);
      TS_ASSERT_EQUALS(A->getAreaPredictedAbsent(),0);
      TS_ASSERT_EQUALS(A->getAreaNotPredicted(),0);
      TS_ASSERT_EQUALS(A->getPredictionThreshold(),Scalar(1.00));
    }

/**
 *Test for AreaStats(const AreaStats *areaStats).
 */

    void testAreaStatsConstructor_2 (){
      std::cout << std::endl;
      std::cout << "Testing AreaStats(const AreaStats *areaStats) ..." << std::endl;
      TS_ASSERT_EQUALS(B->getTotalArea(),0);
      TS_ASSERT_EQUALS(B->getAreaPredictedPresent(),0);
      TS_ASSERT_EQUALS(B->getAreaPredictedAbsent(),0);
      TS_ASSERT_EQUALS(B->getAreaNotPredicted(),0);
      TS_ASSERT_EQUALS(B->getPredictionThreshold(),Scalar(1.00));
    }

/**
 *Test for reset(Scalar predictionThreshold).
 */

    void testResetFunction (){
      std::cout << std::endl;
      std::cout << "Testing reset(Scalar predictiontThreshold) ..." << std::endl;
      A->reset(2.00);
      TS_ASSERT_EQUALS(A->getTotalArea(),0);
      TS_ASSERT_EQUALS(A->getAreaPredictedPresent(),0);
      TS_ASSERT_EQUALS(A->getAreaPredictedAbsent(),0);
      TS_ASSERT_EQUALS(A->getAreaNotPredicted(),0);
      TS_ASSERT_EQUALS(A->getPredictionThreshold(),Scalar(2.00));
    }

/**
 *Test for addPrefiction(Scalar Value).
 */

    void testAddPrefictionFunction_1 (){
      std::cout << std::endl;
      std::cout << "Testing addPrediction(Scalar value) ..." << std::endl;
      A->addPrediction(Scalar(1.00));
      TS_ASSERT_EQUALS(A->getTotalArea(),1);
      TS_ASSERT_EQUALS(A->getAreaPredictedPresent(),1);
      TS_ASSERT_EQUALS(A->getAreaPredictedAbsent(),0);
      TS_ASSERT_EQUALS(A->getAreaNotPredicted(),0);
      TS_ASSERT_EQUALS(A->getPredictionThreshold(),Scalar(1.00));
    }

/**
 *Test for addPrediction(Scalar value).
 */

    void testAddPrefictionFunction_2 (){
      std::cout << std::endl;
      std::cout << "Testing addPrediction(Scalar value) ..." << std::endl;
      A->addPrediction(Scalar(1+DBL_EPSILON));
      TS_ASSERT_EQUALS(A->getTotalArea(),1);
      TS_ASSERT_EQUALS(A->getAreaPredictedPresent(),1);
      TS_ASSERT_EQUALS(A->getAreaPredictedAbsent(),0);
      TS_ASSERT_EQUALS(A->getAreaNotPredicted(),0);
      TS_ASSERT_EQUALS(A->getPredictionThreshold(),Scalar(1.00));
    }

/**
 *Test for addPrediction(Scalar value).
 */

    void testAddPrediction_3 (){
      std::cout << std::endl;
      std::cout << "Testing addPrediction(Scalar value) ..." << std::endl;
      A->addPrediction(Scalar(1-DBL_EPSILON));
      TS_ASSERT_EQUALS(A->getTotalArea(),1);
      TS_ASSERT_EQUALS(A->getAreaPredictedPresent(),0);
      TS_ASSERT_EQUALS(A->getAreaPredictedAbsent(),1);
      TS_ASSERT_EQUALS(A->getAreaNotPredicted(),0);
      TS_ASSERT_EQUALS(A->getPredictionThreshold(),Scalar(1.00));
    }

/**
 *Test for addNonPrediction().
 */

    void testAddNonPrediction (){
      std::cout << std::endl;
      std::cout << "Testing addNonPrediction() ..." << std::endl;
      A->addNonPrediction();
      TS_ASSERT_EQUALS(A->getTotalArea(),1);
      TS_ASSERT_EQUALS(A->getAreaPredictedPresent(),0);
      TS_ASSERT_EQUALS(A->getAreaPredictedAbsent(),0);
      TS_ASSERT_EQUALS(A->getAreaNotPredicted(),1);
      TS_ASSERT_EQUALS(A->getPredictionThreshold(),Scalar(1.00));
    }

/**
 *Test for getConfiguration().
 */

    void testGetConfiguration (){
      std::cout << std::endl;
      std::cout << "Testing getConfiguration() const..." << std::endl;
      //Under development
      //*C=A->getConfiguration();
      //TS_ASSERT(C->getAttributeAsInt("TotalCells",-1)==0);
      //TS_ASSERT(C->getAttributeAsInt("CellsPredicted",-1)==0);
      //TS_ASSERT(C->getAttributeAsInt("PredictionThreshold",-1)==1.00);
    }

  private:
    AreaStats *A;
    AreaStats *B;
    ConfigurationPtr *C;
};

#endif

