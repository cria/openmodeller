/**
 * Definition of Occurrences class.
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

#include "om_occurrences.hh"

#include "list.cpp"     // Template.
#include "occurrence.hh"

#include <string.h>


/****************************************************************/
/************************ Occurrences ***************************/

/*******************/
/*** Constructor ***/

Occurrences::Occurrences( char *name, char *id )
{
  _ocur = new LstOccur;

  _name = new char[ strlen(name) + 1 ];
  strcpy( _name, name );

  _id = new char[ strlen(id) + 1 ];
  strcpy( _id, id );
}


/******************/
/*** Destructor ***/

Occurrences::~Occurrences()
{
  Occurrence *oc;
  for ( _ocur->head(); oc = _ocur->get(); _ocur->next() )
    delete( oc );

  delete _ocur;
}


/**************/
/*** insert ***/
void
Occurrences::insert( Coord longitude, Coord latitude, float pop )
{
  Occurrence *oc = new Occurrence( longitude, latitude, pop );
  _ocur->insertLast( oc );
}


/***********************/
/*** num Occurrences ***/
int
Occurrences::numOccurrences()
{
  return _ocur->length();
}


/************/
/*** head ***/
void
Occurrences::head()
{
  _ocur->head();
}


/************/
/*** next ***/
void
Occurrences::next()
{
  _ocur->next();
}


/***********/
/*** get ***/
Occurrence *
Occurrences::get()
{
  return _ocur->get();
}


/**************/
/*** remove ***/
Occurrence *
Occurrences::remove()
{
  return _ocur->remove();
}


/*************/
/*** print ***/
void
Occurrences::print( char *msg )
{
  printf( "%s\n", msg );

  // Occurrences general data.
  printf( "Name: %s\n", _name );
  printf( "ID  : %s\n", _id );
  printf( "\nOccurrences: %d\n\n", numOccurrences() );

  // Occurrence points.
  Occurrence *c;
  for ( head(); c = get(); next() )
    printf( "(%+9.4f, %+8.4f)\n", c->x, c->y );
}


