/**
 * Definition of Algorithm class.
 * 
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-10-05
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

#include <openmodeller/CallbackWrapper.hh>
#include <openmodeller/Algorithm.hh>
#include <openmodeller/AlgParameter.hh>
#include <openmodeller/AlgorithmFactory.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/Exceptions.hh>

#include <openmodeller/models/AlgoAdapterModel.hh>

// AlgorithmImpl works as a Normalizer factory during deserialization, so 
// all possible Normalizer implementation headers must be included here
#include <openmodeller/ScaleNormalizer.hh>
#include <openmodeller/MeanVarianceNormalizer.hh>

//needed for atoi function
#include <stdlib.h>

using std::string;

#undef DEBUG_MEMORY

/*******************/
/*** constructor ***/

AlgorithmImpl::AlgorithmImpl( AlgMetadata const *metadata ) :
  ReferenceCountedObject(),
  _samp(),
  _normalizerPtr(0),
  _param(),
  _metadata( metadata )
{
#if defined(DEBUG_MEMORY)
  Log::instance()->debug( "AlgorithmImpl::AlgorithmImpl() at %x\n", this );
#endif
}


/******************/
/*** destructor ***/

AlgorithmImpl::~AlgorithmImpl()
{
#if defined(DEBUG_MEMORY)
  Log::instance()->debug("AlgorithmImpl::~AlgorithmImpl() at %x\n",this);
#endif

  if ( _normalizerPtr ) {

    delete _normalizerPtr;
  }
}

/*********************/
/*** configuration ***/

ConfigurationPtr
AlgorithmImpl::getConfiguration() const
{
  ConfigurationPtr config( new ConfigurationImpl("Algorithm") );

  config->addNameValue( "Id", _metadata->id );
  config->addNameValue( "Version", _metadata->version );

  ConfigurationPtr param_config( new ConfigurationImpl("Parameters") );
  ParamSetType::const_iterator p = _param.begin();
  for( ; p != _param.end() ; ++p ) {

    ConfigurationPtr cfg( new ConfigurationImpl("Parameter") );
    param_config->addSubsection( cfg );
    cfg->addNameValue( "Id", p->first );
    cfg->addNameValue( "Value", p->second );
  }

  config->addSubsection( param_config );

  if ( _normalizerPtr ) {

    config->addSubsection( _normalizerPtr->getConfiguration() );
  }

  // Wrapper model element
  ConfigurationPtr model_config( new ConfigurationImpl("Model") );

  _getConfiguration( model_config );

  config->addSubsection( model_config );

  return config;
}

void
AlgorithmImpl::setConfiguration( const ConstConfigurationPtr &config )
{
  // Important: this code runs in the following situations:
  // 1- Loading a fully serialized algorithm, i.e. an algorithm that was just
  //    run and has all its properties filled with content (parameters & model).
  // 2- Loading a createModel request which contains only the algorithm id
  //    and its parameters.
  // 3- Loading only the serialized model of an algorithm.

  try { 

    ConstConfigurationPtr param_config = config->getSubsection( "Parameters" );
  
    Configuration::subsection_list params = param_config->getAllSubsections();

    _param.clear();

    Configuration::subsection_list::const_iterator nv;

    for ( nv = params.begin(); nv != params.end(); ) {

      string id = (*nv)->getAttribute( "Id" );
      string value = (*nv)->getAttribute( "Value" );
      _param.insert( ParamSetValueType( id, value ) );
      ++nv;
    }
  }
  catch( SubsectionNotFound& e ) {
    UNUSED(e);
    // In this case the XML has probably just the model definition that will be 
    // loaded below.
  }

  ConstConfigurationPtr norm_config;

  bool found_normalization_section = false;

  try { 

    norm_config = config->getSubsection( "Normalization" );

    found_normalization_section = true;
  }
  catch( SubsectionNotFound& e ) {
    UNUSED(e);
    // No need to set _normalizerPtr to null, because alg already initializes
    // a default normalizer. Setting it to null will in fact make algorithms
    // crash if you run them with om_create (deserializing from an XML request).
  }

  if ( found_normalization_section ) {

    try { 

      std::string norm_class = norm_config->getAttribute( "Class" );

      if ( norm_class == "ScaleNormalizer" ) {

        _normalizerPtr = new ScaleNormalizer();
      }
      else if ( norm_class == "MeanVarianceNormalizer" ) {

        _normalizerPtr = new MeanVarianceNormalizer();
      }
      else {

        string msg( "Unknown normalizer class: " );
        msg.append( norm_class );

        Log::instance()->error( msg.c_str() );

        throw AlgorithmException( msg.c_str() );
      }
    }
    catch( AttributeNotFound& e ) {

      UNUSED (e);

      // Backwards compatibility
      _normalizerPtr = new ScaleNormalizer();
    }

    _normalizerPtr->setConfiguration( norm_config );
  }

  // Get wrapper model element
  try { 

    ConstConfigurationPtr model_config = config->getSubsection( "Model" );

    _setConfiguration( model_config );
  }
  catch( SubsectionNotFound& e ) {
    UNUSED (e);
  }
}

