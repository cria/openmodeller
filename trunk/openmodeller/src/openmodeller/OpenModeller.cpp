/**
 * Definition of OpenModeller class (former ControlInterface class).
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
#include <map_format.hh>

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

OpenModeller::OpenModeller()
{
  _factory = new AlgorithmFactory( g_search_dirs );

  _env = 0;
  _alg = 0;
  _samp = 0;

  _alg_id     = 0;
  _alg_param  = 0;
  _alg_nparam = 0;

  _presence = 0;
  _absence  = 0;

  f_error[0] = '\0';
}


/******************/
/*** destructor ***/

OpenModeller::~OpenModeller()
{
  /*
  delete _factory;

  if ( _samp )     delete _samp;
  //  if ( _alg )      delete _alg;
  if ( _env )      delete _env;
  if ( _alg_id )   delete _alg_id;
  if ( _presence ) delete _presence;
  if ( _absence )  delete _absence;
  */
}


/*******************/
/*** get Version ***/
char *
OpenModeller::getVersion()
{
  // Defined in "om_defs.hh".
  return OM_VERSION;
}


char * 
OpenModeller::getPluginPath()
{
  return PLUGINPATH;
}


/****************************/
/*** load Algorithms ***/
int 
OpenModeller::loadAlgorithms()
{
  return _factory->loadAlgorithms();
}

/****************************/
/*** available Algorithms ***/
AlgMetadata **
OpenModeller::availableAlgorithms()
{
  return _factory->availableAlgorithms();
}


/**************************/
/*** algorithm Metadata ***/
AlgMetadata *
OpenModeller::algorithmMetadata( char *algorithm_id )
{
  return _factory->algorithmMetadata( algorithm_id );
}


/********************************/
/*** num Available Algorithms ***/
int
OpenModeller::numAvailableAlgorithms()
{
  return _factory->numAvailableAlgorithms();
}


/***********************/
/*** set Environment ***/
int
OpenModeller::setEnvironment( int num_categ,
			      char **categ_map,
			      int num_continuous,
			      char **continuous_map,
			      char *mask )
{
  // set up environmental variables.
  if (_env)
    delete _env;

  _env = new Environment( GeoTransform::cs_default,
			 num_categ, categ_map,
			 num_continuous, continuous_map, mask );

  g_log( "Environment initialized.\n" );

  return 1;
}

/*********************/
/*** set Algorithm ***/
int
OpenModeller::setAlgorithm( char *id, int nparam, AlgParameter *param )
{
  AlgMetadata *meta = algorithmMetadata( id );

  // Check the parameters.
  if ( ! meta || meta->nparam != nparam )
    {
      if (!meta)
	g_log("Can't get metadata for algorithm %s", id);

      if (meta->nparam != nparam)
	g_log("Number of parameters provided (%d) does not match required parameters (%d)", 
	      nparam, meta->nparam);

      return 0;
    }

  // Check if _env is initialized
  if ( !_env)
    {
      g_log("Environmental variables not initialized.");
      return 0;
    }

  // Check if occurrence data is initialized
  if ( !_presence)
    {
      g_log("Occurrence data not initialized.");
      return 0;
    }

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

  // Sampler and algorithm.
  if (_samp)
    delete _samp;

  _samp = new Sampler( _env, _presence, _absence );

  if (_alg)
    delete _alg;

  _alg = _factory->newAlgorithm( _samp, _alg_id, _alg_nparam, _alg_param );

  if ( ! _alg )
    {
      sprintf( f_error, "Could not find (%s) algorithm.", _alg_id );
      return 0;
    }

  return 1;
}


/***********************/
/*** set Occurrences ***/
int
OpenModeller::setOccurrences( Occurrences *presence,
				  Occurrences *absence )
{
  _presence = presence;
  _absence  = absence;

  return 1;
}


/***********/
/*** run ***/
int
OpenModeller::run()
{
  char *error = basicCheck();
  if ( error )
    {
      strcpy( f_error, error );
      return 0;
    }

  g_log( "Modelling occurrences of: %s.\n", _presence->name() );

  // Check if the algorithm needs normalized variables.
  Scalar min, max;
  if ( _alg->needNormalization( &min, &max ) )
    {
      g_log( "Normalizing environment variables.\n" );
      _env->normalize( min, max );
    }

  g_log( "Creating the model\n" );

  // Generate the model.
  if ( ! createModel( _alg, _samp ) )
    return 0;

  return 1;
}


