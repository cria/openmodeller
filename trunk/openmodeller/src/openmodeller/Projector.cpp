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

#include <om_projector.hh>
#include <om_log.hh>
#include <environment.hh>
#include <Model.hh>

#include <env_io/header.hh>
#include <env_io/map.hh>
#include <env_io/raster.hh>
#include <env_io/geo_transform.hh>

#include <om_area_stats.hh>

/******************/
/*** create Map ***/
void
Projector::createMap( const Model& model,
		      const EnvironmentPtr& env,
		      Map *map,
		      AreaStats *areaStats,
		      MapCommand *mapcommand )
{

  // Retrieve possible adjustments and/or additions made
  // on the effective header.
  Header hdr = map->getHeader();

#ifndef GEO_TRANSFORMATIONS_OFF
  if ( ! hdr.hasProj() )
    {
      throw;
    }
#endif

  // Normalize the environment.
  model->setNormalization( env );
 
  // Transformer used by the resulting map.
  GeoTransform *gt = map->getGT();

  // Dimension of environment space.
  int dim = env->numLayers();

  Coord lg, lt;
  Coord y0 = hdr.ymin; // + 0.5 * hdr.ycel;
  Coord x0 = hdr.xmin; // + 0.5 * hdr.xcel;

  float progress = 0.0;
  float progress_step = hdr.ycel / (hdr.ymax - y0);

  if ( areaStats ) {
    areaStats->reset();
  }

  for ( float y = y0; y < hdr.ymax; y += hdr.ycel ) {
    for ( float x = x0; x < hdr.xmax; x += hdr.xcel ) {
      // Transform coordinates (x,y) that are in the resulting
      // map system, in (lat, long) according to the system 
      // accepted by the environment (env).
      gt->transfOut( &lg, &lt, x, y );

      Sample const &amb = env->get( lg, lt );
      // TODO: use mask to check if pixel should contain prediction
      // Read environmental values and find the output value.
      if ( amb.size() == 0 ) {
	// Write noval on the map.
	map->put( lg, lt );
      }
      else {
	Scalar val = model->getValue( amb );
	if ( val < 0.0 ) val = 0.0;
	else if ( val > 1.0 ) val = 1.0;
	if ( areaStats ) {
	  areaStats->addPrediction( val ); 
	}
	// Write value on map.
	map->put( lg, lt, val );
      }

	  
    }
    // Call the callback function if it is set.
    if ( mapcommand ) {
      if ( (progress += progress_step) > 1.0 )
	progress = 1.0;
      try 
	{
	  (*mapcommand)( progress );
	}
      catch( ... ) {}
    }

  }
  
}

