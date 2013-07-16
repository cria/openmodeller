/**
 * Class to read model creation/projection parameters from a request file and run om.
 * 
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2004-06-22
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

#include "request_file.hh"

#include <openmodeller/om.hh>
#include <openmodeller/FileParser.hh>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


/**************************************************************/
/************************ Request File ************************/

RequestFile::RequestFile() :
  _occurrencesSet(0),
  _environmentSet(0),
  _projectionSet(0),
  _presences(),
  _absences(),
  _nonNativeProjection( false ),
  _projectionCategoricalMap(),
  _projectionMap(),
  _inputMask(),
  _outputMask(),
  _inputModelFile(),
  _outputModelFile(),
  _projectionFile(),
  _outputFormat(),
  _spatiallyUnique( false ),
  _environmentallyUnique( false ),
  _calcConfusionMatrix( true ),
  _calcAuc( true )
{ 
}

RequestFile::~RequestFile() 
{
}

/*****************/
/*** configure ***/
int
RequestFile::configure( OpenModeller *om, char *request_file )
{
  std::string input_file( request_file );
  FileParser fp( input_file );

  _inputModelFile = fp.get( "Input model" );

  _occurrencesSet = _setOccurrences( om, fp );
  _environmentSet = _setEnvironment( om, fp );

  // Optional sampler filters
  std::string spatially_unique = fp.get( "Spatially unique" );

  if ( spatially_unique == "true" ) {

    _spatiallyUnique = true;
  }

  std::string environmentally_unique = fp.get( "Environmentally unique" );

  if ( environmentally_unique == "true" ) {

    _environmentallyUnique = true;
  }

  // Optional model statistics
  std::string confusion_matrix = fp.get( "Confusion matrix" );

  if ( confusion_matrix == "false" ) {

    _calcConfusionMatrix = false;
  }

  std::string auc = fp.get( "AUC" );

  if ( auc == "false" ) {

    _calcAuc = false;
  }

  _projectionSet  = _setProjection ( om, fp );
  _algorithmSet   = _setAlgorithm  ( om, fp );

  _outputModelFile = fp.get( "Output model" );

  // Returns ZERO if all was set correctly.
  return 4 - _occurrencesSet - _environmentSet -
    _projectionSet - _algorithmSet;
}


/***********************/
/*** set Occurrences ***/
int
RequestFile::_setOccurrences( OpenModeller *om, FileParser &fp )
{
  // Obtain the Well Known Text string for the localities
  // coordinate system.
  std::string oc_cs = fp.get( "WKT coord system" );

  // Get the name of the file containing localities
  std::string oc_file = fp.get( "Occurrences source" );

  if ( oc_file.empty() ) {

    oc_file = fp.get( "Species file" ); // backwards compatibility
  }

  // Get the name of the taxon being modelled!
  std::string oc_name = fp.get( "Occurrences group" );

  if ( oc_name.empty() ) {

    oc_name = fp.get( "Species" ); // backwards compatibility
  }

  // If user provided a serialized model
  if ( ! _inputModelFile.empty() ) {

    // Warn if unnecessary parameters were specified 
    if ( ! oc_cs.empty() )
      Log::instance()->warn( "'WKT coord system' will be ignored since 'Input model' has been specified...\n" );

    if ( ! oc_file.empty() )
      Log::instance()->warn( "'Occurrences source'/'Species file' will be ignored since 'Input model' has been specified...\n" );

    if ( ! oc_name.empty() )
      Log::instance()->warn( "'Occurrences group'/'Species' will be ignored since 'Input model' has been specified...\n" );

    return 1;
  }

  // When a model needs to be created, 'WKT coord system' and 
  // 'Species file' are mandatory parameters
  if ( oc_cs.empty() ) {
    Log::instance()->error( "'WKT coord system' keyword was not specified in the request file!\n" );
    return 0;
  }

  if ( oc_file.empty() ) {

    Log::instance()->error( "'Occurrences source' keyword was not specified in the request file!\n" );
    return 0;
  }

  // Populate the occurences list from the localities file
  Log::instance()->info( "Reading occurrences...\r" );

  OccurrencesReader* oc_reader = OccurrencesFactory::instance().create( oc_file.c_str(), oc_cs.c_str() );

  _presences = oc_reader->getPresences( oc_name.c_str() );

  _absences = oc_reader->getAbsences( oc_name.c_str() );

  delete oc_reader;

  if ( _absences )
  {
    Log::instance()->info( "Reading occurrences...done\n" );

    return om->setOccurrences( _presences, _absences );
  }
  else if ( _presences ) {

    Log::instance()->info( "Reading occurrences...done\n" );
    return om->setOccurrences( _presences );
  }
  else {

    Log::instance()->error( "Could not read any occurrences!\n" );

    return 0;
  }
}