/*******************/
/*** string Copy ***/
void
OpenModeller::stringCopy( char **dst, char *src )
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
OpenModeller::basicCheck()
{
  // Presence occurrence points.
  if ( ! _presence )
    return "Presence occurrences points were not set.";

  // Sampler
  if ( ! _samp )
    return "Sampler not specified.";

  // Environmental data.
  if ( ! _env )
    return "Environmental variables not specified.";


  // Algorithm.
  if ( ! _alg_id || ! _alg )
    return "Modeling algorithm not specified.";

  return 0;
}


/********************/
/*** create Model ***/
int
OpenModeller::createModel( Algorithm *alg, Sampler *samp )
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
OpenModeller::createMap( Environment *env, char *file, Scalar mult,
			 char * mask, char *map_file )
{
  RasterFile map( map_file );
  return createMap( env, file, mult, mask, &map.header() );
}

/******************/
/*** create Map ***/
int
OpenModeller::createMap( Environment *env, char *file, Scalar mult,
			 char * mask, MapFormat * format )
{
  Header hdr(format->getWidth(), format->getHeight(),
	     format->getXMin(),  format->getYMin(),
	     format->getXMax(),  format->getYMax(),
	     format->getNoDataValue() );

  hdr.setProj( format->getProjection() );

  return createMap( env, file, mult, mask, &hdr );
}

/******************/
/*** create Map ***/
int
OpenModeller::createMap( Environment *env, char *file, Scalar mult, 
			 char * mask, Header *hdr )
{
  if (!_alg)
    {
      sprintf( f_error, 
	       "Algorithm object is empty!\n\n",
	       file );
      return 0;
    }
  else if (!_alg->done())
    {
      sprintf( f_error, 
	       "Algorithm model is not finished yet!\n\n",
	       file );
      return 0;
    }

  if ( ! hdr->hasProj() )
    {
      sprintf( f_error, 
	       "Output map (%s) without a coordinate system!\n\n",
	       file );
      return 0;
    }

  // check if env object is original one (used to create model) or
  // is a different one (caller wants to project model onto it)
  if (env != _env)
    {
      // env objects are not the same, so copy normalization parameters
      // from original source and procced with projection

      g_log("Preparing target environment object for projection.");

      Scalar min, max;
      if ( _alg->needNormalization( &min, &max ) )
	{
	  g_log( "Normalizing environment variables on projection Environment object.\n" );
	  env->copyNormalizationParams(_env);
	}

      /*
      if (!env->copyNormalizationParams(_env))
	{
	  sprintf( f_error, 
		   "Coult not set normalization parameters to projection environment object\n\n",
		   file );
	}
	return 0;
      */
    }
  else
    {
      g_log("Native range projection (using original env object)");
    }

  // Force noval = 0
  hdr->noval = 0.0;
 
  // Create map on disc.
  RasterFile rst( file, *hdr );
  Map map( &rst , env->getCoordinateSystem() );

  // Retrieve possible adjustments and/or additions made
  // on the the effective header.
  *hdr = rst.header();


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
  Coord y0 = hdr->ymin; // + 0.5 * hdr.ycel;
  Coord x0 = hdr->xmin; // + 0.5 * hdr.xcel;

  int row = 0;
  for ( float y = y0; y < hdr->ymax; y += hdr->ycel )
    {
      int col = 0;
      Scalar sum = 0.0;
      for ( float x = x0; x < hdr->xmax; x += hdr->xcel )
	{
	  // Transform coordinates (x,y) that are in the resulting
	  // map system, in (lat, long) according to the system 
	  // accepted by the environment (env).
	  gt->transfOut( &lg, &lt, x, y );


	  // TODO: use mask to check if pixel should contain prediction
	  // Read environmental values and find the output value.
	  if ( ! env->get( lg, lt, amb ) )
	    val = hdr->noval;
	  else
	    {
	      val = _alg->getValue( amb );
	      if ( val < 0.0 ) val = 0.0;
	      if ( val > 1.0 ) val = 1.0;
	      val *= mult;
	    }

	  // Write value on map.
	  map.put( lg, lt, &val );

	  // Sum of line values.
	  sum += val;
	}

      g_log( "Line %04d / %4d : %+7.2f \r", ++row, hdr->ydim,
	     sum / hdr->xdim );
    }
  g_log( "\n" );

  delete amb;
  return 1;
}
