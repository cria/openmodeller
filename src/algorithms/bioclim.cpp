/**
 * Declaration of Bioclimatic Envelope Algorithm - Nix, 1986.
 * 
 * @file
 * @author Mauro Muñoz <mauro@cria.org.br>
 * @date 2004-05-05
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2004 by CRIA -
 * Centro de Referência em Informação Ambiental
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

#include "bioclim.hh"

#include <string.h>
#include <stdio.h>
#include <math.h>

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 1

#define CUTOFF_ID "StandadDeviationCutoff"


/*************************************/
/*** Algorithm parameters metadata ***/

static AlgParamMetadata parameters[NUM_PARAM] = {

  // Metadata of the first parameter.
  {
    CUTOFF_ID,                 // Id
    "Standard deviation cutoff", // Name.
    "Real",                      // Type.

    // Overview
    "The envelop is given by this parameter multiplied by the\
 standard deviation.",

    // Description.
    "Standard deviation cutoff for all bioclimatic envelop.\n\
 Examples of (fraction of inclusion, parameter value) are:\n\
 (50.0%, 0.674); (68.3%, 1.000); (90.0%, 1.645); (95.0%, 1.960);\
 (99.7%, 3.000)",

    1,         // Not zero if the parameter has lower limit.
    0.0,       // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    0.0,       // Parameter's upper limit.
    "0.674"    // Parameter's typical (default) value.
  },
};


/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "Bioclim",   // Id.
  "Bioclim",   // Name.
  "0.1",       // Version.

  // Overview
  "Uses the mean and standard deviation of each variable separately\
 to calculate the envelop.",

  // Description.
  "Implements the Bioclimatic Envelope Algorithm.\
 For each given environmental variable the algorithm finds the mean\
 and standard deviation (assuming normal distribution). Each\
 variable has its own envelop represented by the interval\
 [m - c*s, m + c*s], where 'm' is the mean; 'c' is the cutoff input\
 parameter; and 's' is the standard deviation.\n\
The output is one of the following:\n\
 Suitable: values that fall within the envelops of all variables;\n\
 Marginal: values fall outside some envelop, but within the upper\
 and lower limits.\n\
 Unsuitable: values that fall outside the upper and lower limits\
 for some variable.\n\
The upper and lower limits are taken from the maximum and minimum\
 input points values for each variable.\n\
The bioclim categorical output is mapped to the continuous output of\
 openModeller as Suitable, Marginal and Unsuitable for probabilities\
 of 1.0, 0.5 and 0.0 respectively.",

  "Nix, H. A.",  // Author

  // Bibliography.
  "Nix, H.A. (1986) A biogeographic analysis of Australian elapid\
 snakes. In: Atlas of Elapid Snakes of Australia. (Ed.) R. Longmore,\
 pp. 4-15. Australian Flora and Fauna Series Number 7. Australian\
 Government Publishing Service: Canberra.",

  "Mauro Munoz",            // Code author.
  "mauro [at] cria.org.br", // Code author's contact.

  0,  // Does not accept categorical data.
  0,  // Does not need (pseudo)absence points.

  NUM_PARAM,   // Algorithm's parameters.
  parameters
};



/****************************************************************/
/****************** Algorithm's factory function ****************/

Algorithm * dllexp
algorithmFactory()
{
  return new Bioclim;
}



/****************************************************************/
/**************************** Bioclim ***************************/

/*******************/
/*** constructor ***/

Bioclim::Bioclim()
  : Algorithm( &metadata )
{
  _mean = _deviation = 0;
  _minimum = _maximum = 0;
}


/******************/
/*** destructor ***/

Bioclim::~Bioclim()
{
  if ( _maximum )   delete _maximum;
  if ( _minimum )   delete _minimum;
  if ( _mean )      delete _mean;
  if ( _deviation ) delete _deviation;
}


