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

#include <environment.hh>
#include <occurrences_file.hh>

#include <algorithms/distance.hh>
#include <algorithms/dist_min.hh>
#include <algorithms/bioclim.hh>
#include <algorithms/csm.hh>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Log _log( Log::Info );
/****************************************************************/
/************************* Control Interface ********************/

/********************/
/*** constructors ***/

ControlInterface::ControlInterface()
{
  f_hdr = new Header;

  f_nlayers = 0;
  f_layers  = 0;
  f_mask    = 0;

  f_file  = 0;
  f_mult  = 1.0;
  f_alg   = 0;
  f_param = 0;

  f_oc_file = 0;
  f_oc_cs   = 0;
  f_oc_name = 0;

  f_error = 0;
}

ControlInterface::ControlInterface( int ncateg, int nlayer,
				    char **layers, char *mask )
{
  f_hdr = new Header;

  f_layers = 0;

  f_file = 0;
  f_mult = 1.0;
  f_alg  = 0;
  f_param = 0;

  f_oc_file = 0;
  f_oc_cs   = 0;
  f_oc_name = 0;

  f_error = 0;

  setEnvironment( ncateg, nlayer, layers, mask );
}


/******************/
/*** destructor ***/

ControlInterface::~ControlInterface()
{
  delete f_hdr;

  if ( f_layers )  delete f_layers;
  if ( f_file )    delete f_file;
  if ( f_alg )     delete f_alg;
  if ( f_oc_file ) delete f_oc_file;
  if ( f_oc_cs )   delete f_oc_cs;
  if ( f_oc_name ) delete f_oc_name;
}


/***********************/
/*** set Environment ***/
void
ControlInterface::setEnvironment( int ncateg, int nlayer,
				  char **layers, char *mask )
{
  f_ncateg = ncateg;
  stringCopy( &f_mask, mask );

  // Reallocate vector that stores the names of the layers.
  if ( f_layers )
    delete[] f_layers;
  f_nlayers = nlayer;
  f_layers = new char*[f_nlayers];

  for ( int i = 0; i < f_nlayers; i++ )
    {
      f_layers[i] = 0;
      stringCopy( f_layers + i, layers[i] );
    }
}


/**********************/
/*** set Output Map ***/
void
ControlInterface::setOutputMap( char *file, Header *hdr,
				Scalar mult )
{
  stringCopy( &f_file, file );
  *f_hdr = *hdr;
  f_mult = mult;
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
ControlInterface::setAlgorithm( char *alg, char *param )
{
  stringCopy( &f_alg, alg );
  stringCopy( &f_param, param );
}


/*******************/
/*** set Species ***/
void
ControlInterface::setOccurrences( char *file, char *cs, char *oc )
{
  stringCopy( &f_oc_file, file );
  stringCopy( &f_oc_cs,   cs );
  stringCopy( &f_oc_name, oc );
}


/***********/
/*** run ***/
int
ControlInterface::run()
{
  if ( f_error = basicCheck() )
    return 0;

  // Ocurrence points.
  Occurrences *presence = readOccurrences( f_oc_file, f_oc_cs,
					   f_oc_name );

  // Must implement the reading of absence points.
  Occurrences *absence = 0;

  if ( ! presence )
    {
      f_error = "Occurrences not found with the given name.";
      return 0;
    }
  _log.info( "Modelling occurrences of: %s.\n", presence->name() );

  // Environmental variables.
  char **categs = f_layers;
  char **maps = categs + f_ncateg;
  int nmaps   = f_nlayers - f_ncateg;
  Environment *env = new Environment( GeoTransform::cs_default,
				      f_ncateg, categs,
				      nmaps, maps, f_mask );

  _log.info( "Environment initialized.\n" );

  // Sampler and algorithm.
  //Sampler samp( env, presence, absence );
  Sampler samp( presence, env );
  Algorithm *alg = algorithmFactory( &samp, f_alg, f_param );
  if ( ! alg )
    {
      f_error = "Could not find an algorithm with the given name.";
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
  if ( ! createModel( alg, &samp, f_ncycle ) )
    {
      f_error = "Model creation error.";
      return 0;
    }

  _log.info( "Saving distribution's file:\n" );

  // Create the map with probabilities of occurence.
  if ( ! createMap( env, alg ) )
    {
      f_error = "Occurrence probability map creation error.";
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
  if ( ! f_oc_file )
    return "Occurrences file not found and/or not specified.";

  if ( ! f_oc_cs )
    return "Occurrences' coordinate system not specified.";


  if ( ! f_nlayers )
    return "Environmental variables not specified.";


  // Algoritmo.
  if ( ! f_alg )
    return "Modeling algorithm not specified.";


  // Mapa de saída.
  if ( ! f_file )
    return "The output file name not specified.";

  return 0;
}


/************************/
/*** read Occurrences ***/
Occurrences *
ControlInterface::readOccurrences( char *file, char *cs, char *name )
{
  OccurrencesFile lsp( file, cs, GeoTransform::cs_default );

  // Take last species from the list, which corresponds to the first 
  // inside the file.
  if ( ! name )
    {
      lsp.tail();
      name = lsp.get()->name();
    }

  return lsp.remove( name );
}


/*************************/
/*** algorithm Factory ***/
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


/********************/
/*** create Model ***/
int
ControlInterface::createModel( Algorithm *alg, Sampler *samp, int max )
{
  // Initialize algorithm.  
  if ( ! alg->initialize( 1 ) )
    _log.error( 1, "Algorithm %s could not be initialized.\n",
		alg->name() );

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
  if ( ! f_hdr->hasProj() )
    {
      _log.error( 1,
		  "Output map without a coordinate system!\n\n",
		  f_file );
      return 0;
    }

  // Force noval = 0
  f_hdr->noval = 0.0;
 
  // Create map on disc.
  RasterFile rst( f_file, *f_hdr );
  Map map( &rst , env->getCoordinateSystem() );

  // Retrieve possible adjustments and/or additions made
  // on the the effective header.
  *f_hdr = rst.header();


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
  Coord y0 = f_hdr->ymin; // + 0.5 * f_hdr.ycel;
  Coord x0 = f_hdr->xmin; // + 0.5 * f_hdr.xcel;

  int row = 0;
  for ( float y = y0; y < f_hdr->ymax; y += f_hdr->ycel )
    {
      int col = 0;
      Scalar sum = 0.0;
      for ( float x = x0; x < f_hdr->xmax; x += f_hdr->xcel )
	{
	  // Transform coordinates (x,y) that are in the resulting
	  // map system, in (lat, long) according to the system 
	  // accepted by the environment (env).
	  gt->transfOut( &lg, &lt, x, y );


	  // Read environmental values and find the output value.
	  if ( env->get( lg, lt, amb ) )
	    val = f_mult * alg->getValue( amb );
	  else
	    val = f_hdr->noval;

	  // Write value on map.
	  map.put( lg, lt, &val );

	  // Sum of line values.
	  sum += val;
	}

      _log.info( "Line %04d / %4d : %.2f          \r",
		 ++row, f_hdr->ydim, sum / f_hdr->xdim );
    }
  _log.info( "\n" );

  delete amb;
  return 1;
}
