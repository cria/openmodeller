/**
 * Declarations of GImage, GGraph and GFrame classes.
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

#include "graphic.hh"
#include "color.hh"


/**********************************************************************/
/******************************** GImage ******************************/

/*******************/
/*** constructor ***/

GImage::GImage( int dimx, int dimy )
{
  f_dimx = dimx;
  f_dimy = dimy;
}


/******************/
/*** destructor ***/

GImage::~GImage()
{
}


/***********/
/*** put ***/
void
GImage::put( GGraph *gr )
{
  put( gr->getImage() );
}




/**********************************************************************/
/******************************* GGraph *******************************/

/*******************/
/*** constructor ***/

GGraph::GGraph( GImage *imag )
{
  f_imag = imag;
  f_x0   = 0.0;
  f_y0   = 0.0;
  f_dx   = float( f_imag->dimX() );
  f_dy   = float( f_imag->dimY() );
}


/******************/
/*** destructor ***/

GGraph::~GGraph()
{
}


/*************/
/*** scale ***/
void
GGraph::scale( float x0, float y0, float x1, float y1 )
{
  f_x0 = x0;
  f_y0 = y0;
  f_dx = x1 - x0;
  f_dy = y1 - y0;
}


/************/
/*** text ***/
void
GGraph::text( float x, float y, char *text, GColor &c )
{
  f_imag->text( convX(x), convY(y), text, c );
}


/*************/
/*** pixel ***/
void
GGraph::pixel( float x, float y )
{
  f_imag->pixel( convX( x ), convY( y ) );
}


/************/
/*** line ***/
void
GGraph::line( float x0, float y0, float x1, float y1 )
{
  f_imag->line( convX( x0 ), convY( y0 ),
		convX( x1 ), convY( y1 ) );
}


/*************/
/*** lines ***/
void
GGraph::lines( float *pnt, int npnt )
{
  int pontos[npnt<<1];
  int *dst = pontos;
  int *end = dst + (npnt<<1);
  while ( dst < end )
    {
      *dst++ = convX( *pnt++ );
      *dst++ = convY( *pnt++ );
    }

  f_imag->lines( pontos, npnt );
}


/*************/
/*** polyg ***/
void
GGraph::polyg( float *pnt, int npnt )
{
  int pontos[ 2 + (npnt << 1) ];
  int *dst = pontos;
  int *end = dst + (npnt << 1);
  while ( dst < end )
    {
      *dst++ = convX( *pnt++ );
      *dst++ = convY( *pnt++ );
    }

  // Ligacao entre os ultimos e os primeiros.
  *dst++ = pontos[0];
  *dst   = pontos[1];

  f_imag->lines( pontos, npnt+1 );
}


/**************/
/*** fPolyg ***/
void
GGraph::fPolyg( float *pnt, int npnt )
{
  int pontos[npnt<<1];
  int *dst = pontos;
  int *end = dst + (npnt<<1);
  while ( dst < end )
    {
      *dst++ = convX( *pnt++ );
      *dst++ = convY( *pnt++ );
    }

  f_imag->fPolyg( pontos, npnt );
}


/************/
/*** rect ***/
void
GGraph::rect( float x0, float y0, float w, float h )
{
  int width  = int( f_imag->dimX() * w / f_dx );
  int height = int( f_imag->dimY() * h / f_dy );
  f_imag->rect( convX(x0), convY(y0) - height, width, height );
}


/*************/
/*** fRect ***/
void
GGraph::fRect( float x0, float y0, float w,  float h )
{
  int width  = int( f_imag->dimX() * w / f_dx );
  int height = int( f_imag->dimY() * h / f_dy );
  f_imag->fRect( convX(x0), convY(y0) - height, width, height );
}


/**************/
/*** circle ***/
void
GGraph::circle( float x0, float y0, float raio )
{
  elipse( x0, y0, raio, raio );
}


/***************/
/*** fCircle ***/
void
GGraph::fCircle( float x0, float y0, float raio )
{
  fElipse( x0, y0, raio, raio );
}


/**************/
/*** elipse ***/
void
GGraph::elipse( float x0, float y0, float rx, float ry )
{
  int raiox  = int( f_imag->dimX() * rx / f_dx );
  int raioy  = int( f_imag->dimY() * ry / f_dy );
  f_imag->elipse( convX( x0 ), convY( y0 ), raiox, raioy );
}


/***************/
/*** fElipse ***/
void
GGraph::fElipse( float x0, float y0, float rx, float ry )
{
  int raiox  = int( f_imag->dimX() * rx / f_dx );
  int raioy  = int( f_imag->dimY() * ry / f_dy );
  f_imag->fElipse( convX( x0 ), convY( y0 ), raiox, raioy );
}


