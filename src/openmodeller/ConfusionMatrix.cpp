/**
 * Definition of ConfusionMatrix class 
 * 
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date 2004-10-18
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 *
 * http://www.nhm.ku.edu
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

#include <stdio.h>

#include <openmodeller/ConfusionMatrix.hh>
#include <openmodeller/Sampler.hh>
#include <openmodeller/Algorithm.hh>
#include <openmodeller/Occurrences.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Log.hh>

#include <string.h>

ConfusionMatrix::ConfusionMatrix(Scalar predictionThreshold)
{
  reset(predictionThreshold);
}


ConfusionMatrix::~ConfusionMatrix()
{
}

void ConfusionMatrix::reset(Scalar predictionThreshold)
{
  _ready = false;
  _predictionThreshold = predictionThreshold;
  memset(_confMatrix, 0, sizeof(int) * 4);
}

void ConfusionMatrix::setLowestTrainingThreshold(const Model& model, const SamplerPtr& sampler)
{
  Log::instance()->debug( "Determining lowest training threshold\n" );

  model->setNormalization( sampler );

  OccurrencesPtr presences = sampler->getPresences();

  OccurrencesImpl::const_iterator it = presences->begin();
  OccurrencesImpl::const_iterator fin = presences->end();

  _predictionThreshold = 2.0;

  Scalar predictionValue;

  while( it != fin ) {

    Sample sample = (*it)->environment();

    if ( sample.size() > 0 ) {

      predictionValue = model->getValue( sample );

      if ( predictionValue > 0.0 && predictionValue < _predictionThreshold ) {

        _predictionThreshold = predictionValue;
      }
    }

    ++it;
  }

  if ( _predictionThreshold > 1.0 ) {

    // Reset to default value
    _predictionThreshold = CONF_MATRIX_DEFAULT_THRESHOLD;

    Log::instance()->warn( "Could not find any valid threshold among all training points. Resetting confusion matrix threshold to the default value (%f)\n", CONF_MATRIX_DEFAULT_THRESHOLD );
  }
  else {

    Log::instance()->debug( "Lowest training threshold is %f\n", _predictionThreshold );
  }
}

/* 
 * Confusion Matrix:
 *  1st row is predicted absence  (index [0][x])
 *  2nd row is predicted presence (index [1][x])
 *  1st column is actual absence  (index [y][0])
 *  2nd column is actual presence (index [y][1])
 */

void ConfusionMatrix::calculate(const EnvironmentPtr & env,
				const Model& model,
				const OccurrencesPtr& presences, 
				const OccurrencesPtr& absences)
{
  Log::instance()->debug( "Calculating confusion matrix\n" );

  int i;
  int predictionIndex, actualIndex;
  Scalar predictionValue;

  reset(_predictionThreshold);

  OccurrencesImpl::const_iterator it = presences->begin();
  OccurrencesImpl::const_iterator fin = presences->end();

  Log::instance()->debug( "Testing presences\n" );

  i = 0;
  while( it != fin ) {

    Sample sample; 

    if ( env ) {

      sample = env->get( (*it)->x(), (*it)->y() );
    }
    else {

      sample = (*it)->environment();
    }

    if ( sample.size() > 0 ) {

      ++i;

      predictionValue = model->getValue( sample );
      predictionIndex = (predictionValue >= _predictionThreshold);

      actualIndex = 1; //data.isPresence(i);
      _confMatrix[predictionIndex][actualIndex]++;

      Log::instance()->debug( "Probability for point %s (%f,%f): %f\n", 
                   ((*it)->id()).c_str(), (*it)->x(), (*it)->y(), predictionValue );
    }

    ++it;
  }

  Log::instance()->debug( "Tested %u presence point(s)\n", i );

  Log::instance()->debug( "Testing absences\n" );

  i = 0;

  if ( absences && ! absences->isEmpty() ) {

    it = absences->begin();
    fin = absences->end();

    while( it != fin ) {

      Sample sample;

      if ( env ) {

	sample = env->get( (*it)->x(), (*it)->y() );
      }
      else {

	sample = (*it)->environment();
      }

      if ( sample.size() > 0 ) {

        ++i;

	predictionValue = model->getValue( sample );
	predictionIndex = (predictionValue >= _predictionThreshold);
	actualIndex = 0; //data.isAbsence(i);
	_confMatrix[predictionIndex][actualIndex]++;

        Log::instance()->debug( "Probability for point %s (%f,%f): %f\n", 
                     ((*it)->id()).c_str(), (*it)->x(), (*it)->y(), predictionValue );
      }

      ++it;
    }
  }

  Log::instance()->debug( "Tested %u absence point(s)\n", i );

  _ready = true;
}

void ConfusionMatrix::calculate(const Model& model, const SamplerPtr& sampler)
{
  model->setNormalization( sampler );

  calculate(sampler->getEnvironment(), model, sampler->getPresences(), sampler->getAbsences() );
}


int ConfusionMatrix::getValue(Scalar predictionValue, 
			      Scalar actualValue) const
{
  int predictedIndex, actualIndex;

  predictedIndex = (predictionValue >= _predictionThreshold);
  actualIndex    = (actualValue     >= _predictionThreshold);

  return _confMatrix[predictedIndex][actualIndex];
}


double ConfusionMatrix::getAccuracy() const
{
  Scalar total = 
    _confMatrix[0][0] + _confMatrix[0][1] + 
    _confMatrix[1][0] + _confMatrix[1][1];
  
  if (_ready && total)
    return ( _confMatrix[0][0] + _confMatrix[1][1] ) / total;
  else
    return -1.0;
}


double ConfusionMatrix::getCommissionError() const
{
  Scalar total = _confMatrix[1][0] + _confMatrix[0][0];
  if (_ready && total)
    return _confMatrix[1][0] / total;
  else
    return -1.0;
}


double ConfusionMatrix::getOmissionError() const
{
  Scalar total = _confMatrix[0][1] + _confMatrix[1][1];
  if (_ready && total)
    return _confMatrix[0][1] / total;
  else
    return -1.0;
}

ConfigurationPtr 
ConfusionMatrix::getConfiguration() const
{
  ConfigurationPtr config( new ConfigurationImpl("ConfusionMatrix") );

  config->addNameValue( "Threshold", getThreshold() );
  config->addNameValue( "Accuracy", getAccuracy() * 100 );
  config->addNameValue( "OmissionError", getOmissionError() * 100 );
  config->addNameValue( "CommissionError", getCommissionError() * 100 );
  config->addNameValue( "TruePositives", getValue( 1, 1 ) );
  config->addNameValue( "FalsePositives", getValue( 0, 1 ) );
  config->addNameValue( "TrueNegatives", getValue( 0, 0 ) );
  config->addNameValue( "FalseNegatives", getValue( 1, 0 ) );

  return config;
}