/***********************/
/*** set Environment ***/
int
RequestFile::_setEnvironment( OpenModeller *om, FileParser &fp )
{
  // Mask to select the desired species occurrence points
  _inputMask = fp.get( "Mask" );

  // Initiate the environment with all maps.
  std::vector<std::string> cat = fp.getAll( "Categorical map" );
  std::vector<std::string> map = fp.getAll( "Map" );

  // If user provided a serialized model
  if ( ! _inputModelFile.empty() ) {

    // Warn if unnecessary parameters were specified 
    if ( ! _inputMask.empty() )
      Log::instance()->warn( "'Mask' will be ignored since 'Input model' has been specified...\n" );

    if ( cat.size() > 0 )
      Log::instance()->warn( "'Categorical map' will be ignored since 'Input model' has been specified...\n" );

    if ( map.size() > 0 )
      Log::instance()->warn( "'Map' will be ignored since 'Input model' has been specified...\n" );

    return 1;
  }

  // When a model needs to be created, there should be at least one input map
  if ( ! (cat.size() + map.size()) ) {

    Log::instance()->error( "At least one 'Map' or 'Categorical map' needs to be specified!\n" );
    return 0;
  }

  // Mask is also mandatory
  if ( _inputMask.empty() ) {
    Log::instance()->error( "'Mask' was not specified!\n" );
    return 0;
  }

  // Set input environment
  Log::instance()->info( "Reading layers...\r" );
  om->setEnvironment( cat, map, _inputMask );
  Log::instance()->info( "Reading layers...done\n" );

  return 1;
}


/**********************/
/*** set Projection ***/
int
RequestFile::_setProjection( OpenModeller *om, FileParser &fp )
{
  _projectionFile = fp.get( "Output file" );

  if ( _projectionFile.empty() ) {

    Log::instance()->warn( "'Output file' was not specified.\n" );
    return 1;
  }

  // Categorical environmental maps and the number of these maps.
  _projectionCategoricalMap = fp.getAll( "Categorical output map" );

  // Continuous environmental maps and the number of these maps.
  _projectionMap = fp.getAll( "Output Map" );

  // If user provided a serialized model
  if ( !_inputModelFile.empty() ) {

    // note: should we accept native projections using environment from serialized models?
    _nonNativeProjection = true;

    // So, assume that in this case projection maps are mandatory.
    if ( ! (_projectionCategoricalMap.size() + _projectionMap.size()) ) {

      Log::instance()->error( "At least one 'Output map' or 'Categorical output map' needs to be specified!\n" );
      return 0;
    }
  }
  else {

    // It is ok to not set the projection.
    if ( ! (_projectionCategoricalMap.size() + _projectionMap.size()) ) {

      Log::instance()->info("Projection not set: using training Environment for projection\n");
      _nonNativeProjection = false;
    }
    else {

      _nonNativeProjection = true;
    }
  }

  // Get the output mask
  _outputMask = fp.get( "Output mask" );

  if ( _nonNativeProjection && _outputMask.empty() ) {

    Log::instance()->error( "'Output mask' was not specified!\n" );
    return 0;
  }

  // Template header to be used by the generated map
  std::string format = fp.get( "Output format" );

  if ( ! format.empty() ) {

    _outputFormat = MapFormat( format.c_str() );
  }

  // File type
  std::string fileType = fp.get( "Output file type" );

  if ( ! fileType.empty() ) {

    _outputFormat.setFormat( fileType );
  }

  // Overwrite output extent with values from mask
  const std::string maskFile = ( _nonNativeProjection ) ? _outputMask.c_str() : _inputMask.c_str();

  Raster* mask = RasterFactory::instance().create( maskFile );

  Header h = mask->header();

  _outputFormat.setXMin( h.xmin );
  _outputFormat.setYMin( h.ymin );
  _outputFormat.setXMax( h.xmax );
  _outputFormat.setYMax( h.ymax );

  delete mask;

  return 1;
}


/***********************/
/*** set Algorithm ***/
int
RequestFile::_setAlgorithm( OpenModeller *om, FileParser &fp )
{
  // Find out which model algorithm is to be used.
  AlgMetadata const *metadata;
  std::string alg_id = fp.get( "Algorithm" );

  // If user provided a serialized model
  if ( ! _inputModelFile.empty() ) {
    // Warn if unnecessary parameters were specified 
    if ( ! alg_id.empty() )
      Log::instance()->warn( "'Algorithm' will be ignored since 'Input model' has been specified...\n" );

    return 1;
  }

  // Note: console tries to get an algorithm from user input
  // if it was not specified in the request file.
  if ( alg_id.empty() )
    return 0;

  // Try to use the algorithm specified in the request file.
  // If it cannot be used, return 0.
  try {

    // An exception here means that the algorithm wasn't found.
    metadata = om->algorithmMetadata( alg_id.c_str() );
  }
  catch (...) {

    Log::instance()->error( "Algorithm '%s' specified in the request file was not found\n", 
                 alg_id.c_str() );
    return 0;
  }

  // Obtain any model parameter specified in the request file.
  // read parameters from file into req_param parameters
  std::vector<std::string> req_param = fp.getAll( "Parameter" );

  // For resulting parameters storage.
  int nparam = metadata->nparam;
  AlgParameter *param = new AlgParameter[nparam];

  // Read from console the parameters not set by request
  // file. Fills 'param' with all 'metadata->nparam' parameters
  // set.
  _readParameters( param, metadata, req_param );

  // Set the model algorithm to be used by the controller
  int resp = om->setAlgorithm( metadata->id, nparam, param );

  if ( resp == 0 ) {

    Log::instance()->error( "Could not set the algorithm to be used\n" );
  }

  delete[] param;

  return resp;
}

