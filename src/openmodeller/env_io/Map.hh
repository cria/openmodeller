/**
 * Declaration of Map class.
 * 
 * @file
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-09-05
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

#ifndef _MAPHH_
#define _MAPHH_

#include <om_defs.hh>
#include <env_io/raster.hh>


class GeoTransform;


/****************************************************************/
/****************************** Map *****************************/

/** 
 * Responsable for the geografical and projectional transformations
 * related to reading and writing in raster maps.
 *
 */

/*******/
class Map
{
public:

  /** 
   * Create a new map based on a predefined raster.
   * 
   * @param rst Raster object
   * @param ocs The coordinates given to this object will be in
   *        this geografical coordinate system (in WKT format).
   * @param del If not zero 'rst' will be destroied with this
   *        object.
   */
  Map( Raster *rst, char *ocs, int del=0 );
  ~Map();

  char * getCoordSystem() { return _cs; }

  int isCategorical()  { return _rst->isCategorical(); }

  /** is it normalized? */
  int isNormalized() { return _rst->isNormalized(); }

  /** normalization offset */
  Scalar offset() { return _rst->offset(); }

  /** normalization scale */
  Scalar scale() { return _rst->scale(); }

  /** Normalize map values to the interval [min,max]. */
  int normalize( Scalar min, Scalar max ) 
  {
    return _rst->normalize( min, max );
  }

  /** Copy normalized parameters from source map */
  int copyNormalizationParams(Map * source)
  {
    return _rst->copyNormalizationValues((Raster *) (source->_rst));
  }
  
  /** Number of bands. */
  int numBand()   { return _rst->numBand(); }

  /** Get the map limits. */
  int getRegion( Coord *xmin, Coord *ymin, Coord *xmax,
                 Coord *ymax);

  /** Map dimensions. */
  int getDim( int *xdim, int *ydim ) { _rst->getDim(xdim, ydim); } 

  /** Cell width (in map units) */
  int getCell( Coord *xcel, Coord *ycel ) { _rst->getCell(xcel, ycel); } 

  /**
   * Fills 'val' with the map bands values at (x,y).
   * Returns zero if (x,y) is not defined in the map.
   */
  int get( Coord x, Coord y, Scalar *val );

  /**
   * Put the values at 'val' in the map bands at (x,y).
   * @return Return zero if (x,y) is not defined in the map or the
   * map is read only.
   */
  int put( Coord x, Coord y, Scalar *val );

  GeoTransform *getGT()  { return _gt; }


private:

  char         *_cs;   ///< coordinate system name       
  Raster       *_rst;
  GeoTransform *_gt;
  int           _del; ///< If not zero destroy '_rst' in the destructor.
};


#endif
