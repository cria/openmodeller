/**
 * Declaration of Occurrences class.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-02-25
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


#ifndef _OCCURRENCESHH_
#define _OCCURRENCESHH_

#include "om_defs.hh"

class Occurrence;
template<class T> class DList;


/****************************************************************/
/************************* Occurrences **************************/

/** 
 * Representation of a set of occurrences.
 *
 */
class Occurrences
{
  typedef DList<Occurrence *> LstOccur;


public:

  Occurrences( char *name, char *id );
  ~Occurrences();

  char *name()   { return _name; }
  char *id()     { return _id;   }

  /** Insert an occurrence. */
  void insert( Coord longitude, Coord latitude, float abundance=1.0 );

  /** Number of attributes of the thing occurred. This is the number
   *  of dependent variables, ie the variables to be modelled.
   *  
   *  Fix: By now this is hardcoded to 1 (= abundance).
   */
  int numAttributes()  { return 1; }

  /** Number of occurrences. */
  int numOccurrences();

  /** Navigate on the list of occurrences. */
  void head();
  void next();
  Occurrence *get();

  /** Remove from list the current coordinate, returning it. */
  Occurrence *remove();

  /** Choose an occurrence at random. */
  Occurrence *getRandom();

  /** Print occurrence data and its points. */
  void print( char *msg="" );


private:

  /** Build a vector view for the occurrences in the list _occur. */
  void buildVector();

  char *_name; ///< A name for the list of occurrences (e.g. species name).
  char *_id;   ///< An identifier for the list of occurrences.

  LstOccur *_occur;  ///< Coordinates of the occurrences.

  Occurrence **_vector; ///< Vector view to the occurrences.
};


#endif
