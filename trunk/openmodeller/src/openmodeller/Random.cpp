/**
 * Definition of Random class.
 * 
 * @file
 * @author Mauro E S Munoz
 * @date   1997-06-18
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2003 by CRIA -
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

// Obs.:   The function 'drand48()' returns a number (double) between 0 and 1
//         with uniform distribution.


#include "random.hh"
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <time.h>

void _srand(long seed)	{ srand(seed); };
double _drand(void)		{ return ((double) rand() / (double) RAND_MAX); };

#else
#include <sys/time.h>
#include <sys/resource.h>

void _srand(long seed)	{ srand48(seed); };
double _drand(void)		{ return drand48(); };

#endif


/********************************************************/
/************************ Random ************************/

int Random::not_initialized = 1;


/*******************/
/*** Constructor ***/

Random::Random()
{
  if ( not_initialized )
    {
	  long seed;

#ifndef WIN32
      struct timeval time;
      if ( gettimeofday( &time, (struct timezone *)NULL ) != 0 )
		{
			fprintf( stderr, "Random::Random <error>.\n" );
			exit( 0 );
		}
	  seed = time.tv_usec;

#else
	  // WIN32 version
	  seed = (unsigned)time( NULL );
#endif
      
      _srand( seed );
      not_initialized = 0;
    }
}


/*********************/
/*** Get  (double) ***/
double
Random::Get( double min, double max )
{
  return( (max-min) * _drand() + min );
}

double
Random::Get( double max )
{
  return( max * _drand() );
}

double
Random::Get()
{
  return( _drand() );
}

/****************************/
/*** operator()  (double) ***/
double
Random::operator()( double min, double max )
{
  return( (max-min) * _drand() + min );
}

double
Random::operator()( double max )
{
  return( max * _drand() );
}

double
Random::operator()()
{
  return( _drand() );
}


/******************/
/*** Get  (int) ***/
int
Random::Get( int min, int max )
{
  return( int((max-min) * _drand()) + min );
}

int
Random::Get( int max )
{
  return( int(max * _drand()) );
}


/*************************/
/*** operator()  (int) ***/
int
Random::operator()( int min, int max )
{
  return( int((max-min) * _drand()) + min );
}

int
Random::operator()( int max )
{
  return( int(max * _drand()) );
}


/*******************/
/*** Get  (long) ***/
long
Random::Get( long min, long max )
{
  return( long((max-min) * _drand()) + min );
}

long
Random::Get( long max )
{
  return( long(max * _drand()) );
}


/**************************/
/*** operator()  (long) ***/
long
Random::operator()( long min, long max )
{
  return( long((max-min) * _drand()) + min );
}

long
Random::operator()( long max )
{
  return( long(max * _drand()) );
}


/****************/
/*** Discrete ***/
double
Random::Discrete( float range, float dim_interv )
{
  double d = 2.0 * range / dim_interv;
  int k = int( (d + 1) * _drand() );
  return( k * dim_interv - range );
}


