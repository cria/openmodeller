/**
 * Declarations of GXDrawble, GXPixmap, GXCanvas, GXButton, TBtUnico,
 * TBtUAperta, TBtMult, TBtMExclu, TBtMAperta and GXFrame classes.
 *
 * This is an interface to held different graphic libraries.
 * 
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
#include "graphic_x11.hh"

#include <list.cpp>

#include <X11/Xutil.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
using std::string;


#define Abs(a)  ((a) > 0 ? (a) : -(a))

#define DIM_BUTTON    10


static GColor g_button_bg( 0xe0 );


/**********************************************************************/
/*************************** Frame Creation ***************************/

GFrame *createFrame( string title, int ctr_height, int dimx, int dimy )
{
  return( new GXFrame( (char *)title.c_str(), ctr_height, dimx, dimy ) );
}




/**********************************************************************/
/****************************** GXDrawble *****************************/

/*******************/
/*** constructor ***/

GXDrawble::GXDrawble( GXFrame &xframe, int dimx, int dimy )
  : GImage( dimx, dimy )
{
  f_dpy = xframe.dpy;

  // Graphic context.
  f_gc = DefaultGC( f_dpy, xframe.scr );

  foreground( GColor::Black );
  background( GColor::White );

  // The derived classes must initialize this attribute:
  //  f_draw : (Pixmap or Window)
}


/*************/
/*** clear ***/
void
GXDrawble::clear()
{
  GColor foreg = f_fg;
  fRect( 0, 0, dimX(), dimY(), f_bg );
  foreground( foreg );
}


/******************/
/*** foreground ***/
int
GXDrawble::foreground( GColor c )
{
  f_fg = c;
  return( XSetForeground( f_dpy, f_gc, c ) );
}


/******************/
/*** background ***/
int
GXDrawble::background( GColor c )
{
  f_bg = c;
  return( XSetBackground( f_dpy, f_gc, c ) );
}


/*****************/
/*** copy Mode ***/
void
GXDrawble::copyMode()
{
  XSetFunction( f_dpy, f_gc, GXcopy );
}


/****************/
/*** inv Mode ***/
void
GXDrawble::invMode()
{
  XSetFunction( f_dpy, f_gc, GXinvert );
}


/************/
/*** text ***/
void
GXDrawble::text( int x, int y, char *texto, GColor c )
{
  GColor foreg = f_fg;
  int dim = strlen( texto );
  foreground( c );
  XDrawString( f_dpy, f_draw, f_gc, x, y, texto, dim );
  foreground( foreg );
}


/*************/
/*** pixel ***/
void
GXDrawble::pixel( int x, int y )
{
  XDrawPoint( f_dpy, f_draw, f_gc, x, y );
}


/************/
/*** line ***/
void
GXDrawble::line( int x0, int y0,int x1, int y1 )
{
  XDrawLine( f_dpy, f_draw, f_gc, x0, y0, x1, y1 );
}


/*************/
/*** lines ***/
void
GXDrawble::lines( int *pnt, int npnt )
{
  XDrawLines( f_dpy, f_draw, f_gc, (XPoint *)pnt, npnt,
	      CoordModeOrigin );
}


/************/
/*** rect ***/
void
GXDrawble::rect( int x, int y, int w, int h )
{
  XDrawRectangle( f_dpy, f_draw, f_gc, x, y, w, h );
}


/*****************/
/*** fill Rect ***/
void
GXDrawble::fRect( int x, int y, int w, int h )
{
  XFillRectangle( f_dpy, f_draw, f_gc, x, y, w, h );
}


/******************/
/*** fill Polyg ***/
void
GXDrawble::fPolyg( int *pnt, int npnt )
{
  XFillPolygon( f_dpy, f_draw, f_gc, (XPoint *)pnt, npnt, Convex,
		CoordModeOrigin );
}


/**************/
/*** circle ***/
void
GXDrawble::circle( int x, int y, int raio )
{
  elipse( x, y, raio, raio );
}


/*******************/
/*** fill Circle ***/
void
GXDrawble::fCircle( int x, int y, int raio )
{
  fElipse( x, y, raio, raio );
}


