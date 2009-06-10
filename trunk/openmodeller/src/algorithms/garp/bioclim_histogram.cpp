/**
 * Implementation of Garp Custom Sampler class
 * 
 * @file   garp_sampler.cpp
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-04-01
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 * Copyright(c), David R.B. Stockwell of Symbiotik Pty. Ltd.
 * Copyright(c), CRIA - Centro de Referencia em Informacao Ambiental
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
 * 
 * This is an implementation of the GARP algorithm first developed 
 * by David Stockwell
 * 
 */


#include "bioclim_histogram.hh"

#include <openmodeller/Random.hh>
#include <string.h>

/****************************************************************/
/************************* Bioclim Histogram ********************/
BioclimHistogram::BioclimHistogram() :
  _upperLevels( NULL ),
  _lowerLevels( NULL )
{
  reset();
}

void BioclimHistogram::reset()
  { 
    memset(_matrix, 0, 2 * MAX_ENV_LAYERS * 256 * sizeof(int)); 
    memset(_depend, 0, 2 * sizeof(int));
  }

BioclimHistogram::~BioclimHistogram()
{
  if (_upperLevels)
    delete _upperLevels;
  if (_lowerLevels)
    delete _lowerLevels;
}

void BioclimHistogram::initialize(const OccurrencesPtr& occs)
{
  reset();
  _resamples = occs->numOccurrences();
  
  // number of layers/genes/independent variables
  
  int numLayers = (*occs)[0]->environment().size();
  
  // iterate through the samples
  OccurrencesImpl::const_iterator it = occs->begin();
  OccurrencesImpl::const_iterator end = occs->end();

  int sampleIndex = 0;
  while (it != end)
    {
      Scalar pointValue = ( (*it)->abundance() > 0.0 ) ? 1.0 : 0.0;
      Sample sample = (*it)->environment();

      int predictionIndex = static_cast<int>(pointValue);

      //printf("%d %f=%d (%f,%f) \n", sampleIndex, pointValue, predictionIndex, (*it)->x(), (*it)->y() );

      _depend[predictionIndex]++;

      // iterate through the layers
      //printf("layers: %d %d \n", numLayers, sample.size() );
      for ( int layerIndex = 0; layerIndex < numLayers; layerIndex++)
	    {
	      int normValue = (int) ((sample[layerIndex] + 1.0) / 2.0 * 253.0) + 1;
	      if (normValue < 0) {
		normValue = 0;
	      }
	      else if (normValue > 255 ) {
		normValue = 255;
	      }
	      //printf("Value[%d]: %f (%d)\n", layerIndex, sample[layerIndex], normValue);
	      _matrix[predictionIndex][layerIndex][normValue]++;
	    }
      
      ++it;
      sampleIndex++;
    }

//     printf("Dependent counts: %d %d\n", _depend[0], _depend[1]);

//     _depend[0] = _depend[0] % 256;
//     _depend[1] = _depend[1] % 256;
  
//     printf("Dependent counts: %d %d\n", _depend[0], _depend[1]);
//     for (int i = 0; i < 2; i++)
//       {
// 	for (int k = 0; k < 256; k++)
// 	  {
// 	    printf("Pred=%1d Value=%3d: ", i, k);
// 	    for (int j = 0; j < numLayers; j++)
// 	      { printf("%5d ", _matrix[i][j][k]); }
// 	    printf("\n");
// 	  }
//       }
}

int BioclimHistogram::resamples() const
{
  return _resamples;
}

// ============
void BioclimHistogram::getBioclimRange(Scalar prediction, int layerIndex, 
					Scalar& minCutLevel, Scalar& maxCutLevel) const
{
  Random rnd;
  int sum, n, UL, LL;

  int predIndex = (prediction == 1.0);
  double level = rnd.get(0.1);
  int totalExcluded = (int) (_depend[predIndex] * level);
  
  LL = 0;
  UL = 255;

  sum = 0;
  for (n = 0; n <= 255; n++)
    {
      sum += _matrix[predIndex][layerIndex][n];
      if (sum > totalExcluded) 
	{
	  LL = n;
	  break;
	}
    }
  
  sum = 0;    
  for (n = 255; n >= 0; n--)
    {
      sum += _matrix[predIndex][layerIndex][n];
      if (sum > totalExcluded) 
	{
	  UL = n;
	  break;
	}	
    }
  
  minCutLevel = ( LL / 255.0 * 2 ) - 1.0;
  maxCutLevel = ( UL / 255.0 * 2 ) - 1.0;

  
  //printf("Layer:%3d Excl: %5d Level:%7.4f Cut=(%+7.4f, %+7.4f)\n", 
  // layerIndex, totalExcluded, level, *minCutLevel, *maxCutLevel);
}
