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
#include <openmodeller/Environment.hh>
#include <openmodeller/Occurrence.hh>
#include "request_file.hh"
#include "file_parser.hh"
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

OpenModeller *_om;
GImage  *_cnv;
GImage  *_pm;
GGraph *_graph;


OpenModeller *createModel( char *request_file );
int showAlgorithms ( AlgMetadata const **availables );
char *readParameters ( AlgMetadata *metadata );
AlgMetadata const *readAlgorithm( AlgMetadata const **availables );
void readOccurrences( char const *file, char const *name,
			      char const *coord_system );
int readParameters( AlgParameter *result, AlgMetadata const *metadata );
char *extractParameter( char *name, int nvet, char **vet );

void modelCallback( float progress, void *extra_param );


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

    Log::instance()->error( "\n%s <request>\n\n", argv[0] );
    exit(1);
  }

  char *request_file = argv[1];

  Log::instance()->info( "\nopenModeller Two-dimensional Niche Viewer - CRIA\n" );

  try {

    FileParser fp( request_file );

    // Create the model using openModeller.
    _om = createModel( request_file );
    EnvironmentPtr env = _om->getEnvironment();
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

    // Occurrences file (used to draw, not to create the model).
    string oc_cs   = fp.get( "WKT Coord System" );
    string oc_file = fp.get( "Occurrences source" );

    if ( oc_file.empty() ) {

      oc_file = fp.get( "Species file" );
    }

    string oc_name = fp.get( "Occurrences group" );

    if ( oc_name.empty() ) {

      oc_name = fp.get( "Species" );
    }

    readOccurrences( oc_file.c_str(), oc_name.c_str(), oc_cs.c_str() );

    // Instantiate graphical window.
    int dimx = 256;
    int dimy = 256;
    GFrame *frame = createFrame( "openModeller Niche Viewer", 1, dimx, dimy );
    _cnv = frame->newCanvas( 0, 0, dimx, dimy );
    _pm  = frame->newPixmap( _cnv, dimx, dimy );

    // Drawing area.
    _graph = new GGraph( _pm );
    _graph->scale( min[0], min[1], max[0], max[1] );
    _graph->background( _bg );
    _graph->clear();

    // Zoom in and out with mouse buttons.
    _zoom = 2.0;
    frame->funcShow( draw );

    frame->exec();

    delete _graph;
    delete frame;
    delete _om;
  }
  catch ( std::exception& e ) {
    Log::instance()->info( "Exception occurred\n" );
    //Log::instance()->info( "Message is %s\n", e.what() );
  }
  catch ( ... ) {
    Log::instance()->info( "Unknown Error occurred\n" );
  }

  return 0;
}


/********************/
/*** create Model ***/
OpenModeller *
createModel( char *request_file )
{
  AlgorithmFactory::searchDefaultDirs();
  OpenModeller *om = new OpenModeller;

  // Configure the OpenModeller object from data read from the
  // request file.
  RequestFile request;
  int resp = request.configure( om, request_file );

  if ( resp < 0 ) {

    Log::instance()->error( "Can't read request file %s", request_file );
    exit(1);
  }

  // If something was not set...
  if ( resp )
    {
      if ( ! request.algorithmSet() )
        {
          // Find out which model algorithm is to be used.
          AlgMetadata const **availables = om->availableAlgorithms();
          AlgMetadata const *metadata;

          if ( ! (metadata = readAlgorithm( availables )) )
            return 0;

          Log::instance()->info( "\n> Algorithm used: %s\n\n", metadata->name.c_str() );
          Log::instance()->info( " %s\n\n", metadata->description.c_str() );

          // For resulting parameters storage.
          int nparam = metadata->nparam;
          AlgParameter *param = new AlgParameter[nparam];

          // Read from console the parameters not set by request
          // file. Fills 'param' with all 'metadata->nparam'
          // parameters set.
          readParameters( param, metadata );

          // Set the model algorithm to be used by the controller
          om->setAlgorithm( metadata->id, nparam, param );

          delete[] param;
          delete[] availables;
        }
    }

  /*** Run the model ***/
  om->setModelCallback( modelCallback );

  if ( ! om->createModel() ) {

    Log::instance()->error( "Error: %s\n", om->error() );
    exit(1);
  }

  return om;
}


/***********************/
/*** show algorithms ***/
//
// Print available algorithms.
// Returns the option number associated with 'Quit' that is
// equal to the number of algorithms.
//
int
showAlgorithms( AlgMetadata const **availables )
{
  if ( ! *availables )
    {
      printf( "No algorithm available.\n" );
      return 0;
    }

  int count = 0;
  AlgMetadata const *metadata;
  while (( metadata = *availables++ ))
    printf( " [%d] %s\n", count++, metadata->name.c_str() );

  printf( " [%d] Quit\n", count );
  printf( "\n" );

  return count;
}