/**************/
/*** elipse ***/
void
GXDrawble::elipse( int x, int y, int rx, int ry )
{
  x -= rx;
  y -= ry;
  XDrawArc( f_dpy, f_draw, f_gc, x, y, 2*rx, 2*ry, 0, 360*64 );
}


/***************/
/*** fElipse ***/
void
GXDrawble::fElipse( int x, int y, int rx, int ry )
{
  x -= rx;
  y -= ry;
  XFillArc( f_dpy, f_draw, f_gc, x, y, 2*rx, 2*ry, 0, 360*64 );
}


/*************/
/*** pixel ***/
void
GXDrawble::pixel( int x, int y, GColor c )
{
  foreground( c );
  XDrawPoint( f_dpy, f_draw, f_gc, x, y );
}


/************/
/*** line ***/
void
GXDrawble::line( int x0, int y0,int x1, int y1, GColor c )
{
  foreground( c );
  XDrawLine( f_dpy, f_draw, f_gc, x0, y0, x1, y1 );
}


/*************/
/*** lines ***/
void
GXDrawble::lines( int *pnt, int npnt, GColor c )
{
  foreground( c );
  XDrawLines( f_dpy, f_draw, f_gc, (XPoint *)pnt, npnt,
	      CoordModeOrigin );
}


/************/
/*** rect ***/
void
GXDrawble::rect( int x, int y, int w, int h, GColor c )
{
  foreground( c );
  XDrawRectangle( f_dpy, f_draw, f_gc, x, y, w, h );
}


/*****************/
/*** fill Rect ***/
void
GXDrawble::fRect( int x, int y, int w, int h, GColor c )
{
  foreground( c );
  XFillRectangle( f_dpy, f_draw, f_gc, x, y, w, h );
}


/******************/
/*** fill Polyg ***/
void
GXDrawble::fPolyg( int *pnt, int npnt, GColor c )
{
  foreground( c );
  XFillPolygon( f_dpy, f_draw, f_gc, (XPoint *)pnt, npnt, Convex,
		CoordModeOrigin );
}


/**************/
/*** circle ***/
void
GXDrawble::circle( int x, int y, int raio, GColor c )
{
  elipse( x, y, raio, raio, c );
}


/*******************/
/*** fill Circle ***/
void
GXDrawble::fCircle( int x, int y, int raio, GColor c )
{
  fElipse( x, y, raio, raio, c );
}


/**************/
/*** elipse ***/
void
GXDrawble::elipse( int x, int y, int rx, int ry, GColor c )
{
  x -= rx;
  y -= ry;
  foreground( c );
  XDrawArc( f_dpy, f_draw, f_gc, x, y, 2*rx, 2*ry, 0, 360*64 );
}


/***************/
/*** fElipse ***/
void
GXDrawble::fElipse( int x, int y, int rx, int ry, GColor c )
{
  x -= rx;
  y -= ry;
  foreground( c );
  XFillArc( f_dpy, f_draw, f_gc, x, y, 2*rx, 2*ry, 0, 360*64 );
}


/***********/
/*** put ***/
void
GXDrawble::put( GGraph *graph )
{
  GXDrawble *src = (GXDrawble*)graph->getImage();
  XCopyArea( f_dpy, src->f_draw, f_draw, f_gc, 0, 0,
	     dimX(), dimY(), 0, 0 );
}


/***********/
/*** put ***/
void
GXDrawble::put( GImage *source )
{
  GXDrawble *src = (GXDrawble*)source;
  XCopyArea( f_dpy, src->f_draw, f_draw, f_gc, 0, 0,
	     dimX(), dimY(), 0, 0 );
}


/***********/
/*** put ***/
void
GXDrawble::put( int x0, int y0, GImage *source )
{
  GXDrawble *src = (GXDrawble*)source;
  XCopyArea( f_dpy, src->f_draw, f_draw, f_gc, x0, y0,
	     dimX(), dimY(), 0, 0 );
}


/***********/
/*** put ***/
void
GXDrawble::put( int x, int y, int dimx, int dimy, int x0, int y0,
	       GImage *source )
{
  GXDrawble *src = (GXDrawble*)source;
  XCopyArea( f_dpy, src->f_draw, f_draw, f_gc, x, y, dimx, dimy,
	     x0, y0 );
}




