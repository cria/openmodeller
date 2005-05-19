/**
 * Definition of RasterGdal class.
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

#include <env_io/raster_gdal.hh>
#include <env_io/raster.hh>
#include <env_io/geo_transform.hh>
#include <om_log.hh>

#include <Exceptions.hh>

#include <gdal.h>
#include <gdal_priv.h>
#include <cpl_string.h>

#include <string.h>

using std::string;
using std::vector;

struct {
  char const *GDalDriverName;
  GDALDataType dataType;
  bool hasMeta;
}
Formats[3] = {
  // Floating GeoTiff
  { "GTiff",
    (sizeof(Scalar) == 4) ? GDT_Float32 : GDT_Float64,
    true },
  // Greyscale GeoTiff
  { "GTiff",
    GDT_Byte,
    true },
  // Greyscale BMP
  { "BMP",
    GDT_Byte,
    false }
};

static const GDALDataType f_type = (sizeof(Scalar) == 4) ? GDT_Float32 : GDT_Float64;

/****************************************************************/
/************************** Raster Gdal *************************/

/*******************/
/*** constructor ***/

RasterGdal::RasterGdal( const string& file, char mode ) :
  f_name( file ),
  f_data(0),
  f_size(0),
  f_currentRow(-1),
  f_changed(0)
{
  initGdal();
  open( mode );
}

RasterGdal::RasterGdal( const string& file, int format, const Header& hdr ) :
  f_name( file ),
  f_hdr( hdr ),
  f_data(0),
  f_size(0),
  f_currentRow(-1),
  f_changed(0)
{
  initGdal();
  create( format );
}


/*****************/
/*** destructor ***/

RasterGdal::~RasterGdal()
{
  // Save the last line read, if needed.
  saveRow();

  if ( f_data )
    delete [] f_data;

  GDALClose( f_ds );

}


/************/
/*** read ***/
void
RasterGdal::read( Scalar *buf, int frow, int nrow )
{
  // Header's data.
  int nband = f_hdr.nband;
  int size  = f_size * nrow;

  // Read each band
  for ( int i = 1; i <= nband; i++, buf += size )
    {
      // Gets the i-th band.
      GDALRasterBand *band = f_ds->GetRasterBand( i );

      // Fills 'buf' with new data.
      band->RasterIO( GF_Read, 0, frow, f_size, nrow,
                      buf, f_size, nrow,
		      f_type, 0, 0 );
    }

}


/*************/
/*** write ***/
void
RasterGdal::write( Scalar *buf, int frow, int nrow )
{
  // Header's data.
  int nband = f_hdr.nband;
  int size  = f_size * nrow;
  
  // Write each band in the file.
  for ( int i = 1; i <= nband; i++, buf += size ) {
    // Gets the i-th band.
    GDALRasterBand *band = f_ds->GetRasterBand( i );
    
    // Write the buffer's data in the file.
    int ret = band->RasterIO( GF_Write, 0, frow, f_size, nrow,
			      buf, f_size, nrow,
			      f_type, 0, 0 );
    
    if ( ret == CE_Failure ) {
      g_log.warn( "Unable to write to file %s\n", f_name.c_str());
      throw FileIOException( "Unable to write to file " + f_name, f_name );
      }
  }

}

/************/
/*** open ***/
void
RasterGdal::open( char mode )
{
  // Mode: write or read.
  GDALAccess gmod = (mode == 'w') ? GA_Update : GA_ReadOnly;

  // Opens the file.
  f_ds = (GDALDataset *)GDALOpen( f_name.c_str(), gmod );
  if ( ! f_ds ) {
    g_log.warn( "Unable to open file %s\n", f_name.c_str());
    throw FileIOException( "Unable to open file " + f_name, f_name );
  }

  // Number of bands (channels).
  f_hdr.nband = f_ds->GetRasterCount();

  // Projection.
  f_hdr.setProj( (char *) f_ds->GetProjectionRef() );
  if ( ! f_hdr.hasProj() ) {
    g_log.warn( "The raster %s is not georeferenced.  Assuming WGS84\n", f_name.c_str() );
    f_hdr.setProj( GeoTransform::cs_default );
  }
    


  // Assumes that all bands have the same georeference
  // characteristics, ie, the same header.
  //

  GDALRasterBand *band = f_ds->GetRasterBand( 1 );

  int xd = f_hdr.xdim = band->GetXSize();
  int yd = f_hdr.ydim = band->GetYSize();

  f_hdr.noval = band->GetNoDataValue();


  // Map limits.
  double cf[6];
  f_ds->GetGeoTransform( cf );
  f_hdr.xmin = (Scalar) cf[0];
  f_hdr.xmax = (Scalar) (cf[0] + xd * cf[1] + yd * cf[2]);
  f_hdr.ymax = (Scalar) cf[3];
  f_hdr.ymin = (Scalar) (cf[3] + xd * cf[4] + yd * cf[5]);

  f_hdr.calculateCell();

  for( int i=0; i<6; ++i ) {
    f_hdr.gt[i] = cf[i];
  }

  // Minimum and maximum values.
  f_hdr.min = band->GetMinimum( &f_hdr.minmax );
  if ( f_hdr.minmax )
    f_hdr.max = band->GetMaximum();

  // Assumes grid (in place of 'pixel').
  // todo: how can I read this with GDAL?
  f_hdr.grid = 0;

  // Initialize the Buffer
  initBuffer();
}


