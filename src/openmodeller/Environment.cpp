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

#include <om_log.hh>
#include <env_io/map.hh>
#include <env_io/raster.hh>
#include <env_io/raster_file.hh>
#include <env_io/raster_mem.hh>
#include <env_io/geo_transform.hh>
#include <random.hh>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef HAVE_VALUES_H
#include <values.h>
#else
#include <float.h>
#define MAXFLOAT FLT_MAX
#endif

/*******************/
/*** string Copy ***/
void stringCopy( char **dst, char *src )
{
  if ( *dst )
    delete *dst;

  if ( src )
    {
      *dst = new char[1 + strlen( src )];
      strcpy( *dst, src );
    }
  else
    *dst = 0;
}

/****************************************************************/
/************************* Environment **************************/

/*******************/
/*** constructor ***/

Environment::Environment( char *cs, int ncateg, char **categs,
			  int nmap, char **maps, char *mask )
{
  _ncateg  = ncateg;
  _nlayers = ncateg + nmap;
  _layers  = 0;
  _mask    = 0;
  _cs = 0;

  _layerfiles = 0;
  _maskfile = 0;

  char ** currmap = maps;  
  char ** currcateg = categs;

  setCoordSystem( cs );

  // Reallocate vector that stores the names of the layers.
  if ( _layerfiles )
    delete[] _layerfiles;
  _layerfiles = new char*[_nlayers];

  // stringCopy() needs this.
  memset( _layerfiles, 0, _nlayers * sizeof(char *) );

  char **layers = _layerfiles;

  // Copy categorical maps.
  char **end = _layerfiles + _ncateg;
  while ( layers < end )
    stringCopy( layers++, *currcateg++ );

  // Copy continuos maps.
  end += nmap;
  while ( layers < end )
    stringCopy( layers++, *currmap++ );

  // Initialize mask and read its region.
  if ( ! mask )
    { _maskfile = 0; }
  else 
    {
      stringCopy(&_maskfile, mask);
      if ( ! (_mask = newMap( mask )) )
	{ g_log.error( 1, "Cannot read mask file '%s'.\n", mask ); }
    }

  _layers = 0;
  changeLayers( ncateg, categs, nmap, maps );
}


/******************/
/*** destructor ***/

Environment::~Environment()
{
  if ( _mask )
    delete _mask;

  if ( _layers )
    delete[] _layers;

  if ( _cs )
    delete _cs;
}


/*********************/
/*** change Layers ***/
int
Environment::changeLayers( int ncateg, char **categs, int nmap,
			   char **maps )
{
  if ( ! (_nlayers = ncateg + nmap) )
    return 0;

  // Reallocate vector that stores environmental layers.
  if ( _layers )
    delete[] _layers;
  Map **lay = _layers = new Map *[_nlayers];

  // Categorical maps.
  Map **end = lay + ncateg;
  while ( lay < end )
    *lay++ = newMap( *categs++, 1 );

  // Continuous maps.
  end = lay + nmap;
  while ( lay < end )
    *lay++ = newMap( *maps++ );


  if ( ! calcRegion() )
    g_log.warn( "Maps intersection is empty!!!\n" );

  return _nlayers;
}


/*****************/
/*** var Types ***/
int
Environment::varTypes( int *types )
{
  Map **lay = _layers;
  Map **end = lay + _nlayers;
  while ( lay < end )
    *types++ = (*lay++)->isCategorical();

  return _nlayers;
}


/*****************/
/*** normalize ***/
int
Environment::normalize( Scalar min, Scalar max )
{
  int n = 0;

  Map **lay = _layers;
  Map **end = lay + _nlayers;
  while ( lay < end )
    if ( (*lay++)->normalize( min, max ) )
      n++;

  return n;
}

int 
Environment::copyNormalizationParams( Environment * source )
{
  //TODO: should check whether order of layers and maybe file names match with original
  //TODO: as of now, no check is performed: layer order and value units must match
  int i;

  for (i = 0; i < _nlayers; i++)
    _layers[i]->copyNormalizationParams(source->_layers[i]);

  return _nlayers;
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
  Map **lay = _layers;
  Map **end = lay + _nlayers;
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
      x = rand( _xmin, _xmax );
      y = rand( _ymin, _ymax );

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
  if ( x < _xmin || x > _xmax || y < _ymin || y > _ymax )
    return 0;

  // If there's no mask, accept the point.
  if ( ! _mask )
    return 1;

  Scalar val;
  return _mask->get( x, y, &val ) && val;
}


/******************/
/*** get Region ***/
int
Environment::getRegion( Coord *xmin, Coord *ymin,
			Coord *xmax, Coord *ymax )
{
  *xmin = _xmin;
  *ymin = _ymin;
  *xmax = _xmax;
  *ymax = _ymax;

  return 1;
}


/************************/
/*** set Coord System ***/
void
Environment::setCoordSystem( char *cs )
{
  if ( _cs )
    delete _cs;

  int len = 1 + strlen(cs);
  _cs = new char[len];
  memcpy( _cs, cs, len );
}


/***************/
/*** new Map ***/
Map *
Environment::newMap( char *file, int categ )
{
  return new Map( new RasterFile( file, categ ), _cs, 1 );
  //  return new Map( new RasterMemory( file, categ ), _cs, 1 );
}


/*******************/
/*** calc Region ***/
int
Environment::calcRegion()
{
  Coord xmin, ymin, xmax, ymax;

  _xmin = _ymin =  MAXFLOAT;
  _xmax = _ymax = -MAXFLOAT;

  // The mask region is the default.
  if ( _mask )
    _mask->getRegion( &_xmin, &_ymin, &_xmax, &_ymax );


  // Crop region to fit all layers.
  Map **lay = _layers;
  Map **end = lay + _nlayers;
  while ( lay < end )
    {
      (*lay++)->getRegion( &xmin, &ymin, &xmax, &ymax );

      if ( xmin > _xmin )
	_xmin = xmin;
      
      if ( ymin > _ymin )
	_ymin = ymin;
      
      if ( xmax < _xmax )
	_xmax = xmax;
      
      if ( ymax < _ymax )
	_ymax = ymax;
    }

  return (_xmin < _xmax) && (_ymin < _ymax);
}

