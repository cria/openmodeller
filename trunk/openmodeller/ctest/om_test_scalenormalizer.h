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

    void setUp () {

    }

    void tearDown () {

      delete _scaleNormalizer;
    }

    void instantiateNormalizerWithoutUsingLayerAsRef () {

      // Instantiate normalizer that will be tested
      _scaleNormalizer = new ScaleNormalizer( 0, 1, false );

    }

    void instantiateNormalizerUsingLayerAsRef () {

      // Instantiate normalizer that will be tested
      _scaleNormalizer = new ScaleNormalizer( 0, 100, true );

    }

    void setupSampler () {

      // Prepare input to create a sampler

      // First occurrence
      OccurrencePtr occ1 = new OccurrenceImpl();

      Scalar values1[2] = {10.0, 30.0};
      Sample sample1( 2, values1 );

      occ1->setUnnormalizedEnvironment( sample1 );

      // Second occurrence
      OccurrencePtr occ2 = new OccurrenceImpl();

      Scalar values2[2] = {60.0, 40.0};
      Sample sample2( 2, values2 );

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

    void test1 () {

      setupSampler();
      instantiateNormalizerWithoutUsingLayerAsRef();

      std::cout << "Testing scale normalizer with interval [0,1] using minimum and maximum values from input samples..." << std::endl;

      CxxTest::setAbortTestOnFail( true );

      _scaleNormalizer->computeNormalization( _samplerPtr );

      // Manual calculations:
      // dimension 1: min=10 max=60 scale=1/(60-10)=0.02 offset=0-(0.02*10)=-0.2
      // dimension 2: min=30 max=40 scale=1/(40-30)=0.1  offset=0-(0.1 *30)=-3.0

      // Scales:
      TS_ASSERT( _scaleNormalizer->_scales.size() == 2 );

      TS_ASSERT( _scaleNormalizer->_scales[0] == 0.02 );
      TS_ASSERT( _scaleNormalizer->_scales[1] == 0.1 );

      // Offsets:
      TS_ASSERT( _scaleNormalizer->_offsets.size() == 2 );

      TS_ASSERT( _scaleNormalizer->_offsets[0] == -0.2 );
      TS_ASSERT( _scaleNormalizer->_offsets[1] == -3.0 );

      // Sample to be normalized
      Scalar values[2] = {50.0, 35.0};
      Sample sample( 2, values );

      _scaleNormalizer->normalize( &sample );

      // Manual calculations:
      // dimension 1: (50*0.02)-0.2=0.8
      // dimension 2: (35*0.1)-3.0=0.5

      TS_ASSERT( sample[0] == 0.8 );
      TS_ASSERT( sample[1] == 0.5 );
    }

    void test2 () {

      setupSampler();
      instantiateNormalizerUsingLayerAsRef();

      std::cout << "Testing scale normalizer with interval [0,1] using minimum and maximum values from layers..." << std::endl;

      CxxTest::setAbortTestOnFail( true );

      _scaleNormalizer->computeNormalization( _samplerPtr );

      // Manual calculations:
      // dimension 1: min=0.000    max=2137.000 
      //              scale=100/(2137-0)=0.04679 
      //              offset=0-(0.04679*0)=0.0
      // dimension 2: min=-545.420 max=3342.520 
      //              scale=100/(3342.520+545.420)=100/3887.940=0.02572
      //              offset=0-(0.02572*-545.420)=14.030

      // Scales:
      TS_ASSERT( _scaleNormalizer->_scales.size() == 2 );

      TS_ASSERT_DELTA( _scaleNormalizer->_scales[0], 0.04679, 0.00001 );
      TS_ASSERT_DELTA( _scaleNormalizer->_scales[1], 0.02572, 0.00001 );

      // Offsets:
      TS_ASSERT( _scaleNormalizer->_offsets.size() == 2 );

      TS_ASSERT( _scaleNormalizer->_offsets[0] == 0.0 );
      TS_ASSERT_DELTA( _scaleNormalizer->_offsets[1], 14.030, 0.002 );

      // Sample to be normalized
      Scalar values[2] = {1200.0, 2000.0};
      Sample sample( 2, values );

      _scaleNormalizer->normalize( &sample );

      // Manual calculations:
      // dimension 1: (1200.0*0.04679)-0.0   =56.148
      // dimension 2: (2000.0*0.02572)+14.030=65.470

      TS_ASSERT_DELTA( sample[0], 56.148, 0.01 );
      TS_ASSERT_DELTA( sample[1], 65.470, 0.01 );
    }

  private:

    SamplerPtr _samplerPtr;

    ScaleNormalizer * _scaleNormalizer;
};


#endif

