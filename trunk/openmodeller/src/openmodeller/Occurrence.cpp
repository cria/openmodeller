/**
 * Declaration of OccurrenceImpl class.
 * 
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-25-02
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

#include <openmodeller/Log.hh>

#include <openmodeller/Occurrence.hh>

#include <string.h>

/****************************************************************/
/************************** Occurrence **************************/

/******************/
/*** destructor ***/

OccurrenceImpl::~OccurrenceImpl()
{
}

OccurrenceImpl&
OccurrenceImpl::operator=( const OccurrenceImpl & rhs )
{
  if ( this == &rhs ) {

    return *this;
  }

  x_ = rhs.x_;
  y_ = rhs.y_;
  error_ = rhs.error_;
  abundance_ = rhs.abundance_;

  attr_ = rhs.attr_;
  unnormEnv_ = rhs.unnormEnv_;
  normEnv_ = rhs.normEnv_;

  return *this;
}

Sample const & OccurrenceImpl::environment() const
{
  if ( normEnv_.size() != 0 ) {

    return normEnv_;
  }
  else {

    return unnormEnv_;
  }
}

Sample const & OccurrenceImpl::originalEnvironment() const
{
  return unnormEnv_;
}

void
OccurrenceImpl::normalize( Normalizer * normalizerPtr, size_t categoricalThreshold )
{
  unnormEnv_.setCategoricalThreshold( categoricalThreshold );

  normEnv_ = unnormEnv_;

  if ( normalizerPtr ) {

    normalizerPtr->normalize( &normEnv_ );
  }
}

void
OccurrenceImpl::setNormalizedEnvironment( const Sample& s )
{
  normEnv_ = s;
}

void
OccurrenceImpl::setUnnormalizedEnvironment( const Sample& s )
{
  unnormEnv_ = s;
}

bool
OccurrenceImpl::hasEnvironment() const
{
  return unnormEnv_.size() != 0;
}

void
OccurrenceImpl::setId( const std::string& id )
{
  id_ = id;
}

void
OccurrenceImpl::setAbundance( Scalar value )
{
  abundance_ = value;
}

void
OccurrenceImpl::dump() const
{
  Log::instance()->info( "Occurrence %s\n", id_.c_str() );
  Log::instance()->info( "Unnormalized environment:\n" );
  unnormEnv_.dump();
  Log::instance()->info( "Normalized environment:\n" );
  normEnv_.dump();
}
