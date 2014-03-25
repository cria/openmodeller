/**
 * Declarations of Raster and RasterFormat classes.
 * 
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-08-22
 * @author Alexandre Copertino Jardim <alexcj@dpi.inpe.br>
 * @date 2006-03-21
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

#ifndef _RASTERHH_
#define _RASTERHH_

#include <openmodeller/om_defs.hh>
#include <openmodeller/env_io/Header.hh>

#include <string>

class MapFormat;
class Map;

/****************************************************************/
/**************************** Raster ****************************/
//! A common interface to rasters.
class dllexp Raster
{
public:

    virtual ~Raster();
    
    /** Method to create a raster representation (needed by RasterFactory).
     * @param source A string pointing to a raster source (file name, URL, etc.)
     * @param categ Indicates if the raster is categorical or not
     */
    virtual void createRaster( const std::string& source, int categ = 0 ) = 0;

 #ifdef MPI_FOUND
    /**
     * Method to create a raster representation (needed by RasterFactory).
     * @param source A string pointing to a raster source (file name, URL, etc.)
     * @param file is the name of the output file
     * @param format is the output format specification.
     */
    virtual void createRaster( const std::string& output_file_source, const std::string& file, const MapFormat& format ) = 0;
 #else
    /** Method to create a raster representation (needed by RasterFactory).
     * @param source A string pointing to a raster source (file name, URL, etc.)
     * @param format Map format
     */
    virtual void createRaster( const std::string& source, const MapFormat& format )= 0;
#endif

    //virtual static Raster* CreateRasterCallback() { return 0; };

    /** Returns the header. */
    Header& header() { return f_hdr; }

    /** Returns not zero if this map is categorical. */
    int isCategorical() const { return f_hdr.categ; }

    /** Returns the lowest longitude. */
    Coord xMin() const { return f_hdr.xmin; }

    /** Returns the lowest latitude. */
    Coord yMin() const { return f_hdr.ymin; }

    /** Returns the highest longitude. */
    Coord xMax() const { return f_hdr.xmax; }

    /** Returns the highest latitude. */
    Coord yMax() const { return f_hdr.ymax; }

    /** Returns the longitudinal map dimension. */
    int dimX() const { return f_hdr.xdim; }

    /** Returns the latitudinal map dimension. */
    int dimY() const { return f_hdr.ydim; }

    /** Returns the longitudinal cell dimension. */
    Coord celX() const { return f_hdr.xcel; }

    /** Returns the latitudinal cell dimension. */
    Coord celY() const { return f_hdr.ycel; }

    /** Returns the "noval" value. */
    Scalar noVal() const { return f_hdr.noval; }

    /** Returns the number of bands. */
    int numBand() const { return f_hdr.nband; }

    /** Fills '*val' with the map value at (x,y).
     * @param px Longitude
     * @param py Latitude
     * @param val Value
     * @return zero if (x,y) is out of range.
     */
    virtual int get( Coord px, Coord py, Scalar *val ) = 0;

    /** Put 'val' at the (x,y) coordinate.
     * Supports only single band output files.
     * @param px Longitude
     * @param py Latitude
     * @param val Value
     * @return 0 if (x,y) is out of range or the map is read only.
     */
    virtual int put( Coord px, Coord py, Scalar val ) = 0;

    /** Put 'no data val' at the (x,y) coordinate. 
     * Supports only single band files.
     * @param px Longitude
     * @param py Latitude
     * @return 0 if (x,y) is out of range or the map is read only.
     */
    virtual int put( Coord px, Coord py ) = 0;

    /** Finds the minimum and maximum values in the first band. 
     * @param min Pointer to minimum value
     * @param max Pointer to maximum value
     * @return 1 if values are present, 0 otherwise
     */
    virtual int getMinMax( Scalar *min, Scalar *max ) = 0;

    /** Tells if the min and max have already been computed */
    bool hasMinMax() { return f_hdr.minmax > 0 ? true : false ; }

    /** Support external specification of min/max.
     * @param min Minimum value
     * @param max Maximum value
     */
    void setMinMax( Scalar min, Scalar max );

    /** 
     * Event that must be called to indicate when the projection is finished.
     */
    virtual void finish() {};

    /** Method to delete a raster.
     * @return 1 if the raster was successfully deleted, 0 otherwise.
     */
    virtual int deleteRaster()= 0;

    /** Indicates if the raster has a better way to carry out conversions from its
     *  own coordinate system to the standard system used by openModeller.
     *  @return true if raster has its own geotransform to convert to standard cs, false otherwise.
     */
    virtual bool hasCustomGeotransform() { return false; }

    /** Calculates the raster extent in openModeller's standard coordinate system. 
     *  IMPORTANT: Call this only if hasCustomGeotransform returns true.
     *  When interacting with Map objects, use Map.getExtent instead, which encapsulates
     *  a call to this method when necessary. For some projections, such as lambert azimuth 
     *  equal area, getting the raster extent through manual coordinate conversion, as was
     *  usually done in the Map.getExtent, can be problematic. This method provides a way 
     *  for raster implementations to better perform the task of calculating the extent.
     * @param xmin Pointer to minimum X value
     * @param ymin Pointer to minimum Y value
     * @param xmax Pointer to maximum X value
     * @param ymax Pointer to maximum Y value
     * @return 1 if conversion was successful, 0 otherwise.
     */
    virtual int getExtentInStandardCs( Coord *xmin, Coord *ymin, Coord *xmax, Coord *ymax ) { return 0; }

protected:

    Scalar f_scalefactor;  // used in projection put.

    std::string f_file;
    #ifdef MPI_FOUND
    std::string f_output_file;
    #endif

    // Raster header
    Header f_hdr;

    Raster() {}; // Abstract class.

    // Disable copying.
    //Raster( const Raster& );
    //Raster& operator=( const Raster& );
};

#endif

