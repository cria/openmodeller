/**
 * Definition of Bioclimatic Envelope Algorithm - Nix, 1984.
 * 
 * @file
 * @author Ricardo Scachetti Pereira <ricardo@cria.org.br>
 * @date 2003-09-29
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2003 by The University of Kansas Natural History 
 * Museum and Biodiversity Research Center.
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


/*************************************/
/*** Algorithm parameters metadata ***/

static AlgParamMetadata parameters[NUM_PARAM] = {

  // Metadata of the first parameter.
  {
    "CutOff",    // Name.
    "Real",      // Type.
    "Envelop cutoff", // Description.

    1,         // Not zero if the parameter has lower limit.
    0.0,       // Parameter's lower limit.
    1,         // Not zero if the parameter has upper limit.
    1.0,       // Parameter's upper limit.
    "0.8"      // Parameter's typical (default) value.
  },
};


/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  0,               // Internal usage.
  "Bioclim",       // Name.
  "0.1",       	   // Version.
  "Nix 1986, Busby 1991, McMahon et al. 1996",  // Bibliography.

  // Description.
  "Bioclimatic Envelope Algorithm",

  "Ricardo Scachetti Pereira",  // Author.
  "ricardo [at] cria.org.br",   // Author's contact.

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
  return new Bioclim;
}



/****************************************************************/
/***************************** Bioclim **************************/

/*******************/
/*** constructor ***/

Bioclim::Bioclim()
  : Algorithm( &metadata )
{
  _min = _max = _avg = _half_range = 0;
  _done = 0;
}


/******************/
/*** destructor ***/

Bioclim::~Bioclim()
{
  if ( _min ) delete _min;
  if ( _max ) delete _max;
  if ( _avg ) delete _avg;
  if ( _half_range ) delete _half_range;
}


/******************/
/*** initialize ***/
int
Bioclim::initialize( int ncicle )
{
  if ( ! getParameter( "CutOff", &_cutoff ) )
    return 0;

  _dim    = _samp->numIndependent();
  _max    = new Scalar[_dim];
  _min    = new Scalar[_dim];
  _avg    = new Scalar[_dim];
  _half_range = new Scalar[_dim];
}


/***************/
/*** iterate ***/
int
Bioclim::iterate()
{
  g_log.info( "Reading %d-dimensional occurrence points.\n", _dim );

  SampledData presence;
  _samp->getPresence( &presence );  // Get all presence points.
  Scalar **pnt = presence.getIndependentBase();
  int    npnt = presence.numSamples();

  if ( ! npnt )
    g_log.error( 1, "All occurrences are outside the mask!\n" );

  g_log.info( "Finding the bioclimatic envelope using %d points.\n",
	     npnt );

  // sort values of each independent variable and apply cutoffs
  memset( _min, 0, _dim * sizeof(Scalar) );
  memset( _max, 0, _dim * sizeof(Scalar) );
  memset( _avg, 0, _dim * sizeof(Scalar) );
  memset( _half_range, 0, _dim * sizeof(Scalar) );

  // temp array that will store the values of the current env variable
  // this will be used by the sort algorithm
  Scalar *values = new Scalar[npnt];

  // get number of points on the borders of distribution to be removed
  if (_cutoff > 1.0)
    _cutoff = 1.0;
  else if (_cutoff < 0.0)
    _cutoff = 0.0;

  int ncutoff = (int) ceil(npnt * (1 - _cutoff) / 2.0);

  // visit each variable
  int d, i, j, k;
  for ( d = 0; d < _dim; d++ )
    {
      g_log.info( "Bioclimatic Envelope on dimension %d.\n", d );

      // get all values for this variable and store in second array
      memset( values, 0, npnt * sizeof(Scalar) );
      for ( i = 0; i < npnt; i++)
	values[i] = pnt[i][d];

      // sort the values of this env variable
      // using bubble sort for simplicity
      g_log.debug( "Sorting vector.\n" );
      for ( j = 0; j < npnt; j++)
	{
	  for ( k = 0; k < npnt - 1 - j; k++)
	    {
	      /* bubble sort DEBUG
	      g_log.debug ("j = %d; k = %d\n", j, k);
	      
	      for (int ii = 0; ii < npnt; ii++)
		{
		  char c;
		  if (ii == j)
		    {
		      if (ii == k)
			c = '*';
		      else
			c = 'j';
		    }
		  else
		    {
		      if (ii == k)
			c = 'k';
		      else
			c = ' ';
		    }
		  
		  g_log.debug ("  %c %12.4f\n", c, values[ii]);
		}
	      */

	      if ( values[k] > values[k + 1] )
		{
		  // exchange values[k] with values[k + 1]
		  Scalar aux = values[k];
		  values[k] = values[k + 1];
		  values[k + 1] = aux;
		}
	    }
	}

      // get cutoff values by picking the ncutoff-th and 
      // (npnt-ncutoff)-th elements
      _min[d] = values[ncutoff];
      _max[d] = values[npnt - ncutoff - 1];

      _half_range[d] = (_max[d] - _min[d]) / 2.0;
      _avg[d] = (_max[d] + _min[d]) / 2.0;

      g_log.debug( "%d] Min: %f, Max: %f, Avg: %f, Range: %f\n",
		  d, _min[d], _max[d], _avg[d], _half_range[d] );
    }

  g_log.debug( "Finished model\n" );

  delete values;

  _done = 1;
  return 1;
}


/************/
/*** done ***/
int
Bioclim::done()
{
  return _done;
}


/*****************/
/*** get Value ***/
Scalar
Bioclim::getValue( Scalar *x )
{
  Scalar dif;
  Scalar result[_dim];

  // check whether x is inside all variable intervals
  for ( int d = 0; d < _dim; d++ )

    if ( (x[d] < _min[d]) || (x[d] > _max[d]) )
      return 0.0;

    else
      {
	dif = x[d] - _avg[d];
	result[d] = Abs(dif) / _half_range[d];
      }

  // Inverse of the distance normalized in [0,1].
  return 1.0 - cartesianDistance( result, _avg ) / sqrt( _dim );
}


/***********************/
/*** get Convergence ***/
int
Bioclim::getConvergence( Scalar *val )
{
  *val = 1.0;
  return 1;
}


/**************************/
/*** cartesian Distance ***/
Scalar
Bioclim::cartesianDistance( Scalar *x, Scalar *y )
{
  Scalar dif;
  Scalar dist = 0.0;

  Scalar *end = x + _dim;
  while ( x < end )
    {
      dif = *x++ - *y++;
      dist += dif * dif;
    }

  return sqrt( dist );
}

