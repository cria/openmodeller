/**
 * Definition of OpenModeller class (former ControlInterface class).
 * 
 * @file
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-09-25
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

#include <gdal.h>

#include <openmodeller/OpenModeller.hh>

#include <openmodeller/env_io/Map.hh>
#include <openmodeller/env_io/RasterFactory.hh>

#include <openmodeller/om_defs.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Algorithm.hh>
#include <openmodeller/AlgParameter.hh>
#include <openmodeller/Sampler.hh>
#include <openmodeller/Occurrences.hh>
#include <openmodeller/AreaStats.hh>
#include <openmodeller/Occurrence.hh>
#include <openmodeller/ConfusionMatrix.hh>
#include <openmodeller/MapFormat.hh>

#include <openmodeller/AlgorithmFactory.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Model.hh>

#include <string>
using std::string;

/*** backward compatible callback helper classes ***/

class ModelCallbackHelper : public Algorithm::ModelCommand
{

public:
  ModelCallbackHelper( OpenModeller::ModelCallback func, void *param ) :
    arg( param ),
    func( func ) {};
  void operator()( float d ) {
    func(d, arg );
  }

private:
  void *arg;
  OpenModeller::ModelCallback func;

};

class MapCallbackHelper : public Projector::MapCommand
{

public:
  MapCallbackHelper( OpenModeller::MapCallback func, void *param ) :
    arg( param ),
    func( func ) {};
  void operator()( float d ) {
    func(d, arg );
  }

private:
  void *arg;
  OpenModeller::MapCallback func;

};

void OpenModeller::setModelCallback( ModelCallback func, void *param ) {
  setModelCommand( new ModelCallbackHelper( func, param ) );
}

void OpenModeller::setModelCommand( Algorithm::ModelCommand *func ) {
  if (_model_command) 
    delete _model_command;
  _model_command = func;
}

void OpenModeller::setMapCallback( MapCallback func, void *param ) {
  setMapCommand( new MapCallbackHelper( func, param ) );
}

void OpenModeller::setMapCommand( Projector::MapCommand *func ) {
  _map_command = func;
}

/****************************************************************/
/************************* Open Modeller ************************/

/********************/
/*** constructors ***/

OpenModeller::OpenModeller()
{
  setLogLevel(Log::Debug);

  _map_command = 0;
  _model_command = 0;

  _error[0] = '\0';

  _actualAreaStats = new AreaStats();
  _estimatedAreaStats = new AreaStats();

}


/******************/
/*** destructor ***/

OpenModeller::~OpenModeller()
{
  if ( _map_command ) delete _map_command;

  if ( _model_command ) delete _model_command;

  if ( _estimatedAreaStats ) delete _estimatedAreaStats;

  delete _actualAreaStats;
}


/*********************/
/*** get log level ***/
void
OpenModeller::setLogLevel( Log::Level level )
{
  g_log.setLevel( level );
}


/*******************/
/*** get Version ***/
char *
OpenModeller::getVersion()
{
  // Defined in "om_defs.hh".
  return OM_VERSION; 
}

/****************************/
/*** available Algorithms ***/
AlgMetadata const **
OpenModeller::availableAlgorithms()
{
  return AlgorithmFactory::availableAlgorithms();
}


/**************************/
/*** algorithm Metadata ***/
AlgMetadata const*
OpenModeller::algorithmMetadata( char const *algorithm_id )
{
  return AlgorithmFactory::algorithmMetadata( algorithm_id );
}


/********************************/
/*** num Available Algorithms ***/
int
OpenModeller::numAvailableAlgorithms()
{
  return AlgorithmFactory::numAvailableAlgorithms();
}

bool
OpenModeller::hasEnvironment()
{
  return ( _env || ( (_presence)? _presence->hasEnvironment() : false ) );
}

/***********************/
/*** set Occurrences ***/
int
OpenModeller::setOccurrences( const OccurrencesPtr& presence,
                              const OccurrencesPtr& absence )
{
  if ( !presence || presence->numOccurrences() == 0 ) {
    sprintf(_error,"Presences must not be empty");
    return 0;
  }
  _presence = presence;
  _absence  = absence;

  return 1;
}

/***********************/
/*** set Environment ***/
void
OpenModeller::setEnvironment( std::vector<std::string> categ_map,
			      std::vector<std::string> continuous_map,
			      const std::string& mask )
{
  _env = createEnvironment( categ_map, continuous_map, mask);

  //g_log( "Environment initialized.\n" );
}

/*******************/
/*** set Sampler ***/
void
OpenModeller::setSampler( const SamplerPtr& sampler )
{
  _samp = sampler;
}

/*********************/
/*** set Algorithm ***/
int
OpenModeller::setAlgorithm( char const *id, int nparam,
                            AlgParameter const *param )
{
  if ( nparam && ! param ) {

    g_log.error( 1, "Incoherent number of parameters and parameters pointer\n" );
  }

  if ( ! _samp ) {

    //  Create a default sampler if none was previously provided
    if ( ! hasEnvironment() ) {

      g_log( "Sampler could not be initialized. Environment not set.\n" );
      return 0;
    }
    else if ( ! _presence ) {

      g_log( "Sampler could not be initialized. Occurrences not set.\n" );
      return 0;
    }
    else {

      // _env and _presence are both set
      setSampler( createSampler( _env, _presence, _absence ) );
    }
  }

  _alg = AlgorithmFactory::newAlgorithm( id );

  if ( ! _alg ) {

    sprintf( _error, "Could not find (%s) algorithm.", id );
    return 0;
  }

  _alg->setSampler( _samp );
  _alg->setParameters( nparam, param );

  return 1;
}

