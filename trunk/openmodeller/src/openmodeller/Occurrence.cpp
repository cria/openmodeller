/**
 * Declaration of Occurrence class.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-25-02
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

#include <occurrence.hh>

#include <string.h>


/****************************************************************/
/************************** Occurrence **************************/

/******************/
/*** destructor ***/

Occurrence::~Occurrence()
{
  delete _attr;
}


/***********************/
/*** read Attributes ***/
int
Occurrence::readAttributes( Scalar *buffer )
{
  memcpy( buffer, _attr, _nattr * sizeof(Scalar) );
  return _nattr;
}


/************/
/*** init ***/
void
Occurrence::init( Coord x, Coord y, Scalar error,
		  Scalar abundance, int nattr, Scalar *attr )
{
  _x = x;
  _y = y;
  _error = error;

  _nattr = 1 + nattr;
  _attr  = new Scalar[_nattr];

  // The first attribute is the abundance.
  *_attr = abundance;

  if ( nattr && attr )
    memcpy( _attr + 1, attr, nattr * sizeof(Scalar) );
}

