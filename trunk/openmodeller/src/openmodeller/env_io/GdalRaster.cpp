/**
 * Definition of GDalRaster class.
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

#include <openmodeller/env_io/GdalRaster.hh>
#include <openmodeller/env_io/Raster.hh>
#include <openmodeller/env_io/Map.hh>
#include <openmodeller/env_io/GeoTransform.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/MapFormat.hh>

#include <gdal_version.h>
#include <gdal.h>
#include <gdal_priv.h>
#include <cpl_string.h>

#include <string.h>

using std::string;
using std::vector;

#include <utility>
using std::pair;

#ifdef MPI_FOUND
#include "mpi.h"
#endif

struct GDAL_Format
{
  char const *GDalDriverName;
  GDALDataType dataType;
  bool hasMeta;
};

GDAL_Format Formats[8] =
{
  // Floating GeoTiff
  { "GTiff",
    (sizeof(Scalar) == 4) ? GDT_Float32 : GDT_Float64,
    true },
  // Greyscale GeoTiff scaled 0-254
  { "GTiff",
    GDT_Byte,
    true },
  // Greyscale GeoTiff scaled 0-100 
  { "GTiff",
    GDT_Byte,
    true },
  // Greyscale BMP
  { "BMP",
    GDT_Byte,
    false },
  // Floating HFA (Erdas Imagine Format)
  { "HFA",
    GDT_Float32,
    true },
  // Byte HFA (Erdas Imagine Format scaled 0-100)
  { "HFA",
    GDT_Byte,
    true },
  // Byte ASC (Arc/Info ASCII Grid Format scaled 0-100). 
  // The GDAL code here should be AAIGrid, but this driver does not support the Create 
  // method, so the trick is to create as HFA and then translate to AAIGrid
  { "HFA",
    GDT_Byte,
    true },
  // Floating ASC (Arc/Info ASCII Grid floating point format).
  // The GDAL code here should be AAIGrid, but this driver does not support the Create 
  // method, so the trick is to create as TIF and then translate to AAIGrid
  { "GTiff",
    (sizeof(Scalar) == 4) ? GDT_Float32 : GDT_Float64,
    true },
};

static const GDALDataType f_type = (sizeof(Scalar) == 4) ? GDT_Float32 : GDT_Float64;

#ifdef MPI_FOUND
 int rank = 0;
#endif

/****************************************************************/
/************************** Raster Gdal *************************/

/*********************/
/*** create Raster ***/
void
GdalRaster::createRaster( const string& file, int categ )
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