/*******************/
/*** set Sampler ***/
void
AlgorithmImpl::setSampler( const SamplerPtr& samp )
{
  _samp = samp;
}

/**********************/
/*** set Parameters ***/
void
AlgorithmImpl::setParameters( int nparam, AlgParameter const *param )
{
  _param.clear();

  // Copy 'param' to '_alg_param'.
  AlgParameter const *end = param + nparam;

  while ( param < end ) {

    _param.insert( ParamSetValueType( param->id(), param->value() ) );

    ++param;
  }
}

void
AlgorithmImpl::setParameters( const ParamSetType &params )
{
  _param.clear();
  _param = params;
}

/**********************/
/*** get fresh copy ***/
AlgorithmPtr 
AlgorithmImpl::getFreshCopy()
{
  if ( ! _metadata ) {

    std::string msg = "Cannot produce copies of an algorithm without metadata.\n";

    Log::instance()->error( msg.c_str() );

    throw AlgorithmException( msg.c_str() );
  }

  AlgorithmPtr copy = AlgorithmFactory::newAlgorithm( _metadata->id );

  copy->setParameters( _param );

  return copy;
}

/*********************/
/*** get Parameter ***/
int
AlgorithmImpl::getParameter( string const &id, string *value )
{
  ParamSetType::const_iterator pos = _param.find( id );

  if ( pos == _param.end() ) {

    return 0;
  }
   
  *value = pos->second;

  return 1;
}


/*********************/
/*** get Parameter ***/
int
AlgorithmImpl::getParameter( string const &id, int *value )
{
  string str_value;

  if ( ! getParameter( id, &str_value ) )
    return 0;

  *value = atoi( str_value.c_str() );
  return 1;
}


/*********************/
/*** get Parameter ***/
int
AlgorithmImpl::getParameter( string const &id, double *value )
{
  string str_value;

  if ( ! getParameter( id, &str_value ) )
    return 0;

  *value = atof( str_value.c_str() );
  return 1;
}


/*********************/
/*** get Parameter ***/
int
AlgorithmImpl::getParameter( string const &id, float *value )
{
  string str_value;

  if ( ! getParameter( id, &str_value ) )
    return 0;

  *value = float( atof( str_value.c_str() ) );
  return 1;
}


/**********************/
/*** get Normalizer ***/
Normalizer * 
AlgorithmImpl::getNormalizer() const
{
  if ( _normalizerPtr ) {

    return _normalizerPtr->getCopy();
  }

  return 0;
}


void
AlgorithmImpl::setNormalization( const SamplerPtr& samp) const
{
    samp->normalize( _normalizerPtr );
}

void
AlgorithmImpl::setNormalization( const EnvironmentPtr& env) const
{
    env->normalize( _normalizerPtr );
}

