/**
 * Definition of CallbackWrapper class
 * 
 * @author Renato De Giovanni
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2008 by CRIA -
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

#include <openmodeller/CallbackWrapper.hh>

// Constructor for model creation callbacks
CallbackWrapper::CallbackWrapper() :
    _model_creation_callback( 0 ),
    _model_creation_arg( 0 ),
    _model_projection_callback( 0 ),
    _model_projection_arg( 0 ),
    _abortion_callback ( 0 ),
    _abortion_arg( 0 )
{ }

// Set Model Creation Callback
void CallbackWrapper::setModelCreationCallback( ModelCreationCallback func, void * param )
{
  _model_creation_callback = func;
  _model_creation_arg = param;
}

// Set Model Projection Callback
void CallbackWrapper::setModelProjectionCallback( ModelProjectionCallback func, void * param )
{
  _model_projection_callback = func;
  _model_projection_arg = param;
}

// Set job abortion Callback
void CallbackWrapper::setAbortionCallback( AbortionCallback func, void * param )
{
  _abortion_callback = func;
  _abortion_arg = param;
}

// Wrapper to model creation callbacks
void CallbackWrapper::notifyModelCreationProgress( float progress )
{
  if ( _model_creation_callback ) {

    _model_creation_callback( progress, _model_creation_arg );
  }
}

// Wrapper to model projection callbacks
void CallbackWrapper::notifyModelProjectionProgress( float progress )
{
  if ( _model_projection_callback ) {

    _model_projection_callback( progress, _model_projection_arg );
  }
}

// Wrapper to abortion callbacks
bool CallbackWrapper::abortionRequested()
{
  return ( _abortion_callback ) ? _abortion_callback( _abortion_arg ) : false;
}

