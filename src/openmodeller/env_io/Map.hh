/**
 * Declaration of Map class.
 * 
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

#include <openmodeller/om_defs.hh>
#include <openmodeller/env_io/Raster.hh>
#include <openmodeller/env_io/MapIterator.hh>

#include <string>

class GeoTransform;

/****************************************************************/
/****************************** Map *****************************/

/** 
 * Responsable for the geografical and projectional transformations
 * related to reading and writing in raster maps.
 *
 */

/*******/
class dllexp Map
{
public:

  /** 
  * Create a new map based on a predefined raster.
  * The Map takes ownership of the Raster object pointed
  * to by rst.  It will be deleted when this is deleted.
  * 
  * @param rst Raster object
  */
  Map( Raster *rst );
  ~Map();

  MapIterator begin() const
  {
    return MapIterator( _rst->header(), _gt );
  }

  const Header& getHeader() const { return _rst->header(); }

  int isCategorical() const { return _rst->isCategorical(); }

  /** Support external specification of the min/max */
  void setMinMax( Scalar min, Scalar max )
  {
    _rst->setMinMax( min, max );
  }

  bool hasMinMax() const
  {
    return _rst->hasMinMax();
  }

  /** Find the minimum and maximum values in the first band. */
  int getMinMax( Scalar *min, Scalar *max ) const
  {
    return _rst->getMinMax( min, max );
  }

  /** Number of bands. */
  int numBand() const  { return _rst->numBand(); }

  /** Get the map limits. */
  int getExtent( Coord *xmin, Coord *ymin, Coord *xmax, Coord *ymax) const;

  /** Map dimensions. */
  int getDim( int *xdim, int *ydim ) const
  {
    *xdim = _rst->dimX(); *ydim = _rst->dimY(); return 1;
  } 

  /** Cell width (in map units) */
  int getCell( Coord *xcel, Coord *ycel ) const
  {
    *xcel = _rst->celX(); *ycel = _rst->celY(); return 1;
  } 

  /**
  * Fills 'val' with the map bands values at (x,y).
  * Returns zero if (x,y) is not defined in the map.
  */
  int get( Coord x, Coord y, Scalar *val ) const;

  /**
  * Put the values at 'val' in the first band at (x,y).
  * @return Return zero if (x,y) is not defined in the map or the
  * map is read only.
  */
  int put( Coord x, Coord y, Scalar val );

  /**
  * Put the value for noval in the first band at (x,y).
  * @return Return zero if (x,y) is not defined in the map or the
  * map is read only.
  */
  int put( Coord x, Coord y );

  GeoTransform *getGT() const { return _gt; }

  /** 
  * Return row and column for the given coordinates.
  * @return Return zero if (x,y) is not defined in the map.
  */
  int getRowColumn( Coord x, Coord y, int *row, int *col );

  /** 
  * Event that must be called to indicate when the projection is finished.
  */
  void finish();

  /** 
  * Deletes the associated raster.
  * @return 1 if the associated raster was deleted, 0 otherwise.
  */
  int deleteRaster();

private:

  Raster      *_rst;
  GeoTransform  *_gt;

  // Disable copying.
  Map( const Map& );
  Map& operator=( const Map& );

};


#endif

