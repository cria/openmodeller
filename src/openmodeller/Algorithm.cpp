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
#include <OmAlgParameter.hh>
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
  
  allocID( _metadata );
}


/******************/
/*** destructor ***/

Algorithm::~Algorithm()
{
  if ( _categ )
    delete _categ;

  if ( _metadata && _metadata->id )
    delete _metadata->id;
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
Algorithm::setParameters( int nparam, OmAlgParameter *param )
{
  _nparam = nparam;
  _param  = param;
}


/****************/
/*** alloc ID ***/
int
Algorithm::allocID( AlgMetadata *meta )
{
  const int id_size = 256;

  char *id = meta->id = new char[id_size];

  // String to link algorithm's "name" and "version";
  char *link = " v";

  strncpy( id, meta->name, id_size );
  int name_size = id_size;
  name_size -= strlen( meta->name ) + strlen( link );

  if ( name_size > 0 )
    {
      strcat( id, link );
      strncat( id, meta->version, name_size );
    }

  id[id_size - 1] = '\0';
  return 1;
}


/*********************/
/*** get Parameter ***/
int
Algorithm::getParameter( char *name, char **value )
{
  if ( ! name )
    {
      g_log.warn( "Algorithm %s wants a parameter named \"%s\"!\n",
		  _metadata->name, name );
      return 0;
    }
		

  // If parameters were not setted or zero parameters were setted.
  if ( ! _param || ! _nparam )
    return 0;

  
  OmAlgParameter *param = _param;
  OmAlgParameter *end   = _param + _nparam;

  while ( param < end )
    if ( ! strcmp( name, param->name() ) )
      {
        *value = param->value();
        return 1;
      }

  return 0;
}


/*********************/
/*** get Parameter ***/
int
Algorithm::getParameter( char *name, double *value )
{
  char *str_value;

  if ( ! getParameter( name, &str_value ) )
    return 0;

  *value = atof( str_value );
  return 1;
}


/*********************/
/*** get Parameter ***/
int
Algorithm::getParameter( char *name, float *value )
{
  char *str_value;

  if ( ! getParameter( name, &str_value ) )
    return 0;

  *value = float( atof( str_value ) );
  return 1;
}