/**********************************************************************/
/****************************** GXPixmap ******************************/

/*******************/
/*** constructor ***/

GXPixmap::GXPixmap( GXFrame &xframe, int dimx, int dimy )
  : GXDrawble( xframe, dimx, dimy )
{
  f_dpy  = xframe.dpy;
  f_orig = xframe.win;

  // Create and initialize the Pixmap.
  //
  int pix_colors = DefaultDepth( f_dpy, xframe.scr );
  f_pixmap = XCreatePixmap( f_dpy, f_orig, dimX(), dimY(),
			    pix_colors );
  f_draw = f_pixmap;
}


/******************/
/*** destructor ***/

GXPixmap::~GXPixmap()
{
  // I don't known why this produces a "Segmentation fault" after
  // some code steps. Probably the pixmap had alread been freed
  // before!

  /*  XFreePixmap( f_dpy, f_pixmap ); */
}




/**********************************************************************/
/****************************** GXCanvas ******************************/

/*******************/
/*** constructor ***/

GXCanvas::GXCanvas( GXFrame &xframe, int pos_x, int pos_y, int dimx,
                    int dimy )
  : GXDrawble( xframe, dimx, dimy )
{
  // Control panel window.
  win = XCreateSimpleWindow( xframe.dpy, xframe.win,
			     pos_x, pos_y, dimx, dimy,
			     1,
			     BlackPixel( xframe.dpy, xframe.scr ),
			     WhitePixel( xframe.dpy, xframe.scr ) );

  long mask = ExposureMask | ButtonPressMask | ButtonReleaseMask;
  mask |= ButtonMotionMask;

  XSelectInput( xframe.dpy, win, mask );

  XMapWindow( xframe.dpy, win );
  f_draw = win;

  for ( int i = 0; i < 3; i++ )
    {
      bt_press[i] = 0;
      bt_release[i] = 0;
      bt_drag[i] = 0;
    }
}


/******************/
/*** destructor ***/

GXCanvas::~GXCanvas()
{
}


/********************/
/*** button Press ***/
void
GXCanvas::btPress( int x, int y, int button )
{
  if ( bt_press[button] )
    (*bt_press[button])( x, y );
}


/***********************/
/*** button Released ***/
void
GXCanvas::btRelease( int x, int y, int button )
{
  if ( bt_release[button] )
    (*bt_release[button])( x, y );
}


/*******************/
/*** button Drag ***/
void
GXCanvas::btDrag( int x, int y, int button )
{
  if ( bt_drag[button] )
    (*bt_drag[button])( x, y );
}



/**********************************************************************/
/****************************** GXButton ******************************/

// Default background color for buttons.
GColor GXButton::f_bg( 100 );


/*******************/
/*** constructor ***/

GXButton::GXButton( GXFrame &xframe, int pos_x, int pos_y,
                    int dimx )
{
  f_bg   = g_button_bg;
  f_dimx = dimx;
  f_dimy = 13;

  // Control panel window.
  win = XCreateSimpleWindow( xframe.dpy, xframe.win,
			     pos_x, pos_y, f_dimx, f_dimy,
			     1, g_button_bg, g_button_bg );

  XSelectInput( xframe.dpy, win, ButtonPressMask );

  XMapWindow( xframe.dpy, win );

  f_frm = &xframe;
}


/******************/
/*** destructor ***/

GXButton::~GXButton()
{
}


/*************/
/*** clear ***/
void
GXButton::clear( int xpos, int width )
{
  Display *dpy = f_frm->dpy;
  GC       gc  = f_frm->gc;

  XSetForeground( dpy, gc, f_bg );
  XFillRectangle( dpy, win, gc, xpos, 0, width, f_dimy );
  XSetForeground( dpy, gc, GColor::Black );
}


/*************/
/*** clear ***/
void
GXButton::clear( int xpos, int width, GColor bg )
{
  Display *dpy = f_frm->dpy;
  GC       gc  = f_frm->gc;

  XSetForeground( dpy, gc, bg );
  XFillRectangle( dpy, win, gc, xpos, 0, width, f_dimy );
  XSetForeground( dpy, gc, GColor::Black );
}



/**********************************************************************/
/****************************** TBtUnico ******************************/

