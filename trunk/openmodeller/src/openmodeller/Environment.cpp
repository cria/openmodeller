/**
 * Definition of Environment class.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-03-13
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


#include <environment.hh>

#include <env_io/map.hh>
#include <env_io/raster.hh>
#include <env_io/raster_file.hh>
#include <env_io/raster_mem.hh>
#include <env_io/geo_transform.hh>
#include <random.hh>
#include <log.hh>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef WIN32
#include <float.h>
#define MAXFLOAT FLT_MAX
#else
#include <values.h>
#endif

/****************************************************************/
/************************* Environment **************************/

/*******************/
/*** constructor ***/

Environment::Environment( char *cs, int ncateg, char **categs,
			  int nmap, char **maps, char *mask )
{
  f_cs = 0;
  setCoordSystem( cs );

  // Initialize mask and read its region.
  if ( ! mask )
    f_mask = 0;

  else if ( ! (f_mask = newMap( mask )) )
    {
      _log.error(1, "Cannot read mask file '%s'.\n", mask );
    }

  f_layers = 0;
  changeLayers( ncateg, categs, nmap, maps );
}


/******************/
/*** destructor ***/

Environment::~Environment()
{
  if ( f_mask )
    delete f_mask;

  if ( f_layers )
    delete[] f_layers;

  if ( f_cs )
    delete f_cs;
}


/*********************/
/*** change Layers ***/
int
Environment::changeLayers( int ncateg, char **categs, int nmap,
			   char **maps )
{
  if ( ! (f_nlay = ncateg + nmap) )
    return 0;

  // Reallocate vector that stores environmental layers.
  if ( f_layers )
    delete[] f_layers;
  Map **lay = f_layers = new Map *[f_nlay];

  // Categorical maps.
  Map **end = lay + ncateg;
  while ( lay < end )
    *lay++ = newMap( *categs++, 1 );

  // Continuous maps.
  end = lay + nmap;
  while ( lay < end )
    *lay++ = newMap( *maps++ );


  if ( ! calcRegion() )
    _log.warn( "Maps intersection is empty!!!\n" );

  return f_nlay;
}


/*****************/
/*** var Types ***/
int
Environment::varTypes( int *types )
{
  Map **lay = f_layers;
  Map **end = lay + f_nlay;
  while ( lay < end )
    *types++ = (*lay++)->isCategorical();

  return f_nlay;
}


/*****************/
/*** normalize ***/
int
Environment::normalize( Scalar min, Scalar max )
{
  int n = 0;

  Map **lay = f_layers;
  Map **end = lay + f_nlay;
  while ( lay < end )
    if ( (*lay++)->normalize( min, max ) )
      n++;

  return n;
}


/***********/
/*** get ***/
int
Environment::get( Coord x, Coord y, Scalar *sample )
{
  // Make sure that (x,y) belong to a common region among all
  // layers and the mask, if possible.
  if ( ! check( x, y ) )
    return 0;

  // Read variables values from the layers.
  Map **lay = f_layers;
  Map **end = lay + f_nlay;
  while ( lay < end )
    if ( ! (*lay++)->get( x, y, sample++ ) )
      return 0;
  
  return 1;
}


/******************/
/*** get Random ***/
int
Environment::getRandom( Scalar *sample )
{
  Random rand;
  Coord x, y;

  do
    {
      x = rand( f_xmin, f_xmax );
      y = rand( f_ymin, f_ymax );

    } while ( ! get( x, y, sample ) );

  return 1;
}


/*************/
/*** check ***/
int
Environment::check( Coord x, Coord y )
{
  // Accept the point, regardless of mask, if
  // it falls in a common region among all layers.
  if ( x < f_xmin || x > f_xmax || y < f_ymin || y > f_ymax )
    return 0;

  // If there's no mask, accept the point.
  if ( ! f_mask )
    return 1;

  Scalar val;
  return f_mask->get( x, y, &val ) && val;
}


/******************/
/*** get Region ***/
int
Environment::getRegion( Coord *xmin, Coord *ymin,
			Coord *xmax, Coord *ymax )
{
  *xmin = f_xmin;
  *ymin = f_ymin;
  *xmax = f_xmax;
  *ymax = f_ymax;

  return 1;
}


/************************/
/*** set Coord System ***/
void
Environment::setCoordSystem( char *cs )
{
  if ( f_cs )
    delete f_cs;

  int len = 1 + strlen(cs);
  f_cs = new char[len];
  memcpy( f_cs, cs, len );
}


/***************/
/*** new Map ***/
Map *
Environment::newMap( char *file, int categ )
{
  return new Map( new RasterFile( file, categ ), f_cs, 1 );
  //  return new Map( new RasterMemory( file, categ ), f_cs, 1 );
}


/*******************/
/*** calc Region ***/
int
Environment::calcRegion()
{
  GeoTransform *gt;
  Coord xmin, ymin, xmax, ymax;

  f_xmin = f_ymin =  MAXFLOAT;
  f_xmax = f_ymax = -MAXFLOAT;

  // The mask region is the default.
  if ( f_mask )
    f_mask->getRegion( &f_xmin, &f_ymin, &f_xmax, &f_ymax );


  // Crop region to fit all layers.
  Map **lay = f_layers;
  Map **end = lay + f_nlay;
  while ( lay < end )
    {
      (*lay++)->getRegion( &xmin, &ymin, &xmax, &ymax );

      if ( xmin > f_xmin )
	f_xmin = xmin;
      
      if ( ymin > f_ymin )
	f_ymin = ymin;
      
      if ( xmax < f_xmax )
	f_xmax = xmax;
      
      if ( ymax < f_ymax )
	f_ymax = ymax;
    }

  return (f_xmin < f_xmax) && (f_ymin < f_ymax);
}

