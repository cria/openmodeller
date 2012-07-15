/**
 * Definition of TerralibRaster class.
 * 
 * @author Alexandre Copertino Jardim <alexcj@dpi.inpe.br>
 * @date 2006-03-21
 * $Id$ 
 * 
 *
 * LICENSE INFORMATION
 * 
 * Copyright © 2006 INPE
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


#ifndef _TE_RASTERHH_
#define _TE_RASTERHH_

#include <openmodeller/env_io/Raster.hh>

class TeRaster;
class TeDatabase;
class TeLayer;
class TeRasterParams;
class TeStringParser;

#include <string>
using std::string;

class MapFormat;

//! Suport for a TerraLib Raster.
/**
* Manages raster files with TerraLib http://www.dpi.inpe.br/terralib/index.php .
*/
class dllexp TerralibRaster: public Raster
{
public:
	//! Return a new instance of TerralibRaster.
	/**
	* RasterFactory calls this function to build a new Raster.
	*/
	static Raster* CreateRasterCallback();
	
	//! Empty Constructor.
	/**
	* RasterFactory need a empty constructor.
	*/
	TerralibRaster(): db_(0), raster_(0), layer_(0), params_(0), te_str_parser_(0) {};

	//! Open an existing raster file or a raster in a TerraLib database (read only).
	/** 
	* \param str TerraLib string pointing to the raster.
	* \param categ if != 0 this is a categorical map (ie it can't be interpolated). Othewise this is a continuos map.
	*
	* Needed by RasterFactory..
	*/
	void createRaster( const std::string& str, int categ = 0 );

	//! Create a new file for projections.
	/**
	* \param str TerraLib string pointing to the raster.
	* \param format It is the output format specification.
	*
	* Needed by RasterFactory.
	*/
	void createRaster( const std::string& str, const MapFormat& format );

	/**
	* Destructor
	*/
	~TerralibRaster();

	//! Fills '*val' with the map value at (x,y).
	/**
	* Returns zero if (x,y) is out of range.
	*/
	int get( Coord x, Coord y, Scalar *val );

	//! Put '*val' at the (x,y) coordinate.
	/**
	* Returns 0 if (x,y) is out of range or the map is read only.
	* supports only single band output files.
	*/
	int put( Coord x, Coord y, Scalar val );
	
	//! Put 'no data val' at the (x,y) coordinate.
	/**
	* Returns 0 if (x,y) is out of range or the map is read only.
	* supports only single band output files.
	*/
	int put( Coord x, Coord y );

	/** Finds the minimum and maximum values in the first band. 
	* @param min Pointer to minimum value
	* @param max Pointer to maximum value
	* @return 1 if values are present, 0 otherwise
	*/
	int getMinMax( Scalar *min, Scalar *max );

	/** Deletes the associated raster. 
	* @return 1 if the raster was successfully deleted, 0 otherwise
	*/
	int deleteRaster();
	
private:
	/**
	* TerraLib DataBase Parameters
	*/
	//! Pointer to database connection.
	TeDatabase *db_;
	TeRaster *raster_;
	TeRasterParams* params_; 
	TeLayer *layer_;

	//! TerraLib DataBase string parser.
	TeStringParser *te_str_parser_;

	//! Open an existing TeRaster in a database or file disk.
	void openTeRaster();

	//! Create a new TeRaster in a database or file disk.
	void createTeRaster();
};

#endif
