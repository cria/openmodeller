/**
 * Declaration of MapFormat class.
 * 
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

#include <string>

#include <openmodeller/os_specific.hh>
#include <openmodeller/om_defs.hh>

class Map;

/****************************************************************/
/************************* MapFormat ****************************/

/**
 * Defines the format of the output projection map, i.e., its
 * extent (coordinates), width, height, novalue data and projection
 */
class dllexp MapFormat
{
public:

  MapFormat();

  MapFormat(Coord xcel, Coord ycel, Coord xmin, Coord ymin,
	    Coord xmax, Coord ymax, Scalar noval, char const * proj);

  MapFormat( char const *filenameWithFormat );

  // Compiler generated copy constructor and copy assignment operator are fine for
  // this class.

  ~MapFormat();

  void copyDefaults( const Map& map );

  // This is the output format constants.
  enum { FloatingTiff = 0,
         GreyTiff = 1,
         GreyTiff100 = 2,
         GreyBMP = 3,
         FloatingHFA = 4, //erdas imagine floating point format
         ByteHFA = 5, //erdas imagine single byte format (scaled 0-100)
         ByteASC = 6, //Arc/Info ASCII Grid single byte format (scaled 0-100)
         FloatingASC = 7 //Arc/Info ASCII Grid floating point format
  };

  void unsetFormat() { format = FloatingTiff; }
  void unsetXCel()  { xcelIsSet = false; }
  void unsetYCel() { ycelIsSet = false; }
  void unsetXMin() { xminIsSet = false; }
  void unsetYMin() { yminIsSet = false; }
  void unsetXMax() { xmaxIsSet = false; }
  void unsetYMax() { ymaxIsSet = false; }
  void unsetNoDataValue() { novalIsSet = false; }
  void unsetProjection() { projIsSet = false; }

  void setFormat( int format );
  void setFormat( std::string format );
  void setXCel( Coord xcel );
  void setYCel( Coord ycel );
  void setXMin( Coord xmin );
  void setYMin( Coord ymin );
  void setXMax( Coord xmax );
  void setYMax( Coord ymax );
  void setNoDataValue( Scalar noval );
  void setProjection( const std::string& proj);

  int getFormat() const { return format; }
  int getWidth() const; /**< Computed width in pixels **/
  int getHeight() const; /**< Computed height in pixels **/
  Coord getXCel() const;
  Coord getYCel() const;
  Coord getXMin() const;
  Coord getYMin() const;
  Coord getXMax() const;
  Coord getYMax() const;
  Scalar getNoDataValue() const;
  std::string getProjection() const;

private:
  int format;

  Coord xcel;    /**< Map cell width **/
  bool xcelIsSet;

  Coord ycel;    /**< Map cell height **/
  bool ycelIsSet;

  Coord xmin;  /**< Lowest longitude **/
  bool xminIsSet;

  Coord ymin;  /**< Lowest latitude **/
  bool yminIsSet;

  Coord xmax;  /**< Highest longitude **/
  bool xmaxIsSet;

  Coord ymax;  /**< Highest latitude **/
  bool ymaxIsSet;

  Scalar noval; /**< Value indicating absence of information. **/
  bool novalIsSet;

  //TODO: Perhaps DataType is required, but need to figure out a way
  //       for the user to represent it.
  //DataType dtype; /**< File data type. */

  std::string proj;  /**< Projection specification (in WKT). */
  bool projIsSet;

};


#endif