/**********************/
/*** read algorithm ***/
//
// Let the user choose an algorithm and enter its parameters.
// Returns the choosed algorithm's metadata.
//
AlgMetadata const *
readAlgorithm( AlgMetadata const **availables )
{
  char buf[128];

  while ( 1 )
    {
      printf( "\nChoose an algorithm between:\n" );

      int quit_option = showAlgorithms( availables );
      if ( ! quit_option )
	return 0;

      int option = -1;

      printf( "\nOption: " );
      fgets( buf, 128, stdin );
      option = atoi( buf );

      if ( option == quit_option )
	return 0;

      // An algorithm was choosed.
      else if ( option >= 0 && option < quit_option )
	return availables[option];
    }
}


/***********************/
/*** read Parameters ***/
int
readParameters( AlgParameter *result, AlgMetadata const *metadata )
{
  AlgParamMetadata *param = metadata->param;
  AlgParamMetadata *end   = param + metadata->nparam;

  // Read from stdin each algorithm parameter.
  for ( ; param < end; param++, result++ )
    {
      // The resulting ID is equal the ID set in algorithm's
      // metadata.
      result->setId( param->id );

      // Inform parameter metadata to the user.
      printf( "\n* Parameter: %s\n\n", param->name.c_str() );
      printf( " %s:\n", param->overview.c_str() );

      if ( param->type != String ) {

        if ( param->has_min ) {

          if ( param->type == Integer ) {

            printf( "%s >= %d\n", param->name.c_str(), int( param->min_val ) );
          }
          else {

            printf( " %s >= %f\n", param->name.c_str(), param->min_val );
          }
        }
        if ( param->has_max ) {

          if ( param->type == Integer ) {

            printf( "%s <= %d\n\n", param->name.c_str(), int( param->max_val ) );
          }
          else {

            printf( " %s <= %f\n\n", param->name.c_str(), param->max_val );
          }
        }
      }

      printf( "Enter with value [%s]: ", param->typical.c_str() );

      // Read parameter's value or use the "typical" value
      // if the user does not enter a new value.
      char value[64];
      *value = 0;
      if ( fgets( value, 64, stdin ) && (*value >= ' ') ) {

        // Remove line feed to avoid problems with string parameters 
        if ( param->type == String ) {

          char * pos = strchr( value, '\n' );

          if ( pos ) {

            *pos = '\0';
          }
	}

        result->setValue( value );
      }
      else {

        result->setValue( param->typical );
      }
    }

  return metadata->nparam;
}


/*************************/
/*** extract Parameter ***/
/**
 * Search for 'id' in the 'nvet' elements of the vector 'vet'.
 * If the string 'id' is in the begining of some string vet[i]
 * then returns a pointer to the next character of vet[i],
 * otherwise returns 0.
 */
char *
extractParameter( char *id, int nvet, char **vet )
{
  int length = strlen( id );
  char **end = vet + nvet;

  while ( vet < end )
    if ( ! strncmp( id, *vet++, length ) )
      return *(vet-1) + length;

  return 0;
}


/********************/
/*** model Callback ***/
/**
 * Shows the model creation progress.
 */
void
modelCallback( float progress, void *extra_param )
{
  Log::instance()->info( "Model creation: %07.4f%% \r", 100 * progress );
}


/**************************************************************/

/************/
/*** draw ***/
void
draw()
{
  if ( _redraw )
    {
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
  for ( *y = ymin; *y < ymax; *y += ystep )
    {
      for ( *x = xmin; *x < xmax; *x += xstep )
        {
          GColor color = GColor::Blue;
          color.scale( _om->getValue( amb ) );
          graph->pixel( float(*x), float(*y), color );
        }


      if ( ! (++i % 10) )
	_cnv->put( graph );
    }
}


/******************/
/*** draw occur ***/
void
draw_occur( GGraph *graph, const OccurrencesPtr& occurs, GColor color )
{
  if ( occurs && occurs->numOccurrences() ) {

    // Draw each set of occurrences.
    EnvironmentPtr env = _om->getEnvironment();
    Sample amb;
    OccurrencesImpl::const_iterator oc = occurs->begin();

    for( ; oc != occurs->end(); ++oc ) {

      amb = env->get( (*oc)->x(), (*oc)->y() );

      if ( amb.size() >= 2 ) {

        graph->markAxe( amb[0], amb[1], 1, color);
      }
    }
  }
}

/************************/
/*** read Occurrences ***/
void 
readOccurrences( char const *file, char const *name, char const *coord_system )
{
  OccurrencesReader* oc_file = OccurrencesFactory::instance().create( file, coord_system );

  _presences = oc_file->getPresences( name );

  _absences = oc_file->getAbsences( name );
}
