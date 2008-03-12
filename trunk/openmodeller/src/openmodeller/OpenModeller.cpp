/**
 * Definition of OpenModeller class (former ControlInterface class).
 * 
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

#include <openmodeller/OpenModeller.hh>

#include <openmodeller/om_defs.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Algorithm.hh>
#include <openmodeller/AlgParameter.hh>
#include <openmodeller/Sampler.hh>
#include <openmodeller/Occurrences.hh>
#include <openmodeller/AreaStats.hh>
#include <openmodeller/Occurrence.hh>
#include <openmodeller/ConfusionMatrix.hh>
#include <openmodeller/RocCurve.hh>
#include <openmodeller/MapFormat.hh>
#include <openmodeller/AlgorithmFactory.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Model.hh>
#include <openmodeller/AbortionCommand.hh>

#include <openmodeller/env_io/Map.hh>
#include <openmodeller/env_io/RasterFactory.hh>

#include <openmodeller/Exceptions.hh>


#include <string>
using std::string;
#include <map>
using std::map;


// REMOVE ME
#include <istream>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>  // I/O 
#include <fstream>   // file I/O
#include <sstream>   // treat string as a stream


/*** backward compatible callback helper classes ***/

class ModelCallbackHelper : public Algorithm::ModelCommand
{

public:
  ModelCallbackHelper( ModelCallback func, void *param ) :
    arg( param ),
    func( func ) {};
  void operator()( float d ) {
    func(d, arg );
  }

private:
  void *arg;
  ModelCallback func;

};

class MapCallbackHelper : public Projector::MapCommand
{

public:
  MapCallbackHelper( MapCallback func, void *param ) :
    arg( param ),
    func( func ) {};
  void operator()( float d ) {
    func(d, arg );
  }

private:
  void *arg;
  MapCallback func;

};

/*** Helper class for callback to abort jobs (model creation or model projection) ***/

class AbortionCallbackHelper : public AbortionCommand
{

public:
  AbortionCallbackHelper( AbortionCallback func, void *param ) :
    arg( param ),
    func( func ) {};
  bool operator()() {
    return func( arg );
  }

private:
  void *arg;
  AbortionCallback func;

};

/*** Callback "setters" ***/

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

void OpenModeller::setAbortionCallback( AbortionCallback func, void *param ) {
  setAbortionCommand( new AbortionCallbackHelper( func, param ) );
}

void OpenModeller::setAbortionCommand( AbortionCommand *func ) {
  _abortion_command = func;
}

/****************************************************************/
/************************* Open Modeller ************************/

/********************/
/*** constructors ***/

OpenModeller::OpenModeller()
{
  _map_command = 0;
  _model_command = 0;
  _abortion_command = 0;

  _error[0] = '\0';

  _actualAreaStats = new AreaStats();
  _estimatedAreaStats = new AreaStats();

  _confusion_matrix = new ConfusionMatrix();

  _roc_curve = new RocCurve();
}


/******************/
/*** destructor ***/

OpenModeller::~OpenModeller()
{
  if ( _map_command ) delete _map_command;

  if ( _model_command ) delete _model_command;

  if ( _abortion_command ) delete _abortion_command;

  delete _actualAreaStats;
  delete _estimatedAreaStats;
  delete _confusion_matrix;
  delete _roc_curve;
}


