/**
 * Definition of ConfusionMatrix class 
 * 
 * @file conf_matrix.cpp
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


#include <om_conf_matrix.hh>
#include <om_sampler.hh>
#include <om_sampled_data.hh>
#include <om_algorithm.hh>

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

/* 
 * Confusion Matrix:
 *  1st row is predicted absence  (index [0][x])
 *  2nd row is predicted presence (index [1][x])
 *  1st column is actual absence  (index [y][0])
 *  2nd column is actual presence (index [y][1])
 */

void ConfusionMatrix::calculate(Sampler * sampler, Algorithm * alg)
{
  int i, n;
  int predictionIndex, actualIndex;
  Scalar predictionValue;
  SampledData data;

  reset(_predictionThreshold);
  sampler->getPresence(&data);
  n = data.numSamples();

  for (i = 0; i < n; i++)
    {
      predictionValue = alg->getValue(data.getIndependentSample(i));
      predictionIndex = (predictionValue >= _predictionThreshold);
      actualIndex = data.isPresence(i);

      _confMatrix[predictionIndex][actualIndex]++;
    }

  _ready = true;
}


int ConfusionMatrix::getValue(Scalar predictionValue, 
			 Scalar actualValue)
{
  int predictedIndex, actualIndex;

  predictedIndex = (predictionValue >= _predictionThreshold);
  actualIndex    = (actualValue     >= _predictionThreshold);

  return _confMatrix[predictedIndex][actualIndex];
}


double ConfusionMatrix::getAccuracy()
{
  Scalar total = 
    _confMatrix[0][0] + _confMatrix[0][1] + 
    _confMatrix[1][0] + _confMatrix[1][1];
  
  if (_ready && total)
    return ( _confMatrix[0][0] + _confMatrix[1][1] ) / total;
  else
    return -1.0;
}


double ConfusionMatrix::getCommissionError()
{
  Scalar total = _confMatrix[1][0] + _confMatrix[0][0];
  if (_ready && total)
    return _confMatrix[1][0] / total;
  else
    return -1.0;
}


double ConfusionMatrix::getOmissionError()
{
  Scalar total = _confMatrix[0][1] + _confMatrix[1][1];
  if (_ready && total)
    return _confMatrix[0][1] / total;
  else
    return -1.0;
}

