/**
 * Test class for ScaleNormalizer 
 * 
 * @author Renato De Giovanni
 * @date 2007-08-02
 * $Id $
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
* \brief Test for ScaleNormalizer.
* \note The cpp needs to be generated with the options --have-eh --abort-on-fail
*/

#ifndef TEST_SCALENORMALIZER_HH
#define TEST_SCALENORMALIZER_HH

#include "cxxtest/TestSuite.h"

#include <openmodeller/ScaleNormalizer.hh>
#include <openmodeller/Sample.hh>
#include <openmodeller/Occurrence.hh>
#include <openmodeller/Occurrences.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/Sampler.hh>

class test_ScaleNormalizer : public CxxTest :: TestSuite 
{

  public:

    void setUp (){

      // Instantiate normalizer that will be tested
      _scaleNormalizer = new ScaleNormalizer( 0, 1, false );

      // Prepare input to create a sampler

      // First occurrence
      OccurrencePtr occ1 = new OccurrenceImpl();

      Scalar values1[3] = {10.0, 30.0};
      Sample sample1( 3, values1 );

      occ1->setUnnormalizedEnvironment( sample1 );

      // Second occurrence
      OccurrencePtr occ2 = new OccurrenceImpl();

      Scalar values2[3] = {60.0, 40.0};
      Sample sample2( 3, values2 );

      occ2->setUnnormalizedEnvironment( sample2 );

      // Presences with the two occurrences
      double default_abundance = 1.0;

      OccurrencesPtr presences = new OccurrencesImpl( default_abundance );
      presences->insert( occ1 );
      presences->insert( occ2 );

      // Absences (empty)
      OccurrencesPtr absences = new OccurrencesImpl( default_abundance );

      // Categorical layers (empty)
      std::vector<std::string> categorical_layers;

      // Continuous layers (need two for compatibility with the number of 
      // environment values passed to each occurrence)
      std::vector<std::string> continuous_layers;

      std::string layer1(EXAMPLE_DIR);
      layer1.append("/rain_coolest.tif");

      std::string layer2(EXAMPLE_DIR);
      layer2.append("/temp_avg.tif");

      continuous_layers.push_back( layer1 );
      continuous_layers.push_back( layer2 );

      // Create environment with previous layer sets
      EnvironmentPtr env = createEnvironment( categorical_layers, continuous_layers );

      // Create sampler
      bool is_normalized = false;

      _samplerPtr = new SamplerImpl( env, presences, absences, is_normalized );
    }

    void tearDown (){

      delete _scaleNormalizer;
    }

    void test1 (){

      std::cout << "Testing scale normalizer with interval [0,1] using minimum and maximum values from input samples..." << std::endl;

      CxxTest::setAbortTestOnFail( true );

      _scaleNormalizer->computeNormalization( _samplerPtr );

      TS_ASSERT( _scaleNormalizer->_offsets.size() == 2 );

      // Manual calculations:
      // dimension 1: min=10 max=60 scale=1/(60-10)=0.02   offset=0-(0.02  *10)=-0.2
      // dimension 2: min=30 max=40 scale=1/(40-30)=0.1    offset=0-(0.1   *30)=-3.0

      TS_ASSERT( _scaleNormalizer->_offsets[0] == -0.2 );
      TS_ASSERT( _scaleNormalizer->_offsets[1] == -3.0 );
    }

  private:

    SamplerPtr _samplerPtr;

    ScaleNormalizer * _scaleNormalizer;
};


#endif

