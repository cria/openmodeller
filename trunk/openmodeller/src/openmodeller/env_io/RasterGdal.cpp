/**
 * Definition of RasterGdal class.
 * 
 * @file
 * @author Mauro E S Mu�oz <mauro@cria.org.br>
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

#include <openmodeller/env_io/RasterGdal.hh>
#include <openmodeller/env_io/Raster.hh>
#include <openmodeller/env_io/Map.hh>
#include <openmodeller/env_io/GeoTransform.hh>
#include <openmodeller/Log.hh>

#include <openmodeller/Exceptions.hh>

#include <openmodeller/Log.hh>

#include <openmodeller/MapFormat.hh>

#include <gdal.h>
#include <gdal_priv.h>
#include <cpl_string.h>

#include <string.h>

using std::string;
using std::vector;

#include <utility>
using std::pair;

struct GDAL_Format
{
  char const *GDalDriverName;
  GDALDataType dataType;
  bool hasMeta;
};

GDAL_Format Formats[5] =
{
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
    false },
  // Floating HFA (Erdas Imagine Format which ArcMap can read directly)
  { "HFA",
    GDT_Float32,
    true },
  // Byte HFA (Erdas Imagine Format which ArcMap can read directly)
  { "HFA",
    GDT_Byte,
    true }
};

static const GDALDataType f_type = (sizeof(Scalar) == 4) ? GDT_Float32 : GDT_Float64;

/****************************************************************/
/************************** Raster Gdal *************************/

/*******************/
/*** constructor ***/

RasterGdal::RasterGdal( const string& file, int categ ):
  f_data(0),
  f_size(0),
  f_currentRow(-1),
  f_changed(0)
{
  f_file = file;
  f_scalefactor = 1.0;

  // Opens read only and init the class attributes.
  initGdal();
  open( 'r' );

  // set categorical status.  It's not stored as part of
  // the gdal file's header.
  f_hdr.categ = categ;
}

