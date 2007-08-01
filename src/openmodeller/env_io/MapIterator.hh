/**
 * Declaration of Map Iterator class.
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

#ifndef MAP_ITER_HH
#define MAP_ITER_HH

#include <openmodeller/om_defs.hh>

// For pair
#include <utility>

#include <openmodeller/env_io/Header.hh>
class GeoTransform;

class dllexp MapIterator {

  friend dllexp bool operator == ( const MapIterator&, const MapIterator& );
  friend dllexp bool operator != ( const MapIterator&, const MapIterator& );

public:
  // Default constructor create a Terminal object which can be used in
  // while (miter != MapIterator() {} loops.
  MapIterator();

  MapIterator( const Header& h, GeoTransform* gt );

  // Compiler generated copy constructor, assignment operator and destructor
  // are ok.
  
  // returns the current position in geo coordinates.
  // first is x, second is y.
  std::pair<Coord,Coord> operator*() const;

  // prefix operator++
  MapIterator& operator++();

  // prefix operator--
  MapIterator& operator--();

  // postfix operator++
  MapIterator operator++( int );

  // postfix operator--
  MapIterator operator--( int );

private:
  bool isPastBounds() const;
  // If isTerminal is true, then this MapIterator object
  // represents the terminal condition.
  bool isTerminal;

  GeoTransform *gt;
  Header h;
  int xcur;
  int ycur;
};

#endif
