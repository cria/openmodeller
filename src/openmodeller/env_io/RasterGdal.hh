/**
 * Declaration of RasterGdal class.
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

#ifndef _RASTER_GDALHH_
#define _RASTER_GDALHH_

#include "env_io/header.hh"

#include <string>
#include <vector>

class GDALDataset;

/****************************************************************/
/************************** Raster Gdal *************************/

/** 
 * Manages raster files with GDAL (Geospatial Data Abstraction
 * Library - http://www.remotesensing.org/gdal/index.html)
 *
 */

/**************/
class RasterGdal
{
public:

  /** Open 'file' for read (mode = 'r') or write (mode = 'w'). **/
  RasterGdal( const std::string& file, char mode );

  /** Create a new file for write based on the header 'hdr'. **/
  RasterGdal( const std::string& file, int format, const Header& hdr );

  ~RasterGdal();

  /** Returns the file's header **/
  Header& header() { return f_hdr; }

  int iget( int x, int y, Scalar *val );
  int iput( int x, int y, Scalar val );

private:

  /** Open a raster file. **/
  void open( char mode );

  /** Create a new raster file based on 'hdr'. **/
  void create(int format);

  void initBuffer();

  static void initGdal();

  /**
   * Read 'num_rows' rows starting from 'first_row' to the memory
   * pointed to by 'buf'. Each element read is transformed in a
   * 'Scalar'.
   */
  void read ( Scalar *buf, int first_row, int num_rows );

  /**
   * Write 'num_rows' rows starting from 'first_row' to the memory
   * pointed to by 'buf'.
   */
  void write( Scalar *buf, int first_row, int num_rows );

  void loadRow( int row );
  void saveRow();           // Save the current row.

  Header       f_hdr;
  GDALDataset *f_ds;
  std::string f_name;

  Scalar *f_data; // One line data for all bands.
  int     f_size; // Size of one line.

  int f_currentRow;
  int f_changed;

};


#endif
