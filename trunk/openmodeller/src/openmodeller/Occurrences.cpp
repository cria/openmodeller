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

#include <openmodeller/Occurrences.hh>

#include <openmodeller/Occurrence.hh>
#include <openmodeller/Random.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/env_io/GeoTransform.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Exceptions.hh>

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
  if ( gt_ ) {

    delete gt_;
  }

  gt_ = new GeoTransform( cs_, GeoTransform::cs_default );
}

/******************/
/*** configuration ***/

ConfigurationPtr
OccurrencesImpl::getConfiguration() const
{
  ConfigurationPtr config( new ConfigurationImpl("Occurrences") );

  config->addNameValue( "Label", name() );

  ConfigurationPtr cs( new ConfigurationImpl( "CoordinateSystem" ) );
  cs->setValue( coordSystem() );

  config->addSubsection( cs );

  config->addNameValue( "Count", int(occur_.size()) );

  const_iterator oc = occur_.begin();
  const_iterator end = occur_.end();

  while ( oc != end ) {

    ConfigurationPtr cfg( new ConfigurationImpl("Point") );
    std::string id = (*oc)->id();
    Scalar x = (*oc)->x();
    Scalar y = (*oc)->y();
    gt_->transfIn( &x, &y );
    cfg->addNameValue( "Id", id );
    cfg->addNameValue( "X", x );
    cfg->addNameValue( "Y", y );
    cfg->addNameValue( "Sample", (*oc)->environment() );
    config->addSubsection( cfg );

    oc++;
  }

  return config;
}

void
OccurrencesImpl::setConfiguration( const ConstConfigurationPtr& config )
{
  name_ = config->getAttribute("Label");
  
  ConstConfigurationPtr cs_config = config->getSubsection( "CoordinateSystem", false );
  
  if ( ! cs_config ) {

    g_log.warn( "Occurrences has no Coordinate System. Assuming WSG84\n" );
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

    if ( (*begin)->getName() != "Point" ) {

      continue;
    }

    std::string id = (*begin)->getAttribute("Id");
    Scalar x = (*begin)->getAttributeAsDouble( "X", 0.0 );
    Scalar y = (*begin)->getAttributeAsDouble( "Y", 0.0 );
    Scalar abundance = (*begin)->getAttributeAsDouble( "Abundance", default_abundance_ );

    createOccurrence( id, x, y, 0, abundance, 0, 0, 0, 0 );
  }
}

void 
OccurrencesImpl::setEnvironment( const EnvironmentPtr& env, const char *type )
{
  if ( isEmpty() ) {

    return;
  }

  OccurrencesImpl::iterator oc = occur_.begin();
  OccurrencesImpl::iterator fin = occur_.end();

  while ( oc != fin ) {

    Sample sample = env->getUnnormalized( (*oc)->x(), (*oc)->y() );

    if ( sample.size() == 0 ) {

      g_log( "%s Point \"%s\" at (%f,%f) has no environment. It will be discarded.\n", 
             type, ((*oc)->id()).c_str(), (*oc)->x(), (*oc)->y() );

      oc = occur_.erase( oc );
      fin = occur_.end();
    } 
    else {

      (*oc)->setUnnormalizedEnvironment( sample );
      (*oc)->setNormalizedEnvironment( Sample() );

      ++oc;
    }
  }
}

/*****************/
/*** normalize ***/
void 
OccurrencesImpl::normalize(bool useNormalization,
			   const Sample& offsets, const Sample& scales)
{
  if ( ! useNormalization ) {

    return;
  }

  OccurrencesImpl::const_iterator occ = occur_.begin();
  OccurrencesImpl::const_iterator end = occur_.end();
  
  int dim = (*occ)->environment().size();

  // set the normalized values 
  while ( occ != end ) {

    (*occ)->normalize( offsets, scales );
    ++occ;
  }
}


void
OccurrencesImpl::getMinMax(Sample * min, Sample * max ) const
{
  OccurrencesImpl::const_iterator occ = occur_.begin();
  OccurrencesImpl::const_iterator end = occur_.end();

  *min = Sample( (*occ)->environment() );
  *max = Sample( (*occ)->environment() );

  // grab max and min values per variable
  while ( occ != end ) {

      Sample sample = (*occ)->environment();
      *min &= sample;
      *max |= sample;
      ++occ;
  }
}


/**************/
/*** insert ***/
void
OccurrencesImpl::createOccurrence( const std::string& id, 
                                   Coord longitude, Coord latitude,
                                   Scalar error, Scalar abundance,
                                   int num_attributes, Scalar *attributes,
                                   int num_env, Scalar *env )
{
  // Transforms the given coordinates in the common openModeller
  // coordinate system.
  gt_->transfOut( &longitude, &latitude );
  
  insert( new OccurrenceImpl( id, longitude, latitude, error, abundance,
			      num_attributes, attributes,
			      num_env, env ) );
  
}

void 
OccurrencesImpl::createOccurrence( const std::string& id, 
                                   Coord longitude, Coord latitude,
                                   Scalar error, Scalar abundance,
                                   std::vector<double> attributes,
                                   std::vector<double> env)
{
  // Transforms the given coordinates in the common openModeller
  // coordinate system.
  gt_->transfOut( &longitude, &latitude );
  
  insert( new OccurrenceImpl( id, longitude, latitude, error, abundance,
			      attributes, env ) );
  
}

void
OccurrencesImpl::insert( const OccurrencePtr& oc )
{
  occur_.push_back( oc );
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

bool
OccurrencesImpl::hasEnvironment() const
{
  if ( ! numOccurrences() ) {

      return false;
  }

  const_iterator it = occur_.begin();

  return (*it)->hasEnvironment();
}

int
OccurrencesImpl::dimension() const
{
  if ( hasEnvironment() ) {

      const_iterator it = occur_.begin();

      return (*it)->environment().size();
  }
  else { 

      return 0;
  }
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
OccurrencesImpl::erase( const iterator& it ) 
{
  swap( occur_.back(), (*it) );
  occur_.pop_back();

  return it;
}


void 
OccurrencesImpl::appendFrom(const OccurrencesPtr& source)
{
  const_iterator it = source->begin();
  const_iterator end = source->end();

  while ( it != end) {

      insert(*it);
      ++it;
  }
}


/******************************/
/*** get Environment Matrix ***/
std::vector<ScalarVector> 
OccurrencesImpl::getEnvironmentMatrix()
{
  std::vector<ScalarVector> matrix( dimension() );

  // Initialize matrix
  for ( int i = 0; i < matrix.size(); i++ ) {

    matrix[i] = ScalarVector( numOccurrences() );
  }

  const_iterator c = occur_.begin();
  const_iterator end = occur_.end();

  int j = 0;

  // For each Occurrence
  while ( c != end ) {
      
    Sample const& sample = (*c)->environment();

    // For each layer
    for ( int i = 0; i < matrix.size(); i++ ) {

      // Feed new matrix
      matrix[i][j] = sample[i];
    }

    ++c;
    ++j;
  }

  return matrix;
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


