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
#include "occurrences_file.hh"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int showAlgorithms ( AlgMetadata **availables );
char *readParameters ( AlgMetadata *metadata );
AlgMetadata *readAlgorithm( AlgMetadata **availables );
Occurrences *readOccurrences( char *file, char *name,
			      char *coord_system );
int readParameters( AlgParameter *result, AlgMetadata *metadata,
                    int num_param_values, char **param_values );
char *extractParameter( char *name, int nvet, char **vet );


/**************************************************************/
/*************** openModeller Console Interface ***************/

/************/
/*** main ***/
int
main( int argc, char **argv )
{
  // Reconfigure the global logger.
  g_log.set( Log::Debug, stdout, "Console" );

  if ( argc < 2 )
    g_log.error( 1, "\n%s <request>\n\n", argv[0] );

  // Create a fileparser to read in the request file and a
  // controlInterface to manage the model process.
  FileParser fp( argv[1] );
  OpenModeller om;

  g_log( "\nopenModeller version %s\n", om.getVersion() );

  g_log( "\nAlgorithms will be loaded from: %s\n\n", om.getPluginPath() );


  /*** Occurrence points ***/

  // Obtain the Well Known Text string for the localities
  // coordinate system.
  char *oc_cs = fp.get( "WKT coord system" );

  // Get the name of the file containing localities
  char *oc_file = fp.get( "Species file" );

  // Get the name of the taxon being modelled!
  char *oc_name = fp.get( "Species" );

  Occurrences *oc = readOccurrences( oc_file, oc_name, oc_cs );

  // Populate the occurences list from the localities file
  om.setOccurrences( oc );


  /*** Environmental data (mask and maps) ***/

  // Mask to select the desired species occurrence points
  char *mask = fp.get( "Mask" );

  // Categorical environmental maps and the number of these maps.
  char *cat_label = "Categorical map";
  int  ncat  = fp.count( cat_label );
  char **cat = new char *[ncat];

  // Continuous environmental maps and the number of these maps.
  char *map_label = "Map";
  int  nmap  = fp.count( map_label );
  char **map = new char *[nmap];

  // Initiate the environment with all maps.
  fp.getAll( cat_label, cat );
  fp.getAll( map_label, map );
  om.setEnvironment( ncat, cat, nmap, map, mask );

  delete cat;
  delete map;


  /*** Output Map ***/

  // Get the details for the output Map
  char *output = fp.get( "Output" );
  char *format = fp.get( "Output format" );

  // scale is used to scale the model results e.g. from [0,1] to
  // [0,255] - useful for image generation.
  char *scale  = fp.get( "Scale" );

  // Make sure the basic variables have been defined in the
  // parameter file...
  if ( ! output )
    {
      g_log( "The 'Output' file name was not speciefied!\n" );
      return 1;
    }
  if ( ! format )
    {
      g_log( "The 'Output format' was not specified!\n" );
      return 1;
    }
  if ( ! scale )
    scale = "255.0";


  /*** Algorithm ***/

  // Find out which model algorithm is to be used.
  AlgMetadata **availables = om.availableAlgorithms();
  AlgMetadata *metadata;
  char *alg_id = fp.get( "Algorithm" );

  g_log( "\nNumber of available algorithms: %d\n", om.numAvailableAlgorithms() );

  // Try to used the algorithm specified in the request file.
  // If it can not be used, read it from stdin.
  if ( ! (metadata = om.algorithmMetadata( alg_id )) &&
       ! (metadata = readAlgorithm( availables )) )
    return 1;

  g_log( "Algorithm used: %s\n", metadata->id );
  g_log( " %s\n\n", metadata->description );

  // Obtain any model parameter specified in the request file.
  char *param_label = "Parameter";
  int   req_nparam  = fp.count( param_label );
  char **req_param  = new char *[req_nparam];

  // read parameters from file into req_param parameters
  fp.getAll(param_label, req_param);

  // For resulting parameters storage.
  int nparam = metadata->nparam;
  AlgParameter *param = new AlgParameter[nparam];

  // Read from console the parameters not set by request
  // file. Fills 'param' with all 'metadata->nparam' parameters
  // set.
  readParameters( param, metadata, req_nparam, req_param );

  // Set the model algorithm to be used by the controller
  om.setAlgorithm( metadata->id, nparam, param );

  delete[] param;


  /*** Run the model ***/

  if ( ! om.run() )
    g_log.error( 1, "Error: %s\n", om.error() );
  else
    g_log( "Done.\n" );

  // Prepare the output map
  om.createMap( om.getEnvironment(), output, atof(scale), 0, format);

  g_log( "\n" );
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
showAlgorithms( AlgMetadata **availables )
{
  if ( ! *availables )
    {
      printf( "No algorithm available.\n" );
      return 0;
    }

  int count = 0;
  AlgMetadata *metadata;
  while ( metadata = *availables++ )
    printf( " [%d] %s\n", count++, metadata->id );

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
AlgMetadata *
readAlgorithm( AlgMetadata **availables )
{
  char buf[128];

  while ( 1 )
    {
      printf( "\nChoose an algorithm between:\n" );

      int quit = showAlgorithms( availables );
      int option = -1;

      printf( "\nOption: " );
      fgets( buf, 128, stdin );
      option = atoi( buf );

      if ( option == quit )
	return 0;

      // An algorithm was choosed.
      else if ( option >= 0 && option < quit )
	return availables[option];
    }
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


/***********************/
/*** read Parameters ***/
int
readParameters( AlgParameter *result, AlgMetadata *metadata,
                int str_nparam, char **str_param )
{
  AlgParamMetadata *param = metadata->param;
  AlgParamMetadata *end   = param + metadata->nparam;

  // For each algorithm parameter metadata...
  for ( ; param < end; param++, result++ )
    {
      // The resulting name is equal the name set in
      // algorithm's metadata.
      result->setName( param->name );

      // Read the resulting value from str_param.
      char *value = extractParameter( result->name(), str_nparam,
                                      str_param );

      if ( value )
        result->setValue( value );

      // If the value is not in str_param, read it from console.
      else
        {
          // Informs the parameter's metadata to the user.
          printf( "\nParameter %s:\n", param->name );
          printf( " %s:\n", param->description );
          if ( param->has_min )
            printf( " %s >= %f\n", param->name, param->min );
          if ( param->has_max )
            printf( " %s <= %f\n\n", param->name, param->max );
          printf( "Value [%s]: ", param->typical );

          // Read parameter's value or use the "typical" value
          // if the user does not enter a new value.
          char value[64];
	  *value = 0;
	  if ( fgets( value, 64, stdin ) && (*value >= ' ') )
            result->setValue( value );
          else
            result->setValue( param->typical );
        }
    }

  return metadata->nparam;
}


/*************************/
/*** extract Parameter ***/
/**
 * Search for 'name' in the 'nvet' elements of the vector 'vet'.
 * If the string 'name' is in the begining of some string vet[i]
 * then returns a pointer to the next character of vet[i],
 * otherwise returns 0.
 */
char *
extractParameter( char *name, int nvet, char **vet )
{
  int length = strlen( name );
  char **end = vet + nvet;

  while ( vet < end )
    if ( ! strncmp( name, *vet++, length ) )
      return *(vet-1) + length;

  return 0;
}
