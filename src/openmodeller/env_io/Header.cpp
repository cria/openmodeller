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

#include <om_log.hh>
#include <string.h>


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

  // Given the information provided in this constructor,
  // We pretty much have to assume this is the geotransform.

  gt[0] = xmin;
  gt[1] = xcel;
  gt[2] = 0.0;
  gt[3] = ymin;
  gt[4] = 0.0;
  gt[5] = ycel;
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

  gt[0] = h.gt[0];
  gt[1] = h.gt[1];
  gt[2] = h.gt[2];
  gt[3] = h.gt[3];
  gt[4] = h.gt[4];
  gt[5] = h.gt[5];

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
Header::printHeader( char *msg )
{
  g_log( "%s\n", msg );

  g_log( "xdim: %d\n", xdim );
  g_log( "ydim: %d\n", ydim );
  g_log( "xmin: %.4f\n", xmin );
  g_log( "ymin: %.4f\n", ymin );
  g_log( "xmax: %.4f\n", xmax );
  g_log( "ymax: %.4f\n", ymax );
  g_log( "xcel: %.4f\n", xcel );
  g_log( "ycel: %.4f\n", ycel );
  g_log( "noval: %.4f\n", noval );
  g_log( "band: %d\n", nband );
  g_log( "grid: %d\n", grid );
  g_log( "type: %d\n", dtype );
  g_log( "var : %s\n", categ ? "categórica" : "ordenável" );

  if ( minmax )
    {
      g_log( "min: %f\n", min  );
      g_log( "max: %f\n", max  );
    }
  else
    g_log( "No minimum or maximum available.\n" );
  
  g_log( "proj: %s\n", proj ? proj : "" );
}