Model
AlgorithmImpl::createModel( const SamplerPtr& samp, CallbackWrapper *callbackWrapper ) {

  if ( !samp ) {

    std::string msg = "Sampler not specified.\n";

    Log::instance()->error( msg.c_str() );

    throw AlgorithmException( msg.c_str() );
  }

  if ( !samp->numPresence() && !samp->numAbsence() ) {

    std::string msg = "Cannot create model without any presence or absence point.\n";

    Log::instance()->error( msg.c_str() );

    throw AlgorithmException( msg.c_str() );
  }

  setSampler( samp );

  if ( needNormalization() ) {

    if ( !_samp->isNormalized() ) {

      Log::instance()->info( "Computing normalization\n" );

      if ( _normalizerPtr ) {

        _normalizerPtr->computeNormalization( _samp );

        setNormalization( _samp );
      }
      else {

        std::string msg = "Normalizer not specified.\n";

        Log::instance()->error( msg.c_str() );

        throw AlgorithmException( msg.c_str() );
      }
    }
  }
  else {

    if ( _samp->isNormalized() ) {

      _samp->resetNormalization();
    }    
  }

  if ( ! initialize() ) {

    std::string msg = "Algorithm could not be initialized.\n";

    Log::instance()->error( msg.c_str() );

    throw AlgorithmException( msg.c_str() );
  }

  // Generate model.
  int ncycle = 0;
  int resultFlag = 1;
  int doneFlag = 0;
  bool abort = false;

  while ( resultFlag && ! doneFlag ) {

    if ( callbackWrapper ) {

      try {

        abort = callbackWrapper->abortionRequested();

        if ( abort ) {

          Log::instance()->error( "Model creation aborted.\n" );
          break;
        }
      }
      catch ( char * message ) {

        string error( "Exception in abort callback: " );
        error += message;
        Log::instance()->error( error.c_str() );
        throw AlgorithmException( error.c_str() );
      }
      catch (...) {}
    }

    // I moved thee two calls out of the while() 
    // above and into separate calls because
    // when run in a thread we need to catch 
    // exceptions properly TS
    try {

      resultFlag = iterate();
      doneFlag   = done();
    }
    catch ( char * message ) {

      string error( "Exception in model iteration: " );
      error += message;
      Log::instance()->error( error.c_str() );
      throw AlgorithmException( error.c_str() );
    }

    ncycle++;

    if ( callbackWrapper ) {

      try {

        callbackWrapper->notifyModelCreationProgress( getProgress() );
      }
      catch ( char * message ) {

        string error( "Exception in get progress: " );
        error += message;
        Log::instance()->error( error.c_str() );
        throw AlgorithmException( error.c_str() );
      }
      catch (...) {}
    }
  }

  if ( abort ) {

    std::string msg = "Model creation aborted.\n";

    Log::instance()->error( msg.c_str() );

    throw AlgorithmException( msg.c_str() );
  }

  if ( ! done() ) {

    std::string msg = "Algorithm could not produce a model.\n";

    Log::instance()->error( msg.c_str() );

    throw AlgorithmException( msg.c_str() );
  }

  if ( ! finalize() ) {

    std::string msg = "Algorithm could not be finalized.\n";

    Log::instance()->error( msg.c_str() );

    throw AlgorithmException( msg.c_str() );
  }

  if ( callbackWrapper ) {

    try {

      callbackWrapper->notifyModelCreationProgress( 1.0 );
    }
    catch ( char * message ) {

      string error( "Exception when finalizing progress: " );
      error += message;
      Log::instance()->error( error.c_str() );
      throw AlgorithmException( error.c_str() );
    }
    catch (...) {}
  }
  
  return getModel();
}

Model
AlgorithmImpl::getModel() const
{
  // Need the ugly const_cast to cast away constness of this.
  // ConstAlgorithmPtr must be initialized with the PlainPoinerType = AlgorithmImpl *
  // and not with the const AlgorithmImpl*.
  // Once the ConstAlgorihtmPtr is created, then it behaves as if its const.
  return Model( new AlgoAdapterModelImpl( ConstAlgorithmPtr( const_cast<AlgorithmImpl*>(this) )));
}
