/**
 * Definition of Map class.
 * 
 * @author Mauro E S Muñoz <mauro@cria.org.br>
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
  _gt  = new GeoTransform( rst->header().proj, GeoTransform::getDefaultCS() );
}


/*****************/
/*** destrutor ***/

Map::~Map()
{
  delete _gt;
  delete _rst;
}


/******************/
/*** getExtent  ***/
int
Map::getExtent( Coord *xmin, Coord *ymin, Coord *xmax, Coord *ymax) const
{
  int result = 0;

  if (_rst->hasCustomGeotransform()) {

    result = _rst->getExtentInStandardCs(xmin, ymin, xmax, ymax);
  }
  else {

    *xmin = _rst->xMin();
    *ymin = _rst->yMin();
    *xmax = _rst->xMax();
    *ymax = _rst->yMax();

    //Log::instance()->debug( "Raster boundaries before geotransform: xmin=%f, xmax=%f, ymin=%f, ymax=%f\n", *xmin, *xmax, *ymin, *ymax );

    result = _gt->transfOut( xmin, ymin ) && _gt->transfOut( xmax, ymax );

    //Log::instance()->debug( "Raster boundaries after geotransform: xmin=%f, xmax=%f, ymin=%f, ymax=%f\n", *xmin, *xmax, *ymin, *ymax );
  }

  if (*xmin > *xmax) {

    *xmin = -180;
    *xmax = 180;
  }

  if (*ymin > *ymax) {

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
  int result = _gt->transfIn( &x, &y );

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

/**************/
/*** finish ***/
void 
Map::finish()
{
  _rst->finish();
}

/*********************/
/*** delete Raster ***/
int 
Map::deleteRaster()
{
  if ( _rst ) {

    int retVal = _rst->deleteRaster();

    _rst = 0;

    return retVal;
  }

  return 1;
}
