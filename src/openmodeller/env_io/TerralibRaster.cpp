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

#include <openmodeller/env_io/TerralibRaster.hh>

#include <openmodeller/TeDatabaseManager.hh>
#include <openmodeller/TeStringParser.hh>
#include <TeRaster.h>
#include <TeDatabase.h>
#include <TeLayer.h>

#include <openmodeller/env_io/GeoTransform.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/MapFormat.hh>

#include <TeProjection.h>

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <utility>
using std::pair;

#include <stdio.h>
/****************************************************************/
/************************** Te Raster ***************************/
/**
* Needed by RasterFactory.
*/
Raster*
TerralibRaster::CreateRasterCallback()
{
	return new TerralibRaster();
}

/** 
* If 'categ' != 0 this is a categorical map (ie it can't be
* interpolated). Othewise this is a continuos map.
*
*/
// Open an existing file -- read only.
void
TerralibRaster::createRaster( const string& str, int categ )
{
	te_str_parser_ = new TeStringParser();
	te_str_parser_->str_ = str;
	f_scalefactor = 1.0;
	f_hdr.minmax = 0;

	// Open an existing Raster
	openTeRaster();

	if ( raster_ )
	{

		raster_->init();
		params_ = &raster_->params();

		if (params_->status_ != TeRasterParams::TeReadyToRead)
		{
			std::string msg = "TerralibRaster::createRaster - Raster cannot be opened: ";
                        msg += raster_->errorMessage().c_str();
			Log::instance()->error( msg.c_str() );
			throw RasterException( msg );
		}		
		else
		{
			// Number of bands (channels).
			f_hdr.nband = raster_->nBands();

			// Projection.
			//string teste = TeGetWKTFromTeProjection( raster_->projection() );
			f_hdr.setProj( TeGetWKTFromTeProjection( raster_->projection() ) );
    		if ( ! f_hdr.hasProj() )
			{/**/
				Log::instance()->warn( "Raster %s is not georeferenced.  Assuming LatLong WGS84\n", f_file.c_str() );
				f_hdr.setProj( GeoTransform::getDefaultCS() );
			}/**/

			// Assumes that all bands have the same georeference
			// characteristics, ie, the same header.
			f_hdr.xdim = params_->ncols_;
			f_hdr.ydim = params_->nlines_;

			f_hdr.noval = params_->dummy_[0];
			
			// Map limits.
			f_hdr.xmin = (Scalar) params_->boundingBox().x1_;
			f_hdr.xmax = (Scalar) params_->boundingBox().x2_;
			f_hdr.ymax = (Scalar) params_->boundingBox().y2_;
			f_hdr.ymin = (Scalar) params_->boundingBox().y1_;

			f_hdr.calculateCell();
			
			f_hdr.gt[0] = params_->boundingBox().x1_; // top left x
			f_hdr.gt[1] = params_->resx_;			  // w-e pixel resolution
			f_hdr.gt[2] = 0;			     		  // rotation, 0 if image is "north up"
			f_hdr.gt[3] = params_->boundingBox().y2_; // top left y
			f_hdr.gt[4] = 0;						  // rotation, 0 if image is "north up"
			f_hdr.gt[5] =  - params_->resy_;		  // n-s pixel resolution

			// Minimum and maximum values.
			f_hdr.min = params_->vmin_[0];
			f_hdr.max = params_->vmax_[0];
			
			// If not zero 'min' and 'max' are valid values.
			f_hdr.minmax = 1; //true
			
			// Assumes grid (in place of 'pixel').
			// todo: how can I read this with TerraLib?
			f_hdr.grid = 0;

			// set categorical status.  It's not stored as part of
			// the gdal file's header.
			f_hdr.categ = categ;
		}
	}
}

