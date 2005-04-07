/**
 * Definition of OpenModeller class (former ControlInterface class).
 * 
 * @file
 * @author Mauro E S Mu�oz <mauro@cria.org.br>
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

#include "om_control.hh"

#include <env_io/map.hh>

#include <om_defs.hh>
#include <om_log.hh>
#include <om_algorithm.hh>
#include <om_alg_parameter.hh>
#include <om_sampler.hh>
#include <om_occurrences.hh>
#include <om_area_stats.hh>
#include <occurrence.hh>
#include <om_conf_matrix.hh>

#include <algorithm_factory.hh>
#include <environment.hh>
#include <configuration.hh>
#include <Model.hh>

#include <string.h>
#include <stdlib.h>


/*** backward compatible callback helper classes ***/

class ModelCallbackHelper : public OpenModeller::ModelCommand
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

  if (_env)
    {
      setSampler( createSampler(_env, 
				_presence, 
				_absence));
    }

  return 1;
}

/***********************/
/*** set Environment ***/
void
OpenModeller::setEnvironment( std::vector<std::string> categ_map,
			      std::vector<std::string> continuous_map,
			      std::string& mask )
{
  _env = new EnvironmentImpl( categ_map, continuous_map, mask);

  // TODO: remove duplication of code below
  if (_presence)
    { 
      setSampler( createSampler(_env, _presence, _absence) );
    }

  g_log( "Environment initialized by STL constructor.\n" );
}

/***********************/
/*** set Environment ***/
void
OpenModeller::setEnvironment( int num_categ,
			      char **categ_map,
			      int num_continuous,
			      char **continuous_map,
			      char *mask )
{
  _env = new EnvironmentImpl( num_categ, categ_map,
			      num_continuous, continuous_map, mask );

  if (_presence)
    {
      setSampler( createSampler(_env, 
				_presence, 
				_absence));
    }

  g_log( "Environment initialized.\n" );
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
  AlgMetadata const *meta = algorithmMetadata( id );

  // Check the parameters.
  if ( ! meta || meta->nparam != nparam )
    {
      if ( ! meta )
	g_log( "Can't get metadata for algorithm %s", id );

      if ( meta->nparam != nparam )
	g_log( "Number of parameters provided (%d) does not match required parameters (%d)",
               nparam, meta->nparam);

      return 0;
    }

  if ( nparam && ! param )
    g_log.error( 1, "Incoherent number of parameters and parameters pointer" );

  // Check if sampler is initialized
  if ( ! _samp )
    {
      g_log( "Sampler not initialized." );
      return 0;
    }

  _alg = AlgorithmFactory::newAlgorithm( id );
  if ( ! _alg )
    {
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
  char *error = parameterModelCheck();
  if ( error )
    {
      strcpy( _error, error );
      return 0;
    }

  _alg->computeNormalization( _samp );
  _alg->setNormalization( _samp );

  g_log( "Creating the model\n" );

  // Initialize algorithm.  
  if ( ! _alg->initialize() )
    {
      sprintf( _error, "Algorithm (%s) could not be initialized.",
	       _alg->getID() );
      return 0;
    }

  // Generate model.
  int ncycle = 0;
  while ( _alg->iterate() && ! _alg->done() )
    {
      ncycle++;
      if ( _model_command )
        try 
	  {
	    (*_model_command)( _alg->getProgress() );
	  }
		catch(char * message)
		{
		    sprintf( _error, "Exception: %s", message );
			g_log( "\n" );
			g_log(_error);
			g_log( "\n" );

			return 0;
		}
        catch( ... ) {}
    }

  // get progress one more time to show final 100% done
  if ( _model_command )
    {
		try
		{ 
			(*_model_command)( _alg->getProgress() ); 
		}
  		catch(char * message)
		{
			sprintf( _error, "Exception: %s", message );
			g_log( "\n" );
			g_log(_error);
			g_log( "\n" );

			return 0;
		}
		catch( ... ) {}

    }

  // Algorithm terminated with error.
  if ( ! _alg->done() )
    {
      sprintf( _error, "Algorithm (%s) iteraction error.",
	       _alg->getID() );
      return 0;
    }

  // Finalise algorithm.  
  if ( ! _alg->finalize() )
    {
      sprintf( _error, "Algorithm (%s) could not be finalized.",
	       _alg->getID() );
      return 0;
    }

  g_log( "\n" );

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
OpenModeller::createMap( const EnvironmentPtr & env, char const *output_file )
{
  Model m( _alg->getModel() );

  // Create map on disc.
  // Currently hard coded to create greyscale tiff.
  Map map( new Raster( output_file, 1, env->getMask() ) );

  Projector::createMap( m, env,
			&map,
			_actualAreaStats,
			_map_command );
}


/******************/
/*** create Map ***/
void
OpenModeller::createMap( char const *output_file )
{
  Model m( _alg->getModel() );

  // Create map on disc.
  // Currently hard coded to create greyscale tiff.
  Map map( new Raster( output_file, 1, _env->getMask() ) );

  Projector::createMap( m, _env,
			&map,
			_actualAreaStats,
			_map_command );
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
OpenModeller::getValue( Scalar const *amb )
{
  Sample tmp( _env->numLayers() ,amb );
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
  int i, sampleSize, numCells, xdim, ydim;

  if ( !_estimatedAreaStats )
    { _estimatedAreaStats = new AreaStats; }
  else
    { _estimatedAreaStats->reset(); }

  // get number of cells to sample
  // note that the total area does not take the mask into account
  // thus all cells (masked or unmasked) are counted
  _env->getMask()->getDim(&xdim, &ydim);
  numCells = xdim * ydim; 

  sampleSize = (int) (numCells * proportionAreaToSample);
  for (i = 0; i < sampleSize; i++)
    { 
      const Sample& sample = _env->getRandom();
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
