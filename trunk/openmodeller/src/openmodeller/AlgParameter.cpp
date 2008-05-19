/**
 * Declaration of OmAlgParameter class.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2004-04-16
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2004 by CRIA -
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

#include <openmodeller/AlgParameter.hh>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/****************************************************************/
/*********************** Om Alg Parameter ***********************/

/*******************/
/*** constructor ***/

AlgParameter::AlgParameter()
{
  _id = _value = 0;
}

AlgParameter::AlgParameter( char const *id, char const *value )
{
  _id = _value = 0;

  newCopy( &_id, id );
  newCopy( &_value, value );
}

AlgParameter::AlgParameter( const AlgParameter &param )
{
  _id = _value = 0;

  newCopy( &_id, param._id );
  newCopy( &_value, param._value );
}


/******************/
/*** destructor ***/

AlgParameter::~AlgParameter()
{
  if ( _id  )   delete [] _id;
  if ( _value ) delete [] _value;
}


/******************/
/*** operator = ***/
AlgParameter &
AlgParameter::operator=( const AlgParameter &param )
{
  if ( this == &param )
    return *this;

  newCopy( &_id, param._id );
  newCopy( &_value, param._value );

  return *this;
}


/******************/
/*** value Real ***/
double
AlgParameter::valueReal()
{
  return _value ? atof( _value ) : 0.0;
}


/*****************/
/*** set Value ***/
char *
AlgParameter::setValue( double value )
{
  char buf[32];
  sprintf( buf, "%-32.8f", value );

  return newCopy( &_value, buf );
}


/****************/
/*** new Copy ***/
char *
AlgParameter::newCopy( char const *src )
{
  return src ? strcpy( new char[strlen(src)+1], src ) : 0;
}

char *
AlgParameter::newCopy( char **dst, char const *src )
{
  if ( *dst ) {

    delete [] *dst;
  }

  return *dst = newCopy( src );
}



