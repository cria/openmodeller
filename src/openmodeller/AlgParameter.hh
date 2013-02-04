/**
 * Declaration of AlgParameter class.
 * 
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

#include <openmodeller/os_specific.hh>
#include <openmodeller/om_defs.hh>
#include <string>


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
  AlgParameter( std::string const id, std::string const value=0 );
  AlgParameter( const AlgParameter & );
  ~AlgParameter();

  AlgParameter &operator=( const AlgParameter & );

  /** Returns the parameter's id. */
  std::string const id() const { return _id; }

  /** Set parameter's id. */
  void setId( std::string const id )  { _id = id; }

  /** Returns the parameter's value. */
  std::string const value() const { return _value; }

  /** Returns the parameter's value converted to double. */
  double valueReal();

  /** Set parameter's value. */
  void setValue( std::string const val ) { _value = val; }
  void setValue( double val );

private:

  std::string _id;
  std::string _value;
};


#endif