#ifdef MPI_FOUND
void
GdalRaster::createRaster( const string& output_file, const string& file, const MapFormat& format)
{
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  f_file = file;

  // will be utilized by MPI
  f_output_file = output_file;

  Scalar nv;

  switch( format.getFormat() )
  {
    case MapFormat::GreyBMP:
      f_scalefactor = 255.0;
      nv = 0.0;
      Log::instance()->debug( "GdalRaster format set to MapFormat::GreyBMP:\n" );
      break;
    case MapFormat::GreyTiff:
      f_scalefactor = 254.0;
      nv = 255.0;
      Log::instance()->debug( "GdalRaster format set to MapFormat::GreyTiff:\n" );
      break;
    case MapFormat::GreyTiff100:
      f_scalefactor = 100.0;
      nv = 127.0; //represented as 7bit so cant use 254
      Log::instance()->debug( "GdalRaster format set to MapFormat::GreyTiff100:\n" );
      break;
    case MapFormat::FloatingTiff:
      f_scalefactor = 1.0;
      nv = -1.0;
      Log::instance()->debug( "GdalRaster format set to MapFormat::FloatingTiff:\n" );
      break;
    case MapFormat::FloatingHFA:
      f_scalefactor = 1.0;
      nv = -1.0;
      Log::instance()->debug( "GdalRaster format set to MapFormat::FloatingHFA:\n" );
      break;
    case MapFormat::ByteHFA:
      f_scalefactor = 100;
      nv = 101;
      Log::instance()->debug( "GdalRaster format set to MapFormat::ByteHFA:\n" );
      break;
    case MapFormat::ByteASC:
      f_scalefactor = 100;
      nv = 101;
      Log::instance()->debug( "GdalRaster format set to MapFormat::ByteASC:\n" );
      break;
    case MapFormat::FloatingASC:
      f_scalefactor = 1.0;
      nv = -9999;
      Log::instance()->debug( "GdalRaster format set to MapFormat::FloatingASC:\n" );
      break;
    default:
      Log::instance()->error( "Unsupported output format.\n" );
      throw InvalidParameterException( "Unsupported output format" );
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

#else
void
GdalRaster::createRaster( const string& file, const MapFormat& format)
{
  f_file = file;

  Scalar nv; 

  switch( format.getFormat() )
  {
    case MapFormat::GreyBMP:
      f_scalefactor = 255.0;
      nv = 0.0;
      Log::instance()->debug( "GdalRaster format set to MapFormat::GreyBMP:\n" );
      break;
    case MapFormat::GreyTiff:
      f_scalefactor = 254.0;
      nv = 255.0;
      Log::instance()->debug( "GdalRaster format set to MapFormat::GreyTiff:\n" );
      break;
    case MapFormat::GreyTiff100:
      f_scalefactor = 100.0;
      nv = 127.0; //represented as 7bit so cant use 254
      Log::instance()->debug( "GdalRaster format set to MapFormat::GreyTiff100:\n" );
      break;
    case MapFormat::FloatingTiff:
      f_scalefactor = 1.0;
      nv = -1.0;
      Log::instance()->debug( "GdalRaster format set to MapFormat::FloatingTiff:\n" );
      break;
    case MapFormat::FloatingHFA:
      f_scalefactor = 1.0;
      nv = -1.0;
      Log::instance()->debug( "GdalRaster format set to MapFormat::FloatingHFA:\n" );
      break;
    case MapFormat::ByteHFA:
      f_scalefactor = 100;
      nv = 101;
      Log::instance()->debug( "GdalRaster format set to MapFormat::ByteHFA:\n" );
      break;
    case MapFormat::ByteASC:
      f_scalefactor = 100;
      nv = 101;
      Log::instance()->debug( "GdalRaster format set to MapFormat::ByteASC:\n" );
      break;
    case MapFormat::FloatingASC:
      f_scalefactor = 1.0;
      nv = -9999;
      Log::instance()->debug( "GdalRaster format set to MapFormat::FloatingASC:\n" );
      break;
    default:
      Log::instance()->error( "Unsupported output format.\n" );
      throw InvalidParameterException( "Unsupported output format" );
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
#endif

/*****************/
/*** destructor ***/
GdalRaster::~GdalRaster()
{
#ifdef MPI_FOUND
  if (((rank != 0) && (strcmp (f_file.c_str(), f_output_file.c_str()) != 0)) || (rank == 0)){
#endif
    // Save the last line read, if needed.
    saveRow();

    if ( f_data ) {

      delete[] f_data;
    }

    GDALClose( f_ds );
#ifdef MPI_FOUND
  }
#endif
}


/************/
/*** read ***/
void
GdalRaster::read( Scalar *buf, int frow, int nrow )
{
  // Header's data
  int nband = f_hdr.nband;
  int size  = f_size * nrow;

  // Read each band
  for ( int i = 1; i <= nband; i++, buf += size ) {

    // Gets the i-th band.
    GDALRasterBand *band = f_ds->GetRasterBand( i );

    // Fill 'buf' with new data
    int ret = band->RasterIO( GF_Read, 0, frow, f_size, nrow, buf, f_size, nrow, f_type, 0, 0 );

    if ( ret == CE_Failure ) {

      // Fill 'buf' with nodata
      for ( int j = 0 ; j < f_size; j++ ) {

        buf[j] = f_hdr.noval;
      }
    }
  }
}


/*************/
/*** write ***/
void
GdalRaster::write( Scalar *buf, int frow, int nrow )
{
  // Header's data.
  int nband = f_hdr.nband;
  int size  = f_size * nrow;

  // Write each band in the file.
  for ( int i = 1; i <= nband; i++, buf += size ) {

    // Gets the i-th band.
    GDALRasterBand *band = f_ds->GetRasterBand( i );

    // Write the buffer's data in the file.
    int ret = band->RasterIO( GF_Write, 0, frow, f_size, nrow, buf, f_size, nrow, f_type, 0, 0 );

    if ( ret == CE_Failure )
    {
      Log::instance()->error( "Unable to write to file %s\n", f_file.c_str());
      throw FileIOException( "Unable to write to file " + f_file, f_file );
    }
  }
}

/************/
/*** open ***/
void
GdalRaster::open( char mode )
{
  // Mode: write or read.
  GDALAccess gmod = (mode == 'w') ? GA_Update : GA_ReadOnly;

  // Opens the file.
  f_ds = (GDALDataset *)GDALOpen( f_file.c_str(), gmod );

  if ( ! f_ds ) {

    Log::instance()->error( "Unable to open file %s\n", f_file.c_str());
    throw FileIOException( "Unable to open file " + f_file, f_file );
  }

  // Number of bands (channels).
  f_hdr.nband = f_ds->GetRasterCount();

  // Projection.
  f_hdr.setProj( (char *) f_ds->GetProjectionRef() );

  if ( ! f_hdr.hasProj() ) {

    Log::instance()->warn( "The raster %s is not georeferenced. Assuming LatLong WGS84\n", f_file.c_str() );
    f_hdr.setProj( GeoTransform::getDefaultCS() );
  }

  // Assumes that all bands have the same georeference
  // characteristics, ie, the same header.

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

  //Log::instance()->debug( "Raster boundaries: xmin=%f, xmax=%f, ymin=%f, ymax=%f \n", f_hdr.xmin, f_hdr.xmax, f_hdr.ymin, f_hdr.ymax );

  f_hdr.calculateCell();

  for ( int i=0; i<6; ++i ) {

    f_hdr.gt[i] = cf[i];
  }

  // Minimum and maximum values.
  f_hdr.vmin = band->GetMinimum( &f_hdr.minmax );

  if ( f_hdr.minmax ) {

    f_hdr.vmax = band->GetMaximum();
  }

  // Assumes grid (in place of 'pixel').
  // todo: how can I read this with GDAL?
  f_hdr.grid = 0;

  // Initialize the Buffer
  initBuffer();
}

/**************/
/*** create ***/
void
GdalRaster::create( int format )
{
  // Store format for future reference (used in method "finish")
  f_format = format;

  GDALDriver *poDriver;
  char **papszMetadata;

  char const *fmt = Formats[ format ].GDalDriverName;

  // Added by Tim to see if the chosen format supports the gdal create method
  poDriver = GetGDALDriverManager()->GetDriverByName(fmt);

  if ( poDriver == NULL ) {

    std::string msg = "Unable to load GDAL driver " + string(fmt) + " for file " + f_file;

    Log::instance()->error( msg.c_str() );

    throw FileIOException( "Unable to load GDAL driver " + string(fmt), f_file );
  }

  papszMetadata = poDriver->GetMetadata();

  if ( ! CSLFetchBoolean( papszMetadata, GDAL_DCAP_CREATE, FALSE ) ) {

    Log::instance()->error( "Driver %s, format %s does not support Create() method.\n",
                            poDriver->GetDescription(),
                            fmt );

    throw FileIOException( "Driver " + string(fmt) + " does not support Create() method", f_file );
  }

  // Read the parameters in 'hdr' used to create the file.
  // The other parameters come from the copied header.
  f_hdr.nband = 1;

  // Create the file.
  if ( format == MapFormat::FloatingHFA || format == MapFormat::ByteHFA ) {

    // Note: HFA (erdas imagine) format does not support nodata before GDAL 1.5
    // see http://www.gdal.org/gdal_tutorial.html for options examples
    char **papszOptions = NULL;

    papszOptions = CSLSetNameValue( papszOptions, "COMPRESS", "YES" );
    #ifdef MPI_FOUND
    if (rank ==0){
    #endif
    f_ds = poDriver->Create( f_file.c_str(),
                             f_hdr.xdim, f_hdr.ydim,
                             f_hdr.nband,
                             Formats[format].dataType,
                             papszOptions );
    #ifdef MPI_FOUND
    }
    #endif
    CSLDestroy( papszOptions );
  }
  //ArcMap needs a LZW compression license to read files compressed
  //like this so you may need to comment out this code if you want
  //to open the generated maps without having the license. Compression
  //is enabled for now because it offers significant size reduction.
  else if (format == MapFormat::GreyTiff100)
  {
    //lzw compression and represent each pixel with 7bits only
    char **papszOptions = NULL;
    papszOptions = CSLSetNameValue( papszOptions, "NBITS", "7" );
    papszOptions = CSLSetNameValue( papszOptions, "COMPRESS", "LZW" );
    #ifdef MPI_FOUND
    if (rank ==0){
    #endif
    f_ds = poDriver->Create( f_file.c_str(),
                             f_hdr.xdim, f_hdr.ydim,
                             f_hdr.nband,
                             Formats[format].dataType, //data type
                             papszOptions ); //opt parameters
    #ifdef MPI_FOUND
    }
    #endif
    CSLDestroy( papszOptions );
  }
  //Create temporary ByteHFA file with a different name (original file name + .tmp)
  //It will be converted to ASC in the finish method
  else if (format==MapFormat::ByteASC)
  {
    std::string temp_file = f_file + ".tmp";

    #ifdef MPI_FOUND
    if (rank ==0){
    #endif
    f_ds = poDriver->Create( temp_file.c_str(),
                             f_hdr.xdim, f_hdr.ydim,
                             f_hdr.nband,
                             Formats[format].dataType,
                             NULL );
    #ifdef MPI_FOUND
    }
    #endif
  }
  //Create temporary GeoTiff file with a different name (original file name + .tmp)
  //It will be converted to ASC in the finish method
  else if (format==MapFormat::FloatingASC)
  {
    std::string temp_file = f_file + ".tmp";

    #ifdef MPI_FOUND
    if (rank ==0){
    #endif
    f_ds = poDriver->Create( temp_file.c_str(),
                             f_hdr.xdim, f_hdr.ydim,
                             f_hdr.nband,
                             Formats[format].dataType,
                             NULL );
    #ifdef MPI_FOUND
    }
    #endif
  }
  else {
    #ifdef MPI_FOUND
    if (rank ==0){
    #endif
    //uncompressed
    f_ds = poDriver->Create( f_file.c_str(),
                             f_hdr.xdim, f_hdr.ydim,
                             f_hdr.nband,
                             Formats[format].dataType,
                             NULL );
    #ifdef MPI_FOUND
    }
    #endif
  }

  #ifdef MPI_FOUND
  if (rank ==0) {
  #endif
  if ( ! f_ds ) {

    Log::instance()->warn( "Unable to create file %s.\n",f_file.c_str() );
    throw FileIOException( "Unable to create file " + f_file, f_file );
  }

  if ( Formats[format].hasMeta ) {

    // Metadata

    // Limits (without rotations).
    f_ds->SetGeoTransform( f_hdr.gt );

    // Projection.
    f_ds->SetProjection( f_hdr.proj.c_str() );

    // Sets the "nodata" value in all bands.
    int ret = f_ds->GetRasterBand(1)->SetNoDataValue( f_hdr.noval );

    if ( ret == CE_Failure ) {

      Log::instance()->warn( "Raster %s (%s) does not support nodata value assignment. Nodata values will correspond to %f anyway, but this will not be stored as part of the raster metadata.\n", f_file.c_str(), fmt, f_hdr.noval );
    }
  }
  #ifdef MPI_FOUND
  }
  #endif
  // Initialize the Buffer
  initBuffer();
}

/*******************/
/*** init Buffer ***/
void
GdalRaster::initBuffer()
{
  // Initialize the buffer.
  f_size = f_hdr.xdim;

  if ( f_data ) {

    delete[] f_data;
  }

  f_data = new Scalar[ f_size * f_hdr.nband ];

  f_changed = 0;
  f_currentRow = -1;
}

/*****************/
/*** init Gdal ***/
void
GdalRaster::initGdal()
{
  static int first = 1;

  if ( first ) {

    first = 0;
    GDALAllRegister();
  }
}

/************/
/*** iput ***/
int
GdalRaster::iput( int x, int y, Scalar val )
{
  // Be sure that 'y' line is in the buffer 'f_data'.
  loadRow( y, true );

  // Put values in the first band of (x,y) position.
  f_data[x] = val;

  // Indicates the line 'f_currentRow' has changed.
  f_changed = 1;

  return 1;
}

/************/
/*** iget ***/
int
GdalRaster::iget( int x, int y, Scalar *val )
{
  // Be sure that 'y' line is in the buffer 'f_data'.
  loadRow( y );

  // Get all band's values.
  Scalar *pv = val;
  int nband = f_hdr.nband;
  for ( int i = 0; i < nband; i++, x += f_size ) {

    if ( (*pv++ = f_data[x]) == f_hdr.noval ) {

      return 0;
    }
  }

  return 1;
}

/***********/
/*** get ***/
int
GdalRaster::get( Coord px, Coord py, Scalar *val )
{
  Scalar *pv = val;
  for ( int i = 0; i < f_hdr.nband; i++ ) {

    *pv++ = f_hdr.noval;
  }

  pair<int,int> xy = f_hdr.convertLonLat2XY( px, py );
  int x = xy.first;
  int y = xy.second;

  // If the point is out of range, returns 0.
  if ( x < 0 || x >= f_hdr.xdim || y < 0 || y >= f_hdr.ydim ) {

    //Log::instance()->debug( "Raster::get() Pixel (%d,%d) is not in extent\n",x,y);
    return 0;
  }

  // 'iget()' detects if the coordinate has or not valid
  // information (noval);
  return iget( x, y, val );
}

/***********/
/*** put ***/
int
GdalRaster::put( Coord px, Coord py, Scalar val )
{
  pair<int,int> xy = f_hdr.convertLonLat2XY( px, py );
  int x = xy.first;
  int y = xy.second;

  if ( x < 0 || x >= f_hdr.xdim || y < 0 || y >= f_hdr.ydim ) {

    Log::instance()->warn( "Coordinate (%f, %f) corresponds to a cell (%d, %d) outside the raster boundaries. It will be ignored.\n", px, py, x, y ); 
    return 0;
  }

  return iput( x, y, f_scalefactor*val );
}

/***********/
/*** put ***/
int
GdalRaster::put( Coord px, Coord py )
{
  pair<int,int> xy = f_hdr.convertLonLat2XY( px, py );
  int x = xy.first;
  int y = xy.second;

  Scalar val = f_hdr.noval;

  if ( x < 0 || x >= f_hdr.xdim || y < 0 || y >= f_hdr.ydim ) {

    Log::instance()->warn( "Coordinate (%f, %f) corresponds to a cell (%d, %d) outside the raster boundaries. It will be ignored.\n", px, py, x, y ); 
    return 0;
  }

  return iput( x, y, val );
}

/*******************/
/*** get Min Max ***/
int
GdalRaster::getMinMax( Scalar *min, Scalar *max )
{
  if ( ! calcMinMax() ) {

    return 0;
  }

  *min = f_hdr.vmin;

  *max = f_hdr.vmax;

  return 1;
}

/********************/
/*** calc Min Max ***/
int
GdalRaster::calcMinMax( int band )
{
  if ( f_hdr.minmax ) {

    return 1;
  }

  Scalar *bands;
  bands = new Scalar[ f_hdr.nband ];
  Scalar *val = bands + band;
  Scalar min=0;
  Scalar max=0;

  bool initialized = false;

  // Scan all map values.
  for ( int y = 0; y < f_hdr.ydim; y++ ) {

    for ( int x = 0; x < f_hdr.xdim; x++ ) {

      if ( iget( x, y, bands ) ) {

        if ( !initialized ) {

          initialized = true;
          min = max = *val;
        }

        if ( min > *val ) {

          min = *val;
        }
        else if ( max < *val ) {

          max = *val;
        }
      }
    }
  }

  delete[] bands;

  if ( ! initialized ) {

    return 0;
  }

  // This is only logically const. Here we cast away const to cache the min/max
  Header *f_hdrNoConst = const_cast<Header*>(&f_hdr);

  f_hdrNoConst->minmax = 1;
  f_hdrNoConst->vmin = min;
  f_hdrNoConst->vmax = max;

  return 1;
}

/****************/
/*** load Row ***/
void
GdalRaster::loadRow( int row, bool writeOperation )
{
  // If the line is already read, return.
  if ( row == f_currentRow ) {

    return;
  }

  saveRow();

  // Update f_data
  if ( writeOperation ) {

    // Just reset f_data with nodata
    for ( int i = 0 ; i < f_size; i++ ) {

      f_data[i] = f_hdr.noval;
    }
  }
  else {

    // Read from file
    read( f_data, row, 1 );
  }

  f_currentRow = row;
  f_changed = 0;
}


/****************/
/*** save Row ***/
void
GdalRaster::saveRow()
{
  if ( ! f_changed || f_currentRow < 0 ) {

    return;
  }

  write( f_data, f_currentRow, 1 );

  f_changed = 0;
}

/**************/
/*** finish ***/
void 
GdalRaster::finish()
{
  // Save the last line read, if needed.
  saveRow();

  if ( f_format == MapFormat::ByteASC || f_format == MapFormat::FloatingASC )
  {
    // Temporary file name
    std::string temp_file = f_file + ".tmp";

    // Get ArcInfo/ASC Grid driver
    GDALDriver *hDriver;
    hDriver = GetGDALDriverManager()->GetDriverByName("AAIGrid");

    if ( hDriver == NULL ) {

      std::string msg = "Unable to load AAIGrid GDAL driver";

      Log::instance()->error( msg.c_str() );
      throw FileIOException( msg.c_str(), f_file );
    }

    // Convert temporary raster to ArcInfo/ASC Grid
    GDALDataset * new_ds = hDriver->CreateCopy( f_file.c_str(), f_ds, FALSE, NULL, NULL, NULL );

    if ( ! new_ds ) {

      Log::instance()->warn( "Unable to create raster copy %s.\n",f_file.c_str() );
      throw FileIOException( "Unable to create raster copy " + f_file, f_file );
    }

    // Delete temporary ByteHFA raster
#if GDAL_VERSION_MAJOR > 1 || ( GDAL_VERSION_MAJOR == 1 && GDAL_VERSION_MINOR >= 5 )

      delete f_ds;

      if ( GDALDriver::QuietDelete( temp_file.c_str() ) == CE_Failure )
      {
        Log::instance()->warn( "Could not delete temporary file %s", temp_file.c_str() );
      }
#else
      GDALDriver * temp_driver = f_ds->GetDriver();

      if ( temp_driver->Delete( temp_file.c_str() ) == CE_Failure )
      {
        Log::instance()->warn( "Could not delete temporary file %s", temp_file.c_str() );
      }

      delete f_ds;
#endif

    f_ds = new_ds;
  }
}

/*********************/
/*** delete Raster ***/
int
GdalRaster::deleteRaster()
{
  GDALDriver * driver = f_ds->GetDriver();

  int ret = driver->Delete( f_file.c_str() );

  if ( ret == CE_Failure ) {

    return 0;
  }

  f_ds = 0;

  return 1;
}
