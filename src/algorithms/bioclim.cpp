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
/************************* Bioclim Model ***********************/

/*******************/
/*** constructor ***/

BioclimModel::BioclimModel( Sampler *samp, Scalar cutoff )
  : Algorithm( samp )
{
  f_done   = 0;
  f_cutoff = cutoff;
  f_dim    = samp->dim() - 1;
  f_max    = new Scalar[f_dim];
  f_min    = new Scalar[f_dim];
  f_avg    = new Scalar[f_dim];
  f_range  = new Scalar[f_dim];
}


/******************/
/*** destructor ***/

BioclimModel::~BioclimModel()
{
  delete f_min;
  delete f_max;
}


/***************/
/*** iterate ***/
int
BioclimModel::iterate()
{
  Sampler *samp = getSampler();

  // Generate model from the average of given points.
  int npnt = samp->numOccurrences();

  _log.info( "Reading %d-dimensional occurrence points.\n", f_dim );

  Samples pnt( npnt, f_dim );
  npnt = samp->getOccurrences( npnt, &pnt );

  if ( ! npnt )
    {
      _log.error( 1, "All occurrences are outside the mask!\n" );
    }

  _log.info( "Finding the bioclimatic envelope using %d points.\n", npnt );

  // sort values of each independent variable and apply cutoffs
  memset( f_min,   0, f_dim * sizeof(Scalar) );
  memset( f_max,   0, f_dim * sizeof(Scalar) );
  memset( f_avg,   0, f_dim * sizeof(Scalar) );
  memset( f_range, 0, f_dim * sizeof(Scalar) );

  // temp array that will store the values of the current env variable
  // this will be used by the sort algorithm
  Scalar * values = new Scalar[npnt];

  // get number of points on the borders of distribution to be removed
  if (f_cutoff > 1.0)
    f_cutoff = 1.0;
  else if (f_cutoff < 0.0)
    f_cutoff = 0.0;

  int ncutoff = (int) ceil(npnt * (1 - f_cutoff) / 2.0);

  // visit each variable
  int d, i, j, k;
  for ( d = 0; d < f_dim; d++ )
    {
      _log.info( "Bioclimatic Envelope on dimension %d.\n", d );

      // get all values for this variable and store in second array
      memset( values, 0, npnt * sizeof(Scalar) );
      for ( i = 0; i < npnt; i++)
	values[i] = pnt( i, d + 1 );

      // sort the values of this env variable
      // using bubble sort for simplicity
      _log.debug( "Sorting vector.\n" );
      for ( j = 0; j < npnt; j++)
	{
	  for ( k = 0; k < npnt - 1 - j; k++)
	    {
	      /* bubble sort DEBUG
	      _log.debug ("j = %d; k = %d\n", j, k);
	      
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
		  
		  _log.debug ("  %c %12.4f\n", c, values[ii]);
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
      f_min[d] = values[ncutoff];
      f_max[d] = values[npnt - ncutoff - 1];

      f_range[d] = f_max[d] - f_min[d];
      f_avg[d] = (f_max[d] - f_min[d]) / 2.0;
    }

  _log.debug( "Finished model\n" );

  delete values;

  f_done = 1;
  return 1;
}


/************/
/*** done ***/
int
BioclimModel::done()
{
  return f_done;
}


/*****************/
/*** get Value ***/
Scalar
BioclimModel::getValue( Scalar *x )
{
  Scalar result;
  int d;

  // check whether x is inside all variable intervals
  result = 1.0;
  for (d = 0; d < f_dim; d++)
    if ((x[d] < f_min[d]) || (x[d] > f_max[d]))
      {
	result = 0.0;
	break;
      }
    else
      {
	result *= Abs(x[d] - f_avg[d]) / f_range[d];
      }

  return result;
}


/***********************/
/*** get Convergence ***/
int
BioclimModel::getConvergence( Scalar *val )
{
  *val = 1.0;
  return 1;
}

