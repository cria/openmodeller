/**
 * Definitions of Raster and RasterFormat classes.
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

#include <env_io/raster.hh>

#include <om_log.hh>
#include <string.h>


/****************************************************************/
/************************ Raster Format *************************/

char *RasterFormat::f_name[] = {
  "Undefined",

  // GDAL compatible names.
  "AAIGrid", "AIG", "BMP", "BSB", "CEOS", "DOQ1", "DOQ2",
  "DTED", "ECW", "EHdr", "ENVI", "Envisat", "FAST", "FITS",
  "GIF", "GIO", "GRASS", "GTiff", "HDF4", "HFA", "HKV", "JDEM",
  "JPEG", "JPEG2000", "JP2KAK", "L1B", "MFF", "MrSID", "NITF",
  "OGDI", "PAux", "PNG", "PNM", "SDTS", "SAR_CEOS", "USGSDEM",
  "XPM",

  "Undefined"
};

char *RasterFormat::f_ext[] = {
  "",

  // todo:
  //
  // - There are distinct file formats using the same extention!
  //   (ex: ".doq")
  // - There are different extensions for the same file format!
  //   These are not included here!

  // GDAL compatible extensions.
  ".asc", ".adf", ".bmp", ".kap", ".ceo", ".doq", ".doq",
  ".dt1", ".ecw", ".bil", ".hdr", ".n1", ".fast", ".fits",
  ".gif", ".adf", ".grass", ".tif", ".hdf4", ".img", ".hkv",
  ".mem",
  ".jpg", ".jp2", ".jp2", ".l1b", ".mff", ".msid", ".nitf",
  ".ogdi", ".aux", ".png", ".pnm", ".ddf", ".ceos", ".dem",
  ".xpm",

  "Undefined"
};


/******************/
/*** construtor ***/

RasterFormat::RasterFormat( char *file_name )
{
  // Finds the file format based on the file name extention.
  char *ext;
  f_code = Unknown;
  if ( ext = strrchr( file_name, '.' ) )
    for ( f_code = 1; f_code < Undefined; f_code++ )
      if ( ! strcmp( ext, f_ext[f_code] ) )
        break;
}




/****************************************************************/
/**************************** Raster ****************************/

/******************/
/*** construtor ***/

Raster::Raster( int categ )
{
  f_normal = 0;
  f_hdr.categ = categ;
  f_offset = 0;
  f_scale = 0;
}

Raster::Raster( Header &hdr )
{
  f_normal = 0;
  setHeader( hdr );
}


/*****************/
/*** normalize ***/
int
Raster::normalize( Scalar min, Scalar max )
{
  // Normalization is not for categorical maps.
  if ( f_hdr.categ )
    return 0;

  // Calculate the map's minimum and maximum.
  if ( ! f_hdr.minmax &&
       ! calcMinMax( &f_hdr.min, &f_hdr.max ) )
    return 0;
  f_hdr.minmax = 1;

  // Calculates the scale and offset to transform a value
  // in [f_hdr.min, f_hdr.max] to a value in [min, max]
  // using the transformation: x' = x * f_scale + f_offset.
  //
  f_scale  = (max - min) / (f_hdr.max - f_hdr.min);
  f_offset = min - f_scale * f_hdr.min;
  f_normal = 1;

  printf("Layer normalized. Min=%f Max=%f Scale=%f Offset=%f\n", 
	f_hdr.min, f_hdr.max, f_scale, f_offset);

  return 1;
}

/*******************************/
/*** copyNormalizationValues ***/
int Raster::copyNormalizationValues(Raster * source)
{
  // Calculate the map's minimum and maximum.
  if ( ! f_hdr.minmax &&
       ! calcMinMax( &f_hdr.min, &f_hdr.max ) )
    return 0;
  f_hdr.minmax = 1;

  f_scale = source->f_scale;
  f_offset = source->f_offset;
  f_normal = 1;

  //g_log("Layer copied normalization. Min=%f Max=%f Scale=%f Offset=%f\n", 
  //f_hdr.min, f_hdr.max, f_scale, f_offset);

  return 1;
}


/******************/
/*** get Region ***/
int
Raster::getRegion( Coord *xmin, Coord *ymin, Coord *xmax,
                   Coord *ymax )
{
  *xmin = f_hdr.xmin;
  *ymin = f_hdr.ymin;
  *xmax = f_hdr.xmax;
  *ymax = f_hdr.ymax;

  return 1;
}


/***************/
/*** get Dim ***/
int
Raster::getDim( int *xdim, int *ydim )
{
  *xdim = f_hdr.xdim;
  *ydim = f_hdr.ydim;

  return 1;
}


/****************/
/*** get Cell ***/
int
Raster::getCell( Coord *xcel, Coord *ycel )
{
  *xcel = f_hdr.xcel;
  *ycel = f_hdr.ycel;

  return 1;
}


/***********/
/*** get ***/
/**
 * The normalization is done in iget() (derived classes) not
 * in get(). This way one can normalizes even reading through
 * integer coordinates.
 */
int
Raster::get( Coord px, Coord py, Scalar *val )
{
  Scalar *pv = val;
  for ( int i = 0; i < f_hdr.nband; i++ )
    *pv++ = f_hdr.noval;

  int x = convX( px );
  int y = convY( py );

  // If the point is out of range, returns 0.
  if ( x < 0 || x >= f_hdr.xdim || y < 0 || y >= f_hdr.ydim )
    return 0;

  // 'iget()' detects if the coordinate has or not valid
  // information (noval);
  return iget( x, y, val );
}


/***********/
/*** put ***/
int
Raster::put( Coord px, Coord py, Scalar *val )
{
  int x = convX( px );
  int y = convY( py );

  if ( x < 0 || x >= f_hdr.xdim || y < 0 || y >= f_hdr.ydim )
    return 0;

  return iput( x, y, val );
}


/*************/
/*** print ***/
int
Raster::print( char *msg )
{
  g_log( "%s\n", msg );

  f_hdr.printHeader();
  return 1;
}


/******************/
/*** set Header ***/
int
Raster::setHeader( Header &hdr )
{
  f_hdr = hdr;

  // Confirm the coherence of (xcel, ycel).
  f_hdr.calculateCell();

  return 0;
}


/**************/
/*** conv X ***/
int
Raster::convX( Coord x )
{
  x = (x - f_hdr.xmin) / f_hdr.xcel;
  return int( f_hdr.grid ? x : (x + 0.5) );
}


/**************/
/*** conv Y ***/
int
Raster::convY( Coord y )
{
  y = (y - f_hdr.ymin) / f_hdr.ycel;
  return f_hdr.ydim - int( f_hdr.grid ? y : (y + 0.5) );
}


/*******************/
/*** calc Normal ***/
void
Raster::calcNormal( Scalar *val )
{
  if ( f_normal )
    *val = f_scale * (*val) + f_offset;
}


/********************/
/*** calc Min Max ***/
int
Raster::calcMinMax( Scalar *min, Scalar *max, int band )
{
  Scalar *bands;
  bands = new Scalar[ f_hdr.nband ];
  Scalar *val = bands + band;

  // Initial values.
  if ( ! iget( 0, 0, bands ) )
    return 0;
  *min = *max = *val;

  // Scan all map values.
  for ( int y = 0; y < f_hdr.ydim; y++ )
    for ( int x = 0; x < f_hdr.xdim; x++ )
      if ( ! iget( x, y, bands ) )
	{
	  if ( *min > *val )
	    *min = *val;
	  else if ( *max < *val )
	    *max = *val;
	}

  delete bands;

  return 1;
}


