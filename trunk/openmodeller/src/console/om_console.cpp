/**
 * openModeller console interface.
 * 
 * @author Mauro E S Mu�oz (mauro@cria.org.br)
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
#include "request_file.hh"
#include "file_parser.hh"

#include <openmodeller/Configuration.hh>
#include <istream>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <stdexcept>

int showAlgorithms ( AlgMetadata const **availables );
AlgMetadata const *readAlgorithm( AlgMetadata const **availables );
int readParameters( AlgParameter *result, AlgMetadata const *metadata );
char *extractParameter( char *name, int nvet, char **vet );

void mapCallback( float progress, void *extra_param );
void modelCallback( float progress, void * extra_param );

/**************************************************************/
/*************** openModeller Console Interface ***************/

/************/
/*** main ***/
int
main( int argc, char **argv )
{
  char * path = 0;

  // Reconfigure the global logger.
  Log::instance()->setLevel( Log::Info );
  Log::instance()->setPrefix( "" );

  try {

    if ( argc < 2 ) {

      Log::instance()->error( "\n%s <request>\n\n", argv[0] );
      exit(1);
    }

    char *request_file = argv[1];

    AlgorithmFactory::searchDefaultDirs();
    OpenModeller om;
    Log::instance()->info( "\nopenModeller version %s\n", om.getVersion() );

    delete[] path;

    // Configure the OpenModeller object from data read from the
    // request file.
    RequestFile request;
    int resp = request.configure( &om, request_file );

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
            AlgMetadata const **availables = om.availableAlgorithms();
            AlgMetadata const *metadata;

            if ( ! (metadata = readAlgorithm( availables )) )
              return 1;

            Log::instance()->info( "\n> Algorithm used: %s\n\n", metadata->name );
            Log::instance()->info( " %s\n\n", metadata->overview );

            // For resulting parameters storage.
            int nparam = metadata->nparam;
            AlgParameter *param = new AlgParameter[nparam];

            // Read from console the parameters not set by request
            // file. Fills 'param' with all 'metadata->nparam'
            // parameters set.
            readParameters( param, metadata );

            // Set the model algorithm to be used by the controller
            om.setAlgorithm( metadata->id, nparam, param );

            delete[] param;
            delete[] availables;
          }
      }

    // Run model
    om.setModelCallback( modelCallback );

    request.makeModel( &om );

    if ( request.requestedProjection() ) {

      // Run projection
      om.setMapCallback( mapCallback );

      try {

        request.makeProjection( &om );
      }
      catch ( ... ) {}
    }
    else {

      Log::instance()->warn( "Skipping projection\n" );
    }

    // Instantiate objects for model statistics
    const ConfusionMatrix * const matrix = om.getConfusionMatrix();
    const RocCurve * const roc_curve = om.getRocCurve();

    // Confusion Matrix
    Log::instance()->info( "\nModel statistics\n" );
    Log::instance()->info( "Accuracy:          %7.2f%%\n", matrix->getAccuracy() * 100 );

    int omissions = matrix->getValue(0.0, 1.0);
    int total     = omissions + matrix->getValue(1.0, 1.0);

    Log::instance()->info( "Omission error:    %7.2f%% (%d/%d)\n", matrix->getOmissionError() * 100, omissions, total );

    double commissionError = matrix->getCommissionError();

    if ( commissionError >= 0.0 ) {

      int commissions = matrix->getValue(1.0, 0.0);
      total           = commissions + matrix->getValue(0.0, 0.0);

      Log::instance()->info( "Commission error:  %7.2f%% (%d/%d)\n", commissionError * 100, commissions, total );
    }

    // ROC curve
    Log::instance()->info( "AUC:               %7.2f\n", roc_curve->getArea() );

    // Projection statistics
    if ( request.requestedProjection() ) {

      AreaStats * stats = om.getActualAreaStats();

      Log::instance()->info( "Percentage of cells predicted present: %7.2f%%\n", 
             stats->getAreaPredictedPresent() / (double) stats->getTotalArea() * 100 );
      Log::instance()->info( "Total number of cells: %d\n", stats->getTotalArea() );
      Log::instance()->info( "\nDone.\n" );

      delete stats;
    }
  }
  catch ( std::exception& e ) {
    Log::instance()->info( "%s\n", e.what() );
    Log::instance()->info( "Exception occurred\n" );
  }
  catch ( ... ) {
    Log::instance()->info( "Unknown error occurred\n" );
  }

  return 0;
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
      printf( "Could not find any algorithms.\n" );
      return 0;
    }

  printf( "\nChoose an algorithm between:\n" );

  int count = 0;
  AlgMetadata const *metadata;
  while ( ( metadata = *availables++ ) )
  {
    printf( " [%d] %s\n", count++, metadata->name );
  }
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

  while ( 1 ) {

    int quit_option = showAlgorithms( availables );
    if ( ! quit_option ) {
      return 0;
    }

    int option = -1;

    printf( "\nOption: " );
    fgets( buf, 128, stdin );

    int first_char_ascii = (int)buf[0];

    // Also quit if input is "q" or "Q"
    if ( first_char_ascii == 113 || first_char_ascii == 81 ) {
      return 0;
    }

    option = atoi( buf );

    if ( option == quit_option ) {
      return 0;
    }

    // An algorithm was choosed.
    else if ( option >= 0 && option < quit_option ) {
      return availables[option];
    }
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

      // Informs the parameter's metadata to the user.
      printf( "\n* Parameter: %s\n\n", param->name );
      printf( " %s\n", param->overview );

      if ( param->type != String ) {

        if ( param->has_min ) {

          if ( param->type == Integer ) {

            printf( "%s >= %d\n", param->name, int( param->min_val ) );
          }
          else {

            printf( " %s >= %f\n", param->name, param->min_val );
          }
        }
        if ( param->has_max ) {

          if ( param->type == Integer ) {

            printf( "%s <= %d\n\n", param->name, int( param->max_val ) );
          }
          else {

            printf( " %s <= %f\n\n", param->name, param->max_val );
          }
        }
      }

      printf( "Enter with value [%s]: ", param->typical );

      // Read parameter's value or use the "typical" value
      // if the user does not enter a new value.
      char value[64];
      *value = 0;
      if ( fgets( value, 64, stdin ) && ( *value >= ' ' ) ) {

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
/*** map Callback ***/
/**
 * Shows the map creation progress.
 */
void
modelCallback( float progress, void *extra_param )
{
  Log::instance()->info( "Model creation: %07.4f%% \r", 100 * progress );
}


/********************/
/*** map Callback ***/
/**
 * Shows the map creation progress.
 */
void
mapCallback( float progress, void *extra_param )
{
  Log::instance()->info( "Map creation: %07.4f%% \r", 100 * progress );
}