/******************/
/*** initialize ***/
int
Bioclim::initialize()
{
  // Read and check the standard deviation cutoff parameter.
  Scalar cutoff;
  if ( ! getParameter( CUTOFF_ID, &cutoff ) )
    return 0;
  if ( cutoff <= 0 )
    {
      g_log.warn( "Bioclim - parameter out of range: %f\n",
                  cutoff );
      return 0;
    }

  // Number of independent variables.
  _dim = _samp->numIndependent();
  g_log.info( "Reading %d-dimensional occurrence points.\n",
              _dim );

  // Get all presence points.
  SampledData presence;
  _samp->getPresence( &presence );

  // Check the number of sampled points.
  int npnt = presence.numSamples();
  if ( npnt < 2 )
    g_log.error( 1, "Bioclim needs at least 2 point inside the mask!\n" );
  g_log( "Using %d points to find the bioclimatic envelop.\n", npnt );

  // Gets the minimum, maximum, mean and standard deviations for
  // each variable.
  _minimum = getMinimum( &presence );
  _maximum = getMaximum( &presence );
  _mean    = getMean( &presence );
  _deviation = getStandardDeviation( &presence, _mean );

  // Stores the real standard deviation cutoff value and
  // calculates the standard deviation vector module to be used
  // as the maximum possible distance of a accepted point to the
  // points' mean.
  Scalar module = 0.0;
  Scalar *deviation = _deviation;
  for ( Scalar *end = deviation + _dim; deviation < end; deviation++ )
    {
      *deviation *= cutoff;
      module += *deviation * *deviation;
    }
  _max_distance = sqrt(module);

  return 1;
}


/***************/
/*** iterate ***/
int
Bioclim::iterate()
{
  // This is not an iterative algorithm.
  return 1;
}


/************/
/*** done ***/
int
Bioclim::done()
{
  // This is not an iterative algorithm.
  return 1;
}


/*****************/
/*** get Value ***/
Scalar
Bioclim::getValue( Scalar *x )
{
  Scalar dif;

  // Zero if some point valuble is outside its respective envelop.
  Scalar outside_envelop = 0;

  // Square of the distance between 'x' and '_mean'.
  Scalar square_distance = 0.0;

  // Finds the distance the each variable mean to the respective
  // point value.
  Scalar *minimum   = _minimum;
  Scalar *maximum   = _maximum;
  Scalar *mean      = _mean;
  Scalar *mean_end  = mean + _dim;
  Scalar *deviation = _deviation;
  while ( mean < mean_end )
    {
      // Point value for each variable: x[i].
      Scalar xi = *x++;

      // If some x[i] is out of the upper and lower range, predicts
      // no occurrence.
      if ( xi < *minimum++ || xi > *maximum++ )
        return 0.0;

      // If some x[i] is outside its envelop, signals.
      Scalar cutoff = *deviation++;
      dif = xi - *mean++;
      if ( dif > cutoff || dif < -cutoff )
        outside_envelop = 1;
    }

  // If all point values are within the envelop, returns probability
  // 1.0. Else, if some point is outside the envelop but inside
  // the upper and lower ranges, returns 0.5 of probability.
  return outside_envelop ? 0.5 : 1.0;
}


/***********************/
/*** get Convergence ***/
int
Bioclim::getConvergence( Scalar *val )
{
  *val = 1.0;
  return 1;
}


/*******************/
/*** get Minimum ***/
Scalar *
Bioclim::getMinimum( SampledData *points )
{
  int npnt = points->numSamples();
  int dim  = points->numIndependent();

  if ( ! npnt )
    return 0;

  // Allocates the minimum vector.
  Scalar *minimum = new Scalar[dim];
  Scalar *minimum_end = minimum + dim;

  // To pass through all points.
  Scalar **pnt = points->getIndependentBase();
  Scalar **pnt_end = pnt + npnt;

  // Initializes the minimum vector with the first point.
  Scalar *m = minimum;
  Scalar *p = *pnt++;
  while ( m < minimum_end )
    *m++ = *p++;

  // For each point, finds the minimum values.
  while ( pnt < pnt_end )
    {
      // Finds the minimum value.
      p = *pnt++;
      for ( m = minimum; m < minimum_end; m++, p++ )
        if ( *p < *m )
          *m = *p;
    }

  return minimum;
}


/*******************/
/*** get Maximum ***/
Scalar *
Bioclim::getMaximum( SampledData *points )
{
  int npnt = points->numSamples();
  int dim  = points->numIndependent();

  if ( ! npnt )
    return 0;

  // Allocates the maximum vector.
  Scalar *maximum = new Scalar[dim];
  Scalar *maximum_end = maximum + dim;

  // To pass through all points.
  Scalar **pnt = points->getIndependentBase();
  Scalar **pnt_end = pnt + npnt;

  // Initializes the maximum vector with the first point.
  Scalar *m = maximum;
  Scalar *p = *pnt++;
  while ( m < maximum_end )
    *m++ = *p++;

  // For each point, finds the maximum values.
  while ( pnt < pnt_end )
    {
      // Finds the maximum value.
      p = *pnt++;
      for ( m = maximum; m < maximum_end; m++, p++ )
        if ( *p > *m )
          *m = *p;
    }

  return maximum;
}


