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

#include "map_format.hh"

#include <env_io/header.hh>


MapFormat::MapFormat( int xd, int yd, Coord xm, Coord ym,
		      Coord xM, Coord yM, Scalar nv, char * projection )
{
  _xdim  = xd;
  _ydim  = yd;
  _xmin  = xm;
  _ymin  = ym;
  _xmax  = xM;
  _ymax  = yM;
  _noval = nv;

  int len = (strlen(projection) + 1) * sizeof(char);
  _proj = new char[len];
  memcpy( _proj, projection, len );
}

MapFormat::~MapFormat()
{
  if (_proj)
    delete _proj;
}
