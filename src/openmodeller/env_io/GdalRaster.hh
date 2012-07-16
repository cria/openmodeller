/**
 * Declaration of GdalRaster class.
 * 
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-08-22
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

#ifndef _GDAL_RASTERHH_
#define _GDAL_RASTERHH_

#include <openmodeller/om_defs.hh>
#include <openmodeller/env_io/Header.hh>
#include <openmodeller/env_io/Raster.hh>

#include <string>
#include <vector>

class GDALDataset;
class MapFormat;

/****************************************************************/
/************************** Raster Gdal *************************/

/** 
 * Manages raster files with GDAL (Geospatial Data Abstraction
 * Library - http://www.remotesensing.org/gdal/index.html)
 *
 */
class dllexp GdalRaster : public Raster
{
public:

  // Empty constructor
  GdalRaster(): f_data(0), f_size(0), f_format(-1), f_currentRow(-1), f_changed(0) {};

  /** 
  * Open an existing file -- read only.
  * If 'categ' != 0 this is a categorical map (ie it can't be
  * interpolated). Othewise this is a continuos map.
  */
  void createRaster( const std::string& file, int categ = 0 );

  #ifdef MPI_FOUND
  /**
  * Create a new file for projections.
  * @param file is the name of the output file
  * @param format is the output format specification.
  */
  void createRaster( const string& output_file_source, const string& file, const MapFormat& format );
  #else 
  /**
  * Create a new file for projections.
  * @param file is the name of the output file
  * @param format is the output format specification.
  */
  void createRaster( const std::string& file, const MapFormat& format );
  #endif

  /**
  * Destructor
  */
  ~GdalRaster();

  int iget( int x, int y, Scalar *val );
  int iput( int x, int y, Scalar val );

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

  /**
  * Put 'no data val' at the (x,y) coordinate.
  * Returns 0 if (x,y) is out of range or the map is read only.
  * supports only single band output files.
  */
  int put( Coord x, Coord y );

  /** Finds the minimum and maximum values in the first band. 
   * @param min Pointer to minimum value
   * @param max Pointer to maximum value
   * @return 1 if values are present, 0 otherwise
   */
  int getMinMax( Scalar *min, Scalar *max );

  /** Find the minimum and maximum values in 'band'. */
  int calcMinMax( int band=0 );

  /** 
   * Event that must be called to indicate when the projection is finished.
   */
  void finish();

  /** Method to delete a raster.
   * @return 1 if the raster was successfully deleted, 0 otherwise.
   */
  int deleteRaster();

private:

  /** Open a raster file. **/
  void open( char mode );

  /** Create a new raster file based on 'hdr'. **/
  void create( int format );

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

  void loadRow( int row, bool writeOperation=false );

  void saveRow(); // Save the current row.

  GDALDataset *f_ds;
  
  Scalar *f_data; // One line data for all bands.
  int     f_size; // Size of one line.

  int     f_format; // File format used to create the raster (MapFormat::getFormat())

  int f_currentRow;
  int f_changed;

  // Disable copying.
  GdalRaster( const GdalRaster& );
  GdalRaster& operator=( const GdalRaster& );
};

#endif

