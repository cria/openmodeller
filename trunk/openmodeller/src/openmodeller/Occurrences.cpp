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
  f_ocur = new LstOccur;

  f_name = new char[ strlen(name) + 1 ];
  strcpy( f_name, name );

  f_id = new char[ strlen(id) + 1 ];
  strcpy( f_id, id );
}


/******************/
/*** Destructor ***/

Occurrences::~Occurrences()
{
  Occurrence *oc;
  for ( f_ocur->Head(); oc = f_ocur->Get(); f_ocur->Next() )
    delete( oc );

  delete f_ocur;
}


/**************/
/*** insert ***/
void
Occurrences::insert( Coord longitude, Coord latitude, float pop )
{
  Occurrence *oc = new Occurrence( longitude, latitude, pop );
  f_ocur->InsertLast( oc );
}


/***********************/
/*** num Occurrences ***/
int
Occurrences::numOccurrences()
{
  return f_ocur->Length();
}


/************/
/*** head ***/
void
Occurrences::head()
{
  f_ocur->Head();
}


/************/
/*** next ***/
void
Occurrences::next()
{
  f_ocur->Next();
}


/***********/
/*** get ***/
Occurrence *
Occurrences::get()
{
  return f_ocur->Get();
}


/**************/
/*** remove ***/
Occurrence *
Occurrences::remove()
{
  return f_ocur->Delete();
}


/*************/
/*** print ***/
void
Occurrences::print( char *msg )
{
  printf( "%s\n", msg );

  // Occurrences general data.
  printf( "Name: %s\n", f_name );
  printf( "ID  : %s\n", f_id );
  printf( "\nOccurrences: %d\n\n", numOccurrences() );

  // Occurrence points.
  Occurrence *c;
  for ( head(); c = get(); next() )
    printf( "(%+9.4f, %+8.4f)\n", c->x, c->y );
}