/**
* Create a new file for projections.
* @param file is the name of the output file
* @param format is the output format specification.
*/
void
TerralibRaster::createRaster( const std::string& str, const MapFormat& format )
{
	te_str_parser_ = new TeStringParser();
	te_str_parser_->str_ = str;

	Scalar nv; 

	TeDataType rtype;
	switch( format.getFormat() )
	{
		case MapFormat::GreyBMP:
			f_scalefactor = 255.0;
			nv = 0.0;
			rtype = TeUNSIGNEDCHAR;
			break;
		case MapFormat::GreyTiff:
			f_scalefactor = 254.0;
			nv = 255.0;
			rtype = TeUNSIGNEDCHAR;
			break;
		case MapFormat::FloatingTiff:
            f_scalefactor = 1.0;
			nv = -1.0;
			rtype = TeFLOAT;
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
	
	// Create a new TeRaster.
	createTeRaster();
	
	if ( raster_ )
	{
		params_ = &raster_->params();
		
		params_->nBands( 1 );
		params_->setDataType( rtype );
		params_->setDummy(nv, 0);

		params_->boundingBoxLinesColumns(format.getXMin(), format.getYMin(),
			format.getXMax(), format.getYMax(),
			format.getHeight(), format.getWidth());

		// Block Size.
		/*if(format.getHeight() > 128 || format.getWidth() > 128)
		{
			params_->blockHeight_ = 128;
			params_->blockWidth_ = 128; 
		}
		else
		{
			params_->blockHeight_ = format.getHeight();
			params_->blockWidth_ = format.getWidth(); 
		}*/
		params_->blockHeight_ = 1;
		params_->blockWidth_ = format.getWidth(); 
		
		raster_->init();

		if (params_->status_ != TeRasterParams::TeReadyToWrite)
		{
			Log::instance()->warn( "TerraLib raster not ready: %s.\n", raster_->errorMessage() );
		}
	}
}

/**
* Destructor
*/
TerralibRaster::~TerralibRaster()
{
	if( layer_ )
	{
		layer_->addRasterGeometry( raster_ );
	}

	delete te_str_parser_;
}

/**
* Fills '*val' with the map value at (x,y).
* Returns zero if (x,y) is out of range.
*/
int
TerralibRaster::get( Coord px, Coord py, Scalar *val )
{
	pair<int,int> xy = f_hdr.convertLonLat2XY( px, py );
	int x = xy.first;
	int y = xy.second;

	// If the point is out of range, returns 0.
	if ( x < 0 || x >= f_hdr.xdim || y < 0 || y >= f_hdr.ydim )
	{
		//Log::instance()->debug( "Raster::get() Pixel (%d,%d) is not in extent\n",x,y);
		return 0;
	}

	if( raster_->getElement(x, y, *val , 0) )
		return 1;

	return 0;
};

/**
* Put '*val' at the (x,y) coordinate.
* Returns 0 if (x,y) is out of range or the map is read only.
* supports only single band output files.
*/
int
TerralibRaster::put( Coord px, Coord py, Scalar val )
{
	pair<int,int> xy = f_hdr.convertLonLat2XY( px, py );
	int x = xy.first;
	int y = xy.second;

	if ( x < 0 || x >= f_hdr.xdim || y < 0 || y >= f_hdr.ydim )
		return 0;

	if( raster_->setElement(x, y, f_scalefactor*val, 0) )
		return 1;

	return 0;
}

/**
* Put 'no data val' at the (x,y) coordinate.
* Returns 0 if (x,y) is out of range or the map is read only.
* supports only single band output files.
*/
int
TerralibRaster::put( Coord px, Coord py )
{
	pair<int,int> xy = f_hdr.convertLonLat2XY( px, py );
	int x = xy.first;
	int y = xy.second;

	if ( x < 0 || x >= f_hdr.xdim || y < 0 || y >= f_hdr.ydim )
		return 0;

	if( raster_->setElement(x, y, f_hdr.noval, 0) )
		return 1;

	return 0;
}

/**
*
*/
void
TerralibRaster::openTeRaster()
{
	if( te_str_parser_->parse() )
	{
		db_ = TeDatabaseManager::instance().create( *te_str_parser_ );
        
		if ( !db_->isConnected() )
		{
			//delete db_;
			std::string msg = "TerralibRaster::openTeRaster - Cannot connect to database: ";
                        msg += db_->errorMessage().c_str();
			Log::instance()->error( msg.c_str() );
			throw RasterException( msg );
		}
		else
		{
			if (db_->layerExist( te_str_parser_->layerName_ ))
			{
				layer_ = new TeLayer(te_str_parser_->layerName_, db_);
                raster_ = layer_->raster();
			}
		}
	}
	else
	{
		// Disk file.
		raster_ = new TeRaster( te_str_parser_->str_, 'r');
	}
}

/**
*
*/
void
TerralibRaster::createTeRaster()
{
	if( te_str_parser_->parse() )
	{
		db_ = TeDatabaseManager::instance().create( *te_str_parser_ );

		if ( !db_->isConnected() )
		{
			//delete db_;
			std::string msg = "TerralibRaster::createTeRaster - Cannot connect to database: ";
                        msg += db_->errorMessage().c_str();
			Log::instance()->error( msg.c_str() );
			throw RasterException( msg );
		}
		else
		{
			TeProjection* proj = TeGetTeProjectionFromWKT( f_hdr.proj );
			
			layer_ = new TeLayer(te_str_parser_->layerName_, db_, proj);
			
			// create a raster geometry in a TerraLib database
			TeRasterParams params;
			params.fileName_ = te_str_parser_->layerName_;
			params.mode_ = 'c';
			// parameters specific of the database decoder
			params.decoderIdentifier_ = "DB";			// a database decoder 
			params.database_ = db_;						// pointer to the database
			// we do not intent to mosaic any raster data to this
			params.tiling_type_ = TeRasterParams::TeNoExpansible;
			
			// the photometric interpretation of the raster
			params.setPhotometric(TeRasterParams::TeMultiBand);

			raster_ = new TeRaster( params );
		}
	}
	else
	{
		// Disk file.
		raster_ = new TeRaster( te_str_parser_->str_, 'c');
	}
}

/*******************/
/*** get Min Max ***/
int
TerralibRaster::getMinMax( Scalar *min, Scalar *max )
{
	*min = f_hdr.min;

	*max = f_hdr.max;

	return 1;
}


/*********************/
/*** delete Raster ***/
int
TerralibRaster::deleteRaster()
{
	db_ = TeDatabaseManager::instance().create( *te_str_parser_ );
    
	if ( !db_->isConnected() )
	{
		//delete db_;
		std::string msg = "TerralibRaster::openTeRaster - Cannot connect to database: ";
                    msg += db_->errorMessage().c_str();
		Log::instance()->error( msg.c_str() );
		throw RasterException( msg );
		return 0;
	}
	else
	{
		if ( db_->layerExist( layer_->name() ) )
		{
			db_->deleteLayer( layer_->id() );
		}

		if( raster_ )
		{
			delete raster_;
		}

		if( params_ )
		{
			delete params_;
		}

		if( te_str_parser_ )
		{
			delete te_str_parser_;
		}

		return 1;
	}

	// If raster is a file disk.
	if( raster_ )
	{
		delete raster_;
	}

	if( params_ )
	{
		delete params_;
	}

	if( te_str_parser_ )
	{
		delete te_str_parser_;
	}

	return 1;
}
