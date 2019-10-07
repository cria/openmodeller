/**
 * Declaration of OmAlgParameter class.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2004-04-16
 * $Id: AlgParameter.cpp 4352 2008-06-11 16:39:53Z rdg $
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

/********************/
/*** constructors ***/

AlgParameter::AlgParameter():
  _id(""),
  _value("")
{

}

AlgParameter::AlgParameter( std::string const id, std::string const value )
{
  _id = id;
  _value = value;
}

AlgParameter::AlgParameter( const AlgParameter &param )
{
  _id = param._id;
  _value = param._value;
}


/******************/
/*** destructor ***/

AlgParameter::~AlgParameter()
{

}


/******************/
/*** operator = ***/
AlgParameter &
AlgParameter::operator=( const AlgParameter &param )
{
  if ( this == &param ) {

    return *this;
  }

  _id = param._id;
  _value = param._value;

  return *this;
}


/******************/
/*** value Real ***/
double
AlgParameter::valueReal()
{
  return _value.c_str() ? atof( _value.c_str() ) : 0.0;
}


/*****************/
/*** set Value ***/
void
AlgParameter::setValue( double value )
{
  char buf[320];
  sprintf( buf, "%-32.8f", value );

  _value = std::string(buf);
}



