/**
 * Definition of GeoTransform class.
 * 
 * @file
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-08-22
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

#include "env_io/geo_transform.hh"
#include <om_defs.hh>

#include <ogr_api.h>
#include <ogr_spatialref.h>
#include <cpl_error.h>


/****************************************************************/
/******************** Geographic Transformation *****************/

char *GeoTransform::cs_default = OM_COORDINATE_SYSTEM;

void
errorHandler( CPLErr eErrClass, int err_no, const char *msg )
{
  printf( "Error: %s\n", msg );
}


/******************/
/*** construtor ***/

GeoTransform::GeoTransform( char *dst_desc, char *src_desc )
{
  OGRSpatialReference src, dst;
 
  if ( src.importFromWkt( &src_desc ) != OGRERR_NONE ||
       dst.importFromWkt( &dst_desc ) != OGRERR_NONE )
    {
      printf("GeoTransform::GeoTransform - invalid projection:\n");
      printf( " src: (%s)\n", src_desc );
      printf( " dst: (%s)\n", dst_desc );
      exit( 1 );
    }

  f_ctin = OGRCreateCoordinateTransformation( &src, &dst );

  if ( ! f_ctin )
    {
      printf("GeoTransform::GeoTransform - invalid projection.\n");
      printf("De:\n" );
      printf("*  %s\n", src_desc );
      printf("Para:\n" );
      printf("*  %s\n", dst_desc );
      exit( 1 );
    }

  f_ctout = OGRCreateCoordinateTransformation( &dst, &src );

  if ( ! f_ctout )
    {
      printf("GeoTransform::GeoTransform - invalid projection.\n");
      printf("De:\n" );
      printf("*  %s\n", dst_desc );
      printf("Para:\n" );
      printf("*  %s\n", src_desc );
      exit( 1 );
    }

  // Deactivate GDAL error messages.
  CPLSetErrorHandler( errorHandler );
}


/*****************/
/*** destrutor ***/

GeoTransform::~GeoTransform()
{
  delete f_ctin;
  delete f_ctout;
}


/*****************/
/*** transf In ***/
int
GeoTransform::transfIn( float *x, float *y )
{
  double px = *x;
  double py = *y;

  if ( ! f_ctin->Transform( 1, &px, &py ) )
    return 0;

  *x = px;
  *y = py;
  return 1;
}


/*****************/
/*** transf In ***/
int
GeoTransform::transfIn( float *x, float *y,
			double x0, double y0 )
{
  double px = x0;
  double py = y0;

  if ( ! f_ctin->Transform( 1, &px, &py ) )
    return 0;

  *x = px;
  *y = py;
  return 1;
}


/*****************/
/*** transf In ***/
int
GeoTransform::transfIn( double *x, double *y )
{
  return f_ctin->Transform( 1, x, y );
}


/*****************/
/*** transf In ***/
int
GeoTransform::transfIn( double *x, double *y,
			double x0, double y0 )
{
  *x = x0;
  *y = y0;

  return f_ctin->Transform( 1, x, y );
}


/******************/
/*** transf Out ***/
int
GeoTransform::transfOut( float *x, float *y )
{
  double px = *x;
  double py = *y;

  if ( ! f_ctout->Transform( 1, &px, &py ) )
    return 0;

  *x = px;
  *y = py;
  return 1;
}


/******************/
/*** transf Out ***/
int
GeoTransform::transfOut( float *x, float *y,
			double x0, double y0 )
{
  double px = x0;
  double py = y0;

  if ( ! f_ctout->Transform( 1, &px, &py ) )
    return 0;

  *x = px;
  *y = py;
  return 1;
}


/******************/
/*** transf Out ***/
int
GeoTransform::transfOut( double *x, double *y )
{
  return f_ctout->Transform( 1, x, y );
}


/******************/
/*** transf Out ***/
int
GeoTransform::transfOut( double *x, double *y,
			 double x0, double y0 )
{
  *x = x0;
  *y = y0;

  return f_ctout->Transform( 1, x, y );
}
