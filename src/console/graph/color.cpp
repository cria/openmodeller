/**
 * Definition of GColor class.
 * 
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-10-25
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

#include "color.hh"

// Debug
#include <stdio.h>
using std::string;

#define Limit(v)  ((v) < 0 ? 0 : (v > MAX_COLOR) ? MAX_COLOR : (v))


/**********************************************************************/
/******************************** GColor ******************************/

int GColor::f_depth = 24;

GColor GColor::Black( MIN_COLOR, MIN_COLOR, MIN_COLOR );
GColor GColor::Grey ( MID_COLOR, MID_COLOR, MID_COLOR );
GColor GColor::White( MAX_COLOR, MAX_COLOR, MAX_COLOR );
GColor GColor::Red  ( MAX_COLOR, MIN_COLOR, MIN_COLOR );
GColor GColor::Green( MIN_COLOR, MAX_COLOR, MIN_COLOR );
GColor GColor::Blue ( MIN_COLOR, MIN_COLOR, MAX_COLOR );


/********************/
/*** construtores ***/

GColor::GColor( double red, double green, double blue )
{
  r = int( red   * NUM_COLOR );
  g = int( green * NUM_COLOR );
  b = int( blue  * NUM_COLOR );

  if ( r > MAX_COLOR ) { r = MAX_COLOR; }
  if ( g > MAX_COLOR ) { g = MAX_COLOR; }
  if ( b > MAX_COLOR ) { b = MAX_COLOR; }
}

GColor::GColor( double tom )
{
  int t = int( tom * MAX_COLOR );
  if ( t > MAX_COLOR ) { t = MAX_COLOR; }
  r = g = b = t;
}


/******************/
/*** operator = ***/
GColor &
GColor::operator=( int tom )
{
  r = g = b = tom;
  return *this;
}

GColor &
GColor::operator=( GColor &cor )
{
  r = cor.r;
  g = cor.g;
  b = cor.b;
  return *this;
}


/*********************/
/*** unsigned long ***/

GColor::operator unsigned long ()
{
  if ( f_depth == 24 )
    return ((r & 0xff) << 16) | ((g & 0xff) << 8) | (b & 0xff);

  if ( f_depth == 16 )
    return ((r & 0xf8) << 8) | ((g & 0xFc) << 3) | ((b & 0xF8) >> 3);

  return 0;
}


/*****************/
/*** set Depth ***/
void
GColor::setDepth( int depth )
{
  // Just allows 8, 16 or 24 bits.
  if ( depth &= 0x18 )
    f_depth = depth;
}


/*************/
/*** scale ***/
void
GColor::scale( double esc )
{
  double e = esc++;

  r = Limit( int(e * r) );
  g = Limit( int(e * g) );
  b = Limit( int(e * b) );
}


/*************/
/*** print ***/
void
GColor::print( const std::string& msg )
{
  printf( "%s: ", msg.c_str() );
  printf( "(%d, %d, %d)\n", r, g, b );
}

