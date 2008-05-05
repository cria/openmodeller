/**
 * Definition of ChiSquare class 
 * 
 * @author Missae (missae at dpi . inpe . br)
 * $Id$
 *
 * LICENSE INFORMATION
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

#include <openmodeller/pre/ChiSquare.hh>
#include <openmodeller/Sampler.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Exceptions.hh>

#include <stdio.h>
#include <math.h>

ChiSquare::ChiSquare() //constructor
{
}

ChiSquare::~ChiSquare() //destructor
{
  if (statistic1 != NULL)
  {
    delete [] statistic1; 
    statistic1=NULL;
  }
  if (statistic2 != NULL)
  {
    delete [] statistic2; 
    statistic2=NULL;
  }
}

bool 
ChiSquare::checkParameters( const PreParameters& parameters ) const
{
  SamplerPtr samplerPtr;
  if( ! parameters.retrive( "Sampler", samplerPtr ) ) 
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
ChiSquare::runImplementation()
{
  size_t layer1, layer2;

  init();

  for( layer1 = 0; layer1 < num_layers; layer1++ ) 
  {
    statistic1[layer1] = 0;
    statistic2[layer1] = 0;
  }

  for( layer1 = 0; layer1 < num_layers; layer1++ ) 
    for(layer2 = layer1+1; layer2 < num_layers; layer2++)
    {
      setMeasured(layer1, layer2);
      setExpected();
      setChicell();
      setStatistic(layer1, layer2);
    }
  return true;
}

void
ChiSquare::resetState( PreParameters& params )
{

}

void 
ChiSquare::init()
{
  SamplerPtr samplerPtr;
  params_.retrive( "Sampler", samplerPtr );

  setNlayers(samplerPtr);

  my_presences = samplerPtr->getPresences();

  setNpoints();
  setNclass();
  setMinimum();
  setDelta();

  if (nclass > classLimit)
  {
    Log::instance()->error( "ChiSquare: measured, expected, chicell: number of class > %d\n", classLimit );
    exit(0);	
  }

  statistic1 = new size_t[num_points];
  if( statistic1 == NULL ) 
  {
    Log::instance()->error( "ChiSquare: statistic1: memory allocation problem\n" );
    exit(0);	
  }

  statistic2 = new size_t[num_points];
  if( statistic2 == NULL ) 
  {
    Log::instance()->error( "ChiSquare: statistic2: memory allocation problem\n" );
    exit(0);	
  }
}

size_t
ChiSquare::getNpoints()
{
  return num_points;
}

void 
ChiSquare::setNpoints()
{
  num_points = my_presences->numOccurrences();
}

size_t 
ChiSquare::getNlayers()
{
  return num_layers;
}

void 
ChiSquare::setNlayers(SamplerPtr samplerPtr)
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
ChiSquare::getNclass()
{
  return nclass;
}

void 
ChiSquare::setNclass()
{
  nclass = (size_t)floor(sqrt((double)(num_points)));
}

void 
ChiSquare::setMinimum()
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
ChiSquare::getMinimum()
{
  return minimum;
}

void 
ChiSquare::setDelta()
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
ChiSquare::getDelta()
{
  return delta;
}

void 
ChiSquare::setMeasured(size_t layer1, size_t layer2)
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
ChiSquare::setExpected()
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
ChiSquare::setChicell()
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
ChiSquare::setStatistic(size_t layer1, size_t layer2)
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

void 
ChiSquare::showResult()
{
  size_t i;

  Log::instance()->debug( "\n*****************************************\nchisquare results:  " );
  for (i = 0; i < num_layers; i++)
  {
    statistic1[i] += statistic2[i];
    Log::instance()->debug( "%d ", statistic1[i] );
  }
}

