/**
 * Declaration of MapFormat class.
 * 
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

#include <openmodeller/MapFormat.hh>
#include <openmodeller/env_io/Map.hh>
#include <openmodeller/env_io/RasterFactory.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/Log.hh>

using std::string;

#define DEFAULT_FORMAT ByteHFA

MapFormat::MapFormat( ) :
  format( DEFAULT_FORMAT ),
  xcel( 0.0 ),
  xcelIsSet( false ),
  ycel( 0.0 ),
  ycelIsSet( false ),
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

MapFormat::MapFormat( Coord xcel, Coord ycel, Coord xmin, Coord ymin,
		      Coord xmax, Coord ymax, Scalar noval, char const * proj ) :
  format( DEFAULT_FORMAT ),
  xcel( xcel ),
  xcelIsSet( true ),
  ycel( ycel ),
  ycelIsSet( true ),
  xmin( xmin ),
  xminIsSet( true ),
  ymin( ymin ),
  yminIsSet( true ),
  xmax( xmax ),
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
  Raster* r = RasterFactory::instance().create( filenameWithFormat );

  Header h = r->header();

  setXCel( h.xcel );
  setYCel( h.ycel );
  setXMin( h.xmin );
  setYMin( h.ymin );
  setXMax( h.xmax );
  setYMax( h.ymax );
  setNoDataValue( h.noval );
  setProjection( h.proj );

  delete r;
}

void MapFormat::copyDefaults( const Map& map ) {

  Header h = map.getHeader();

  if ( ! xcelIsSet ) {

    Log::instance()->debug( "Copying cell width = %d\n", h.xcel );
    setXCel( h.xcel );
  }

  if ( ! ycelIsSet ) {

    Log::instance()->debug( "Copying cell height = %d\n", h.ycel );
    setYCel( h.ycel );
  }

  if ( ! xminIsSet ) {

    Log::instance()->debug( "Copying xmin = %f\n", h.xmin );
    setXMin( h.xmin );
  }

  if ( ! yminIsSet ) {

    Log::instance()->debug( "Copying ymin = %f\n", h.ymin );
    setYMin( h.ymin );
  }

  if ( ! xmaxIsSet ) {

    Log::instance()->debug( "Copying xmax = %f\n", h.xmax );
    setXMax( h.xmax );
  }

  if ( ! ymaxIsSet ) {

    Log::instance()->debug( "Copying ymax = %f\n", h.ymax );
    setYMax( h.ymax );
  }

  if ( ! projIsSet ) {

    Log::instance()->debug( "Copying projection\n" );
    setProjection( h.proj );
  }
}

MapFormat::~MapFormat()
{}

void MapFormat::setFormat( int f ) {

  format = f;

  if ( format < 0 || format > ByteASC ) {

    format = DEFAULT_FORMAT;
  }
}

void MapFormat::setFormat( std::string format ) {

  if ( format == "GreyTiff" ) {

    setFormat( GreyTiff );
  }
  else if ( format == "GreyTiff100" ) {

    setFormat( GreyTiff100 );
  }
  else if ( format == "FloatingTiff" ) {

    setFormat( FloatingTiff );
  }
  else if ( format == "GreyBMP" ) {

    setFormat( GreyBMP );
  }
  else if ( format == "FloatingHFA" ) {

    setFormat( FloatingHFA );
  }
  else if ( format == "ByteHFA" ) {

    setFormat( ByteHFA );
  }
  else if ( format == "ByteASC" ) {

    setFormat( ByteASC );
  }
  else {

    std::string msg = "Unknown map format: ";
    msg.append( format );
    msg.append( "\n" );

    Log::instance()->warn( msg.c_str() );
  }
}

void MapFormat::setXCel( Coord v ) {
  xcel = v;
  xcelIsSet = true;
}

void MapFormat::setYCel( Coord v ) {
  ycel = v;
  ycelIsSet = true;
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

  Coord xmin = getXMin();
  Coord xmax = getXMax();
  Coord xcel = getXCel();

  int width = static_cast<int>(  (xmax-xmin) / xcel +0.5);

  return width;
}

int MapFormat::getHeight() const {

  int height = static_cast<int>(  (ymax-ymin) / ycel +0.5);

  return height;
}

Coord MapFormat::getXCel() const {

  if ( ! xcelIsSet ) {

    std::string msg = "Cell width not set.\n";

    Log::instance()->error( msg.c_str() );

    throw OmException( msg );
  }

  return xcel;
}

Coord MapFormat::getYCel() const {

  if ( ! ycelIsSet ) {

    std::string msg = "Cell height not set.\n";

    Log::instance()->error( msg.c_str() );

    throw OmException( msg );
  }

  return ycel;
}

Coord MapFormat::getXMin() const {

  if ( ! xminIsSet ) {

    std::string msg = "XMin not set.\n";

    Log::instance()->error( msg.c_str() );

    throw OmException( msg );
  }

  return xmin;
}

Coord MapFormat::getYMin() const {

  if ( !yminIsSet ) {

    std::string msg = "Ymin not set.\n";

    Log::instance()->error( msg.c_str() );

    throw OmException( msg );
  }

  return ymin;
}

Coord MapFormat::getXMax() const {

  if ( ! xmaxIsSet ) {

    std::string msg = "Xmax not set.\n";

    Log::instance()->error( msg.c_str() );

    throw OmException( msg );
  }

  return xmax;
}

Coord MapFormat::getYMax() const {

  if ( ! ymaxIsSet ) {

    std::string msg = "Ymax not set.\n";

    Log::instance()->error( msg.c_str() );

    throw OmException( msg );
  }

  return ymax;
}

Scalar MapFormat::getNoDataValue() const {

  if ( ! novalIsSet ) {

    std::string msg = "NoDataValue not set.\n";

    Log::instance()->error( msg.c_str() );

    throw OmException( msg );
  }

  return noval;
}

string MapFormat::getProjection() const {

  if ( ! projIsSet ) {

    std::string msg = "Projection not set.\n";

    Log::instance()->error( msg.c_str() );

    throw OmException( msg );
  }

  return proj;
}

