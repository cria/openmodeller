/**
* Declaration of class PreAlgorithm
*
* @author Missae Yamamoto (missae at dpi . inpe . br)
* $Id$
*
* LICENSE INFORMATION
* 
* Copyright(c) 2008 by INPE -
* Instituto Nacional de Pesquisas Espaciais
*
* http://www.inpe.br
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

#include "PreAlgorithm.hh"

#include <openmodeller/Log.hh>
#include <openmodeller/Exceptions.hh>

PreAlgorithm::PreAlgorithm()
{
}

PreAlgorithm::PreAlgorithm( const PreAlgorithm& )
{
};


PreAlgorithm::~PreAlgorithm()
{
}


bool PreAlgorithm::reset( const PreParameters& params )
{
  if( checkParameters( params ) ) {

    params_.clear();
    params_ = params;

    return true;
  } 
  else 
  {
     std::string msg = "PreAlgorithm::Reset: Invalid supplied parameters.\n";
     Log::instance()->error( msg.c_str() );
	 throw InvalidParameterException( msg );
  }
}

void
PreAlgorithm::resetState( PreParameters& params )
{
  params.clear();
  params = params_;
}


bool PreAlgorithm::apply()
{
  bool return_value = false;
  
  if( checkInternalParameters() ) {
    return_value = runImplementation();
  } 
  else 
  {
     std::string msg = "TePreAlgorithm::apply: Invalid supplied parameters.\n";
     Log::instance()->error( msg.c_str() );
	 throw InvalidParameterException( msg );
  }
  
  return return_value;
}

const PreParameters& PreAlgorithm::getParameters() const
{
  return params_;
}


bool PreAlgorithm::checkInternalParameters() const
{
  return checkParameters( params_ );
}


const PreAlgorithm& PreAlgorithm::operator=( 
  const PreAlgorithm& external )
{
  std::string msg = "PreAlgorithm::operator=: Algorithms cannot be copied.\n";
  Log::instance()->error( msg.c_str() );
  throw InvalidParameterException( msg );
  
  return external;
}

void PreAlgorithm::getLayerResult( const string layer_id, PreParameters& result )
{
  result.clear();
  result = result_by_layer_[layer_id];
}