/************************ with color ************************/

/*************/
/*** pixel ***/
void
GGraph::pixel( float x, float y, GColor &c )
{
  f_imag->pixel( convX( x ), convY( y ), c );
}


/************/
/*** line ***/
void
GGraph::line( float x0, float y0, float x1, float y1,
		GColor &c )
{
  f_imag->line( convX(x0), convY(y0), convX(x1), convY(y1), c );
}


/*************/
/*** lines ***/
void
GGraph::lines( float *pnt, int npnt, GColor &c )
{
  foreground( c );
  lines( pnt, npnt );
}


/**************/
/*** polyg ***/
void
GGraph::polyg( float *pnt, int npnt, GColor &c )
{
  foreground( c );
  polyg( pnt, npnt );
}


/**************/
/*** fPolyg ***/
void
GGraph::fPolyg( float *pnt, int npnt, GColor &c )
{
  foreground( c );
  fPolyg( pnt, npnt );
}


/************/
/*** rect ***/
void
GGraph::rect( float x0, float y0, float w, float h, GColor &c )
{
  int width  = int( f_imag->dimX() * w / f_dx );
  int height = int( f_imag->dimY() * h / f_dy );
  f_imag->rect( convX(x0), convY(y0)-height, width, height, c );
}


/*************/
/*** fRect ***/
void
GGraph::fRect( float x0, float y0, float w, float h, GColor &c )
{
  int width  = int( f_imag->dimX() * w / f_dx );
  int height = int( f_imag->dimY() * h / f_dy );
  f_imag->fRect( convX(x0), convY(y0)-height, width, height, c);
}


/**************/
/*** circle ***/
void
GGraph::circle( float x0, float y0, float raio, GColor &c )
{
  elipse( x0, y0, raio, raio, c );
}


/***************/
/*** fCircle ***/
void
GGraph::fCircle( float x0, float y0, float raio, GColor &c )
{
  fElipse( x0, y0, raio, raio, c );
}


/**************/
/*** elipse ***/
void
GGraph::elipse( float x0, float y0, float rx, float ry,
		  GColor &c )
{
  int raiox  = int( f_imag->dimX() * rx / f_dx );
  int raioy  = int( f_imag->dimY() * ry / f_dy );
  f_imag->elipse( convX( x0 ), convY( y0 ), raiox, raioy, c );
}


/***************/
/*** fElipse ***/
void
GGraph::fElipse( float x0, float y0, float rx, float ry,
		   GColor &c )
{
  int raiox  = int( f_imag->dimX() * rx / f_dx );
  int raioy  = int( f_imag->dimY() * ry / f_dy );
  f_imag->fElipse( convX( x0 ), convY( y0 ), raiox, raioy, c );
}


/*******************/
/*** mark Square ***/
void
GGraph::markSquare( float x, float y, int size, GColor &c )
{
  int dim = 2 * size + 1;
  f_imag->rect( convX(x)-size, convY(y)-size, dim, dim, c );
}


/***********************/
/*** mark Fill Square ***/
void
GGraph::markFSquare( float x, float y, int size, GColor &c )
{
  int dim = 2 * size + 1;
  f_imag->fRect( convX(x)-size, convY(y)-size, dim, dim, c );
}


/******************/
/*** mark Round ***/
void
GGraph::markRound( float x, float y, int size, GColor &c )
{
  f_imag->circle( convX(x), convY(y), size, c );
}


/***********************/
/*** mark Fill Round ***/
void
GGraph::markFRound( float x, float y, int size, GColor &c )
{
  f_imag->fCircle( convX(x), convY(y), size, c );
}


/****************/
/*** mark Axe ***/
void
GGraph::markAxe( float x, float y, int size, GColor &c )
{
  int px = convX( x );
  int py = convY( y );
  GColor old( f_imag->foreground( c ) );
  f_imag->line( px-size, py-size, px+size, py+size );
  f_imag->line( px-size, py+size, px+size, py-size );
  f_imag->foreground( old );
}


/*************/
/*** convX ***/
int
GGraph::convX( float x )
{
  return( int( 0.5 + f_imag->dimX() * (x - f_x0) / f_dx ) );
}


/*************/
/*** convY ***/
int
GGraph::convY( float y )
{
  return( f_imag->dimY() -1 -
	  int( 0.5 + f_imag->dimY() * (y - f_y0) / f_dy ) );
}


/****************/
/*** transf X ***/
float
GGraph::transfX( int x )
{
  return( f_x0 + f_dx * x / f_imag->dimX() );
}


/****************/
/*** transf Y ***/
float
GGraph::transfY( int y )
{
  return( f_y0 + f_dy * y / f_imag->dimY() );
}

