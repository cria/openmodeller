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
#include <om_alg_parameter.hh>
#include <om_sampler.hh>
#include <om_occurrences.hh>

#include <algorithm_factory.hh>
#include <environment.hh>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

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

  _alg_id     = 0;
  _alg_param  = 0;
  _alg_nparam = 0;

  _presence = 0;
  _absence  = 0;

  f_error[0] = '\0';
}


/******************/
/*** destructor ***/

ControlInterface::~ControlInterface()
{
  delete _factory;
  delete _hdr;

  if ( _layers )   delete _layers;
  if ( _file )     delete _file;
  if ( _alg_id )   delete _alg_id;
  if ( _presence ) delete _presence;
  if ( _absence )  delete _absence;
}


/*******************/
/*** get Version ***/
char *
ControlInterface::getVersion()
{
  // Defined in "om_defs.hh".
  return OM_VERSION;
}


/****************************/
/*** available Algorithms ***/
AlgMetadata **
ControlInterface::availableAlgorithms()
{
  return _factory->availableAlgorithms();
}


/**************************/
/*** algorithm Metadata ***/
AlgMetadata *
ControlInterface::algorithmMetadata( char *algorithm_id )
{
  return _factory->algorithmMetadata( algorithm_id );
}


/********************************/
/*** num Available Algorithms ***/
int
ControlInterface::numAvailableAlgorithms()
{
  return _factory->numAvailableAlgorithms();
}


/***********************/
/*** set Environment ***/
int
ControlInterface::setEnvironment( int num_categ,
				  char **categ_map,
				  int num_continuos,
				  char **continuous_map,
				  char *mask )
{
  _ncateg  = num_categ;
  _nlayers = num_categ + num_continuos;

  stringCopy( &_mask, mask );

  // Reallocate vector that stores the names of the layers.
  if ( _layers )
    delete[] _layers;
  _layers = new char*[_nlayers];

  // stringCopy() needs this.
  memset( _layers, 0, _nlayers * sizeof(char *) );

  char **layers = _layers;

  // Copy categorical maps.
  char **end = _layers + _ncateg;
  while ( layers < end )
    stringCopy( layers++, *categ_map++ );

  // Copy continuos maps.
  end += num_continuos;
  while ( layers < end )
    stringCopy( layers++, *continuous_map++ );

  return 1;
}

/**********************/
/*** set Output Map ***/
int
ControlInterface::setOutputMap( char *file, Header *hdr,
				Scalar mult )
{
  stringCopy( &_file, file );
  *_hdr = *hdr;
  _mult = mult;

  return 1;
}


/**********************/
/*** set Output Map ***/
int
ControlInterface::setOutputMap( char *file, char *map_file,
				Scalar mult )
{
  RasterFile map( map_file );
  return setOutputMap( file, &map.header(), mult );
}


/*********************/
/*** set Algorithm ***/
int
ControlInterface::setAlgorithm( char *id, int nparam,
				AlgParameter *param )
{
  AlgMetadata *meta = algorithmMetadata( id );

  // Check the parameters.
  if ( ! meta || meta->nparam != nparam )
    return 0;

  stringCopy( &_alg_id, id );

  // Reallocate '_alg_param' to stores 'nparam' parameters.
  _alg_nparam = nparam;
  if ( _alg_param )
    delete[] _alg_param;
  _alg_param = new AlgParameter[ _alg_nparam ];

  // Copy 'param' to '_alg_param'.
  AlgParameter *dst = _alg_param;
  AlgParameter *end = _alg_param + _alg_nparam;
  while ( dst < end )
    *dst++ = *param++;

  return 1;
}


/***********************/
/*** set Occurrences ***/
int
ControlInterface::setOccurrences( Occurrences *presence,
				  Occurrences *absence )
{
  _presence = presence;
  _absence  = absence;

  return 1;
}


/***********/
/*** run ***/
int
ControlInterface::run()
{
  char *error = basicCheck();
  if ( error )
    {
      strcpy( f_error, error );
      return 0;
    }

  g_log( "Modelling occurrences of: %s.\n", _presence->name() );

  // Environmental variables.
  char **categs = _layers;
  char **maps   = categs + _ncateg;
  int  nmaps    = _nlayers - _ncateg;
  Environment *env = new Environment( GeoTransform::cs_default,
				      _ncateg, categs,
				      nmaps, maps, _mask );

  g_log( "Environment initialized.\n" );

  // Sampler and algorithm.
  Sampler samp( env, _presence, _absence );

  Algorithm *alg;
  alg = _factory->newAlgorithm( &samp, _alg_id, _alg_nparam,
                                _alg_param );

  if ( ! alg )
    {
      sprintf( f_error, "Could not find (%s) algorithm.", _alg_id );
      return 0;
    }


  // Check if the algorithm needs normalized variables.
  Scalar min, max;
  if ( alg->needNormalization( &min, &max ) )
    {
      g_log( "Normalizing environment variables.\n" );
      env->normalize( min, max );
    }

  g_log( "Creating the model\n" );

  // Generate the model.
  if ( ! createModel( alg, &samp ) )
    return 0;

  g_log( "Saving distribution's file:\n" );

  // Create the map with probabilities of occurence.
  if ( ! createMap( env, alg ) )
    return 0;

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
  // Presence occurrence points.
  if ( ! _presence )
    return "Presence occurrences points were not setted.";


  // Environmental data.
  if ( ! _nlayers )
    return "Environmental variables not specified.";


  // Algorithm.
  if ( ! _alg_id )
    return "Modeling algorithm not specified.";


  // Output map file name.
  if ( ! _file )
    return "The output file name not specified.";

  return 0;
}


/********************/
/*** create Model ***/
int
ControlInterface::createModel( Algorithm *alg, Sampler *samp )
{
  // Initialize algorithm.  
  if ( ! alg->initialize() )
    {
      sprintf( f_error, "Algorithm (%s) could not be initialized.",
	       alg->getID() );
      return 0;
    }

  // Generate model.
  int ncycle = 0;
  while ( alg->iterate() && ! alg->done() )
    ncycle++;

  return alg->done();
}


/******************/
/*** create Map ***/
int
ControlInterface::createMap( Environment *env, Algorithm *alg )
{
  if ( ! _hdr->hasProj() )
    {
      sprintf( f_error,
	       "Output map (%s) without a coordinate system!\n\n",
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

      g_log( "Line %04d / %4d : %+7.2f \r", ++row, _hdr->ydim,
	     sum / _hdr->xdim );
    }
  g_log( "\n" );

  delete amb;
  return 1;
}
