/**
 * Declarations of Raster and RasterFormat classes.
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

#ifndef _RASTERHH_
#define _RASTERHH_

#include <om_defs.hh>
#include <env_io/header.hh>

#include <string>

class RasterGdal;
class Map;

/****************************************************************/
/**************************** Raster ****************************/

class Raster
{
public:

  // This is the output format constants.
  enum { FloatingTiff = 0,
	 GreyTiff = 1,
	 GreyBMP = 2
  };

  /** 
   * If 'categ' != 0 this is a categorical map (ie it can't be
   * interpolated). Othewise this is a continuos map.
   *
   */

  // Open an existing file -- read only.
  Raster( const std::string& file, int categ=0 );

  /**
   * Create a new file for projections.
   * @param file is the name of the output file
   * @param format must be one of the prescribed constants in the enum.
   * @param hdr is a Map from which to copy the raster header params..
   */
  Raster( const std::string& file, int format, const Map* hdr );

  ~Raster();

  /** Get the header. */
  Header& header() { return f_hdr; }

  /** Returns not zero if this map is categorical. */
  int isCategorical()  { return f_hdr.categ; }

  /** Lowest longitude. */
  Coord xMin() { return f_hdr.xmin; }

  /** Lowest latitude. */
  Coord yMin() { return f_hdr.ymin; }

  /** Highest longitude. */
  Coord xMax() { return f_hdr.xmax; }

  /** Highest latitude. */
  Coord yMax() { return f_hdr.ymax; }

  /** Longitudinal map dimension. */
  int dimX()   { return f_hdr.xdim; }

  /** Latitudinal map dimension. */
  int dimY()   { return f_hdr.ydim; }

  /** Longitudinal cell dimension. */
  Coord celX()   { return f_hdr.xcel; }

  /** Latitudinal cell dimension. */
  Coord celY()   { return f_hdr.ycel; }

  /**
   * Returns not zero if it is stored like a grid map and
   * zero if it is stored like a pixel map.
   */
  int getGrid()   { return f_hdr.grid; }

  /** Returns the "noval" value. */
  Scalar noVal()  { return f_hdr.noval; }

  /** Number of bands. */
  int numBand()   { return f_hdr.nband; }

  /**
   * Fills '*val' with the map value at (x,y).
   * Returns zero if (x,y) is out of range.
   */
  int get( Coord x, Coord y, Scalar *val );

  /**
   * Put '*val' at the (x,y) coordinate.
   * Returns 0 if (x,y) is out of range or the map is read only.
   * supports only single band output files.
   */
  int put( Coord x, Coord y, Scalar val );

  /** Find the minimum and maximum values in the first band. */
  int getMinMax( Scalar *min, Scalar *max );

private:

  /**
   * Convert georeferenced coordinate x in the map to
   * (column) coordinated in raster image.
   */
  int convX( Coord x );

  /**
   * Convert georeferenced coordinate y in the map to
   * (row) coordinated in raster image.
   */
  int convY( Coord y );

  /** Find the minimum and maximum values in 'band'. */
  int calcMinMax( Scalar *min, Scalar *max, int band=0 );

  /** The file format is known by the file name extension. **/
  void load();

  RasterGdal *f_rst;

  Scalar f_scalefactor;  // used in projection put.

  std::string f_file;

  Header f_hdr;

  // Disable copying.
  Raster( const Raster& );
  Raster& operator=( const Raster& );

};


#endif
