/**
 * Definition of GeoTransform class.
 * 
 * @file
 * @author Mauro E S Mu�oz <mauro@cria.org.br>
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

using std::string;

static bool compareCoordSystemStrings(char const * s1, char const * s2)
{
 int i = 0, j = 0;
 while (s1[i])
 {
  // skip spaces in both strings
  while (s1[i] == ' ') i++;
  while (s2[j] == ' ') j++; 

  if (s1[i] != s2[j]) {
    return false;
  }  

  if (s1[i]) i++;
  if (s2[j]) j++;
 }
 // skip trailing spaces that s2 might still have
 while (s2[j] == ' ') j++;

 // both s1[i] and s2[j] should be NULL to be equal
 return (s1[i] == s2[j]);
}


/****************************************************************/
/******************** Geographic Transformation *****************/

// Coordinate systems.
//
#define OM_WGS84 "GEOGCS[\"WGS84\", DATUM[\"WGS84\", \
  SPHEROID[\"WGS84\", 6378137.0, 298.257223563]], \
  PRIMEM[\"Greenwich\", 0.0], \
  UNIT[\"degree\",0.017453292519943295], \
  AXIS[\"Longitude\",EAST], AXIS[\"Latitude\",NORTH]]"

// Common Coordinate System for openModeller.
#define OM_COORDINATE_SYSTEM OM_WGS84

char const *GeoTransform::cs_default = OM_COORDINATE_SYSTEM;

void
errorHandler( CPLErr eErrClass, int err_no, const char *msg )
{
  g_log( "Error: %s\n", msg );
}


/******************/
/*** construtor ***/

GeoTransform::GeoTransform( ) :
  identity( true ),
  f_ctin( 0 ),
  f_ctout( 0 )
{ }

GeoTransform::GeoTransform( const string& dst_desc, const string& src_desc ) :
  identity( true ),
  f_ctin( 0 ),
  f_ctout( 0 )
{
  change( dst_desc, src_desc );
}

void 
GeoTransform::change()
{
  //
  // We need to change the geotransform.
  // Reset this to the identity - a nice sane state.
  identity = true;
  if (f_ctin)
    delete f_ctin;
  f_ctin = 0;
  if (f_ctout)
    delete f_ctout;
  f_ctout = 0;
}

void 
GeoTransform::change( const string& dst_desc, const string& src_desc )
{

  //
  // We need to change the geotransform.
  // Reset this to the identity - a nice sane state.
  identity = true;
  if (f_ctin)
    delete f_ctin;
  f_ctin = 0;
  if (f_ctout)
    delete f_ctout;
  f_ctout = 0;

  // If the two coordinate strings are the same, we need the identity transform.
  // So we can just return.
  if ( compareCoordSystemStrings( dst_desc.c_str(), src_desc.c_str() ) ) {
    return;
  }

  identity = false;

  OGRSpatialReference src, dst;

  // OGRSpatialReference::importFromWkt takes a char** as a parameter.
  // This parameter points to a variable pointing to the firs character
  // of the WKT.  The variable will be updated to point to the last
  // character of the WKT used.
  // Since pointer (dst_desc) are passed by value, we have our own
  // local copies and can safely take it's address and let importFromWkt
  // change it.
  // However, we still need to cast away const-ness :(
  char * src_desc_noconst = const_cast<char*>(src_desc.c_str());
  char * dst_desc_noconst = const_cast<char*>(dst_desc.c_str());

  if ( src.importFromWkt( &src_desc_noconst ) != OGRERR_NONE ||
       dst.importFromWkt( &dst_desc_noconst ) != OGRERR_NONE )
    {
      g_log.error( 1, "GeoTransform - invalid projection:\n src: (%s)\n dst: (%s)\n",
		   src_desc.c_str(), dst_desc.c_str() );
    }

  f_ctin = OGRCreateCoordinateTransformation( &src, &dst );

  if ( ! f_ctin )
    {
      g_log.error( 1, "GeoTransform - invalid projection:\n src: (%s)\n dst: (%s)\n",
		   src_desc.c_str(), dst_desc.c_str() );
    }

  f_ctout = OGRCreateCoordinateTransformation( &dst, &src );

  if ( ! f_ctout )
    {
      g_log.error( 1, "GeoTransform - invalid projection:\n src: (%s)\n dst: (%s)\n",
		   src_desc.c_str(), dst_desc.c_str() );
    }

  // Deactivate GDAL error messages.
  CPLSetErrorHandler( errorHandler );
}


/*****************/
/*** destrutor ***/

GeoTransform::~GeoTransform()
{
  if ( f_ctin )
    delete f_ctin;
  if ( f_ctout )
    delete f_ctout;
}


/*****************/
/*** transf In ***/
int
GeoTransform::transfIn( float *x, float *y ) const
{

  if ( identity )
    return 1;
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
			double x0, double y0 ) const
{
  *x = x0;
  *y = y0;

  if ( identity )
    return 1;

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
  return 1;

#endif
}


/*****************/
/*** transf In ***/
int
GeoTransform::transfIn( double *x, double *y ) const
{
  if ( identity )
    return 1;
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
			double x0, double y0 ) const
{
  *x = x0;
  *y = y0;
  if ( identity )
    return 1;

#ifndef GEO_TRANSFORMATIONS_OFF
  return f_ctin->Transform( 1, x, y );
#else
  return 1;
#endif
}


/******************/
/*** transf Out ***/
int
GeoTransform::transfOut( float *x, float *y ) const
{

  if ( identity )
    return 1;

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
			double x0, double y0 ) const
{
  *x = x0;
  *y = y0;
  if ( identity )
    return 1;
#ifndef GEO_TRANSFORMATIONS_OFF

  double px = x0;
  double py = y0;

  if ( ! f_ctout->Transform( 1, &px, &py ) )
    return 0;

  *x = px;
  *y = py;
  return 1;

#else

  return 1;

#endif
}


/******************/
/*** transf Out ***/
int
GeoTransform::transfOut( double *x, double *y ) const
{
  if ( identity )
    return 1;
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
                         double x0, double y0 ) const
{
  *x = x0;
  *y = y0;
  if ( identity )
    return 1;

#ifndef GEO_TRANSFORMATIONS_OFF
  return f_ctout->Transform( 1, x, y );
#else
  return 1;
#endif
}

