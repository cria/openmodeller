/**
 * Definition of ControlInterface class.
 * 
 * @file
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-09-25
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

#include "om_control.hh"

#include <env_io/map.hh>
#include <env_io/header.hh>
#include <env_io/raster_file.hh>
#include <env_io/geo_transform.hh>

#include <om_defs.hh>
#include <om_log.hh>
#include <om_algorithm.hh>
#include <om_sampler.hh>
#include <om_occurrences.hh>

#include <algorithm_factory.hh>
#include <environment.hh>
#include <occurrences_file.hh>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Log _log( Log::Debug );


// Directories to search for dynamic libraries.
// Fixme: read this from some configuration file.
static char *g_search_dirs[] = {
  PLUGINPATH,
  0
};



/****************************************************************/
/************************* Control Interface ********************/

/********************/
/*** constructors ***/

ControlInterface::ControlInterface()
{
  _factory = new AlgorithmFactory( g_search_dirs );

  _hdr = new Header;

  _nlayers = 0;
  _layers  = 0;
  _mask    = 0;

  _file = 0;
  _mult = 1.0;

  _alg_id    = 0;
  _alg_param = 0;

  _oc_file = 0;
  _oc_cs   = 0;
  _oc_name = 0;

  _error = 0;
}

ControlInterface::ControlInterface( int ncateg, int nlayer,
				    char **layers, char *mask )
{
  _factory = new AlgorithmFactory( g_search_dirs );

  _hdr = new Header;

  _layers = 0;

  _file = 0;
  _mult = 1.0;

  _alg_id    = 0;
  _alg_param = 0;

  _oc_file = 0;
  _oc_cs   = 0;
  _oc_name = 0;

  _error = 0;

  setEnvironment( ncateg, nlayer, layers, mask );
}


/******************/
/*** destructor ***/

ControlInterface::~ControlInterface()
{
  delete _factory;
  delete _hdr;

  if ( _layers )  delete _layers;
  if ( _file )    delete _file;
  if ( _alg_id )     delete _alg_id;
  if ( _oc_file ) delete _oc_file;
  if ( _oc_cs )   delete _oc_cs;
  if ( _oc_name ) delete _oc_name;
}


/****************************/
/*** available Algorithms ***/
Algorithm **
ControlInterface::availableAlgorithms()
{
  return _factory->availableAlgorithms();
}


/***********************/
/*** set Environment ***/
void
ControlInterface::setEnvironment( int ncateg, int nlayer,
				  char **layers, char *mask )
{
  _ncateg = ncateg;
  stringCopy( &_mask, mask );

  // Reallocate vector that stores the names of the layers.
  if ( _layers )
    delete[] _layers;
  _nlayers = nlayer;
  _layers = new char*[_nlayers];

  for ( int i = 0; i < _nlayers; i++ )
    {
      _layers[i] = 0;
      stringCopy( _layers + i, layers[i] );
    }
}


/**********************/
/*** set Output Map ***/
void
ControlInterface::setOutputMap( char *file, Header *hdr,
				Scalar mult )
{
  stringCopy( &_file, file );
  *_hdr = *hdr;
  _mult = mult;
}


/**********************/
/*** set Output Map ***/
void
ControlInterface::setOutputMap( char *file, char *map_file,
				Scalar mult )
{
  RasterFile map( map_file );
  setOutputMap( file, &map.header(), mult );
}


/*********************/
/*** set Algorithm ***/
void
ControlInterface::setAlgorithm( char *id, char *param )
{
  stringCopy( &_alg_id, id );
  stringCopy( &_alg_param, param );
}


/*******************/
/*** set Species ***/
void
ControlInterface::setOccurrences( char *file, char *cs, char *oc )
{
  stringCopy( &_oc_file, file );
  stringCopy( &_oc_cs,   cs );
  stringCopy( &_oc_name, oc );
}


/***********/
/*** run ***/
int
ControlInterface::run()
{
  if ( _error = basicCheck() )
    return 0;

  // Ocurrence points.
  Occurrences *presence = readOccurrences( _oc_file, _oc_cs,
					   _oc_name );

  // Must implement the reading of absence points.
  Occurrences *absence = 0;

  if ( ! presence )
    {
      _error = "Occurrences not found with the given name.";
      return 0;
    }
  _log.info( "Modelling occurrences of: %s.\n", presence->name() );

  // Environmental variables.
  char **categs = _layers;
  char **maps = categs + _ncateg;
  int nmaps   = _nlayers - _ncateg;
  Environment *env = new Environment( GeoTransform::cs_default,
				      _ncateg, categs,
				      nmaps, maps, _mask );

  _log.info( "Environment initialized.\n" );

  // Sampler and algorithm.
  Sampler samp( env, presence, absence );
  Algorithm *alg;
  alg = _factory->newAlgorithm( &samp, _alg_id, _alg_param );
  if ( ! alg )
    {
      _error = "Could not find an algorithm with the given name.";
      return 0;
    }


  // Check if the algorithm needs normalized variables.
  Scalar min, max;
  if ( alg->needNormalization( &min, &max ) )
    {
      _log.info( "Normalizing environment variables.\n" );
      env->normalize( min, max );
    }


  _log.info( "Creating the model\n" );

  // Generate the model.
  if ( ! createModel( alg, &samp, _ncycle ) )
    {
      _error = "Model creation error.";
      return 0;
    }

  _log.info( "Saving distribution's file:\n" );

  // Create the map with probabilities of occurence.
  if ( ! createMap( env, alg ) )
    {
      _error = "Occurrence probability map creation error.";
      return 0;
    }

  delete presence;
  delete alg;
  delete env;

  return 1;
}