/*******************/
/*** constructor ***/

TBtUnico::TBtUnico( GXFrame &xframe, int posx, int posy, string label,
                    GFrame::FuncExec func, GColor c )
  : GXButton( xframe, posx, posy, 8 * label.size() )
{
  f_func = func;
  f_cor  = c;
  f_label = label;
}


/******************/
/*** destructor ***/
TBtUnico::~TBtUnico()
{
}


/***********/
/*** set ***/
void
TBtUnico::set( int x, int y )
{
  if ( f_func )
    {
      label( f_cor );
      (*f_func)();
      label();
    }
}


/*************/
/*** label ***/
void
TBtUnico::label()
{
  clear( 0, f_dimx );

  int y = (f_dimy+9) / 2;
  int dim = f_label.size();
  XDrawString( f_frm->dpy, win, f_frm->gc, 1, y, f_label.c_str(), dim );
  XFlush( f_frm->dpy );
}


/*************/
/*** label ***/
void
TBtUnico::label( GColor c )
{
  clear( 0, f_dimx, c );

  int y = (f_dimy+9) / 2;
  int dim = f_label.size();
  XDrawString( f_frm->dpy, win, f_frm->gc, 1, y, f_label.c_str(), dim );
  XFlush( f_frm->dpy );
}


/**********************************************************************/
/***************************** TBtUAperta *****************************/

/******************/
/*** constructor ***/

TBtUAperta::TBtUAperta( GXFrame &frame, int posx, int posy,
                        string label, GFrame::FuncExec func, GColor c )
  : TBtUnico( frame, posx, posy, label, func, c )
{
  f_estado = 0;
}

/******************/
/*** destructor ***/

TBtUAperta::~TBtUAperta()
{
}


/***********/
/*** set ***/
void
TBtUAperta::set( int x, int y )
{
  if ( f_func )
    {
      if ( ! f_estado )
	label( f_cor );
      else
	label();
      f_estado = 1 - f_estado;
      
      (*f_func)();
    }
}


/**********************************************************************/
/****************************** TBtMult *******************************/

/*******************/
/*** constructor ***/

TBtMult::TBtMult( GXFrame &frame, int posx, int posy, char *label,
		  GFrame::FuncNotif func, GColor *cores )
  : GXButton( frame, posx, posy, DIM_BUTTON * strlen(label) )
{
  f_func = func;

  f_nbot  = strlen( label );
  strcpy( f_label, label );
  f_cores = new GColor[f_nbot];

  for ( int i = 0; i < f_nbot; i++ )
    f_cores = (cores ? cores+i : &f_bg);
}


/******************/
/*** destructor ***/

TBtMult::~TBtMult()
{
  delete( f_cores );
}


/************/
/*** draw ***/
void
TBtMult::draw()
{
  for ( int i = 0; i < f_nbot; i++ )
    label( i );
}


/*************/
/*** label ***/
void
TBtMult::label( int x )
{
  clear( x * DIM_BUTTON, DIM_BUTTON );

  char letra[2];
  letra[0] = f_label[x];
  letra[1] = 0;
  XDrawString( f_frm->dpy, win, f_frm->gc, x * DIM_BUTTON + 1,
	       (f_dimy+9)/2, letra, 1 );
  XFlush( f_frm->dpy );
}


/*************/
/*** label ***/
void
TBtMult::label( int x, GColor c )
{
  clear( x * DIM_BUTTON, DIM_BUTTON, c );

  char letra[2];
  letra[0] = f_label[x];
  letra[1] = 0;
  XDrawString( f_frm->dpy, win, f_frm->gc, x * DIM_BUTTON + 1,
	       (f_dimy+9)/2, letra, 1 );
  XFlush( f_frm->dpy );
}


/**********************************************************************/
/****************************** TBtMExclu *****************************/

/*******************/
/*** constructor ***/

TBtMExclu::TBtMExclu( GXFrame &frame, int posx, int posy, char *label,
                      GFrame::FuncNotif func, GColor *cores )
  : TBtMult( frame, posx, posy, label, func, cores )
{
  f_escolha = 0;
}


/******************/
/*** destructor ***/

TBtMExclu::~TBtMExclu()
{
}


