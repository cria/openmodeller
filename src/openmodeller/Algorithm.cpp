/**
 * Definition of Algorithm class.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-10-05
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

#include <om_algorithm.hh>
#include <om_alg_parameter.hh>
#include <om_log.hh>

#include <string.h>
#include <stdlib.h>


/****************************************************************/
/************************** Algorithm ***************************/

/*******************/
/*** constructor ***/

Algorithm::Algorithm( AlgMetadata *metadata )
{
  _metadata = metadata;

  _samp   = 0;
  _categ  = 0;
  _nparam = 0;
  _param  = 0;
}


/******************/
/*** destructor ***/

Algorithm::~Algorithm()
{
  if ( _categ )
    delete _categ;
}


/*******************/
/*** set Sampler ***/
void
Algorithm::setSampler( Sampler *samp )
{
  _samp = samp;

  if ( _categ )
    delete _categ;

  _categ = new int[ _samp->numIndependent() ];
  _samp->varTypes( _categ );
}


/**********************/
/*** set Parameters ***/
void
Algorithm::setParameters( int nparam, AlgParameter *param )
{
  _nparam = nparam;
  _param  = param;
}


/*********************/
/*** get Parameter ***/
int
Algorithm::getParameter( char *id, char **value )
{
  if ( ! id )
    {
      g_log.warn( "Algorithm %s wants a parameter with id '%s'!\n",
		  _metadata->id, id );
      return 0;
    }
		

  // If parameters were not set or zero parameters were set.
  if ( ! _param || ! _nparam )
    return 0;

  
  AlgParameter *param = _param;
  AlgParameter *end   = _param + _nparam;

  for( ; param < end; param++ )  
    if ( ! strcmp( id, param->id() ) )
      return (*value = param->value()) ? 1 : 0;

  return 0;
}


/*********************/
/*** get Parameter ***/
int
Algorithm::getParameter( char *id, int *value )
{
  char *str_value;

  if ( ! getParameter( id, &str_value ) )
    return 0;

  *value = atoi( str_value );
  return 1;
}


/*********************/
/*** get Parameter ***/
int
Algorithm::getParameter( char *id, double *value )
{
  char *str_value;

  if ( ! getParameter( id, &str_value ) )
    return 0;

  *value = atof( str_value );
  return 1;
}


/*********************/
/*** get Parameter ***/
int
Algorithm::getParameter( char *id, float *value )
{
  char *str_value;

  if ( ! getParameter( id, &str_value ) )
    return 0;

  *value = float( atof( str_value ) );
  return 1;
}


