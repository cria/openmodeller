/**
 * Declaration of MapFormat class.
 * 
 * @file
 * @author Ricardo Scachetti Pereira <rpereira@ku.edu>
 * @date 2004-05-27
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

#ifndef _OM_MAPFORMATHH_
#define _OM_MAPFORMATHH_

#include <string.h>
#include <om_defs.hh>
#include <env_io/header.hh>

/****************************************************************/
/************************* MapFormat ****************************/

/**
 * Defines the format of the output projection map, i.e., its
 * extent (coordinates), width, height, novalue data and projection
 */
class MapFormat
{
public:

  // TODO: Add default constructor when sets and gets are implemented
  //       Right now default constructor is useless

  MapFormat(int xd, int yd, Coord xm, Coord ym,
	    Coord xM, Coord yM, Scalar nv, char * proj);
  ~MapFormat();

  // TODO: create set methods (acessors)

  int getWidth()          { return _xdim; }
  int getHeight()         { return _ydim; }
  Coord getXMin()         { return _xmin; }
  Coord getYMin()         { return _ymin; }
  Coord getXMax()         { return _xmax; }
  Coord getYMax()         { return _ymax; }
  Scalar getNoDataValue() { return _noval; }
  char * getProjection()  { return _proj; }


private:
  int _xdim;    /**< Map width **/
  int _ydim;    /**< Map height **/
  Coord _xmin;  /**< Lowest longitude **/
  Coord _ymin;  /**< Lowest latitude **/
  Coord _xmax;  /**< Highest longitude **/
  Coord _ymax;  /**< Highest latitude **/
  Coord _xcel;  /**< Cell width **/
  Coord _ycel;  /**< Cell hight **/

  Scalar _noval; /**< Value indicating absence of information. **/

  //TODO: Perhaps DataType is required, but need to figure out a way
  //       for the user to represent it.
  //DataType dtype; /**< File data type. */

  char *_proj;  /**< Projection specification (in WKT). */
};


#endif
