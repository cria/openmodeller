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
#include "random.hh"

#include <string.h>


/****************************************************************/
/************************ Occurrences ***************************/

/*******************/
/*** Constructor ***/

Occurrences::Occurrences( char *name, char *id )
{
  _occur  = new LstOccur;
  _vector = 0;

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
  for ( _occur->head(); oc = _occur->get(); _occur->next() )
    delete( oc );

  delete _occur;

  if ( _vector )
    delete _vector;
}


/**************/
/*** insert ***/
void
Occurrences::insert( Coord longitude, Coord latitude, float pop )
{
  Occurrence *oc = new Occurrence( longitude, latitude, pop );
  _occur->insertLast( oc );

  // Signal to rebuild vector view the next time "getRandom()"
  // is called.
  if ( _vector )
    {
      delete _vector;
      _vector = 0;
    }
}


/***********************/
/*** num Occurrences ***/
int
Occurrences::numOccurrences()
{
  return _occur->length();
}


/************/
/*** head ***/
void
Occurrences::head()
{
  _occur->head();
}


/************/
/*** next ***/
void
Occurrences::next()
{
  _occur->next();
}


/***********/
/*** get ***/
Occurrence *
Occurrences::get()
{
  return _occur->get();
}


/**************/
/*** remove ***/
Occurrence *
Occurrences::remove()
{
  return _occur->remove();
}


/******************/
/*** get Random ***/
Occurrence *
Occurrences::getRandom()
{
  if ( ! _vector )
    buildVector();

  Random rnd;
  int selected = (int) rnd( numOccurrences() );

  return _vector[ selected ];
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


/********************/
/*** build Vector ***/
void
Occurrences::buildVector()
{
  // Stores the actual node of "_occur".
  void *list_pos = _occur->getPos();

  int noccur = numOccurrences();
  Occurrence **vector = _vector = new (Occurrence *)[ noccur ];

  Occurrence *oc;
  for ( _occur->head(); oc = _occur->get(); _occur->next() )
    *vector++ = oc;

  // Restores the actual node of "_occur".
  _occur->setPos( list_pos );
}


