/**
 * Simple map/occurrences viewer.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-10-09
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

#include <om.hh>

#include <env_io/map.hh>
#include <env_io/geo_transform.hh>
#include <env_io/raster_file.hh>
#include <occurrences_file.hh>
#include <occurrence.hh>
#include <file_parser.hh>

#include <graph/graphic.hh>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


float _zoom;
int   _redraw    = 1;
int   _last_draw = 0;

GColor _bg( 0, 140, 150 );

int   _nmap;
Map **_maps;

Occurrences *_occurs;

GImage  *_cnv;
GImage  *_pm;
GGraph *_graph;


void draw();
void draw_map( GGraph *graph, Map *map );
void draw_occur( GGraph *graph, Map *map, Occurrences * );
void findRegion( int nmap, Map **map, Coord *xmin, Coord *ymin,
		 Coord *xmax, Coord *ymax );
Occurrences *readOccurrences( char *file, char *name,
			      char *coord_system );


/**************************************************************/
/**************************** main ****************************/

int
main( int argc, char **argv )
{
  if ( argc < 2 )
    {
      printf( "\n%s [-r] <request>\n\n", argv[0] );
      exit( 1 );
    }
  printf( "\nopenModeller Viewer - CRIA\n" );

  int show_map  = strcmp( argv[1], "-r" );
  char *request = (show_map ? argv[1] : argv[2]);

  FileParser fp( request );

  Raster **rst;
  if ( show_map )
    {
      // Maps to be shown.
      _nmap = fp.count( "Map" );

      if ( ! _nmap )
	_log.error( 1, "No map to be shown!?!\n" );

      _maps = new (Map *)[_nmap];
      rst = new (Raster *)[_nmap];
      char *mapfile[_nmap];
      fp.getAll( "Map", mapfile );
      
      for ( int i = 0; i < _nmap; i++ )
	{
	  // Generate a raster using map "i".
	  rst[i]   = new RasterFile( mapfile[i] );
	  _maps[i] = new Map( rst[i], GeoTransform::cs_default );
	  _maps[i]->normalize( 0.0, 255.0 );
	}
    }

  // Visualize result.
  else
    {
      _nmap = 1;
      _maps = new (Map *)[_nmap];
      char *result = fp.get( "Output" );
      rst = new (Raster *)[_nmap];
      rst[0] = new RasterFile( result );
      _maps[0] = new Map( rst[0], GeoTransform::cs_default );
      _maps[0]->normalize( 0.0, 255.0 );
    }

  // Region to be visualized. Must include all maps.
  Coord xmin, ymin, xmax, ymax;
  findRegion( _nmap, _maps, &xmin, &ymin, &xmax, &ymax );

  // Image dimensions.
  int dimx = 1024;
  int dimy = int( dimx * (ymax - ymin) / (xmax - xmin) );
  if ( dimy > 700 )
    {
      dimy = 700;
      dimx = int( dimy * (xmax - xmin) / (ymax - ymin) );
    }
  printf( "Dimensions: %d x %d\n", dimx, dimy );


  // Occurrences file.
  char *oc_cs   = fp.get( "WKT Coord System" );
  char *oc_file = fp.get( "Species file" );
  char *oc_name = fp.get( "Species" );
  _occurs = readOccurrences( oc_file, oc_name, oc_cs );


  // Instantiate graphical window.
  GFrame *frame = createFrame( "openModeller Viewer", 1, dimx, dimy );
  _cnv = frame->newCanvas( 0, 0, dimx, dimy );
  _pm  = frame->newPixmap( _cnv, dimx, dimy );

  // Drawing area.
  _graph = new GGraph( _pm );
  _graph->scale( float(xmin), float(ymin), float(xmax),
		 float(ymax) );
  _graph->background( _bg );
  _graph->clear();


  // Zoom in and out with mouse buttons.
  _zoom = 2.0;
  //  frame->FuncBtPress( _cnv, set_float_coord, 1 );
  //  frame->FuncBtPress( _cnv, set_int_coord,  3 );
  frame->funcShow( draw );

  frame->exec();

  delete _graph;
  delete frame;

  delete[] _maps;
  delete[] rst;
}


/************/
/*** draw ***/
void
draw()
{
  if ( _redraw )
    {
      for ( int i = 0; i < _nmap; i++ )
	{
	  draw_map( _graph, _maps[i] );
	  draw_occur( _graph, _maps[i], _occurs );
	}

      _redraw = 0;
    }

  _cnv->put( _graph );
}


/****************/
/*** draw map ***/
void
draw_map( GGraph *graph, Map *map )
{
  Coord xmin  =	graph->minX();
  Coord ymin  =	graph->minY();
  Coord xmax  =	graph->maxX();
  Coord ymax  =	graph->maxY();
  Coord xstep =	graph->stepX();
  Coord ystep =	graph->stepY();

  Scalar val[ map->numBand() ];

  int i = 0;
  GColor color;
  for ( Coord y = ymin; y < ymax; y += ystep )
    {
      for ( Coord x = xmin; x < xmax; x += xstep )
	if ( map->get( x, y, val ) )
	  {
	    color = int( *val );
	    graph->pixel( float(x), float(y), color );
	  }


      if ( ! (++i % 10) )
	_cnv->put( graph );
    }
}


/******************/
/*** draw occur ***/
void
draw_occur( GGraph *graph, Map *map, Occurrences *occurs )
{
  GColor color = GColor::Red;

  // Draw each set of occurrences.
  float x, y;
  Occurrence *oc;
  for ( occurs->head(); oc = occurs->get(); occurs->next() )
    {
      //	  gt->transfIn( &x, &y, occur->x, occur->y );
      x = float( oc->x() );
      y = float( oc->y() );

      graph->markAxe( x, y, 1, color);
    }
}


/*******************/
/*** find Region ***/
void
findRegion( int nmap, Map **map, Coord *xmin, Coord *ymin,
	    Coord *xmax, Coord *ymax )
{
  map[0]->getRegion( xmin, ymin, xmax, ymax );
}


/************************/
/*** read Occurrences ***/
Occurrences *
readOccurrences( char *file, char *name, char *coord_system )
{
  OccurrencesFile oc_file( file, coord_system );

  // Take last species from the list, which corresponds to the
  // first inside the file.
  if ( ! name )
    {
      oc_file.tail();
      name = oc_file.get()->name();
    }

  return oc_file.remove( name );
}
