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

#include "occurrence.hh"
#include "random.hh"
#include "om_log.hh"
#include "env_io/geo_transform.hh"
#include "configuration.hh"
#include "Exceptions.hh"

#include <string>
using std::string;

/****************************************************************/
/************************ Occurrences ***************************/

/*******************/
/*** Constructor ***/

OccurrencesImpl::~OccurrencesImpl()
{
  delete gt_;
}

void
OccurrencesImpl::setCoordinateSystem( const string& cs )
{
  cs_ = cs;
  initGeoTransform();
}

void
OccurrencesImpl::initGeoTransform()
{
  if (gt_) delete gt_;

  gt_ = new GeoTransform( cs_, GeoTransform::cs_default );

}

/******************/
/*** configuration ***/

ConfigurationPtr
OccurrencesImpl::getConfiguration() const
{
  ConfigurationPtr config( new ConfigurationImpl("Occurrences") );

  config->addNameValue( "SpeciesName", name() );

  ConfigurationPtr cs( new ConfigurationImpl( "CoordinateSystem" ) );
  cs->setValue( coordSystem() );

  config->addSubsection( cs );

  config->addNameValue( "Count", int(occur_.size()) );

  const_iterator oc = occur_.begin();
  const_iterator end = occur_.end();

  while( oc != end ) {

    ConfigurationPtr cfg( new ConfigurationImpl("Point") );
    Scalar x = (*oc)->x();
    Scalar y = (*oc)->y();
    gt_->transfIn( &x, &y );
    cfg->addNameValue( "X", x );
    cfg->addNameValue( "Y", y );
    config->addSubsection( cfg );

    oc++;

  }

  return config;
}

void
OccurrencesImpl::setConfiguration( const ConstConfigurationPtr& config )
{

  name_ = config->getAttribute("SpeciesName");
  
  ConstConfigurationPtr cs_config = config->getSubsection( "CoordinateSystem", false );
  
  if ( !cs_config ) {
    g_log.warn( "Occurrences has no Coordinate System.  Assuming WSG84" );
    cs_ = GeoTransform::cs_default;
  }
  else {
    cs_ = cs_config->getValue();
  }

  initGeoTransform( );

  Configuration::subsection_list subs = config->getAllSubsections();

  Configuration::subsection_list::iterator begin = subs.begin();
  Configuration::subsection_list::iterator end = subs.end();
  for ( ; begin != end; ++begin ) {

    if ( (*begin)->getName() != "Point" ) 
      continue;

    Scalar x = (*begin)->getAttributeAsDouble( "X", 0.0 );
    Scalar y = (*begin)->getAttributeAsDouble( "Y", 0.0 );
    Scalar abundance = (*begin)->getAttributeAsDouble( "Abundance", default_abundance_ );

    createOccurrence( x, y, 0, abundance, 0, 0, 0, 0 );
    
  }

}

/**************/
/*** insert ***/
void
OccurrencesImpl::createOccurrence( Coord longitude, Coord latitude,
				   Scalar error, Scalar abundance,
				   int num_attributes, Scalar *attributes,
				   int num_env, Scalar *env )
{
  // Transforms the given coordinates in the common openModeller
  // coordinate system.
  gt_->transfOut( &longitude, &latitude );
  
  insert( new OccurrenceImpl( longitude, latitude, error, abundance,
			      num_attributes, attributes,
			      num_env, env ) );
  
}

void
OccurrencesImpl::insert( const OccurrencePtr& oc )
{
  occur_.push_back(oc);
}

OccurrencesImpl*
OccurrencesImpl::clone() const
{
  
  const_iterator it = occur_.begin();
  const_iterator end = occur_.end();
  
  OccurrencesImpl* clone = new OccurrencesImpl( name_, cs_ );

  while( it != end ) {
    
    clone->insert( new OccurrenceImpl( *(*it) ) );
    
    it++;
  }

  return clone;
}

/******************/
/*** get Random ***/
ConstOccurrencePtr
OccurrencesImpl::getRandom() const
{

  Random rnd;
  int selected = (int) rnd( numOccurrences() );

  return occur_[ selected ];
}

OccurrencesImpl::iterator
OccurrencesImpl::erase( const iterator& it ) {
  swap( occur_.back(), (*it) );
  occur_.pop_back();
  return it;
}

/*************/
/*** print ***/
void
OccurrencesImpl::print( char *msg ) const
{
  g_log( "%s\n", msg );

  // Occurrences general data.
  g_log( "Name: %s\n", name_.c_str() );
  g_log( "\nOccurrences: %d\n\n", numOccurrences() );

  const_iterator c = occur_.begin();
  const_iterator end = occur_.end();
  while ( c != end ) {
    g_log( "(%+8.4f, %+8.4f)", (*c)->x(), (*c)->y() );
    g_log( " - %6.2", (*c)->error() );
    
    // Print the attributes.
    Sample::const_iterator attr = (*c)->attributes().begin();
    Sample::const_iterator end = (*c)->attributes().end();
    g_log(" [" );
    while ( attr != end ) {
      g_log( "%+8.4f, ", *attr++ );
    }
    g_log( "]\n" );
    c++;
  }
}