/************/
/*** draw ***/
void
TBtMExclu::draw()
{
  for ( int i = 0; i < f_nbot; i++ )
    {
      if ( i == f_escolha )
	label( i, f_cores[i] );
      else
	label( i );
    }
}


/*************/
/*** set ***/
void
TBtMExclu::set( int x, int y )
{
  if ( f_func )
    {
      int novo = x / DIM_BUTTON;
      
      if ( novo == f_escolha )
	return;
      
      label( f_escolha );
      label( novo, f_cores[novo] );
      
      f_escolha = novo;
      
      (*f_func)( f_escolha );  
    }
}


/**********************************************************************/
/***************************** TBtMAperta *****************************/

/*******************/
/*** constructor ***/

TBtMAperta::TBtMAperta( GXFrame &frame, int posx, int posy, char *label,
                        GFrame::FuncNotif func, GColor *cores )
  : TBtMult( frame, posx, posy, label, func, cores )
{
  f_escolha = new int[f_nbot];
  memset( f_escolha, 0, f_nbot );
}


/******************/
/*** destructor ***/

TBtMAperta::~TBtMAperta()
{
  delete( f_escolha );
}


/************/
/*** draw ***/
void
TBtMAperta::draw()
{
  for ( int i = 0; i < f_nbot; i++ )
    {
      if ( f_escolha[i] )
	label( i, f_cores[i] );
      else
	label( i );
    }
}


/*************/
/*** set ***/
void
TBtMAperta::set( int x, int y )
{
  int novo = x / DIM_BUTTON;
  f_escolha[novo] = 1 - f_escolha[novo];

  if ( f_escolha[novo] )
    label( novo, f_cores[novo] );
  else
    label( novo );

  (*f_func)( novo );
}




/**********************************************************************/
/******************************* GXFrame ******************************/

/*******************/
/*** Constructor ***/

GXFrame::GXFrame( char *titulo, int crt_height, int dimx, int dimy )
{
  f_fim = 0;

  // Display and Screen.
  //
  if ( (dpy = XOpenDisplay( NULL )) == NULL )
    {
      printf( "Erro: can't open display\n" );
      exit( -1 );
    }
  scr = DefaultScreen( dpy );

  // Set default GColor depth
  int depth = DefaultDepth( dpy, scr );
  GColor::setDepth( depth );

  // Main window creation.
  //
  f_crt_height = crt_height * DIM_BUTTON + 10;
  f_winx = dimx + 3;
  f_winy = dimy + f_crt_height + 4;

  unsigned border_width    = 0;
  unsigned long border     = BlackPixel( dpy, scr );
  unsigned long background = GColor(  );

  win = XCreateSimpleWindow( dpy, RootWindow( dpy, scr ),
			     40, 40, f_winx, f_winy,
			     border_width, border, background );

  // Control panel window.
  crt = XCreateSimpleWindow( dpy, win,
			     0, 1, dimx, f_crt_height,
			     1, border, g_button_bg );
  
  // Main window's basic properties.
  //
  XSizeHints size_hints;
  XClassHint class_hints;
  XWMHints   wm_hints;

  // Dimensions.
  size_hints.flags      = PPosition | PSize | PMinSize;
  size_hints.min_width  = f_winx;
  size_hints.min_height = f_winy;

  // Window and icone names.
  XTextProperty win_name, icon_name;
  XStringListToTextProperty( &titulo, 1, &win_name );
  XStringListToTextProperty( &titulo, 1, &icon_name );

  // Input, icone, initial state, etc.
  /* XWMHints allows activates the keyboard, icone, etc */
  wm_hints.flags = StateHint;
  wm_hints.initial_state = NormalState;

  // Class
  string res_name("openModeller");
  string res_class("CRIA");

  class_hints.res_name  = (char *)res_name.c_str();
  class_hints.res_class = (char *)res_class.c_str();

  char **argv = 0;
  char   argc = 0;
  XSetWMProperties( dpy, win, &win_name, &icon_name, argv, argc,
		    &size_hints, &wm_hints, &class_hints );

  // Font load.
  string font_name("8x13");
  Font font;

  if ( (font = XLoadFont( dpy, (char *)font_name.c_str() )) == BadName )
    {
      printf( "Could not find font: %s.\n", font_name.c_str() );
      exit( -1 );
    }

  // Graphic context creation.
  //
  XGCValues values;
  unsigned long value_mask;

  value_mask      = GCFunction | GCFont;
  values.function = GXcopy;
  values.font     = font;
  gc = XCreateGC( dpy, win, value_mask, &values );

  XSetForeground( dpy, gc, BlackPixel( dpy, scr ) );
  XSetBackground( dpy, gc, WhitePixel( dpy, scr ) );

  // Select the control and main windows events.
  long mask1 = ExposureMask | StructureNotifyMask;
  XSelectInput( dpy, win, mask1 );

  long mask2 = ExposureMask | ButtonPressMask;
  XSelectInput( dpy, crt, mask2 );


  // Exit button.
  long mask3 = ExposureMask | ButtonPressMask | ButtonReleaseMask;
  f_btsaida = new TBtUnico( *this, f_winx - 16, 4, "X", 0, GColor::Black );
  XSelectInput( dpy, f_btsaida->win, mask3 );

  f_falways = 0;
  f_fshow   = 0;
  f_finit   = 0;
}



