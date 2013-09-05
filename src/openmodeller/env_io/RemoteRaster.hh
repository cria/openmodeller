/**
 * Declaration of RemoteRaster class.
 * 
 * @author Renato De Giovanni <renato (at) cria . org . br>
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2012 by CRIA -
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

#ifndef _REMOTE_RASTERHH_
#define _REMOTE_RASTERHH_

#include <openmodeller/env_io/Raster.hh>
#include <openmodeller/env_io/GdalRaster.hh>

#include <string>
#include <stdio.h>

#define OM_REMOTE_RASTER_SUBDIR "r_layers"

class GDALDataset;
class MapFormat;

/*************************************************************/
/*********************** Remote Raster ***********************/

struct CacheFile {

  const char *fileName;
  FILE *stream;
};

/** 
 * Fetches raster identifiers starting with http:// or https:// into local 
 * files to be opened with GDAL
 *
 */
class dllexp RemoteRaster : public GdalRaster
{
public:
  /**
  * RasterFactory calls this function to build a new Raster.
  */
  static Raster* CreateRasterCallback();

  /**
  * RasterFactory needs an empty constructor.
  */
  RemoteRaster():GdalRaster() {};

  /**
  * Destructor
  */
  ~RemoteRaster();

  /** 
  * Open a raster (read only). Method needed by RasterFactory.
  * @param str WCS string pointing to the raster.
  * @param categ if != 0 this is a categorical map (ie it can't be interpolated). Othewise this is a continuos map.
  *
  * Needed by RasterFactory.
  */
  void createRaster( const std::string& str, int categ = 0 );

  /**
  * Create a new raster for projections. If called will throw an exception
  * since the WCS driver in GDAL does not support creation. Method needed by 
  * RasterFactory.
  * @param str WCS string pointing to the raster.
  * @param format Output format specification.
  */
  #ifdef MPI_FOUND
  void createRaster( const std::string& output_file, const std::string& file, const MapFormat& format );
  #else
  void createRaster( const std::string& str, const MapFormat& format );
  #endif

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

  /** 
   * Event that must be called to indicate when the projection is finished.
   */
  void finish();

  /** Method to delete a raster.
   * @return 1 if the raster was successfully deleted, 0 otherwise.
   */
  int deleteRaster();

private:

  // Disable copying.
  RemoteRaster( const RemoteRaster& );
  RemoteRaster& operator=( const RemoteRaster& );

  /** Callback method for CURL when fetching data.
   */
  static size_t _writeData(void *buffer, size_t size, size_t nmemb, void *stream);

  /** Indicates if raster comes from an untrusted source.
   */
  bool _isFromRejectedSource( const std::string& str );
};

#endif
