/**
 * Simple species niche visualizer.
 * 
 * @author Mauro E S Muñoz (mauro@cria.org.br)
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

#include <openmodeller/om.hh>

#include "graph/graphic.hh"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <stdexcept>

#include <string>
using std::string;

float _zoom;
int   _redraw    = 1;
int   _last_draw = 0;

GColor _bg( 0, 140, 150 );

int   _nmap;
Map **_maps;

OccurrencesPtr _presences;
OccurrencesPtr _absences;

OpenModeller _om;
GImage  *_cnv;
GImage  *_pm;
GGraph *_graph;

void draw();
void draw_niche( GGraph *graph );
void draw_occur( GGraph *graph, const OccurrencesPtr& oc, GColor color );



/**************************************************************/
/**************************** main ****************************/

int
main( int argc, char **argv )
{
  // Reconfigure the global logger.
  Log::instance()->setLevel( Log::Error );
  Log::instance()->setPrefix( "" );

  if ( argc < 2 ) {

    Log::instance()->error( "\n%s <file with serialized model>\n\n", argv[0] );
    exit(1);
  }

  char *model_file = argv[1];

  Log::instance()->info( "\nopenModeller Two-dimensional Niche Viewer - CRIA\n" );

  try {

    AlgorithmFactory::searchDefaultDirs();

    ConfigurationPtr input = Configuration::readXml( model_file );

    _om.setModelConfiguration( input );

    EnvironmentPtr env = _om.getEnvironment();

    // Normalize environment if necessary
    // IMPORTANT: this must be done before calling "getExtremes" below, 
    // otherwise min/max won't be normalized.
    
    AlgorithmPtr alg = _om.getAlgorithm();

    alg->setNormalization( env );

    // Get environment info
    int nmap = env->numLayers();

    Sample min;
    Sample max;

    env->getExtremes( &min, &max );

    if ( nmap < 2 ) {

      Log::instance()->error( "Need more than one environmental variable!\n" );
      exit(2);
    }
    else if ( nmap > 2 ) {

      Log::instance()->error( "Maximum number of environmental variables (2) exceeded!\n" );
      exit(3);
    }

    // Occurrences
    SamplerPtr samp = _om.getSampler();

    _presences = samp->getPresences();
    _absences = samp->getAbsences();

    // Instantiate graphical window
    int dimx = 256;
    int dimy = 256;
    GFrame *frame = createFrame( "openModeller Niche Viewer", 1, dimx, dimy );
    _cnv = frame->newCanvas( 0, 0, dimx, dimy );
    _pm  = frame->newPixmap( _cnv, dimx, dimy );

    // Drawing area
    _graph = new GGraph( _pm );
    _graph->scale( min[0], min[1], max[0], max[1] );
    _graph->background( _bg );
    _graph->clear();

    // Zoom in and out with mouse buttons
    _zoom = 2.0;

    frame->funcShow( draw );

    frame->exec();

    delete _graph;
    delete frame;
  }
  catch ( std::exception& e ) {

    Log::instance()->error( "Exception occurred: %s\n", e.what() );
  }
  catch ( ... ) {

    Log::instance()->error( "Unknown Error occurred\n" );
  }

  return 0;
}


/**************************************************************/

/************/
/*** draw ***/
void
draw()
{
  if ( _redraw ) {

    draw_niche( _graph );
    draw_occur( _graph, _presences, GColor::Green );
    draw_occur( _graph, _absences, GColor::Red );

    _redraw = 0;
  }

  _cnv->put( _graph );
}


/******************/
/*** draw niche ***/
void
draw_niche( GGraph *graph )
{
  Scalar xmin  = graph->minX();
  Scalar ymin  = graph->minY();
  Scalar xmax  = graph->maxX();
  Scalar ymax  = graph->maxY();
  Scalar xstep = graph->stepX();
  Scalar ystep = graph->stepY();

  int i = 0;
  Scalar amb[2];
  Scalar *x = amb;
  Scalar *y = amb + 1;

  for ( *y = ymin; *y < ymax; *y += ystep ) {

    for ( *x = xmin; *x < xmax; *x += xstep ) {

      GColor color = GColor::Blue;
      color.scale( _om.getValue( amb ) );
      graph->pixel( float(*x), float(*y), color );
    }

    if ( ! ( ++i % 10 ) ) {

	_cnv->put( graph );
    }
  }
}


/******************/
/*** draw occur ***/
void
draw_occur( GGraph *graph, const OccurrencesPtr& occurs, GColor color )
{
  if ( occurs && occurs->numOccurrences() ) {

    // Draw each set of occurrences.
    EnvironmentPtr env = _om.getEnvironment();
    Sample amb;
    OccurrencesImpl::const_iterator oc = occurs->begin();

    for ( ; oc != occurs->end(); ++oc ) {

      amb = env->get( (*oc)->x(), (*oc)->y() );

      if ( amb.size() >= 2 ) {

        graph->markAxe( amb[0], amb[1], 1, color );
      }
    }
  }
}

