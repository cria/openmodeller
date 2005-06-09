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

#include <om_algorithm.hh>
#include <om_alg_parameter.hh>
#include <om_log.hh>
#include <configuration.hh>
#include <environment.hh>
#include <Exceptions.hh>

#include <models/AlgoAdapterModel.hh>

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
  g_log.debug( "AlgorithmImpl::AlgorithmImpl() at %x\n", this );
#endif
}


/******************/
/*** destructor ***/

AlgorithmImpl::~AlgorithmImpl()
{
#if defined(DEBUG_MEMORY)
  g_log.debug("AlgorithmImpl::~AlgorithmImpl() at %x\n",this);
#endif
}

/******************/
/*** configuration ***/

ConfigurationPtr
AlgorithmImpl::getConfiguration() const
{

  ConfigurationPtr config( new ConfigurationImpl("Algorithm") );

  ConfigurationPtr meta_config( new ConfigurationImpl("AlgorithmMetadata") );

  meta_config->addNameValue( "Id", _metadata->id );
  meta_config->addNameValue( "Name", _metadata->name );
  meta_config->addNameValue( "Version", _metadata->version );

  ConfigurationPtr overview( new ConfigurationImpl("Overview" ) );
  overview->setValue( _metadata->overview );
  meta_config->addSubsection( overview );

  meta_config->addNameValue( "Author", _metadata->author );
  meta_config->addNameValue( "CodeAuthor", _metadata->code_author );
  meta_config->addNameValue( "Contact", _metadata->contact );

  config->addSubsection( meta_config );

  ConfigurationPtr param_config( new ConfigurationImpl("AlgorithmParameters") );
  ParamSetType::const_iterator p = _param.begin();
  for( ; p != _param.end() ; ++p ) {
    ConfigurationPtr cfg( new ConfigurationImpl("Param") );
    param_config->addSubsection( cfg );
    cfg->addNameValue( "Id", p->first );
    cfg->addNameValue( "Value", p->second );
  }

  config->addSubsection( param_config );

  if ( _has_norm_params ) {
    ConfigurationPtr norm_config( new ConfigurationImpl("NormalizationParameters") );
  
    norm_config->addNameValue( "Offsets", _norm_offsets );
    norm_config->addNameValue( "Scales", _norm_scales );

    config->addSubsection( norm_config );
  }

  _getConfiguration( config );

  return config;
  
}

void
AlgorithmImpl::setConfiguration( const ConstConfigurationPtr &config )
{
  ConstConfigurationPtr param_config = config->getSubsection( "AlgorithmParameters" );
  
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
    ConstConfigurationPtr norm_config = config->getSubsection( "NormalizationParameters" );

    _has_norm_params = true;
    _norm_offsets = norm_config->getAttributeAsSample( "Offsets" );
    _norm_scales = norm_config->getAttributeAsSample( "Scales" );
    
  }
  catch( SubsectionNotFound& e ) {
    _has_norm_params = false;
  }

  _setConfiguration( config );

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
	_norm_offsets[i] = min[i] - _norm_scales[i] * min[i];
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
AlgorithmImpl::getModel() const
{
  // Need the ugly const_cast to cast away constness of this.
  // ConstAlgorithmPtr must be initialized with the PlainPoinerType = AlgorithmImpl *
  // and not with the const AlgorithmImpl*.
  // Once the ConstAlgorihtmPtr is created, then it behaves as if its const.
  return Model( new AlgoAdapterModelImpl( ConstAlgorithmPtr( const_cast<AlgorithmImpl*>(this) )));
}