/********************/
/*** create Model ***/
int
OpenModeller::createModel()
{
  g_log( "Creating the model\n" );

  char *error = parameterModelCheck();
  if ( error )
    {
      strcpy( _error, error );
      return 0;
    }

  _alg->createModel( _samp, _model_command );

  g_log( "\nFinished Creating Model\n" );

  return 1;
}


/*****************************/
/*** parameter Model Check ***/
char *
OpenModeller::parameterModelCheck()
{
  // Sampler
  if ( ! _samp )
    return "Sampler not specified.";

  // Algorithm.
  if ( ! _alg )
    return "Modeling algorithm not specified.";

  return 0;
}


/******************/
/*** create Map ***/
void
OpenModeller::createMap( const EnvironmentPtr & env, char const *output_file, MapFormat& output_format )
{
  Model m( _alg->getModel() );

  Map *mask = env->getMask();

  // If mask is undefined, use first layer as a mask
  if (!mask)
    mask = env->getLayer(0);

  // copy mask settings to the output format ONLY when they are undefined 
  output_format.copyDefaults( *mask );

  // try to infer the output file format (default is 64 bit floating tiff).
  string fname(output_file);
  int pos = fname.length() - 4;
  if (pos >0 && fname.compare( pos, 4, ".bmp" ) == 0 ) {
    output_format.setFormat( MapFormat::GreyBMP );
  }

  // Create map on disc.
  Map map( RasterFactory::instance().create( output_file, output_format ) );

  Projector::createMap( m, env,
			&map,
			_actualAreaStats,
			_map_command );
}

void
OpenModeller::createMap( const EnvironmentPtr & env, char const *output_file )
{
  MapFormat mf;
  return createMap( env, output_file, mf );
}

/******************/
/*** create Map ***/
void
OpenModeller::createMap( char const *output_file )
{
  MapFormat mf;
  return createMap( _env, output_file, mf );
}

/******************/
/*** create Map ***/
void
OpenModeller::createMap( char const *output_file, MapFormat& output_format )
{
  return createMap( _env, output_file, output_format );
}

/**********************************/
/******* get Value ****************/
Scalar
OpenModeller::getValue(const ConstEnvironmentPtr& env, Coord x, Coord y)
{
  if ( !_env ) {
    return -1.0;
  }

  // FIXME: enable geotransformation
  const Sample& sample = env->get( x, y );
  if ( sample.size() == 0 )  {
    return -1.0;
  }

  Scalar val = _alg->getValue( sample );
  if ( val < 0.0 ) val = 0.0;
  if ( val > 1.0 ) val = 1.0;

  return val;

}


/**********************************/
/******* get Value ****************/
Scalar
OpenModeller::getValue( Scalar const *environment_values )
{
  Sample tmp( _env->numLayers() ,environment_values );
  return _alg->getValue( tmp );
}


/**********************************/
/******* getActualAreaStats *******/
AreaStats *
OpenModeller::getActualAreaStats()
{
  return _actualAreaStats;
}


/**********************************/
/******* getActualAreaStats *******/
AreaStats * OpenModeller::getEstimatedAreaStats(double proportionAreaToSample)
{
  return getEstimatedAreaStats(_env, proportionAreaToSample);
}

AreaStats * OpenModeller::getEstimatedAreaStats(const ConstEnvironmentPtr& env,
						double proportionAreaToSample)
{
  int i, sampleSize, numCells, xdim, ydim;

  if ( !env)
    // this method does not work without _env properly set
    return NULL;

  if ( !_estimatedAreaStats )
    { _estimatedAreaStats = new AreaStats; }
  else
    { _estimatedAreaStats->reset(); }

  // get number of cells to sample
  // note that the total area does not take the mask into account
  // thus all cells (masked or unmasked) are counted
  env->getMask()->getDim(&xdim, &ydim);
  numCells = xdim * ydim; 

  sampleSize = (int) (numCells * proportionAreaToSample);
  for (i = 0; i < sampleSize; i++)
    { 
      const Sample& sample = env->getRandom();
      _estimatedAreaStats->addPrediction(_alg->getValue(sample)); 
    }

  return _estimatedAreaStats;
}

/**********************************/
/******* getConfusionMatrix *******/
ConfusionMatrix * OpenModeller::getConfusionMatrix()
{
  ConfusionMatrix *cm = new ConfusionMatrix();

  cm->calculate( getModel(), getSampler() );

  return cm;
}

ConfigurationPtr
OpenModeller::getConfiguration() const
{

  ConfigurationPtr config( new ConfigurationImpl("OpenModeller"));

  ConfigurationPtr sampler_config( _samp->getConfiguration() );

  config->addSubsection( sampler_config );

  ConfigurationPtr alg_config( _alg->getConfiguration() );

  config->addSubsection( alg_config );

  return config;
}

void
OpenModeller::setConfiguration( const ConstConfigurationPtr & config )
{

  _samp = createSampler( config->getSubsection( "Sampler" ) );
  _env = _samp->getEnvironment();

  _alg = AlgorithmFactory::newAlgorithm( config->getSubsection( "Algorithm" ) );
  _alg->setSampler( _samp );

}