/******************/
/*** destructor ***/

GXFrame::~GXFrame()
{
  GXButton *b;
  for ( f_lbot.head(); (b = f_lbot.get()); f_lbot.next() )
    delete( b );

  GXPixmap *p;
  for ( f_lpix.head(); (p = f_lpix.get()); f_lpix.next() )
    delete( p );

  GXCanvas *c;
  for ( f_lcnv.head(); (c = f_lcnv.get()); f_lcnv.next() )
    delete( c );
}


/*************************/
/*** func Button Press ***/
void
GXFrame::funcBtPress( GImage *canvas, FuncButton func, int button )
{
  if ( button >= 1 && button <= 3 )
    {
      GXCanvas *canv = (GXCanvas *)canvas;
      canv->bt_press[button-1] = func;
    }
}


/***************************/
/*** func Button Release ***/
void
GXFrame::funcBtRelease( GImage *canvas, FuncButton func, int button )
{
  if ( button >= 1 && button <= 3 )
    {
      GXCanvas *canv = (GXCanvas *)canvas;
      canv->bt_release[button-1] = func;  
    }
}


/************************/
/*** func Button Drag ***/
void
GXFrame::funcBtDrag( GImage *canvas, FuncButton func, int button )
{
  if ( button >= 1 && button <= 3 )
    {
      GXCanvas *canv = (GXCanvas *)canvas;
      canv->bt_drag[button-1] = func;
    }
}

Bool check_func( Display *dpy, XEvent *event, XPointer arg )
{
  return( True );
}


/************/
/*** exec ***/
void
GXFrame::exec()
{
  static char inicio = 1;
  XEvent report;

  XMapWindow( dpy, win );
  XMapWindow( dpy, crt );

  while( !f_fim )
    {
      if ( XCheckIfEvent(dpy, &report, check_func, XPointer(0)) != True )
	{
	  if ( f_falways )
	    (*f_falways)();
	  else
	    usleep( 10 );
	}
      else
	{
	switch( report.type )
	  {
	  case DestroyNotify:
	    f_fim = 1;
	    break;
	    
	  case Expose:
	    if ( !report.xexpose.count )
	      {
		if ( report.xexpose.window == win )
		  {
		    if ( f_fshow )
		      (*f_fshow)();
		  }
		else
		  {
		    GXButton *bot;
		    f_lbot.head();
		    for ( ; (bot = f_lbot.get()); f_lbot.next() )
		      bot->draw();
		    f_btsaida->draw();
		  }
	      }
	    break;
	    
	  case ConfigureNotify:
	    if ( inicio && f_finit )
	      {
		inicio = 0;
		(*f_finit)();
	      }
	    
	    f_winx = report.xconfigure.width;
	    f_winy = report.xconfigure.height;
	    break;
	    
	  case ButtonPress:
	  case ButtonRelease:
	  case MotionNotify:
	    distributeButton( report.xbutton.window, report );
	    break;
	  }
	}
    }

  XCloseDisplay( dpy );
}


/*************/
/*** flush ***/
void
GXFrame::flush()
{
  XFlush( dpy );
}