/*********************/
/*** get log level ***/
void
OpenModeller::setLogLevel( Log::Level level )
{
  Log::instance()->setLevel( level );
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
  if ( ( ! presence || presence->numOccurrences() == 0 ) &&
       ( ! absence  || absence->numOccurrences() == 0 ) ) {

    Log::instance()->error( "Occurrences must not be empty\n" );
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

    Log::instance()->error( "Wrong parameters when setting algorithm\n" );
    return 0;
  }

  if ( ! _samp ) {

    //  Create a default sampler if none was previously provided
    if ( ! hasEnvironment() ) {

      Log::instance()->warn( "Sampler could not be initialized. Environment not set.\n" );
      return 0;
    }
    else if ( ( ! _presence ) && ( ! _absence ) ) {

      Log::instance()->warn( "Sampler could not be initialized. Occurrences not set.\n" );
      return 0;
    }
    else {

      // _env and (_presence or _absence) are set
      setSampler( createSampler( _env, _presence, _absence ) );
    }
  }

  _alg = AlgorithmFactory::newAlgorithm( id );

  if ( ! _alg ) {

    Log::instance()->error( _error, "Could not find (%s) algorithm.", id );
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
  _confusion_matrix->reset();
  _roc_curve->reset();

  Log::instance()->info( "Creating model\n" );

  // Sampler
  if ( ! _samp ) {

    Log::instance()->error( "Sampler not specified for model creation.\n" );
    return 0;
  }

  // Algorithm.
  if ( ! _alg ) {

    Log::instance()->error( "Algorithm not specified for model creation.\n" );
    return 0;
  }

  _alg->createModel( _samp, _model_command, _abortion_command );

  Log::instance()->info( "Finished creating model\n" );

  return 1;
}


/******************/
/*** create Map ***/
int
OpenModeller::createMap( const EnvironmentPtr & env, char const *output_file, MapFormat& output_format )
{
  Log::instance()->info( "Projecting model\n" );

  if ( ! env ) {

    Log::instance()->error( "Projection environment not specified\n" );
    return 0;
  }

  _projEnv = env;

  Model model( _alg->getModel() );

  Map *mask = _projEnv->getMask();

  // If mask is undefined, use first layer as a mask
  if ( ! mask ) {

    mask = _projEnv->getLayer(0);
  }

  // Store output format in property 
  _format = output_format;

  // copy mask settings to the output format ONLY when they are undefined 
  _format.copyDefaults( *mask );

  // try to infer the output file format (default is 64 bit floating tiff).
  string fname( output_file );

  int pos = fname.length() - 4;

  if ( pos > 0 ) {

    if ( fname.compare( pos, 4, ".bmp" ) == 0 ) {

      _format.setFormat( MapFormat::GreyBMP );
      Log::instance()->warn ( "Using greyscale bmp as output format based on extension\n" );
    }
  }

  // Create map on disc.
  Map map( RasterFactory::instance().create( output_file, _format ) );

  bool finished = Projector::createMap( model, _projEnv, &map, _actualAreaStats, _map_command, _abortion_command );

  if ( ! finished ) {

    Log::instance()->info( "Error during model projection\n" );
    return 0;
  }

  Log::instance()->info( "Finished projecting model\n" );

  return 1;
}

int
OpenModeller::createMap( const EnvironmentPtr & env, char const *output_file )
{
  return createMap( env, output_file, _format );
}

/******************/
/*** create Map ***/
int
OpenModeller::createMap( char const *output_file )
{
  if ( ! _projEnv ) {

    _projEnv = _env;
  }

  return createMap( _projEnv, output_file, _format );
}

/******************/
/*** create Map ***/
int
OpenModeller::createMap( char const *output_file, MapFormat& output_format )
{
  if ( ! _projEnv ) {

    _projEnv = _env;
  }

  return createMap( _projEnv, output_file, output_format );
}

/**********************************/
/******* get Value ****************/
Scalar
OpenModeller::getValue(const ConstEnvironmentPtr& env, Coord x, Coord y)
{
  if ( ! _env ) {

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


/************************************/
/******* get Actual AreaStats *******/
AreaStats *
OpenModeller::getActualAreaStats()
{
  return new AreaStats( _actualAreaStats );
}


/***************************************/
/******* get Estimated AreaStats *******/
AreaStats * OpenModeller::getEstimatedAreaStats(double proportionAreaToSample)
{
  return getEstimatedAreaStats( _env, proportionAreaToSample );
}


/***************************************/
/******* get Estimated AreaStats *******/
AreaStats * OpenModeller::getEstimatedAreaStats(const ConstEnvironmentPtr& env,
						double proportionAreaToSample)
{
  int i, sampleSize, numCells, xdim, ydim;

  if ( !env ) {

    // this method does not work without _env properly set
    return NULL;
  }

  if ( !_estimatedAreaStats ) { 

    _estimatedAreaStats = new AreaStats; 
  }
  else { 

    _estimatedAreaStats->reset(); 
  }

  // get number of cells to sample
  // note that the total area does not take the mask into account
  // thus all cells (masked or unmasked) are counted
  env->getMask()->getDim(&xdim, &ydim);
  numCells = xdim * ydim; 

  sampleSize = (int) (numCells * proportionAreaToSample);

  for (i = 0; i < sampleSize; i++) { 

    const Sample& sample = env->getRandom();

    _estimatedAreaStats->addPrediction(_alg->getValue(sample)); 
  }

  return _estimatedAreaStats;
}


/************************************/
/******* get Confusion Matrix *******/
const ConfusionMatrix * const OpenModeller::getConfusionMatrix()
{
  if ( _confusion_matrix->ready() ) {

    return _confusion_matrix;
  }

  Log::instance()->debug( "Calculating confusion matrix using training dataset\n" );

  _confusion_matrix->calculate( getModel(), getSampler() );

  return _confusion_matrix;
}


/*****************************/
/******* get Roc Curve *******/
const RocCurve * const OpenModeller::getRocCurve()
{
  if ( _roc_curve->ready() ) {

    return _roc_curve;
  }

  Log::instance()->debug( "Calculating ROC curve using training dataset\n" );

  int num_presences = _samp->numPresence();

  int num_absence = _samp->numAbsence();

  SamplerPtr sampler;

  if ( ! num_absence ) {

    // If there are no absences, generate pseudo-absences (same number of presences)

    Log::instance()->debug( "Sampling pseudo-absences\n" );

    OccurrencesImpl * pseudo_absences = new OccurrencesImpl( 0.0 );

    for ( int i = 0; i < num_presences; ++i ) {

      OccurrencePtr oc = _samp->getPseudoAbsence();
      pseudo_absences->insert( oc );

      Log::instance()->debug( "(%f,%f)\n", oc->x(), oc->y() );
    }

    sampler = createSampler( _env, _samp->getPresences(), pseudo_absences );
  }
  else {

   // If there are absences, use the same sampler

    sampler = getSampler();
  }

  _roc_curve->calculate( getModel(), sampler );

  return _roc_curve;
}


/***************************************/
/******* get Model Configuration *******/
ConfigurationPtr
OpenModeller::getModelConfiguration() const
{
  ConfigurationPtr config( new ConfigurationImpl("SerializedModel"));

  ConfigurationPtr sampler_config( _samp->getConfiguration() );

  config->addSubsection( sampler_config );

  ConfigurationPtr alg_config( _alg->getConfiguration() );

  config->addSubsection( alg_config );

  ConfigurationPtr stats_config( new ConfigurationImpl("Statistics") );

  if ( _confusion_matrix->ready() ) {

    ConfigurationPtr cm_config( _confusion_matrix->getConfiguration() );

    stats_config->addSubsection( cm_config );
  }

  if ( _roc_curve->ready() ) {

    ConfigurationPtr roc_config( _roc_curve->getConfiguration() );

    stats_config->addSubsection( roc_config );
  }

  config->addSubsection( stats_config );

  return config;
}


/***************************************/
/******* set Model Configuration *******/
void
OpenModeller::setModelConfiguration( const ConstConfigurationPtr & config )
{
  Log::instance()->debug( "Setting model configuration\n" );

  _confusion_matrix->reset();
  _roc_curve->reset();

  Log::instance()->debug( "Creating sampler\n" );

  _samp = createSampler( config->getSubsection( "Sampler" ) );

  Log::instance()->debug( "Getting sampler attributes\n" );

  _env = _samp->getEnvironment();

  _presence = _samp->getPresences();

  _absence = _samp->getAbsences();

  Log::instance()->debug( "Getting algorithm from algorithm factory\n" );

  _alg = AlgorithmFactory::newAlgorithm( config->getSubsection( "Algorithm" ) );

  // Model creation options
  if ( ConstConfigurationPtr options_config = config->getSubsection( "Options", false ) ) {

    ConstConfigurationPtr occ_filter_config = options_config->getSubsection( "OccurrencesFilter", false );

    if ( occ_filter_config ) {

      ConstConfigurationPtr su_config = occ_filter_config->getSubsection( "SpatiallyUnique", false );

      if ( su_config ) {

        _samp->spatiallyUnique();
      }

      ConstConfigurationPtr eu_config = occ_filter_config->getSubsection( "EnvironmentallyUnique", false );

      if ( eu_config ) {

        _samp->environmentallyUnique();
      }
    }
  }

  Log::instance()->debug( "Assigning sampler to algorithm\n" );

  _alg->setSampler( _samp );
}


/********************************************/
/******* set Projection Configuration *******/
void
OpenModeller::setProjectionConfiguration( const ConstConfigurationPtr & config )
{
  Log::instance()->debug( "Setting projection configuration\n" );

  try { 

    _alg = AlgorithmFactory::newAlgorithm( config->getSubsection( "Algorithm" ) );

    _projEnv = createEnvironment( config->getSubsection( "Environment" ) );

    ConstConfigurationPtr output_param_config = config->getSubsection( "OutputParameters" );

    ConstConfigurationPtr template_layer_config = output_param_config->getSubsection( "TemplateLayer" );

    string formatId = template_layer_config->getAttribute( "Id" );

    _format = MapFormat( formatId.c_str() );

    try {

      string fileType;

      fileType = output_param_config->getAttribute( "FileType" );

      // Default is 8-bit tiff
      int type = MapFormat::GreyTiff;

      Log::instance()->info( "Setting Output file type to: %s\n", fileType.c_str() );

      if ( ! fileType.empty() ) {

        if ( fileType == "GreyTiff" ) {

          // nothing to do - it's already the default
        }
        else if ( fileType == "GreyTiff100" ) {

          type = MapFormat::GreyTiff100;
        }
        else if ( fileType == "FloatingTiff" ) {

          type = MapFormat::FloatingTiff;
        }
        else if ( fileType == "GreyBMP" ) {

          type = MapFormat::GreyBMP;
        }
        else if ( fileType == "FloatingHFA" ) {

          type = MapFormat::FloatingHFA;
        }
        else if ( fileType == "ByteHFA" ) {

          type = MapFormat::ByteHFA;
        }
        else {

          Log::instance()->warn( "Wrong value for 'Output file type' (%s). It should be GreyTiff, FloatingTiff, GreyBMP, FloatingHFA or ByteHFA. Using default.\n", fileType.c_str() );
        }
      }

      _format.setFormat( type );
    }
    catch ( AttributeNotFound& e ) { 

      // FileType attribute is optional
      UNUSED(e);
    }
  }
  catch( ConfigurationException& e ) { 

    Log::instance()->error( "Projection deserialization exception: %s\n", e.what() );

    throw e;
  }
}
