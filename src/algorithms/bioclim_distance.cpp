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

#include "bioclim_distance.hh"

#include <string.h>
#include <stdio.h>
#include <math.h>


/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 1

#define CUTOFF_NAME "StandadDeviationCutoff"


/*************************************/
/*** Algorithm parameters metadata ***/

static AlgParamMetadata parameters[NUM_PARAM] = {

  // Metadata of the first parameter.
  {
    CUTOFF_NAME,                 // Id
    "Standard deviation cutoff", // Name.
    "Real",                      // Type.

    // Overview
    "The envelop is given by this parameter multiplied by the\
 standard deviation.",

    "Standard deviation cutoff for all bioclimatic envelop.\
 Examples of (fraction of inclusion, parameter value) are:\
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

  "BioclimDistance",     // Id.
  "Bioclim (distance)",  // Name.
  "0.1",                 // Version.

  // Overview
  "Uses the mean and standard deviation of each variable separately\
 to calculate the envelop. When with the envelop uses the distance\
 to the mean.",

  // Description.
  "Implements the Bioclimatic Envelope Algorithm using a \
normalized distance to the points' mean to compute the occurrence \
probability.\n\
For each given variable the algorithm finds the mean and standard \
deviation (assuming normal distribution). Each variable has its own \
envelop represented by the interval [m - c*s, m + c*s], \
where 'm' is the mean; 'c' is cutoff input parameter; and 's' is the \
standard deviation.\n",

  "Nix, H. A. Modified by Mauro Muñoz",  // Author.
  "",                                    // Bibliography.

  "Mauro Muñoz",              // Code author.
  "mauro [at] cria.org.br",   // Author's contact.

  0,  // Does not accept categorical data.
  0,  // Does not need (pseudo)absence points.

  NUM_PARAM,   // Algorithm's parameters.
  parameters
};



/****************************************************************/
/****************** Algorithm's factory function ****************/

Algorithm *
algorithmFactory()
{
  return new BioclimDistance;
}



/****************************************************************/
/*********************** Bioclim Distance ***********************/

/*******************/
/*** constructor ***/

BioclimDistance::BioclimDistance()
  : Algorithm( &metadata )
{
  _mean = _std_dev = 0;
}


/******************/
/*** destructor ***/

BioclimDistance::~BioclimDistance()
{
  if ( _mean )    delete _mean;
  if ( _std_dev ) delete _std_dev;
}


/******************/
/*** initialize ***/
int
BioclimDistance::initialize()
{
  // Read and check the standard deviation cutoff parameter.
  Scalar cutoff;
  if ( ! getParameter( CUTOFF_NAME, &cutoff ) )
    return 0;
  if ( cutoff <= 0 )
    {
      g_log.warn( "BioclimDistance - parameter out of range: %f\n",
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
    g_log.error( 1, "BioclimDistance needs at least 2 point inside the mask!\n" );
  g_log.info( "Using %d points to find the bioclimatic envelop.\n",
              npnt );

  // Gets the mean and standard deviations for each variable
  _mean    = getMean( &presence );
  _std_dev = getStandardDeviation( &presence, _mean );

  g_log( "BioclimDistanceatic envelop width used:\n" );
  int i = 0;

  // Stores the real standard deviation cutoff value and
  // calculates the standard deviation vector module to be used
  // as the maximum possible distance of a accepted point to the
  // points' mean.
  Scalar module = 0.0;
  Scalar *std_dev = _std_dev;
  for ( Scalar *end = std_dev + _dim; std_dev < end; std_dev++ )
    {
      *std_dev *= cutoff;
      module += *std_dev * *std_dev;

      g_log( " %02d] %f\n", i++, *std_dev );
    }
  _max_distance = sqrt(module);

  return 1;
}


/***************/
/*** iterate ***/
int
BioclimDistance::iterate()
{
  // This is not an iterative algorithm.
  return 1;
}


/************/
/*** done ***/
int
BioclimDistance::done()
{
  // This is not an iterative algorithm.
  return 1;
}


/*****************/
/*** get Value ***/
Scalar
BioclimDistance::getValue( Scalar *x )
{
  Scalar dif;

  // Square of the distance between 'x' and '_mean'.
  Scalar square_distance = 0.0;

  // Finds the distance from each variable mean to the respective
  // point value.
  Scalar *mean     = _mean;
  Scalar *mean_end = mean + _dim;
  Scalar *std_dev  = _std_dev;
  while ( mean < mean_end )
    {
      Scalar cutoff = *std_dev++;
      dif = *x++ - *mean++;

      // If some x[i] is out of its bioclimatic envelop, predicts
      // no occurrence.
      if ( dif > cutoff || dif < -cutoff )
        return 0.0;

      square_distance += dif * dif;
    }

  return 1.0 - (sqrt( square_distance ) / _max_distance);
}


/***********************/
/*** get Convergence ***/
int
BioclimDistance::getConvergence( Scalar *val )
{
  *val = 1.0;
  return 1;
}


/****************/
/*** get Mean ***/
Scalar *
BioclimDistance::getMean( SampledData *points )
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
BioclimDistance::getStandardDeviation( SampledData *points,
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
  Scalar *std_dev = new Scalar[dim];
  Scalar *sd  = std_dev;
  Scalar *var = variance;
  while ( var < variance_end )
    *sd++ = sqrt( *var++ / npnt );

  delete variance;
  return std_dev;
}

