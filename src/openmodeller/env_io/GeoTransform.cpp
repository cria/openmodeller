/**
 * Definition of GeoTransform class.
 * 
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

#include <openmodeller/env_io/GeoTransform.hh>
#include <openmodeller/om_defs.hh>
#include <openmodeller/Log.hh>

#include <openmodeller/Exceptions.hh>

#include <ogr_api.h>
#include <ogr_spatialref.h>
#include <cpl_error.h>

using std::string;


/****************************************************************/
/******************** Geographic Transformation *****************/

// Default spatial reference system.
//
#define OM_WGS84 "GEOGCS[\"WGS84\", DATUM[\"WGS84\", \
  SPHEROID[\"WGS84\", 6378137.0, 298.257223563]], \
  PRIMEM[\"Greenwich\", 0.0], \
  UNIT[\"degree\",0.017453292519943295], \
  AXIS[\"Longitude\",EAST], AXIS[\"Latitude\",NORTH]]"


void
errorHandler( CPLErr eErrClass, int err_no, const char *msg )
{
  Log::instance()->info( "Error: %s\n", msg );
}


/******************/
/*** construtor ***/

GeoTransform::GeoTransform( ) :
  identity( true ),
  f_ctin( 0 ),
  f_ctout( 0 )
{ 
   
}

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

  if ( src.importFromWkt( src_desc.c_str() ) != OGRERR_NONE ||
       dst.importFromWkt( dst_desc.c_str() ) != OGRERR_NONE )
    {
      std::string msg = "Invalid GeoTransform projection:\n src (";
      msg += src_desc.c_str();
      msg += ")\n dst (";
      msg += dst_desc.c_str();
      msg += ")\n.";
      Log::instance()->error( msg.c_str() );
      throw InvalidParameterException( msg );
    }

  f_ctin = OGRCreateCoordinateTransformation( &src, &dst );

  if ( ! f_ctin )
    {
      std::string msg = "Invalid GeoTransform projection:\n src (";
      msg += src_desc.c_str();
      msg += ")\n dst (";
      msg += dst_desc.c_str();
      msg += ")\n.";
      Log::instance()->error( msg.c_str() );
      throw InvalidParameterException( msg );
    }

  f_ctout = OGRCreateCoordinateTransformation( &dst, &src );

  if ( ! f_ctout )
    {
      std::string msg = "Invalid GeoTransform projection:\n src (";
      msg += src_desc.c_str();
      msg += ")\n dst (";
      msg += dst_desc.c_str();
      msg += ")\n.";
      Log::instance()->error( msg.c_str() );
      throw InvalidParameterException( msg );
    }

  // Deactivate GDAL error messages.
  CPLSetErrorHandler( (CPLErrorHandler) errorHandler );
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

/********************/
/*** getDefaultCS ***/
const char * GeoTransform::getDefaultCS()
{
  return OM_WGS84;
}


/*********************************/
/*** compareCoordSystemStrings ***/
bool GeoTransform::compareCoordSystemStrings(char const * s1, char const * s2)
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
