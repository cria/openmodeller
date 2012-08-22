/**
 * Declaration of class PreChiSquare
 *
 * @author Missae Yamamoto (missae at dpi . inpe . br)
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2008 by INPE -
 * Instituto Nacional de Pesquisas Espaciais
 *
 * http://www.inpe.br
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

#include <openmodeller/pre/PreChiSquare.hh>
#include <openmodeller/Sampler.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Exceptions.hh>

#include <stdio.h>
#include <math.h>

PreChiSquare::PreChiSquare() //constructor
{
}

PreChiSquare::~PreChiSquare() //destructor
{
}

bool 
PreChiSquare::checkParameters( const PreParameters& parameters ) const
{
  SamplerPtr samplerPtr;
  if( ! parameters.retrieve( "Sampler", samplerPtr ) ) 
  {
     Log::instance()->error( "Missing parameter: Sampler. \n" );
     return false;
   }

   if ( ! samplerPtr->getEnvironment() ) 
   {
     std::string msg = "Sampler has no environment.\n";

     Log::instance()->error( msg.c_str() );

     throw InvalidParameterException( msg );
   }
   return true;
}

bool 
PreChiSquare::runImplementation()
{
  size_t layer1, layer2;

  init();

  for( layer1 = 0; layer1 < num_layers; layer1++ ) 
  {
    statistic1.push_back(0);
    statistic2.push_back(0);
  }

  for( layer1 = 0; layer1 < num_layers; layer1++ ) 
    for(layer2 = layer1+1; layer2 < num_layers; layer2++)
    {
      setMeasured(layer1, layer2);
      setExpected();
      setChicell();
      setStatistic(layer1, layer2);
    }

  SamplerPtr samplerPtr;
  params_.retrieve( "Sampler", samplerPtr );

  size_t aux;
  for( unsigned int ind = 0 ; ind < num_layers ; ind++ ) 
  {
    string layer_id = samplerPtr->getEnvironment()->getLayerPath(ind);

    PreParameters result;

    aux = statistic1[ind] + statistic2[ind];

    result.store( "number of correlated layers", (int)aux );

    result_by_layer_[layer_id] = result;
  }

  return true;
}

void
PreChiSquare::getAcceptedParameters( stringMap& info)
{
	info["Sampler"] = "samplerPtr";
}

void
PreChiSquare::getLayersetResultSpec ( stringMap& info)
{
	
}

void
PreChiSquare::getLayerResultSpec ( stringMap& info)
{
	info["number of correlated layers"] = "int";
}

bool 
PreChiSquare::init()
{
  SamplerPtr samplerPtr;
  params_.retrieve( "Sampler", samplerPtr );

  setNlayers(samplerPtr);

  my_presences = samplerPtr->getPresences();

  setNpoints();
  setNclass();
  setMinimum();
  setDelta();

  if (nclass > classLimit)
  {
    Log::instance()->error( "ChiSquare: measured, expected, chicell: number of class > %d\n", classLimit );
    return false;
  }

  return true;
}

size_t
PreChiSquare::getNpoints()
{
  return num_points;
}

void 
PreChiSquare::setNpoints()
{
  num_points = my_presences->numOccurrences();
}

size_t 
PreChiSquare::getNlayers()
{
  return num_layers;
}

void 
PreChiSquare::setNlayers(SamplerPtr samplerPtr)
{
  num_layers = samplerPtr->numIndependent();

  if ( num_layers < 2 ) 
  {
    std::string msg = "chisquare needs at least 2 layers.\n";

    Log::instance()->error( msg.c_str() );

    throw InvalidParameterException( msg );
  }
}

size_t 
PreChiSquare::getNclass()
{
  return nclass;
}

void 
PreChiSquare::setNclass()
{
  nclass = (size_t)floor(sqrt((double)(num_points)));
}

void 
PreChiSquare::setMinimum()
{
  OccurrencesImpl::iterator it = my_presences->begin(); 
  OccurrencesImpl::iterator last = my_presences->end();

  Sample sample = (*it)->environment();
  minimum = sample;
  ++it;
  while ( it != last ) 
  {     
    Sample const& sample = (*it)->environment();
    minimum &= sample;
    ++it;
  }
}

Sample 
PreChiSquare::getMinimum()
{
  return minimum;
}

void 
PreChiSquare::setDelta()
{
  OccurrencesImpl::iterator it = my_presences->begin(); 
  OccurrencesImpl::iterator last = my_presences->end();

  Sample sample = (*it)->environment();
  delta = sample;
  ++it;
  while ( it != last ) 
  {     
    Sample const& sample = (*it)->environment();
    delta |= sample;
    ++it;
  }

  delta -= minimum;
  delta /= (Scalar)nclass;
}

Sample 
PreChiSquare::getDelta()
{
  return delta;
}

void 
PreChiSquare::setMeasured(size_t layer1, size_t layer2)
{
  OccurrencesImpl::iterator it = my_presences->begin(); 
  OccurrencesImpl::iterator last = my_presences->end();

  size_t row, col;

  for (size_t i = 0; i < classLimit; i++)
    for (size_t j = 0; j < classLimit; j++)
      measured[i][j] = 0.0;

  while ( it != last ) 
  {     
    Sample const& sample = (*it)->environment();

    row = (size_t)floor( ( sample[layer1] - minimum[layer1] ) / delta[layer1] );
    if (row == nclass)
      row--;
    col = (size_t)floor( ( sample[layer2] - minimum[layer2] ) / delta[layer2] );
    if (col == nclass)
      col--;
    measured[row][col] += 1.0;

    ++it;
  }
}
void 
PreChiSquare::setExpected()
{
  size_t i, j;
  Scalar col_sum[classLimit], row_sum[classLimit], sum;

  for (i = 0; i < nclass; i++)
  {
    col_sum[i] = measured[i][0];
    for ( j = 1; j < nclass; j++)
      col_sum[i] += measured[i][j];
  }

  for (j = 0; j < nclass; j++)
  {
    row_sum[j] = measured[0][j];
    for ( i = 1; i < nclass; i++)
      row_sum[j] += measured[i][j];
  }

  sum = col_sum[0];
  for (i = 1; i < nclass; i++)
    sum += col_sum[i];

  for (i = 0; i < nclass; i++)
    for (j = 0; j < nclass; j++)
      expected[i][j] = col_sum[i] * row_sum[j] / sum;
}
void 
PreChiSquare::setChicell()
{
  size_t i, j;
  Scalar aux;

  for (i = 0; i < nclass; i++)
    for (j = 0; j < nclass; j++)
      if (expected[i][j] == 0.0)
        chicell[i][j] = 0.0;
      else
      {
        aux = expected[i][j] - measured[i][j];
        chicell[i][j] = (aux * aux) / expected[i][j]; 
      }
}

void 
PreChiSquare::setStatistic(size_t layer1, size_t layer2)
{
  size_t i, j;
  Scalar chi=0.0;
  Scalar delimita[classLimit] = {0, 3.8415, 5.9915, 7.8147, 9.4877, 11.0705, 12.5916, 14.0671, 15.5073, 16.9190, 18.3070, 19.6751, 21.0261, 22.3620, 23.6848, 24.9958 };

  for (i = 0; i < nclass; i++)
    for (j = 0; j < nclass; j++)
      chi += chicell[i][j];

  if (chi < delimita[nclass - 1])
  {
    statistic1[layer1] += 1;
    statistic2[layer2] += 1;
  }
}
