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

#include <env_io/map.hh>
#include <env_io/raster.hh>
#include <env_io/raster_gdal.hh>

#include <om_log.hh>

#include <Exceptions.hh>

using std::string;

/****************************************************************/
/**************************** Raster ****************************/

/******************/
/*** construtor ***/

Raster::Raster( const string& file, int categ ) :
  f_rst(0),
  f_scalefactor(1.0),
  f_file(file),
  f_hdr()
{
  // Opens read only and init the class attributes.
  load();

  // set categorical status.  It's not stored as part of
  // the gdal file's header.
  f_hdr.categ = categ;
}

Raster::Raster( const string& file, int format, const Map* hdr ) :
  f_file( file ),
  f_hdr( hdr->getHeader() )
{

  if ( format >= 3 ) {
    throw GraphicsDriverException( "Unsupported output format" );
  }

  switch( format ) {
  case GreyBMP:
    f_scalefactor = 255.0;
    f_hdr.noval = 0.0;
    break;
  case GreyTiff:
    f_scalefactor = 254.0;
    f_hdr.noval = -1.0;
    break;
  case FloatingTiff:
  default:
    f_scalefactor = 1.0;
    f_hdr.noval = -1.0;
  }

  // Create a new file in disk.
  f_rst = new RasterGdal( file, format, f_hdr );

  f_hdr = f_rst->header();

}

/*****************/
/*** destrutor ***/

Raster::~Raster()
{
  delete f_rst;
}

/***********/
/*** get ***/
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
  return f_rst->iget( x, y, val );
}

/***********/
/*** put ***/
int
Raster::put( Coord px, Coord py, Scalar val )
{
  int x = convX( px );
  int y = convY( py );

  if ( x < 0 || x >= f_hdr.xdim || y < 0 || y >= f_hdr.ydim )
    return 0;

  return f_rst->iput( x, y, f_scalefactor*val );
}

/*******************/
/*** get Min Max ***/
int
Raster::getMinMax( Scalar *min, Scalar *max )
{
  if ( ! calcMinMax( &f_hdr.min, &f_hdr.max ) )
    return 0;

  *min = f_hdr.min;

  *max = f_hdr.max;

  return 1;
}


/**************/
/*** conv X ***/
int
Raster::convX( Coord x )
{
  x = (x - f_hdr.gt[0]) / f_hdr.gt[1];
  return int( f_hdr.grid ? x : (x + 0.5) );
}


/**************/
/*** conv Y ***/
int
Raster::convY( Coord y )
{
  y = (y - f_hdr.gt[3]) / f_hdr.gt[5];
  return int( f_hdr.grid ? y : (y + 0.5) );
}

/********************/
/*** calc Min Max ***/
int
Raster::calcMinMax( Scalar *min, Scalar *max, int band )
{
  if ( f_hdr.minmax ) {
    return 1;
  }
  Scalar *bands;
  bands = new Scalar[ f_hdr.nband ];
  Scalar *val = bands + band;

  bool initialized = false;

  // Scan all map values.
  for ( int y = 0; y < f_hdr.ydim; y++ )
    for ( int x = 0; x < f_hdr.xdim; x++ )
      if ( f_rst->iget( x, y, bands ) ) {
        if ( !initialized ) {
          initialized = true;
          *min = *max = *val;
        }
        if ( *min > *val )
          *min = *val;
        else if ( *max < *val )
          *max = *val;
      }

  delete [] bands;

  if (!initialized)
    return 0;

  f_hdr.minmax = 1;

  if (!initialized)
    return 0;

  return 1;
}


/************/
/*** load ***/
void
Raster::load( )
{
  // Opens read only.
  if ( f_rst ) {
    delete f_rst;
  }
  f_rst = new RasterGdal( f_file, 'r' );

  // Read the new file's header.
  f_hdr = f_rst->header();

}

