/**
 * Declaration of AlgParameter class.
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


#ifndef _ALG_PARAMETERHH_
#define _ALG_PARAMETERHH_

#include <om_defs.hh>


/*************************************************************/
/*********************** Alg Parameter ***********************/

/**
 * Algorithms' parameters. A couple (id, value)
 * 
 **/
class dllexp AlgParameter
{
public:

  AlgParameter();
  AlgParameter( char *id, char *value=0 );
  AlgParameter( AlgParameter & );
  ~AlgParameter();

  AlgParameter &operator=( AlgParameter & );


  /** Returns the parameter's id. */
  char *id()  { return _id; }

  /** Returns an allocated copy of id. */
  char *idCopy()  { return newCopy( _id ); }

  /** Set parameter's id. */
  char *setId( char *id )  { return newCopy(&_id, id); }


  /** Returns the parameter's value. */
  char *value()  { return _value; }

  /** Returns the parameter's value converted to double. */
  double valueReal();

  /** Returns an allocated copy of value. */
  char *valueCopy()  { return newCopy( _value ); }

  /** Set parameter's value. */
  char *setValue( char  *val )  { return newCopy(&_value,val); }
  char *setValue( double val );


private:

  char *newCopy( char *src );
  char *newCopy( char **dst, char *src );


  char *_id;
  char *_value;

};


#endif


