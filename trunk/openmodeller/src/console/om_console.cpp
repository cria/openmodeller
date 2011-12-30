/**
 * openModeller console interface.
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
#include <openmodeller/Configuration.hh>
#include <openmodeller/os_specific.hh>

#include "request_file.hh"
#include "file_parser.hh"
#include "om_cmd_utils.hh"

#include <istream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <stdexcept>

#ifdef MPI_FOUND
#include "mpi.h"
#endif

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
  if ( argc < 2 ) {

    // Obs: default log level includes "error" messages
    Log::instance()->error( "\n%s request_file [log_level]\n\n", argv[0] );
    exit(1);
  }

  // Reconfigure the global logger
  string log_level("info");

  if ( argc > 2 ) {

    log_level.assign( argv[2] );
  }

  Log::Level level_code = getLogLevel( log_level );
  Log::instance()->setLevel( level_code );
  Log::instance()->setPrefix( "" );

  // Set up any related external resources
  setupExternalResources();
  
  try {

    char *request_file = argv[1];

    AlgorithmFactory::searchDefaultDirs();
    OpenModeller om;
    Log::instance()->info( "openModeller version %s\n", om.getVersion().c_str() );
    Log::instance()->debug("CPUs detected: %u\n", getNCPU());

    // Configure the OpenModeller object from data read from the
    // request file.
    RequestFile request;
    int resp = request.configure( &om, request_file );

    if ( resp < 0 ) {

      Log::instance()->error( "Could not read request file %s", request_file );
      exit(1);
    }

    // If something was not set...
    if ( resp ) {

      if ( ! request.occurrencesSet() ) {

          exit(1);
      }

      if ( ! request.algorithmSet() ) {

        // Find out which model algorithm is to be used.
        AlgMetadata const **availables = om.availableAlgorithms();
        AlgMetadata const *metadata;

        if ( ! (metadata = readAlgorithm( availables )) ) {

          return 1;
        }

        Log::instance()->info( "\n> Algorithm used: %s\n\n", metadata->name.c_str() );
        Log::instance()->info( " %s\n\n", metadata->overview.c_str() );

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

#ifdef MPI_FOUND

      Log::instance()->info( "Running the parallel version of the algorithm\n" );

      MPI_Init( &argc, &argv ); // MPI initialization

      int rank;

      MPI_Comm_rank( MPI_COMM_WORLD, &rank );

#endif

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

    if ( request.calcConfusionMatrix() ) {

      // Instantiate objects for model statistics
      const ConfusionMatrix * const matrix = om.getConfusionMatrix();

      // Confusion Matrix
      Log::instance()->info( "\n" );
      Log::instance()->info( "Model statistics for training data\n" );
      Log::instance()->info( "Threshold:         %7.2f%%\n", matrix->getThreshold() * 100 );
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

      ConfusionMatrix auxMatrix;
      auxMatrix.setLowestTrainingThreshold( om.getModel(), om.getSampler() );
      Log::instance()->info( "Lowest prediction: %7.2f\n", auxMatrix.getThreshold() );
    }

    if ( request.calcAuc() ) {

      RocCurve * const roc_curve = om.getRocCurve();

      // ROC curve
      Log::instance()->info( "AUC:               %7.2f\n", roc_curve->getTotalArea() );
    }

    // Projection statistics
    if ( request.requestedProjection() ) {

      Log::instance()->info( "\n" );
      Log::instance()->info( "Projection statistics\n" );

      AreaStats * stats = om.getActualAreaStats();

      Log::instance()->info( "Threshold:                 50%%\n" );
      Log::instance()->info( "Cells predicted present: %7.2f%%\n", 
             stats->getAreaPredictedPresent() / (double) stats->getTotalArea() * 100 );
      Log::instance()->info( "Total number of cells:     %d\n", stats->getTotalArea() );
      Log::instance()->info( "Done.\n" );

      delete stats;
    }
  }
  catch ( std::exception& e ) {
    Log::instance()->info( "Exception occurred: %s", e.what() );
  }
  catch ( ... ) {
    Log::instance()->info( "Unknown error occurred\n" );
  }

  #ifdef MPI_FOUND
    MPI_Finalize();
  #endif

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

  int count = 1;
  AlgMetadata const *metadata;
  while ( ( metadata = *availables++ ) )
  {
    printf( " [%d] %s\n", count++, metadata->name.c_str() );
  }
  printf( " [q] Quit\n" );
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

    // Quit if input is "q" or "Q"
    if ( first_char_ascii == 113 || first_char_ascii == 81 ) {
      return 0;
    }

    option = atoi( buf );

    if ( option <= 0 || option >= quit_option ) {
      return 0;
    }

    // An algorithm was choosed.
    else {
      return availables[option-1];
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
      printf( "\n* Parameter: %s\n\n", param->name.c_str() );
      printf( " %s\n", param->overview.c_str() );

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
  Log::instance()->info( "Model creation: %07.4f%%\r", 100 * progress );
}


/********************/
/*** map Callback ***/
/**
 * Shows the map creation progress.
 */
void
mapCallback( float progress, void *extra_param )
{
  Log::instance()->info( "Map creation: %07.4f%%\n", 100 * progress );
}
