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

#include <openmodeller/env_io/Header.hh>

#include <openmodeller/Log.hh>

using std::string;
using std::pair;
using std::make_pair;

/****************************************************************/
/***************************** Header ***************************/

/******************/
/*** construtor ***/

Header::Header( int xd, int yd, Coord xm, Coord ym,
                Coord xM, Coord yM, Scalar nv, int nb, int gd ) :
  xdim( xd ),
  ydim( yd ),
  xmin( xm ),
  ymin( ym ),
  xmax( xM ),
  ymax( yM ),
  xcel( 0 ),
  ycel( 0 ),
  noval( nv ),
  nband( nb ),
  grid( gd ),
  categ( 0 ),
  minmax( 0 ),
  vmin( 0.0 ),
  vmax( 0.0 ),
  proj()
{
  calculateCell();

  // Given the information provided in this constructor,
  // We pretty much have to assume this is the geotransform.

  gt[0] = xmin;
  gt[1] = xcel;
  gt[2] = 0.0;
  gt[3] = ymax;
  gt[4] = 0.0;
  gt[5] = -1.0*ycel;
}

Header::Header( const Header &h )
{
  operator=( h );
}


/*****************/
/*** destrutor ***/

Header::~Header()
{
}


/******************/
/*** operator = ***/
Header &
Header::operator=( const Header &h )
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
  categ = h.categ;

  minmax = h.minmax;
  vmin = h.min;
  vmax = h.max;

  proj = h.proj;

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
void 
Header::setProj( const string& projection )
{
  proj = projection;
}

// Does not support rotations.
pair<Coord,Coord>
Header::convertXY2LonLat( int x, int y ) const
{
  // Get the coordinates associated with the center of the cell
  Coord lon = gt[1]*(x+0.5) + gt[0];
  Coord lat = gt[5]*(y+0.5) + gt[3];
  
  return make_pair(lon,lat);
}

// Does not support rotations.
pair<int,int>
Header::convertLonLat2XY( Coord lon, Coord lat ) const
{
  int x = static_cast<int>((lon - gt[0]) / gt[1]);
  int y = static_cast<int>((lat - gt[3]) / gt[5]);

  return make_pair(x,y);
}

/*************/
/*** print ***/
void
Header::printHeader( char *msg ) const
{
  Log::instance()->info( "%s\n", msg );

  Log::instance()->info( "xdim: %d\n", xdim );
  Log::instance()->info( "ydim: %d\n", ydim );
  Log::instance()->info( "xmin: %.4f\n", xmin );
  Log::instance()->info( "ymin: %.4f\n", ymin );
  Log::instance()->info( "xmax: %.4f\n", xmax );
  Log::instance()->info( "ymax: %.4f\n", ymax );
  Log::instance()->info( "xcel: %.4f\n", xcel );
  Log::instance()->info( "ycel: %.4f\n", ycel );
  Log::instance()->info( "noval: %.4f\n", noval );
  Log::instance()->info( "band: %d\n", nband );
  Log::instance()->info( "grid: %d\n", grid );
  Log::instance()->info( "var : %s\n", categ ? "categórica" : "ordenável" );

  if ( minmax )
    {
      Log::instance()->info( "min: %f\n", vmin  );
      Log::instance()->info( "max: %f\n", vmax  );
    }
  else
    Log::instance()->info( "No minimum or maximum available.\n" );

  for( int i=0;i<6; i++ ) {
    Log::instance()->info( "GT[%d] = %f\n",i,gt[i] );
  }
  
  Log::instance()->info( "proj: %s\n", proj.c_str() );
}
