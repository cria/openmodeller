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
#include <om_log.hh>

#include <gdal.h>
#include <gdal_priv.h>

#include <string.h>


/****************************************************************/
/************************** Raster Gdal *************************/

static char error[256];

// Used to translate Header::DataType to GDAL data types.
static GDALDataType g_dtype[Header::MaxDataType] = {
  GDT_Byte,
  GDT_Int16, GDT_UInt16,
  GDT_Int32, GDT_UInt32,
  GDT_Float32, GDT_Float64
};


/*******************/
/*** constructor ***/

RasterGdal::RasterGdal( char *file, char mode )
{
  initGdal();

  // Is 'Scalar' a float or a double?
  f_type = (sizeof(Scalar) == 4) ? GDT_Float32 : GDT_Float64;

  if ( ! open( file, mode ) )
    {
      sprintf( error, "RasterGdal: can't open file '%s'.\n",
               file );
      throw error;
    }
}

RasterGdal::RasterGdal( char *file, Header &hdr )
{
  initGdal();

  // Is 'Scalar' a float or a double?
  f_type = (sizeof(Scalar) == 4) ? GDT_Float32 : GDT_Float64;

  if ( ! create( file, hdr ) )
    {
      sprintf( error, "RasterGdal: can't create file '%s'.\n",
               file );
      throw error;
    }
}


/*****************/
/*** destrutor ***/

RasterGdal::~RasterGdal()
{
  GDALClose( f_ds );
}


/************/
/*** read ***/
int
RasterGdal::read( Scalar *buf, int frow, int nrow )
{
  // Header's data.
  int nband = f_hdr.nband;
  int xdim  = f_hdr.xdim; // Elements in a row (by band).
  int size  = xdim * nrow;

  // Read each band
  for ( int i = 1; i <= nband; i++, buf += size )
    {
      // Gets the i-th band.
      GDALRasterBand *band = f_ds->GetRasterBand( i );

      // Fills 'buf' with new data.
      band->RasterIO( GF_Read, 0, frow, xdim, nrow,
                      buf, xdim, nrow,
		      (GDALDataType) f_type, 0, 0 );
    }

  return 1;
}


/*************/
/*** write ***/
int
RasterGdal::write( Scalar *buf, int frow, int nrow )
{
  // Header's data.
  int nband = f_hdr.nband;
  int xdim  = f_hdr.xdim; // Elements in a row (by band).
  int size  = xdim * nrow;

  // Write each band in the file.
  for ( int i = 1; i <= nband; i++, buf += size )
    {
      // Gets the i-th band.
      GDALRasterBand *band = f_ds->GetRasterBand( i );

      // Write the buffer's data in the file.
      int ret = band->RasterIO( GF_Write, 0, frow, xdim, nrow,
                                buf, xdim, nrow,
				(GDALDataType) f_type, 0, 0 );

      if ( ret == CE_Failure )
        return 0;
    }

  return 1;
}


/************/
/*** open ***/
int
RasterGdal::open( char *file, char mode )
{
  // Mode: write or read.
  GDALAccess gmod = (mode == 'w') ? GA_Update : GA_ReadOnly;

  // Opens the file.
  f_ds = (GDALDataset *)GDALOpen( file, gmod );
  if ( ! f_ds )
    return 0;

  // Number of bands (channels).
  f_hdr.nband = f_ds->GetRasterCount();

  // Projection.
  f_hdr.setProj( (char *) f_ds->GetProjectionRef() );
  if ( ! f_hdr.hasProj() )
    _log.warn( "The map %s is not georeferenced!\n", file );


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

  // Minimum and maximum values.
  f_hdr.min = band->GetMinimum( &f_hdr.minmax );
  if ( f_hdr.minmax )
    f_hdr.max = band->GetMaximum();

  // Assumes grid (in place of 'pixel').
  // todo: how can I read this with GDAL?
  f_hdr.grid = 0;

  return 1;
}


/**************/
/*** create ***/
int
RasterGdal::create( char *file, Header &hdr )
{
  // Find GDAL particular string to describe the raster file type
  // to create.
  RasterFormat format( file );
  char *fmt = format.name();

  // GDAL's driver.
  GDALDriver *drv = GetGDALDriverManager()->GetDriverByName(fmt);
  if ( ! drv )
    {
      fprintf( stderr, "GDAL driver %s not found.\n", fmt );
      return 0;
    }

  // If the driver cannot support the Crete() method, returns 0.
  // => Why this does not work?!
  /*
  char **md = drv->GetMetadata();
  if ( CSLFetchBoolean( md, GDAL_DCAP_CREATE, FALSE ) )
    {
      printf( "GDAL driver %s cannot support Create().\n",
              format );
      return 0;
    }
  */

  // Read the parameters in 'hdr' used to create the file.
  //
  // todo: insert in header: xmin, ymin, xmax, ymax, noVal,
  //       grid, etc.
  //
  int xdim  = hdr.xdim;
  int ydim  = hdr.ydim;
  int nband = hdr.nband;

  // Find the GDAL data type to be used.
  Header::DataType dt = hdr.dtype;
  GDALDataType dtype = GDT_Byte;   // default.
  if ( ( dt >= 0 ) && ( dt < Header::MaxDataType ) )
    dtype = g_dtype[dt];

  // Create the file.
  f_ds = drv->Create( file, xdim, ydim, nband, dtype, NULL );
  if ( ! f_ds )
    {
      fprintf( stderr,
               "RasterMemory::saveGdal: error creating %s.\n",
               file );
      return 0;
    }
  f_hdr = hdr;


  /*** Metadata ***/

  // Limits (without rotations).
  double gt[6] = {
    hdr.xmin, hdr.xcel, 0.0, hdr.ymax, 0.0, hdr.ycel
  };
  f_ds->SetGeoTransform( gt );

  // Projection.
  f_ds->SetProjection( hdr.proj );

  // Sets the "nodata" value in all bands.
  for ( int b = 1; b <= nband; b++ )
    f_ds->GetRasterBand(b)->SetNoDataValue( hdr.noval );

  return 1;
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


