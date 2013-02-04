/**
 * Declaration of Random class.
 * 
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



#ifndef _RANDOMHH_
#define _RANDOMHH_

#include <openmodeller/os_specific.hh>
#include <openmodeller/om_defs.hh>


/********************************************************/
/************************ Random ************************/

/** 
 * Class to generate random numbers
 *
 */
class dllexp Random
{
public:
  Random();

  /** Return real numbers between [min, max).*/
  double get( double min, double max );
  /** Return real numbers between [0, max).*/
  double get( double max );
  /** Return real numbers between [0, 1).*/
  double get()   { return random(); }

  double operator()( double min, double max );
  double operator()( double max );
  double operator()()  { return random(); }


  /** Return integer numbers between [min, max).*/
  int get( int min, int max );
  /** Return integer numbers between [0, max).*/
  int get( int max );

  int operator()( int min, int max );
  int operator()( int max );

  /** Return numbers "long int's" between [min, max).*/
  long get( long min, long max );
  /** Return numbers "long int's" between [0, max).*/
  long get( long max );

  long operator()( long min, long max );
  long operator()( long max );

  /** Generate a random number, r, between [-range, range], so
   *  that r = -range + k * dim_interv, where k = 0,..,N.
   */
  double discrete( float range, float dim_interv );


private:

  /** Return real numbers in the interval [0, 1).*/
  double random();

  static int _initialized;
};


#endif


