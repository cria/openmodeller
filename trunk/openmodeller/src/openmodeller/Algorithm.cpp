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

#include <string.h>
#include <stdlib.h>


/****************************************************************/
/************************** Algorithm ***************************/

/*******************/
/*** constructor ***/

Algorithm::Algorithm( AlgorithmMetadata *metadata )
{
  _metadata = metadata;

  _samp  = 0;
  _categ = 0;
  _param = 0;
  _param_setted = 0;
  
  findID( _metadata );
}


/******************/
/*** destructor ***/

Algorithm::~Algorithm()
{
  if ( _categ ) delete _categ;
  if ( _param ) delete _param;
}


/*******************/
/*** set Sampler ***/
void
Algorithm::setSampler( Sampler *samp )
{
  _samp = samp;

  if ( _categ )
    delete _categ;

  _categ = new int[ _samp->dimEnv() ];
  _samp->varTypes( _categ );
}


/**********************/
/*** set Parameters ***/
int
Algorithm::setParameters( char *str_param )
{
  _param_setted = 1;

  if ( _param )
    delete _param;

  // Number of parameters setted in metadata.
  int nparam = _metadata->nparam;
  if ( ! nparam )
    {
      _param = 0;
      return 0;
    }
  
  // Vector of parameters.
  Scalar *par = _param = new Scalar[nparam];

  // Read the parameters from str_param string.
  for ( int i = 0; i < nparam; i++ )
    *par++ = strtod( str_param, &str_param );

  return nparam;
}


/***************/
/*** copy ID ***/
char *
Algorithm::copyID()
{
  char *id = new char[ strlen(_id) + 1 ];
  strcpy( id, _id );
  return id;
}


/***************/
/*** find ID ***/
int
Algorithm::findID( AlgorithmMetadata *meta )
{
  // String to link algorithm's "name" and "version";
  char *link = " v";

  strncpy( _id, meta->name, 256 );
  int name_size = 256 - strlen( meta->name ) - strlen(link);

  if ( name_size > 0 )
    {
      strcat( _id, link );
      strncat( _id, meta->version, name_size );
    }

  _id[255] = '\0';
  return 1;
}


/*********************/
/*** get Parameter ***/
int
Algorithm::getParameter( int i, Scalar *value )
{
  // Parameters were not setted or the i-th parameter does not
  // exits.
  if ( ! _param_setted || i >= _metadata->nparam )
    return 0;

  // It could be that parameters were setted to none!
  if ( _param )
    *value = _param[i];

  return 1;
}