/**************/
/*** create ***/
void
RasterGdal::create(int format)
{
  //
  GDALDriver *poDriver;
  char **papszMetadata; 
  GDALAllRegister();

  char const *fmt = Formats[ format ].GDalDriverName;

  // Added by Tim to see if the chosen format supports the gdal create method
  poDriver = GetGDALDriverManager()->GetDriverByName(fmt);

  if( poDriver == NULL ) {
    throw FileIOException( "Unable to load graphics driver " + string(fmt), f_name );
  }

  papszMetadata = poDriver->GetMetadata();
  if( ! CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) ) {
    g_log.warn( "Driver %s, format %s  DOES NOT support Create() method.\n", 
		poDriver->GetDescription(),
		fmt
		);
    throw FileIOException( "Driver " + string(fmt)
			   + " does not support Create() method", f_name );
  }

  // 
  // Read the parameters in 'hdr' used to create the file.
  //
  // The other parameters come from the copied header.
  f_hdr.nband = 1;

  // Create the file.
  f_ds = poDriver->Create( f_name.c_str(),
			   f_hdr.xdim, f_hdr.ydim,
			   f_hdr.nband,
			   /* data type */ Formats[format].dataType,
			   /* opt parameters */ NULL );
  if ( ! f_ds ) {
    g_log.warn( "Unable to create file %s.\n",f_name.c_str() );
    throw FileIOException( "Unable to create file " + f_name, f_name );
  }


  if ( Formats[format].hasMeta ) {
    /*** Metadata ***/

    // Limits (without rotations).
    f_ds->SetGeoTransform( f_hdr.gt );
    
    // Projection.
    f_ds->SetProjection( f_hdr.proj.c_str() );

    // Sets the "nodata" value in all bands.
    f_ds->GetRasterBand(1)->SetNoDataValue( f_hdr.noval );
  }

  // Here we fill the raster with novals.  So we don't need to worry about
  // having the projector actually cover every pixel.
  Scalar * buffer;
  buffer = new Scalar[f_hdr.xdim];
  for( int x=0;x<f_hdr.xdim;x++ )
    buffer[x] = f_hdr.noval;

  GDALRasterBand *band = f_ds->GetRasterBand(1);
  for( int y=0;y<f_hdr.ydim;y++ ) {
    band->RasterIO( GF_Write, 0, y, f_hdr.xdim, 1,
		    buffer, f_hdr.xdim, 1,
		    f_type, 0, 0 );
  }

  // Initialize the Buffer
  initBuffer();

  delete[] buffer;
}

void
RasterGdal::initBuffer() {
  //
  // Initialize the buffer.
  f_size = f_hdr.xdim;
  if ( f_data ) {
    delete [] f_data;
  }
  f_data = new Scalar[ f_size * f_hdr.nband ];

  f_changed = 0;
  f_currentRow = -1;
}

/*****************/
/*** init Gdal ***/
void
RasterGdal::initGdal()
{
  static int first = 1;

  if ( first )
    {
      first = 0;
      GDALAllRegister();
    }
}


/************/
/*** iput ***/
int
RasterGdal::iput( int x, int y, Scalar val )
{
  // Be sure that 'y' line is in the buffer 'f_data'.
  loadRow( y );

  // Put values in the first band of (x,y) position.
  f_data[x] = val;

  // Indicates the line 'f_last' has changed.
  f_changed = 1;

  return 1;
}

/************/
/*** iget ***/
int
RasterGdal::iget( int x, int y, Scalar *val )
{
  // Be sure that 'y' line is in the buffer 'f_data'.
  loadRow( y );

  // Get all band's values.
  Scalar *pv = val;
  int nband = f_hdr.nband;
  for ( int i = 0; i < nband; i++, x += f_size )
    if ( (*pv++ = f_data[x]) == f_hdr.noval )
      return 0;

  return 1;
}


/****************/
/*** load Row ***/
void
RasterGdal::loadRow( int row )
{
  // If the line is already read, return.
  if ( row == f_currentRow )
    return;

  saveRow();

  read( f_data, row, 1 );

  f_currentRow = row;
  f_changed = 0;

}


/****************/
/*** save Row ***/
void
RasterGdal::saveRow()
{
  if ( ! f_changed || f_currentRow < 0)
    return;

  write( f_data, f_currentRow, 1 );

  f_changed = 0;
}