/*******************/
/*** string Copy ***/
void
ControlInterface::stringCopy( char **dst, char *src )
{
  if ( *dst )
    delete *dst;

  if ( src )
    {
      *dst = new char[1 + strlen( src )];
      strcpy( *dst, src );
    }
  else
    *dst = 0;
}


/*******************/
/*** basic Check ***/
char *
ControlInterface::basicCheck()
{
  // Ocorrências da espécies.
  if ( ! _oc_file )
    return "Occurrences file not found and/or not specified.";

  if ( ! _oc_cs )
    return "Occurrences' coordinate system not specified.";


  if ( ! _nlayers )
    return "Environmental variables not specified.";


  // Algoritmo.
  if ( ! _alg_id )
    return "Modeling algorithm not specified.";


  // Mapa de saída.
  if ( ! _file )
    return "The output file name not specified.";

  return 0;
}


/************************/
/*** read Occurrences ***/
Occurrences *
ControlInterface::readOccurrences( char *file, char *cs,
				   char *name )
{
  OccurrencesFile lsp( file, cs, GeoTransform::cs_default );

  // Take last species from the list, which corresponds to the
  // first inside the file.
  if ( ! name )
    {
      lsp.tail();
      name = lsp.get()->name();
    }

  return lsp.remove( name );
}


/*************************/
/*** algorithm Factory ***/
/*
Algorithm *
ControlInterface::algorithmFactory( Sampler *samp, char *name,
			       char *param )
{
  if ( ! strcasecmp( name, "Distance" ) )
    return new DistanceModel( samp, param ? atof(param) : 0 );

  if ( ! strcasecmp( name, "MinDistance" ) )
    return new MinDistanceModel( samp, param ? atof(param) : 0 );

  if ( ! strcasecmp( name, "Bioclim" ) )
    return new BioclimModel( samp, param ? atof(param) : 0 );

  if ( ! strcasecmp( name, "Csm" ) )
    return new Csm( samp );
    

  return 0;
}
*/


/********************/
/*** create Model ***/
int
ControlInterface::createModel( Algorithm *alg, Sampler *samp,
			       int max )
{
  // Initialize algorithm.  
  if ( ! alg->initialize( 1 ) )
    _log.error( 1, "Algorithm %s could not be initialized.\n",
		alg->getID() );

  // Generate model.
  int ncycle = 0;
  while ( alg->iterate() && ! alg->done() && ncycle < max )
    ncycle++;

  return alg->done() || ncycle >= max;
}


/******************/
/*** create Map ***/
int
ControlInterface::createMap( Environment *env, Algorithm *alg )
{
  if ( ! _hdr->hasProj() )
    {
      _log.error( 1,
		  "Output map without a coordinate system!\n\n",
		  _file );
      return 0;
    }

  // Force noval = 0
  _hdr->noval = 0.0;
 
  // Create map on disc.
  RasterFile rst( _file, *_hdr );
  Map map( &rst , env->getCoordinateSystem() );

  // Retrieve possible adjustments and/or additions made
  // on the the effective header.
  *_hdr = rst.header();


  // Transformer used by the resulting map.
  GeoTransform *gt = map.getGT();

  // Dimension of environment space.
  int dim = env->numLayers();


  // Debug: minimum and maximum.
  Scalar min = 1e10;
  Scalar max = -1e10;


  // Fill the map with probabilities given by the model.
  Scalar *amb; 
  amb = new Scalar[dim];
  *amb = 0.0;

  Scalar val;
  Coord lg, lt;
  Coord y0 = _hdr->ymin; // + 0.5 * _hdr.ycel;
  Coord x0 = _hdr->xmin; // + 0.5 * _hdr.xcel;

  int row = 0;
  for ( float y = y0; y < _hdr->ymax; y += _hdr->ycel )
    {
      int col = 0;
      Scalar sum = 0.0;
      for ( float x = x0; x < _hdr->xmax; x += _hdr->xcel )
	{
	  // Transform coordinates (x,y) that are in the resulting
	  // map system, in (lat, long) according to the system 
	  // accepted by the environment (env).
	  gt->transfOut( &lg, &lt, x, y );


	  // Read environmental values and find the output value.
	  if ( ! env->get( lg, lt, amb ) )
	    val = _hdr->noval;
	  else
	    {
	      val = alg->getValue( amb );
	      if ( val < 0.0 ) val = 0.0;
	      if ( val > 1.0 ) val = 1.0;
	      val *= _mult;
	    }

	  // Write value on map.
	  map.put( lg, lt, &val );

	  // Sum of line values.
	  sum += val;
	}

      _log.info( "Line %04d / %4d : %.2f          \r",
		 ++row, _hdr->ydim, sum / _hdr->xdim );
    }
  _log.info( "\n" );

  delete amb;
  return 1;
}