/*********************/
/*** get Presences ***/
OccurrencesPtr
RequestFile::getPresences( )
{
  if ( ! _presences ) {

    Log::instance()->error( "Could not read occurrences from request file. Make sure 'Occurrences source' has been specified.\n" );
  }

  return _presences;
}


/*********************/
/*** get Absences ***/
OccurrencesPtr
RequestFile::getAbsences( )
{
  return _absences;
}

/***********************/
/*** read Parameters ***/
int
RequestFile::_readParameters( AlgParameter *result,
                              AlgMetadata const *metadata,
                              std::vector<std::string> str_param )
{
  AlgParamMetadata *param = metadata->param;
  AlgParamMetadata *end   = param + metadata->nparam;

  // For each algorithm parameter metadata...
  for ( ; param < end; param++, result++ ) {

    // The resulting name is equal the name set in
    // algorithm's metadata.
    result->setId( param->id );

    // Read the resulting value from str_param.
    std::string value = extractParameter( result->id(), str_param );

    // If the parameter is not referenced in the file, set it
    // with the default value extracted from the parameter
    // metadata.
    if ( value.empty() )
      value = param->typical;

    result->setValue( value.c_str() );
  }

  return metadata->nparam;
}


/*************************/
/*** extract Parameter ***/
std::string
RequestFile::extractParameter( std::string const name, 
			       std::vector<std::string> vet )
{
  int length = name.length();
  std::vector<std::string>::iterator it = vet.begin();
  std::vector<std::string>::iterator end = vet.end();

  while ( it != end ) {

    if ( name == (*it).substr( 0, length ) ) {

      std::string value = (*it).substr( length );

      // Left trim the value 
      std::string::size_type pos = value.find_first_not_of(' ');

      if ( pos != std::string::npos ) {

        value.erase( 0, pos );
      }

      return value;
    }

    ++it;
  }

  return "";
}

/****************************/
/*** requested Projection ***/
bool
RequestFile::requestedProjection( )
{
  return ! _projectionFile.empty();
}

/******************/
/*** make Model ***/
void
RequestFile::makeModel( OpenModeller *om )
{
  // No serialized model - create model with all settings from the request file
  if ( _inputModelFile.empty() ) {

    // Apply sampler filters if requested by user
    if ( _spatiallyUnique ) {

      SamplerPtr sampler = om->getSampler();

      if ( sampler ) {

        sampler->spatiallyUnique();
      }
      else {

        Log::instance()->warn( "Cannot set spatially unique filter: no sampler available\n" );
      }
    }

    if ( _environmentallyUnique ) {

      SamplerPtr sampler = om->getSampler();

      if ( sampler ) {

        sampler->environmentallyUnique();
      }
      else {

        Log::instance()->warn( "Cannot set environmentally unique filter: no sampler available\n" );
      }
    }
  }
  // If user provided a serialized model, just load it
  else {

    Log::instance()->info( "Loading serialized model\n" );

    char* file_name = new char [_inputModelFile.size() + 1];
    strcpy( file_name, _inputModelFile.c_str() );

    ConfigurationPtr conf = Configuration::readXml( file_name );

    om->setModelConfiguration( conf );

    delete[] file_name;

    return;
  }

  // Build model
  if ( ! om->createModel() ) {

    Log::instance()->error( "Error during model creation: %s\n", om->error() );
    return;
  }

  if ( calcConfusionMatrix() ) {

    // Calculate confusion matrix to store in the serialized model
    om->getConfusionMatrix();
  }

  if ( calcAuc() ) {

    // Calculate ROC curve to store in the serialized model
    om->getRocCurve()->getTotalArea();
  }

  // Serialize model, if requested
  if ( _inputModelFile.empty() && ! _outputModelFile.empty() ) {

    char* file_name = new char [_outputModelFile.size() + 1];
    strcpy( file_name, _outputModelFile.c_str() );

    ConfigurationPtr cfg = om->getModelConfiguration();
    Configuration::writeXml( cfg, file_name );

    delete[] file_name;
  }
}


/***********************/
/*** make Projection ***/
void
RequestFile::makeProjection( OpenModeller *om )
{
  if ( _projectionSet == 0 ) {

    Log::instance()->error( "Error during projection: Request not properly initialized\n" );
    return;
  }

  if ( !_nonNativeProjection ) {

    om->createMap( _projectionFile.c_str(), _outputFormat );
  }
  else {

    EnvironmentPtr env = createEnvironment( _projectionCategoricalMap, _projectionMap, _outputMask );

    om->createMap( env, _projectionFile.c_str(), _outputFormat );
  }
}