RasterGdal::RasterGdal( const string& file, const MapFormat& format):
  f_data(0),
  f_size(0),
  f_currentRow(-1),
  f_changed(0)
{
  f_file = file;

  Scalar nv; 

  switch( format.getFormat() )
  {
    case MapFormat::GreyBMP:
      f_scalefactor = 255.0;
      nv = 0.0;
      g_log.debug( "Raster:: format set to MapFormat::GreyBMP:\n");
      break;
    case MapFormat::GreyTiff:
      f_scalefactor = 254.0;
      nv = 255.0;
      g_log.debug( "Raster:: format set to MapFormat::GreyTiff:\n");
      break;
    case MapFormat::FloatingTiff:
      f_scalefactor = 1.0;
      nv = -1.0;
      g_log.debug( "Raster:: format set to MapFormat::FloatingTiff:\n");
      break;
    case MapFormat::FloatingHFA:
      f_scalefactor = 1.0;
      nv = -1.0;
      g_log.debug( "Raster:: format set to MapFormat::FloatingHFA:\n");
      break;
    case MapFormat::ByteHFA:
      f_scalefactor = 100;
      nv = 0;
      g_log.debug( "Raster:: format set to MapFormat::ByteHFA:\n");
      break;
    default:
      throw GraphicsDriverException( "Unsupported output format" );
  }

  f_hdr = Header( format.getWidth(),
      format.getHeight(),
      format.getXMin(),
      format.getYMin(),
      format.getXMax(),
      format.getYMax(),
      nv,
      1,
      0 );

  f_hdr.setProj( format.getProjection() );

  // Create a new file in disk.
  initGdal();
  create( format.getFormat() );
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
  for ( int i = 1; i <= nband; i++, buf += size )
  {
    // Gets the i-th band.
    GDALRasterBand *band = f_ds->GetRasterBand( i );

    // Write the buffer's data in the file.
    int ret = band->RasterIO( GF_Write, 0, frow, f_size, nrow,
        buf, f_size, nrow,
        f_type, 0, 0 );

    if ( ret == CE_Failure )
    {
      g_log.warn( "Unable to write to file %s\n", f_file.c_str());
      throw FileIOException( "Unable to write to file " + f_file, f_file );
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
  f_ds = (GDALDataset *)GDALOpen( f_file.c_str(), gmod );
  if ( ! f_ds )
  {
    g_log.warn( "Unable to open file %s\n", f_file.c_str());
    throw FileIOException( "Unable to open file " + f_file, f_file );
  }

  // Number of bands (channels).
  f_hdr.nband = f_ds->GetRasterCount();

  // Projection.
  f_hdr.setProj( (char *) f_ds->GetProjectionRef() );
  if ( ! f_hdr.hasProj() )
  {
    g_log.warn( "The raster %s is not georeferenced.  Assuming WGS84\n", f_file.c_str() );
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

  for( int i=0; i<6; ++i ) 
  {
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

  if( poDriver == NULL )
  {
    throw FileIOException( "Unable to load graphics driver " + string(fmt), f_file );
  }

  papszMetadata = poDriver->GetMetadata();
  if( ! CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) )
  {
    g_log.warn( "Driver %s, format %s  DOES NOT support Create() method.\n", 
        poDriver->GetDescription(),
        fmt);

    throw FileIOException( "Driver " + string(fmt)
        + " does not support Create() method", f_file );
  }

  // 
  // Read the parameters in 'hdr' used to create the file.
  //
  // The other parameters come from the copied header.
  f_hdr.nband = 1;

  // Create the file.
  if (format==MapFormat::FloatingHFA || format==MapFormat::ByteHFA)
  {
    // HFA (erdas imagine) format does not support nodata
    // so we need to set the background value instead
    // also for this format we want to enable comressions to 
    // reduce the file size

    //see http://www.gdal.org/gdal_tutorial.html for options examples
    char **papszOptions = NULL;
    papszOptions = CSLSetNameValue( papszOptions, "BACKGROUND", "-1" );
    papszOptions = CSLSetNameValue( papszOptions, "COMPRESS", "NO" );
    f_ds = poDriver->Create( f_file.c_str(),
        f_hdr.xdim, f_hdr.ydim,
        f_hdr.nband,
        /* data type */ Formats[format].dataType,
        /* opt parameters */ papszOptions );
  }
  else
  {
    f_ds = poDriver->Create( f_file.c_str(),
        f_hdr.xdim, f_hdr.ydim,
        f_hdr.nband,
        /* data type */ Formats[format].dataType,
        /* opt parameters */ NULL );
  }
  if ( ! f_ds )
  {
    g_log.warn( "Unable to create file %s.\n",f_file.c_str() );
    throw FileIOException( "Unable to create file " + f_file, f_file );
  }


  if ( Formats[format].hasMeta )
  {
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
  for( int y=0;y<f_hdr.ydim;y++ )
  {
    band->RasterIO( GF_Write, 0, y, f_hdr.xdim, 1,
        buffer, f_hdr.xdim, 1,
        f_type, 0, 0 );
  }

  // Initialize the Buffer
  initBuffer();

  delete[] buffer;
}

  void
RasterGdal::initBuffer()
{
  //
  // Initialize the buffer.
  f_size = f_hdr.xdim;
  if ( f_data )
  {
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

/***********/
/*** get ***/
  int
RasterGdal::get( Coord px, Coord py, Scalar *val )
{
  Scalar *pv = val;
  for ( int i = 0; i < f_hdr.nband; i++ )
    *pv++ = f_hdr.noval;

  pair<int,int> xy = f_hdr.convertLonLat2XY( px, py );
  int x = xy.first;
  int y = xy.second;

  // If the point is out of range, returns 0.
  if ( x < 0 || x >= f_hdr.xdim || y < 0 || y >= f_hdr.ydim )
  {
    //g_log.debug( "Raster::get() Pixel (%d,%d) is not in extent\n",x,y);
    return 0;
  }

  // 'iget()' detects if the coordinate has or not valid
  // information (noval);
  return iget( x, y, val );
}

/***********/
/*** put ***/
  int
RasterGdal::put( Coord px, Coord py, Scalar val )
{
  pair<int,int> xy = f_hdr.convertLonLat2XY( px, py );
  int x = xy.first;
  int y = xy.second;

  if ( x < 0 || x >= f_hdr.xdim || y < 0 || y >= f_hdr.ydim )
    return 0;

  return iput( x, y, f_scalefactor*val );
}

/***********/
/*** put ***/
  int
RasterGdal::put( Coord px, Coord py  )
{
  pair<int,int> xy = f_hdr.convertLonLat2XY( px, py );
  int x = xy.first;
  int y = xy.second;

  Scalar val = f_hdr.noval;

  if ( x < 0 || x >= f_hdr.xdim || y < 0 || y >= f_hdr.ydim )
    return 0;

  return iput( x, y, val );
}

/*******************/
/*** get Min Max ***/
  int
RasterGdal::getMinMax( Scalar *min, Scalar *max )
{
  if ( ! calcMinMax() )
    return 0;

  *min = f_hdr.min;

  *max = f_hdr.max;

  return 1;
}

/********************/
/*** calc Min Max ***/
  int
RasterGdal::calcMinMax( int band )
{
  if ( f_hdr.minmax )
  {
    return 1;
  }
  Scalar *bands;
  bands = new Scalar[ f_hdr.nband ];
  Scalar *val = bands + band;
  Scalar min;
  Scalar max;

  bool initialized = false;

  // Scan all map values.
  for ( int y = 0; y < f_hdr.ydim; y++ )
    for ( int x = 0; x < f_hdr.xdim; x++ )
      if ( iget( x, y, bands ) )
      {
        if ( !initialized )
        {
          initialized = true;
          min = max = *val;
        }
        if ( min > *val )
          min = *val;
        else if ( max < *val )
          max = *val;
      }

  delete [] bands;

  if (!initialized)
    return 0;

  /*
     This is only logically const.  Here we cast away const to cache the min/max
     */
  Header *f_hdrNoConst = const_cast<Header*>(&f_hdr);

  f_hdrNoConst->minmax = 1;
  f_hdrNoConst->min = min;
  f_hdrNoConst->max = max;

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
