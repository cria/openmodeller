/**
 * openModeller console interface.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-09-16
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
#include <file_parser.hh>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/**************************************************************/
/*************** openModeller Console Interface ***************/

/***********************/
/*** show algorithms ***/
//
// Print available algorithms.
// Returns the option number associated with 'Quit' that is
// equal to the number of algorithms.
//
int
show_algorithms( Algorithm **algorithms )
{
  if ( ! *algorithms )
    {
      printf( "No algorithms available.\n" );
      return 0;
    }

  int nalg = 0;
  Algorithm *alg;
  while ( alg = *algorithms++ )
    printf( " [%d] %s\n", nalg++, alg->getID() );

  printf( " [%d] Quit\n", nalg );
  printf( "\n" );

  return nalg;
}


/*********************/
/*** get algorithm ***/
Algorithm *
get_algorithm( Algorithm **algorithms, char *alg_id )
{
  if ( ! alg_id )
    return 0;

  Algorithm *alg;
  while ( alg = *algorithms++ )
    if ( ! strcmp( alg_id, alg->getID() ) )
      return alg;

  return 0;
}


/***********************/
/*** print algorithm ***/
//
// Prints all algorithm metadata.
//
void
print_algorithm( Algorithm *algorithms )
{
}


/***********************/
/*** read parameters ***/
//
// Let the user choose the algorithm's parameters.
//
char *
read_parameters( Algorithm *algorithms )
{
  AlgorithmMetadata *meta = algorithms->getMetadata();

  int nparam = meta->nparam;
  AlgorithmParameter *param = meta->param;

  // Read the parameters' values in an array of floats.
  float *values = new float[nparam];
  for ( int i = 0; i < nparam; i++, param++ )
    {
      printf( "\nParameter %s:\n", param->name );
      printf( " %s:\n", param->description );
      if ( param->has_min )
	printf( " %s >= %f\n", param->name, param->min );
      if ( param->has_max )
	printf( " %s <= %f\n\n", param->name, param->max );

      // Read parameter's value.
      printf( "Value [%f]: ", values[i] = param->typical );
      scanf( "%f", values+i );
    }

  // Transform the array of floats in a string.
  char *str_values = new char[32 * nparam];
  char *end = str_values;
  for ( int i = 0; i < nparam; i++ )
    {
      sprintf( end, "%10.4f", values[i] );
      end += strlen(end);
    }

  // Array of floats.
  delete values;

  return str_values;
}


/**********************/
/*** read algorithm ***/
//
// Let the user choose an algorithm and enter its parameters.
// Returns the choosed algorithm's metadata.
//
Algorithm *
read_algorithm( Algorithm **algorithms )
{
  while ( 1 )
    {
      printf( "\nChoose an algorithm between:\n" );

      int quit = show_algorithms( algorithms );
      int option = -1;

      printf( "\nOption: " );
      scanf( "%d", &option );

      if ( option == quit )
	return 0;

      // An algorithm was choosed.
      else if ( option >= 0 && option < quit )
	return algorithms[option];
    }
}


/************/
/*** main ***/
int
main( int argc, char **argv )
{
  printf( "\nopenModeller - CRIA\n" );

  if ( argc < 2 )
    _log.error( 1, "\n%s <request>\n\n", argv[0] );

  //
  // Create a fileparser to read in the request file
  // and a controlInterface to manage the model process
  //
  FileParser fp( argv[1] );
  ControlInterface om;


  //
  // Read the input file for maps parameters to build the environment.
  //

  // Mask to select the desired species occurrence points
  char *mask = fp.get( "Mask" );

  // Categorical environmental maps and the number of these maps.
  char *cat_label = "Categorical map";
  int  ncat = fp.count( cat_label );

  // Continuous environmental maps and the number of these maps.
  char *map_label = "Map";
  int nmap = fp.count( map_label );

  // Total number of maps of any type.
  int nlayers = nmap + ncat;

  // create a char array and populate it with all layer names
  char **layers = new char*[nlayers];

  // Initiate the environment with all maps.
  fp.getAll( cat_label, layers );
  fp.getAll( map_label, layers + ncat );
  om.setEnvironment( ncat, nlayers, layers, mask );

  delete layers;

  //
  // Output Map
  //

  // Get the details for the output Map
  char *output = fp.get( "Output" );
  char *format = fp.get( "Output format" );

  // scale is used to scale the model results e.g. from [0,1] to
  // [0,255] - useful for image generation.
  char *scale  = fp.get( "Scale" );

  // Obtain the Well Known Text string for the localities
  // coordinate system.
  char *oc_cs = fp.get( "WKT coord system" );

  // Get the name of the file containing localities
  char *oc_file = fp.get( "Species file" );

  // Get the name of the taxon being modelled!
  char *oc_name = fp.get( "Species" );


  //
  // Make sure the basic variables have been defined in the
  // parameter file...
  //
  if ( ! output )
    {
      printf( "The 'Output' file name was not speciefied!\n" );
      return 1;
    }
  if ( ! format )
    {
      printf( "The 'Output format' was not specified!\n" );
      return 1;
    }
  if ( ! scale )
    scale = "255.0";

  // Prepare the output map
  om.setOutputMap( output, format, atof(scale) );


  //
  // Set up the algorithm.
  //

  // Find out which model algorithm is to be used.
  Algorithm **algorithms = om.availableAlgorithms();
  Algorithm *alg;
  char *alg_id = fp.get( "Algorithm" );

  // Try to used the algorithm specified in the request file.
  // If it can not be used, read it from stdin.
  if ( ! (alg = get_algorithm( algorithms, alg_id )) &&
       ! (alg = read_algorithm( algorithms )) )
    return 1;

  printf( "Algorithm used: %s\n", alg->getID() );
  printf( " %s\n\n", alg->getMetadata()->description );

  // Obtain any model parameters that are specified in the request
  // file
  char *param = fp.get( "Parameters" );
  if ( ! param )
    param = read_parameters( alg );


  // Set the model algorithm to be used by the controller
  om.setAlgorithm( alg->getID(), param );


  //
  // Set up the model controller
  //

  // Populate the occurences list from the localities file
  om.setOccurrences( oc_file, oc_cs, oc_name );


  //
  // Run the model
  //
  if ( ! om.run() )
    printf( "Error: %s\n", om.error() );
  else
    printf( "Done.\n" );
  printf( "\n" );

  return 0;
}