/****************/
/*** get Mean ***/
Scalar *
Bioclim::getMean( SampledData *points )
{
  int npnt = points->numSamples();
  int dim  = points->numIndependent();

  // Initialize the mean to zero.
  Scalar *mean = new Scalar[dim];
  memset( mean, 0, dim * sizeof(Scalar) );

  // Last position of the mean point.
  Scalar *mean_end = mean + dim;

  // For each point...
  Scalar **pnt = points->getIndependentBase();
  Scalar **pnt_end = pnt + npnt;
  while ( pnt < pnt_end )
    {
      // Calculates the sum of all points in 'mean'.
      Scalar *m = mean;
      Scalar *p = *pnt++;
      while ( m < mean_end )
        *m++ += *p++;
    }

  // Divides each mean component by the number of points summed.
  Scalar *m = mean;
  while ( m < mean_end )
    *m++ /= npnt;

  return mean;
}


/******************************/
/*** get Standard Deviation ***/
Scalar *
Bioclim::getStandardDeviation( SampledData *points,
                                       Scalar *mean )
{
  int npnt = points->numSamples();
  int dim  = points->numIndependent();

  // Variance vector initialized with zeros.
  Scalar *variance = new Scalar[dim];
  memset( variance, 0, dim * sizeof( Scalar ) );
  Scalar *variance_end = variance + dim;

  // For each point...
  Scalar **pnt = points->getIndependentBase();
  Scalar **pnt_end = pnt + npnt;
  while ( pnt < pnt_end )
    {
      // Calculates the variance for each variable (dimension).
      Scalar *v = variance;
      Scalar *m = mean;
      Scalar *p = *pnt++;
      while ( v < variance_end )
        {
          Scalar dif = *p++ - *m++;
          *v++ += dif * dif;
        }
    }

  // In variance, we divide by (npnt - 1), not npnt!
  npnt--;

  // Calculates the standard deviation (square root of variance).
  // Standard deviation vector initialized with zeros.
  Scalar *deviation = new Scalar[dim];
  Scalar *sd  = deviation;
  Scalar *var = variance;
  while ( var < variance_end )
    *sd++ = sqrt( *var++ / npnt );

  delete variance;
  return deviation;
}

/******************/
/*** serialize ***/
int
Bioclim::serialize(Serializer * s)
{
  s->writeStartSection("BioclimModel");
  s->writeDouble("MaxDistance", _max_distance);
  s->writeArrayDouble("Mean", _mean, _dim);
  s->writeArrayDouble("StdDev", _deviation, _dim);
  s->writeArrayDouble("Minumum", _minimum, _dim);
  s->writeArrayDouble("Maximum", _maximum, _dim);
  s->writeEndSection("BioclimModel");
  return 1;
}

/********************/
/*** deserialize ***/
int
Bioclim::deserialize(Deserializer * s)
{
  int size1, size2, size3, size4;

  s->readStartSection("BioclimModel");
  _max_distance = s->readDouble("MaxDistance");
  _mean = s->readArrayDouble("Mean", &size1);
  _deviation = s->readArrayDouble("StdDev", &size2);
  _minimum = s->readArrayDouble("Minumum", &size3);
  _maximum = s->readArrayDouble("Maximum", &size4);
  s->readEndSection("BioclimModel");
  _dim = size4;
  
  return ((size1 == size2) && (size2 == size3) && (size3 == size4));
}

/*******************/
/*** log Envelop ***/
void
Bioclim::logEnvelop()
{
  g_log( "Envelop with %d dimensions (variables).\n\n", _dim );

  for ( int i = 0; i < _dim; i++ )
    {
      g_log( "Variable %02d:", i );
      g_log( " Mean     : %f\n", _mean[i] );
      g_log( " Deviation: %f\n", _deviation[i] );
      g_log( " Minumum  : %f\n", _minimum[i] );
      g_log( " Maximum  : %f\n", _maximum[i] );
      g_log( "\n" );
    }
}
