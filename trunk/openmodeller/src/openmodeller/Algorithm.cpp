/**
 * Definition of Algorithm class.
 * 
 * @file
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

#include <openmodeller/Algorithm.hh>
#include <openmodeller/AlgParameter.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/Exceptions.hh>

#include <openmodeller/models/AlgoAdapterModel.hh>

using std::string;

#undef DEBUG_MEMORY

/*******************/
/*** constructor ***/

AlgorithmImpl::AlgorithmImpl( AlgMetadata const *metadata ) :
  ReferenceCountedObject(),
  _samp(),
  _metadata( metadata ),
  _param(),
  _norm_offsets(),
  _norm_scales(),
  _has_norm_params( false )
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
}

/******************/
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

  if ( _has_norm_params ) {

    ConfigurationPtr norm_config( new ConfigurationImpl("Normalization") );
  
    norm_config->addNameValue( "Offsets", _norm_offsets );
    norm_config->addNameValue( "Scales", _norm_scales );

    config->addSubsection( norm_config );
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

  try { 

    ConstConfigurationPtr norm_config = config->getSubsection( "Normalization" );

    _has_norm_params = true;
    _norm_offsets = norm_config->getAttributeAsSample( "Offsets" );
    _norm_scales = norm_config->getAttributeAsSample( "Scales" );
    
  }
  catch( SubsectionNotFound& e ) {

    _has_norm_params = false;
  }

  // Get wrapper model element
  try { 

    ConstConfigurationPtr model_config = config->getSubsection( "Model" );

    _setConfiguration( model_config );
  }
  catch( SubsectionNotFound& e ) {

    // Try getting directly from config object (backwards compatibility)
    _setConfiguration( config );
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

/*********************/
/*** get Parameter ***/
void
AlgorithmImpl::computeNormalization( const ConstSamplerPtr& samp )
{

  Scalar lbound, ubound;
  if ( needNormalization( &lbound, &ubound ) ) {

    _has_norm_params = true;

    int dim = samp->numIndependent();
    Sample min(dim), max(dim);
    samp->getMinMax(&min, &max);

    _norm_scales.resize(dim);
    _norm_offsets.resize(dim);

    for (int i = 0; i < dim; ++i)
      {
	_norm_scales[i] = (ubound - lbound) / (max[i] - min[i]);
	_norm_offsets[i] = lbound - _norm_scales[i] * min[i];
      }
  }
}

void
AlgorithmImpl::setNormalization( const SamplerPtr& samp) const
{
  samp->normalize( _has_norm_params, _norm_offsets, _norm_scales );
}

void
AlgorithmImpl::setNormalization( const EnvironmentPtr& env) const
{
  env->normalize( _has_norm_params, _norm_offsets, _norm_scales );
}

Model
AlgorithmImpl::createModel( const SamplerPtr& samp, Algorithm::ModelCommand *model_command ) {

  if ( !samp )
    throw AlgorithmException( "Sampler not specified." );

  if (!samp->numPresence() && !samp->numAbsence())
  {
    //std::cout << "Presence count: " << samp->numPresence() << std::endl;
    //std::cout << "Absence  count: " << samp->numAbsence() << std::endl;
    throw AlgorithmException( "Cannot create model without any presence or absence point." );
    return 0;
  }
  setSampler( samp );

  computeNormalization( _samp );
  setNormalization( _samp );

  if ( !initialize() )
    throw AlgorithmException( "Algorithm could not be initialized." );

  // Generate model.
  int ncycle = 0;
  int resultFlag = 1;
  int doneFlag   = 0;
  while ( resultFlag && !doneFlag )
  {
    // I moved thee two calls out of the while() 
    // above and into seperate calls because
    // when run in a thread we need to catch 
    // exceptions properly TS
    try {
      resultFlag = iterate();
      doneFlag   = done();
    }
    catch ( char * message ) {
      string error( "Exception: " );
      error += message;
      Log::instance()->error(1, error.c_str() );
      return 0;
    }
    ncycle++;
    if ( model_command ) {
      try {
        (*model_command)(getProgress() );
      }
      catch ( char * message ) {
        string error( "Exception: " );
        error += message;
        Log::instance()->error(1, error.c_str() );
        return 0;
      }
      catch (...) {}
    }
  }

  if ( !done() )
    throw AlgorithmException( "Algorithm error in done().") ;

  if ( ! finalize() )
    throw AlgorithmException( "Algorithm error in finalize." );

  if ( model_command ) {
    try {
      (*model_command)( 1.0 );
    }
    catch ( char * message ) {
      string error( "Exception: " );
      error += message;
      Log::instance()->error(1, error.c_str() );
      return 0;
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
