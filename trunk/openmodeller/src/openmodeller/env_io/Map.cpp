/**
 * Definition of Map class.
 * 
 * @file
 * @author Mauro E S Mu�oz <mauro@cria.org.br>
 * @date 2003-09-05
 * @author Alexandre Copertino Jardim <alexcj@dpi.inpe.br>
 * @date 2006-03-21
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

#include <openmodeller/env_io/Map.hh>

#include <openmodeller/env_io/GeoTransform.hh>
#include <openmodeller/env_io/Raster.hh>
#include <openmodeller/Log.hh>

#include <math.h>

/****************************************************************/
/****************************** Map *****************************/

/******************/
/*** construtor ***/
Map::Map( Raster *rst ) 
{
  _rst = rst;
  _gt  = new GeoTransform( rst->header().proj, GeoTransform::cs_default );
}


/*****************/
/*** destrutor ***/

Map::~Map()
{
  delete _gt;
  delete _rst;
}


/******************/
/*** getRegion  ***/
int
Map::getRegion( Coord *xmin, Coord *ymin, Coord *xmax,
                Coord *ymax) const
{
  *xmin = _rst->xMin();
  *ymin = _rst->yMin();
  *xmax = _rst->xMax();
  *ymax = _rst->yMax();

  bool result = 
  _gt->transfOut( xmin, ymin ) &&
  _gt->transfOut( xmax, ymax );

  if (*xmin > *xmax)
  {
    *xmin = -180;
    *xmax = 180;
  }

  if (*ymin > *ymax)
  {
    *ymin = -90;
    *ymax = 90;
  }

  return result;
}


/***********/
/*** get ***/
int
Map::get( Coord x, Coord y, Scalar *val ) const
{
  return _gt->transfIn( &x, &y ) ? _rst->get( x, y, val ) : 0;
}

/***********/
/*** put ***/
int
Map::put( Coord x, Coord y, Scalar val )
{
  return _gt->transfIn( &x, &y ) ? _rst->put( x, y, val ) : 0;
}

/***********/
/*** put ***/
int
Map::put( Coord x, Coord y )
{
  return _gt->transfIn(&x,&y) ? _rst->put( x,y ) : 0;
}

/**********************/
/*** get row column ***/
int
Map::getRowColumn( Coord x, Coord y, int *row, int *col )
{
  // Transform the given coordinates into the raster coordinate system & projection
  bool result = _gt->transfIn( &x, &y );

  Coord xmin = _rst->xMin();
  Coord ymin = _rst->yMin();
  Coord xmax = _rst->xMax();
  Coord ymax = _rst->yMax();

  int xdim = _rst->dimX();
  int ydim = _rst->dimY();

  double xres = (xmax - xmin) / xdim;
  double yres = (ymax - ymin) / ydim;

  // Offset, not the cell index -> floor
  *col = (int) floor ( (x - xmin) / xres );
  *row = (int) floor ( (ymax - y) / yres );

  return result;
}
