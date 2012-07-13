/**
 * Definition of ViewModel class.
 *
 * This is an interface to held different graphic libraries.
 * 
 * @file
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-10-25
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

#include <view_model.hh>

#include <graphic.hh>
#include <openmodeller/Algorithm.hh>
#include <openmodeller/Sample.hh>

#include <stdio.h>
#include <stdlib.h>

#define Abs(x)      ((x) < 0 ? -(x) : x)

/****************************************************************/
/*************************** View Model *************************/

ViewModel *ViewModel::f_this = 0;


/******************/
/*** construtor ***/

ViewModel::ViewModel( AlgorithmPtr alg, char *name, int dim ) :
  f_alg( alg )
{
  if ( f_this )
    {
      printf( "You can only have one ViewModel object by process!\n" );
      exit( 1 );
    }
  f_this = this;

  f_frame = createFrame( name, 1, dim, 5*dim/4 );

  f_imap  = f_frame->newCanvas( 0, 0, dim, dim );
  f_ifunc = f_frame->newCanvas( 0, dim, dim, dim/2 );

  GImage *pm_top = f_frame->newPixmap( f_imap, dim, dim );
  GImage *pm_bot = f_frame->newPixmap( f_ifunc, dim, dim/4 );

  // Map.
  f_gmap = new GGraph( pm_top );

  // Extra function (eg: convergence) graphic visualization.
  f_cicle = 0;
  f_gfunc = new GGraph( pm_bot );
  funcScale( 0.0, -0.2, 10.0, 2.0 );

  // Zoom in and out with mouse buttons.
  f_zoom = 2.0;
  f_frame->funcBtPress( f_imap, zoomIn, 1 );
  f_frame->funcBtPress( f_imap, zoomOut, 3 );

  f_frame->funcAlways( funcDraw );
}


/*****************/
/*** destrutor ***/

ViewModel::~ViewModel()
{
  delete f_gmap;
  delete f_gfunc;
  delete f_frame;
}


/******************/
/*** func Scale ***/
void
ViewModel::funcScale( float xmin, float ymin, float xmax, float ymax )
{
  f_gfunc->scale( xmin, ymin, xmax, ymax );
  f_gfunc->clear();

  // Draw the axes in red.
  f_gfunc->foreground( GColor::Red );
  f_gfunc->line( xmin, 0.0, xmax, 0.0 );
  f_gfunc->line( 0.0, ymin, 0.0, ymax );
  f_gfunc->foreground( GColor::Black );
}


/************/
/*** exec ***/
int
ViewModel::exec()
{
  f_frame->exec();

  return 1;
}


/************/
/*** draw ***/
void
ViewModel::draw()
{
  // Coordinates of last plotted point in extra fuction window.
  // static double x0,y0;

  printf( "ViewModel::draw()\n" );

  // A new training has started.
  if ( ! f_cicle )
    {
      // x0 = y0 = 0.0;
      f_alg->initialize();
    }

  f_alg->iterate();

  // Draws the map.
  f_gmap->scale( -f_zoom, -f_zoom, f_zoom, f_zoom );
  drawMap( f_gmap );
  f_imap->put( f_gmap );

  /*
  // Extra function graphic.
  double func;
  if ( f_alg->getConvergence( &func ) )
    {
      f_gfunc->line( x0, y0, f_cicle, func );
      f_ifunc->put( f_gfunc );
      x0 = f_cicle;
      y0 = func;
    }
  */
  
  f_cicle++;
}


/****************/
/*** draw Map ***/
void
ViewModel::drawMap( GGraph *gr )
{
  float  val;
  GColor color;

  float contraste = 256.0;

  Sample coord(2);
  float x0 = gr->transfX( 0 );
  float y0 = gr->transfY( 0 );

  for ( float y = y0; y < gr->maxY(); y += gr->stepY() )
    for ( float x = x0; x < gr->maxX(); x += gr->stepX() )
      {
        coord[0] = x;
        coord[1] = y;
        val = f_alg->getValue( coord );

        // Red for zero and almost zero.
	if ( Abs( val ) < 0.001 )
	  color = GColor::Red;

        // Green por positives.
	else if ( val > 0.0 )
	  {
	    int tom = int( val * contraste );
	    if ( tom > 255 )
	      tom = 255;
	    color.set( 0, tom, 0 );
	  }

        // Blue for negatives
	else
	  {
	    int tom = int( -val * contraste );
	    if ( tom > 255 )
	      tom = 255;
	    color.set( 0, 0, tom );
	  }
	gr->pixel( x, y, color );
      }
}


/*****************/
/*** func Draw ***/
void
ViewModel::funcDraw()
{
  AlgorithmPtr alg = f_this->f_alg;

  if ( ! alg->done() )
    f_this->draw();
}


/***************/
/*** zoom In ***/
void
ViewModel::zoomIn( int x, int y )
{
  f_this->f_zoom /= 1.2;
  f_this->draw();

  printf( "ViewModel::zoomIn()\n" );
}


/****************/
/*** zoom Out ***/
void
ViewModel::zoomOut( int x, int y )
{
  f_this->f_zoom *= 1.2;
  f_this->draw();

  printf( "ViewModel::zoomOut()\n" );
}
