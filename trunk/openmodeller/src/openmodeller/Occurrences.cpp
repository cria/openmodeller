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
#include "om_log.hh"
#include "env_io/geo_transform.hh"

#include <string.h>


/****************************************************************/
/************************ Occurrences ***************************/

/*******************/
/*** Constructor ***/

Occurrences::Occurrences( char *name, char *coord_system )
{
  _occur  = new LstOccur;
  _vector = 0;

  _name = new char[ strlen(name) + 1 ];
  strcpy( _name, name );

  // Only use the GeoTransform object if the coordinate system
  // of the given occurrences is different from the internal
  // common openModeller coordinate system.
  //
  if ( strcmp( coord_system, OM_COORDINATE_SYSTEM ) )
    _gt = new GeoTransform( coord_system, OM_COORDINATE_SYSTEM );
  else
    _gt = 0;
}


/******************/
/*** Destructor ***/

Occurrences::~Occurrences()
{
  Occurrence *oc;
  for ( _occur->head(); oc = _occur->get(); _occur->next() )
    delete( oc );

  if ( _vector )
    delete _vector;

  delete _occur;

  if ( _gt )
    delete _gt;
}


/**************/
/*** insert ***/
void
Occurrences::insert( Coord longitude, Coord latitude,
		     Scalar error, Scalar abundance,
		     int num_attributes, Scalar *attributes )
{
  // Transforms the given coordinates in the common openModeller
  // coordinate system.
  if ( _gt )
    _gt->transfOut( &longitude, &latitude );

  Occurrence *oc = new Occurrence( longitude, latitude, error,
				   abundance, num_attributes,
				   attributes );
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
  g_log( "%s\n", msg );

  // Occurrences general data.
  g_log( "Name: %s\n", _name );
  g_log( "\nOccurrences: %d\n\n", numOccurrences() );

  // Occurrence points.
  Occurrence *c;
  for ( head(); c = get(); next() )
    {
      g_log( "(%+8.4f, %+8.4f)", c->x(), c->y() );
      g_log( " - %6.2", c->error() );

      // Print the attributes.
      Scalar *attr = c->attributes();
      Scalar *end  = attr + c->numAttributes();
      g_log( " [%+8,4f", *attr++ );
      while ( attr < end )
	g_log( "%+8.4f, ", *attr++ );
      g_log( "]\n" );
    }
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


