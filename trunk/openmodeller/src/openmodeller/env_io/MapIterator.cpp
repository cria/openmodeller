/**
 * Definition of Map Iterator class
 * 
 * @file
 * @author Kevin Ruland <kruland@ku.edu>
 * @date 2005-05-19
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

#include <openmodeller/env_io/MapIterator.hh>
#include <openmodeller/env_io/Map.hh>
#include <openmodeller/env_io/GeoTransform.hh>

using std::pair;

// Default constructor for terminal
MapIterator::MapIterator() :
  isTerminal( true )
{ }

MapIterator::MapIterator( const Header& h,
			  GeoTransform* gt ) :
  isTerminal( false ),
  gt( gt ),
  h( h ),
  xcur( 0 ),
  ycur( 0 )
{}

pair<Coord,Coord> MapIterator::operator*() const
{
  pair<Coord,Coord> lonlat = h.convertXY2LonLat(xcur, ycur);
  gt->transfOut( &lonlat.first, &lonlat.second);
  return lonlat;
}

MapIterator& MapIterator::operator++()
{
  ++xcur;
  if (xcur >= h.xdim ) {
    ++ycur;
    xcur = 0;
  }
  return *this;
}

MapIterator& MapIterator::operator--()
{
  --xcur;
  if (xcur < 0 ) {
    --ycur;
    xcur = h.xdim-1;
  }
  return *this;
}

MapIterator MapIterator::operator++(int)
{
  MapIterator rval = *this;
  operator++();
  return rval;
}

MapIterator MapIterator::operator--(int)
{
  MapIterator rval = *this;
  operator--();
  return rval;
}

bool
MapIterator::isPastBounds() const
{

  //
  // Note, because the increment/decrement operators
  // manipulate x first then y the foolowing should be true:
  //   1.  xcur is in [xmin,xmax)
  //   2.  ycur is the true test for out of bounds.
  // Also, since I expect the majority of usage will be with operator++,
  // I expect the condition ycur>= ymax, to be the most likely test.
  return (ycur >= h.ydim) ||
    (ycur < 0) ||
    (xcur >= h.xdim) ||
    (xcur < 0 );

}

bool
operator==( const MapIterator& lhs, const MapIterator& rhs )
{

  if ( lhs.isTerminal ) {
    return rhs.isPastBounds();
  }
  if ( rhs.isTerminal ) {
    return lhs.isPastBounds();
  }

  return (lhs.xcur == rhs.xcur) && (lhs.ycur == rhs.ycur);

}

bool
operator!=( const MapIterator& lhs, const MapIterator& rhs )
{
  return ! (lhs == rhs );
}

#ifdef MPI_FOUND
//new method for the parallel version
void MapIterator::nextblock(int init_pixel)
{



  ycur=init_pixel/h.xdim;
  xcur=init_pixel%h.xdim;



}
#endif
