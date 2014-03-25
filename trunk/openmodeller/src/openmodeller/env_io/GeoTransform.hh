/**
 * Declaration of GeoTransform class.
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

#ifndef _GEO_TRANSFHH_
#define _GEO_TRANSFHH_

#include <openmodeller/os_specific.hh>
#include <openmodeller/om_defs.hh>
#include <string>

class OGRCoordinateTransformation;


/****************************************************************/
/******************** Geographic Transformation *****************/

/** 
 * Perform geographic transformations between different coordinate
 * systems and projections.
 *
 * The "source" and "destination" coordinate systems and
 * theirs projections need to be specified using the WKT representation.
 * After that, one can transform points from "source" to "destination"
 * and vice versa.
 *
 * This class is a wrapper to the OGR library.
 *
 * @see http://www.remotesensing.org/gdal/ogr for OGR
 * @see http://gdal.velocet.ca/~warmerda/wktproblems.html for WKT
 */

/****************/
class dllexp GeoTransform
{
public:

  // 'dst' and 'src' must follow the WKT (Well Known Text) format.
  //
  // TODO: expand to use EPSG, ESRI or Proj4 formats.
  //
  GeoTransform();
  GeoTransform( const std::string& in, const std::string& out );
  ~GeoTransform();

  void change( const std::string& in, const std::string& out );
  void change();

  // From 'out' to 'in'.
  int transfIn( double *x, double *y ) const;
  int transfIn( double *x, double *y, double x0, double y0 ) const;

  // From 'in' to 'out'.
  int transfOut( double *x, double *y ) const;
  int transfOut( double *x, double *y, double x0, double y0 ) const;

  // Default coordinate system.
  static char const * getDefaultCS();

  // Compare two coordinate system strings (WKT).
  static bool compareCoordSystemStrings(char const * s1, char const * s2);

private:

  bool identity;

  OGRCoordinateTransformation *f_ctin;
  OGRCoordinateTransformation *f_ctout;

  // Disable copying.
  GeoTransform( const GeoTransform& );
  GeoTransform& operator=( const GeoTransform& );

};



#endif
