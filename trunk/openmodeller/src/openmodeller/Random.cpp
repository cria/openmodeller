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

#include "random.hh"
#include "os_specific.hh"

#include <stdlib.h>


/********************************************************/
/************************ Random ************************/

int Random::_initialized = 0;


/*******************/
/*** Constructor ***/

Random::Random()
{
  if ( ! _initialized )
    _initialized = initRandom();
}


/********************/
/*** get (double) ***/
double
Random::get( double min, double max )
{
  return( (max-min) * random() + min );
}

double
Random::get( double max )
{
  return( max * random() );
}


/***************************/
/*** operator() (double) ***/
double
Random::operator()( double min, double max )
{
  return( (max-min) * random() + min );
}

double
Random::operator()( double max )
{
  return( max * random() );
}


/*****************/
/*** get (int) ***/
int
Random::get( int min, int max )
{
  return( int((max-min) * random()) + min );
}

int
Random::get( int max )
{
  return( int(max * random()) );
}


/************************/
/*** operator() (int) ***/
int
Random::operator()( int min, int max )
{
  return( int((max-min) * random()) + min );
}

int
Random::operator()( int max )
{
  return( int(max * random()) );
}


/*******************/
/*** get  (long) ***/
long
Random::get( long min, long max )
{
  return( long((max-min) * random()) + min );
}

long
Random::get( long max )
{
  return( long(max * random()) );
}


/**************************/
/*** operator()  (long) ***/
long
Random::operator()( long min, long max )
{
  return( long((max-min) * random()) + min );
}

long
Random::operator()( long max )
{
  return( long(max * random()) );
}


/****************/
/*** discrete ***/
double
Random::discrete( float range, float dim_interv )
{
  double d = 2.0 * range / dim_interv;
  int k = int( (d + 1) * random() );
  return( k * dim_interv - range );
}


/**************/
/*** random ***/
double
Random::random()
{
  return ::rand() / (RAND_MAX + 1.0);
}


