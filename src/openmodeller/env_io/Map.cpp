/**
 * Definition of Map class.
 * 
 * @file
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-09-05
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

#include "env_io/map.hh"

#include <env_io/geo_transform.hh>
#include <env_io/raster.hh>
#include <log.hh>


// Debug
#include <string.h>


/****************************************************************/
/****************************** Map *****************************/

/******************/
/*** construtor ***/

Map::Map( Raster *rst, char *ocs, int del )
{
  if ( ! rst->header().hasProj() )
    _log.error( 1, "Not a georeferenced map!\n" );

  f_rst = rst;
  f_gt  = new GeoTransform( rst->header().proj, ocs );
  f_del = del;
}


/*****************/
/*** destrutor ***/

Map::~Map()
{
  delete f_gt;
  if ( f_del )
    delete f_rst;
}


/******************/
/*** getRegion  ***/
int
Map::getRegion( Coord *xmin, Coord *ymin, Coord *xmax,
                Coord *ymax)
{
  f_rst->getRegion( xmin, ymin, xmax, ymax );

  return
    f_gt->transfOut( xmin, ymin ) &&
    f_gt->transfOut( xmax, ymax );
}


/***********/
/*** get ***/
int
Map::get( Coord x, Coord y, Scalar *val )
{
  return f_gt->transfIn( &x, &y ) ? f_rst->get( x, y, val ) : 0;

  /*
  printf( "\nMap::get: (%f, %f)\n", x, y );
  if ( f_gt->transfIn( &x, &y ) )
    {
      printf( "transformou: (%f, %f)\n", x, y );
      int ret = f_rst->get( x, y, val );
      if ( ret )
	printf( "Leu: %f\n", val );
      else
	printf( "Não leu\n" );
      return ret;
    }
  else
    printf( "Não transformou\n" );

  return 0;
  */
}


/***********/
/*** put ***/
int
Map::put( Coord x, Coord y, Scalar *val )
{
  return f_gt->transfIn( &x, &y ) ? f_rst->put( x, y, val ) : 0;
}

