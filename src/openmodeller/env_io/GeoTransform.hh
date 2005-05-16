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

#include <om_defs.hh>
#include <string>

class OGRCoordinateTransformation;


/****************************************************************/
/******************** Geographic Transformation *****************/

/** 
 * Do the geographic transformations between different coordinate
 * systems and projections.
 *
 * Basicaly the origem and target coordinate systems and
 * theirs projections are specified by its WKT representations.
 * After that one can transforms points from origem to target
 * and from target to origem.
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

  // 'dst' e 'src' devem seguir o mesmo padrão dentre:
  // WKT (Well Known Text)
  //
  // fazer: expandir para usar EPSG, ESRI ou Proj4.
  //
  GeoTransform();
  GeoTransform( const std::string& in, const std::string& out );
  ~GeoTransform();

  void change( const std::string& in, const std::string& out );
  void change();

  // From 'out' to 'in'.
  int transfIn( float *x, float *y ) const;
  int transfIn( float *x,  float *y,  double x0, double y0 ) const;
  int transfIn( double *x, double *y ) const;
  int transfIn( double *x, double *y, double x0, double y0 ) const;

  // From 'in' to 'out'.
  int transfOut( float *x, float *y ) const;
  int transfOut( float *x,  float *y,  double x0, double y0 ) const;
  int transfOut( double *x, double *y ) const;
  int transfOut( double *x, double *y, double x0, double y0 ) const;

  // Default coordinate system.
  static char const *cs_default;

private:

  bool identity;

  OGRCoordinateTransformation *f_ctin;
  OGRCoordinateTransformation *f_ctout;

  // Disable copying.
  GeoTransform( const GeoTransform& );
  GeoTransform& operator=( const GeoTransform& );

};



#endif
