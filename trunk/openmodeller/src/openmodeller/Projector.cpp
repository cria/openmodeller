/**
 * Definition of OpenModeller class (former ControlInterface class).
 * 
 * @author Mauro E S Mu�z <mauro@cria.org.br>
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

#include <openmodeller/AbortionCommand.hh>
#include <openmodeller/Projector.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/Model.hh>

#include <openmodeller/env_io/Header.hh>
#include <openmodeller/env_io/Map.hh>
#include <openmodeller/env_io/Raster.hh>
#include <openmodeller/env_io/GeoTransform.hh>

#include <openmodeller/AreaStats.hh>

#include <utility>
using std::pair;

/******************/
/*** create Map ***/
bool
Projector::createMap( const Model& model,
		      const EnvironmentPtr& env,
		      Map *map,
		      AreaStats *areaStats,
		      MapCommand *mapcommand,
		      AbortionCommand *abortcommand )
{

  // Retrieve possible adjustments and/or additions made
  // on the effective header.
  Header hdr = map->getHeader();

#ifndef GEO_TRANSFORMATIONS_OFF
  if ( ! hdr.hasProj() ) {

    throw;
  }
#endif

  // Normalize the environment.
  model->setNormalization( env );
 
  if ( areaStats ) {
    areaStats->reset();
  }

  MapIterator fin;
  MapIterator it = map->begin();

  int pixels = 0;
  int pixelcount = hdr.ydim * hdr.xdim;
  int pixelstep = pixelcount/20;
  bool abort = false;

  while( it != fin ) {

    // Call the abort callback function if it is set.
    if ( abortcommand && pixels%pixelstep == 0 ) {

      try {

	  abort = (*abortcommand)();

          if ( abort ) {

            Log::instance()->info( "Projection aborted." );

            if ( ! map->deleteRaster() ) {

              Log::instance()->warn( "Could not delete map file." );
            }

            return false;
          }
      }
      catch( ... ) {}
    }
    
    pair<Coord,Coord> lonlat = *it;

    pixels++;
    ++it;

    Coord lg = lonlat.first;
    Coord lt = lonlat.second;

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

    
    // Call the callback function if it is set.
    if ( mapcommand && pixels%pixelstep == 0 ) {

      float progress = pixels/(float)pixelcount;
      if ( progress > 1.0 )
	progress = 1.0;

      try {

	  (*mapcommand)( progress );
      }
      catch( ... ) {}
    }
    
  }
  
  // Call the callback function if it is set.
  if ( mapcommand ) {
    try 
      {
	(*mapcommand)( 1.0 );
      }
    catch( ... ) {}
    }

  return true;
}

