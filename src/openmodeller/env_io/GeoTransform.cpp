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
#include <om_log.hh>

#include <ogr_api.h>
#include <ogr_spatialref.h>
#include <cpl_error.h>


/****************************************************************/
/******************** Geographic Transformation *****************/

char *GeoTransform::cs_default = OM_COORDINATE_SYSTEM;

void
errorHandler( CPLErr eErrClass, int err_no, const char *msg )
{
  g_log( "Error: %s\n", msg );
}


/******************/
/*** construtor ***/

GeoTransform::GeoTransform( char *dst_desc, char *src_desc )
{
  OGRSpatialReference src, dst;
 
  if ( src.importFromWkt( &src_desc ) != OGRERR_NONE ||
       dst.importFromWkt( &dst_desc ) != OGRERR_NONE )
    {
      g_log.error( 1, "GeoTransform - invalid projection:\n src: (%s)\n dst: (%s)\n",
		   src_desc, dst_desc );
    }

  f_ctin = OGRCreateCoordinateTransformation( &src, &dst );

  if ( ! f_ctin )
    {
      g_log.error( 1, "GeoTransform - invalid projection:\n src: (%s)\n dst: (%s)\n",
		   src_desc, dst_desc );
    }

  f_ctout = OGRCreateCoordinateTransformation( &dst, &src );

  if ( ! f_ctout )
    {
      g_log.error( 1, "GeoTransform - invalid projection:\n src: (%s)\n dst: (%s)\n",
		   src_desc, dst_desc );
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
#ifndef GEO_TRANSFORMATIONS_OFF

  double px = *x;
  double py = *y;

  if ( ! f_ctin->Transform( 1, &px, &py ) )
    return 0;

  *x = px;
  *y = py;
  return 1;

#else

// if GeoTransformations are turned off, just leave the values alone
  return 1;

#endif
}


/*****************/
/*** transf In ***/
int
GeoTransform::transfIn( float *x, float *y,
			double x0, double y0 )
{
#ifndef GEO_TRANSFORMATIONS_OFF

  double px = x0;
  double py = y0;

  if ( ! f_ctin->Transform( 1, &px, &py ) )
    return 0;

  *x = px;
  *y = py;
  return 1;

#else

  // if GeoTransformations are turned off, just copy the values over
  *x = x0;
  *y = y0;
  return 1;

#endif
}


/*****************/
/*** transf In ***/
int
GeoTransform::transfIn( double *x, double *y )
{
#ifndef GEO_TRANSFORMATIONS_OFF
  return f_ctin->Transform( 1, x, y );
#else
  return 1;
#endif
}


/*****************/
/*** transf In ***/
int
GeoTransform::transfIn( double *x, double *y,
			double x0, double y0 )
{
  *x = x0;
  *y = y0;

#ifndef GEO_TRANSFORMATIONS_OFF
  return f_ctin->Transform( 1, x, y );
#else
  return 1;
#endif
}


/******************/
/*** transf Out ***/
int
GeoTransform::transfOut( float *x, float *y )
{
#ifndef GEO_TRANSFORMATIONS_OFF

  double px = *x;
  double py = *y;

  if ( ! f_ctout->Transform( 1, &px, &py ) )
    return 0;

  *x = px;
  *y = py;
  return 1;

#else

// if GeoTransformations are turned off, just leave the values alone
  return 1;

#endif
}


/******************/
/*** transf Out ***/
int
GeoTransform::transfOut( float *x, float *y,
			double x0, double y0 )
{
#ifndef GEO_TRANSFORMATIONS_OFF

  double px = x0;
  double py = y0;

  if ( ! f_ctout->Transform( 1, &px, &py ) )
    return 0;

  *x = px;
  *y = py;
  return 1;

#else

  *x = x0;
  *y = y0;
  return 1;

#endif
}


/******************/
/*** transf Out ***/
int
GeoTransform::transfOut( double *x, double *y )
{
#ifndef GEO_TRANSFORMATIONS_OFF
  return f_ctout->Transform( 1, x, y );
#else
  return 1;
#endif
}


/******************/
/*** transf Out ***/
int
GeoTransform::transfOut( double *x, double *y,
			 double x0, double y0 )
{
  *x = x0;
  *y = y0;

#ifndef GEO_TRANSFORMATIONS_OFF
  return f_ctout->Transform( 1, x, y );
#else
  return 1;
#endif
}
