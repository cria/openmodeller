/**
 * Definition of Header class.
 * 
 * @file
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-08-22
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

#include "env_io/header.hh"

#include <string.h>
#include <stdio.h>


/****************************************************************/
/***************************** Header ***************************/

/******************/
/*** construtor ***/

Header::Header( int xd, int yd, Coord xm, Coord ym,
                Coord xM, Coord yM, Scalar nv, int nb, int gd )
{
  xdim  = xd;
  ydim  = yd;
  xmin  = xm;
  ymin  = ym;
  xmax  = xM;
  ymax  = yM;
  noval = nv;
  nband = nb;
  grid  = gd;
  dtype = UInt8;
  categ = 0;

  minmax = 0;
  min = 0.0;
  max = 0.0;

  proj  = 0;

  calculateCell();
}

Header::Header( Header &h )
{
  proj = 0;
  operator=( h );
}


/*****************/
/*** destrutor ***/

Header::~Header()
{
  if ( proj )
    delete proj;
}


/******************/
/*** operator = ***/
Header &
Header::operator=( Header &h )
{
  xdim = h.xdim;
  ydim = h.ydim;

  xmin = h.xmin;
  ymin = h.ymin;
  xmax = h.xmax;
  ymax = h.ymax;

  xcel = h.xcel;
  ycel = h.ycel;

  noval = h.noval;
  nband = h.nband;
  grid  = h.grid;
  dtype = h.dtype;
  categ = h.categ;

  minmax = h.minmax;
  min = h.min;
  max = h.max;

  setProj( h.proj );

  return *this;
}


/**********************/
/*** calculate Cell ***/
void
Header::calculateCell()
{
  // todo: verify if it is true for "grid" and "pixel".

  xcel = (xmax - xmin) / xdim;
  ycel = (ymax - ymin) / ydim;
}


/****************/
/*** set Proj ***/
char *
Header::setProj( char *projection )
{
  if ( proj )
    delete proj;

  int len = (strlen(projection) + 1) * sizeof(char);
  proj = new char[len];
  memcpy( proj, projection, len );

  return proj;
}


/*************/
/*** print ***/
void
Header::print( char *msg )
{
  printf( "%s\n", msg );

  printf( "xdim: %d\n", xdim );
  printf( "ydim: %d\n", ydim );
  printf( "xmin: %.4f\n", xmin );
  printf( "ymin: %.4f\n", ymin );
  printf( "xmax: %.4f\n", xmax );
  printf( "ymax: %.4f\n", ymax );
  printf( "xcel: %.4f\n", xcel );
  printf( "ycel: %.4f\n", ycel );
  printf( "noval: %.4f\n", noval );
  printf( "band: %d\n", nband );
  printf( "grid: %d\n", grid );
  printf( "type: %d\n", dtype );
  printf( "var : %s\n", categ ? "categórica" : "ordenável" );

  if ( minmax )
    {
      printf( "min: %f\n", min  );
      printf( "max: %f\n", max  );
    }
  else
    printf( "No minimum or maximum available.\n" );
  
  printf( "proj: %s\n", proj ? proj : "" );
}