/*******************/
/*** new Canvas ***/
GImage *
GXFrame::newCanvas( int posx, int posy, int dimx, int dimy )
{
  GXCanvas *canvas = new GXCanvas( *this, posx,
				   posy + f_crt_height + 2,
				   dimx, dimy );
  f_lcnv.insert( canvas );
  return( canvas );
}


/*******************/
/*** new Pixmap ***/
GImage *
GXFrame::newPixmap( GImage *, int dimx, int dimy )
{
  GXPixmap *pix = new GXPixmap( *this, dimx, dimy );
  f_lpix.insert( pix );
  return( pix );
}


/***************/
/*** buttons ***/
void
GXFrame::button( int x, int y, char *label, FuncExec f, GColor c )
{
  f_lbot.insert( new TBtUnico( *this, x, y, label, f, c ) );
}

void
GXFrame::buttonA( int x, int y, char *label, FuncExec f, GColor c )
{
  f_lbot.insert( new TBtUAperta( *this, x, y, label, f, c ) );
}

void
GXFrame::buttonMA( int x, int y, char *label, FuncNotif f, GColor *c )
{
  f_lbot.insert( new TBtMAperta( *this, x, y, label, f, c ) );
}

void
GXFrame::buttonME( int x, int y, char *label, FuncNotif f, GColor *c )
{
  f_lbot.insert( new TBtMExclu( *this, x, y, label, f, c ) );
}


/*************************/
/*** distribute Button ***/
int
GXFrame::distributeButton( Window win, XEvent &e )
{
  static int drag_button = 0;

  // Eventos das janelas graficas (mouse).
  GXCanvas *canv;
  for ( f_lcnv.head(); (canv = f_lcnv.get()); f_lcnv.next() )
    if ( canv->win == win )
      {
	int x = e.xbutton.x;
	int y = e.xbutton.y;

	switch( e.type )
	  {
	  case MotionNotify:
	    if ( drag_button )
	      canv->btDrag( x, y, drag_button - 1 );
	    break;

	  case ButtonPress:
	    drag_button = e.xbutton.button;
	    canv->btPress( x, y, e.xbutton.button - 1 );
	    break;

	  case ButtonRelease:
	    drag_button = 0;
	    canv->btRelease( x, y, e.xbutton.button - 1 );
	  }

	return( 1 );
      }
  
  // Verifica se um botao do controle foi pressionado
  GXButton *bot;
  for ( f_lbot.head(); (bot = f_lbot.get()); f_lbot.next() )
    if ( bot->win == crt )
      {
	if ( e.type == ButtonPress )
	  bot->set( e.xbutton.x, e.xbutton.y );
	return( 1 );
      }

  // Botao de saida.
  if ( win == f_btsaida->win )
    f_fim = 1;

  return( 0 );
}


/*************/
/*** print ***/
void
GXFrame::print( Colormap cm )
{
  XColor cor[256];

  for ( int i = 0; i < 256; i++ )
    cor[i].pixel = i;

  XQueryColors( dpy, cm, cor, 256 );

  XColor *c = cor;
  for ( int i = 0; i < 256; i++, c++ )
    printf( "%04ld - [%04hu, %04hu, %04hu]\n", c->pixel, c->red,
	    c->green, c->blue );
  printf( "\n" );
}


/********************/
/*** set Colormap ***/
void
GXFrame::setColormap()
{
  // A colormap with homogeneos distributed colors.
  Colormap cm = DefaultColormap( dpy, scr );
  // cm = XCreateColormap( dpy, win, visual, AllocAll );
  XColor cores[256];
  XColor *cor = cores;
  int ci = 0;
  for ( int r = 0; r < 8; r++ )
    for ( int g = 0; g < 4; g++ )
      for ( int b = 0; b < 8; b++ )
	{
	  cor->pixel = ci++;
	  cor->red   = r << 13;
	  cor->green = g << 14;
	  cor->blue  = b << 13;
	  cor->flags = 7;
	  XAllocColor( dpy, cm, cor++ );
	}
  XStoreColors( dpy, cm, cores+200, 10 );

  //  cores[0].red = cores[0].green = cores[0].blue = 0;
  //  cores[1].red = cores[1].green = cores[1].blue = 0xffff;
}

