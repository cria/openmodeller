/**
 * Declaration of MapFormat class.
 * 
 * @file
 * @author Ricardo Scachetti Pereira <rpereira@ku.edu>
 * @date 2003-09-25
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 * Copyright(c), David R.B. Stockwell of Symbiotik Pty. Ltd.
 * Copyright(c), CRIA - Centro de Referencia em Informacao Ambiental
 *
 * http://www.nhm.ku.edu
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

#include <map_format.hh>
#include <env_io/map.hh>
#include <env_io/raster.hh>
#include <Exceptions.hh>
#include <om_log.hh>

using std::string;

#define DEFAULT_FORMAT FloatingTiff

MapFormat::MapFormat( ) :
  format( DEFAULT_FORMAT ),
  width( 0 ),
  widthIsSet( false ),
  height( 0 ),
  heightIsSet( false ),
  xmin( 0 ),
  xminIsSet( false ),
  ymin( 0 ),
  yminIsSet( false ),
  xmax( 0 ),
  xmaxIsSet( false ),
  ymax( 0 ),
  ymaxIsSet( false ),
  noval( 0 ),
  novalIsSet( false ),
  proj( ),
  projIsSet( false )
{}

MapFormat::MapFormat( int width, int height, Coord xmin, Coord ymin,
		      Coord xmax, Coord ymax, Scalar noval, char const * proj ) :
  format( DEFAULT_FORMAT ),
  width( width ),
  widthIsSet( true ),
  height( height ),
  heightIsSet( true ),
  xmin( xmin ),
  xminIsSet( true ),
  ymin( ymin ),
  yminIsSet( true ),
  xmax( ymax ),
  xmaxIsSet( true ),
  ymax( ymax ),
  ymaxIsSet( true ),
  noval( noval ),
  novalIsSet( true ),
  proj( proj ),
  projIsSet( true )
{}

MapFormat::MapFormat( char const *filenameWithFormat ) :
  format( DEFAULT_FORMAT )
{
  // TODO - determine the format from the raster file....
  Raster r( filenameWithFormat );

  Header h = r.header();

  setWidth( h.xdim );
  setHeight( h.ydim );
  setXMin( h.xmin );
  setYMin( h.ymin );
  setXMax( h.xmax );
  setYMax( h.ymax );
  setNoDataValue( h.noval );
  setProjection( h.proj );

}

void MapFormat::copyDefaults( const Map& map ) {

  Header h = map.getHeader();

  if (!widthIsSet) {
    g_log.debug( "Copying width = %d\n", h.xdim );
    setWidth( h.xdim );
  }

  if (!heightIsSet) {
    g_log.debug( "Copying height = %d\n", h.ydim );
    setHeight( h.ydim );
  }

  if (!xminIsSet) {
    g_log.debug( "Copying xmin = %f\n", h.xmin );
    setXMin( h.xmin );
  }

  if (!yminIsSet) {
    g_log.debug( "Copying ymin = %f\n", h.ymin );
    setYMin( h.ymin );
  }

  if (!xmaxIsSet) {
    g_log.debug( "Copying xmax = %f\n", h.xmax );
    setXMax( h.xmax );
  }

  if (!ymaxIsSet) {
    g_log.debug( "Copying ymax = %f\n", h.ymax );
    setYMax( h.ymax );
  }

  if (!projIsSet ) {
    g_log.debug( "Copying projection\n" );
    setProjection( h.proj );
  }

}

MapFormat::~MapFormat()
{}

void MapFormat::setFormat( int f ) {
  switch( f ) {
  FloatingTiff:
  GreyTiff:
  GreyBMP:
    format = f;
    break;
  default:
    format = FloatingTiff;
  }
}

void MapFormat::setWidth( int v ) {
  width = v;
  widthIsSet = true;
}

void MapFormat::setHeight( int v ) {
  height = v;
  heightIsSet = true;
}

void MapFormat::setXMin( Coord v ) {
  xmin = v;
  xminIsSet = true;
}

void MapFormat::setYMin( Coord v ) {
  ymin = v;
  yminIsSet = true;
}

void MapFormat::setXMax( Coord v ) {
  xmax = v;
  xmaxIsSet = true;
}

void MapFormat::setYMax( Coord v ) {
  ymax = v;
  ymaxIsSet = true;
}

void MapFormat::setNoDataValue( Scalar v ) {
  noval = v;
  novalIsSet = true;
}

void MapFormat::setProjection( const string& v ) {
  proj = v;
  projIsSet = true;
}

int MapFormat::getWidth() const {
  if ( !widthIsSet )
    throw InvalidParameterException( "Width not set" );

  return width;

}

int MapFormat::getHeight() const {
  if ( !heightIsSet )
    throw InvalidParameterException( "Height not set" );

  return height;

}

Coord MapFormat::getXMin() const {
  if ( !xminIsSet )
    throw InvalidParameterException( "XMin not set" );

  return xmin;

}

Coord MapFormat::getYMin() const {
  if ( !yminIsSet )
    throw InvalidParameterException( "Ymin not set" );

  return ymin;

}

Coord MapFormat::getXMax() const {
  if ( !xmaxIsSet )
    throw InvalidParameterException( "Xmax not set" );

  return xmax;

}

Coord MapFormat::getYMax() const {
  if ( !ymaxIsSet )
    throw InvalidParameterException( "Ymax not set" );

  return ymax;

}

Scalar MapFormat::getNoDataValue() const {
  if ( !novalIsSet )
    throw InvalidParameterException( "NoDataValue not set" );

  return noval;

}

string MapFormat::getProjection() const {
  if ( !projIsSet )
    throw InvalidParameterException( "Projection not set" );

  return proj;

}

