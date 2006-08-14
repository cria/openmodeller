/**
 * Definition of AlgorithmFactory class.
 * 
 * @file
 * @author Mauro E S Mu�oz (mauro@cria.org.br)
 * @date   2004-03-19
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

#include <openmodeller/AlgorithmFactory.hh>
#include <openmodeller/Algorithm.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Exceptions.hh>

#include <string>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;

#include <string.h>

class testDLLId {
public:
  testDLLId( char const *id ) :
    id(id)
  {}
  inline bool operator()( const AlgorithmFactory::DLLPtr& dll ) {
    return !strcmp( id, dll->getMetadata()->id );
  }
private:
  char const *id;
};

/**************************************************************************
 *
 * Implementation of AlgorithmFactory::DLL object
 *
 * Handles status of dlls.
 *************************************************************************/

/***********************/
/*** DLL class definitions ***/

AlgorithmFactory::DLL::DLL() :
  _handle( 0 ),
  _factory( 0 ),
  _metadata( 0 ),
  _file()
{
}

AlgorithmFactory::DLL::DLL( const string& file ) :
  _handle( 0 ),
  _factory( 0 ),
  _metadata( 0 ),
  _file( file )
{
  _handle = dllOpen( file.c_str() );

  // Opens the DLL file.
  if ( ! _handle )
    {
      char *error = dllError( _handle );
      g_log.warn( "Error loading %s: %s\n", file.c_str(), error );
      goto error;
    }

  _factory = (TAlgFactory) dllFunction( _handle, "algorithmFactory" );

  if ( ! _factory )
    {
      char *error = dllError( _handle );
      g_log.warn( "Algorithm %s is not openModeller compatible! (TAlgFactory mismatch)\n", file.c_str() );
      g_log.warn( "Error: %s\n", error );
      goto error;
    }

  _metadata = (TAlgMetadata) dllFunction( _handle, "algorithmMetadata" );
  
  if ( ! _metadata )
    {
      char *error = dllError( _handle );
      g_log.warn( "Algorithm %s is not openModeller compatible! (TAlgMetadata mismatch)\n", file.c_str() );
      g_log.warn( "Error: %s\n", error );
      goto error;
    }

  return;

 error:
  _handle = 0;
  _factory = 0;
  _metadata = 0;

}

/**********************/
/*** DLL destructor ***/

AlgorithmFactory::DLL::~DLL()
{
  if ( _handle ) {
    g_log.debug( "- Unloading: %s ...currently, dlls are not closed.\n", _file.c_str() );
    dllClose( _handle );
  }
}

/**********************/
/*** DLL operator bool() ***/

AlgorithmFactory::DLL::operator bool() const
{
  return ( _handle != 0 );
}

/*************************/
/*** DLL new Algorithm ***/
AlgorithmPtr
AlgorithmFactory::DLL::newAlgorithm()
{
  AlgorithmImpl *ai = (*_factory)();
  return AlgorithmPtr(ai);
}

/*************************/
/*** DLL new Algorithm ***/
AlgMetadata const *
AlgorithmFactory::DLL::getMetadata()
{
  return (*_metadata)();
}

/**************************************************************************
 *
 * Implementation of AlgorithmFactory object
 *
 *************************************************************************/

/*******************/
/*** constructor ***/

AlgorithmFactory::AlgorithmFactory() :
  _dlls(),
  _pluginpath()
{
}

/******************/
/*** destructor ***/

AlgorithmFactory::~AlgorithmFactory()
{
}

/******************/
/*** Singleton accessor ***/

AlgorithmFactory&
AlgorithmFactory::getInstance()
{
  static AlgorithmFactory theInstance;
  return theInstance;
}

/****************************/
/*** available Algorithms ***/
AlgMetadata const **
AlgorithmFactory::availableAlgorithms()
{

  AlgorithmFactory& af = getInstance();

  int dll_count = af._dlls.size();

  // Make room for the algorithms' metadatas.
  AlgMetadata const **all = new AlgMetadata const *[ dll_count + 1];

  AlgMetadata const **metadata = all;

  // If there are no DLLs loaded, return NULL;
  if ( dll_count > 0 ) {

    // For each DLL found:
    ListDLL::iterator dll = af._dlls.begin();
    for ( ; dll != af._dlls.end(); ++metadata, ++dll )
      *metadata = (*dll)->getMetadata();

  }
  // Null terminated.
  *metadata = 0;
  
  return all;
}


/********************************/
/*** num Available Algorithms ***/
int
AlgorithmFactory::numAvailableAlgorithms()
{
  AlgorithmFactory& af = getInstance();

  int dll_count = af._dlls.size();

  return dll_count;
}


/***************************/
/***  algorithm Metadata ***/
AlgMetadata const *
AlgorithmFactory::algorithmMetadata( char const *id )
{
  if ( ! id )
    throw InvalidParameterException( "Algorithm id not specified" );

  AlgorithmFactory& af = getInstance();

  // Metadata to be returned.
  AlgMetadata *metadata = 0;

  testDLLId test( id );
  ListDLL::iterator dll = find_if( af._dlls.begin(),
				   af._dlls.end(),
				   test );

  if ( dll != af._dlls.end() ) {
    return (*dll)->getMetadata();
  }

  string msg("Algorithm ");
  msg += id;
  msg += " not found";
  throw InvalidParameterException( msg );
}

/*********************/
/*** new Algorithm ***/
AlgorithmPtr
AlgorithmFactory::newAlgorithm( char const *id )
{

  AlgorithmFactory& af = getInstance();

  int dll_count = af._dlls.size();

  if ( dll_count == 0 )
    throw InvalidParameterException( "No algorithms loaded");

  testDLLId test( id );
  ListDLL::iterator dll = find_if( af._dlls.begin(), af._dlls.end(), test );

  if ( dll != af._dlls.end() ) {
    return (*dll)->newAlgorithm();
  }

  throw InvalidParameterException( "Algorithm not found");
}

AlgorithmPtr
AlgorithmFactory::newAlgorithm( const ConstConfigurationPtr & config ) {

  ConstConfigurationPtr meta_config = config->getSubsection( "AlgorithmMetadata" );

  string id = meta_config->getAttribute( "Id" );

  AlgorithmPtr alg( newAlgorithm( id.c_str() ) );

  if ( !alg )
    return alg;

  alg->setConfiguration( config );

  return alg;
}

/*****************/
/*** load DLLs ***/
int
AlgorithmFactory::searchDefaultDirs( )
{

  vector<string> entries = initialPluginPath();

  vector<string>::iterator it = entries.begin();
  while( it != entries.end() ) {
    AlgorithmFactory::addDir( (*it).c_str() );
    ++it;
  }

  return 1;
}

int
AlgorithmFactory::addDir( const string& dir )
{
  AlgorithmFactory& af = getInstance();

  return af.p_addDir( dir );
}

/****************/
/*** p_addDir ***/
int
AlgorithmFactory::p_addDir( const string& dir )
{
  vector<string> entries = scanDirectory( dir );

  if ( entries.empty() ) {
    g_log( "No algoritm found in directory %s\n", dir.c_str() );
    return 0;
  }

  vector<string>::const_iterator it = entries.begin();
  while( it != entries.end() ) {

    p_addDll( *it );

    ++it;
  }

  return _dlls.size();
}

/*****************/
/*** load one dll ***/
bool
AlgorithmFactory::addDll( const string& file )
{
  AlgorithmFactory& af = getInstance();

  return af.p_addDll( file );
}

/****************/
/*** p_addDll ***/
bool
AlgorithmFactory::p_addDll( const string& file )
{
  g_log( "- Loading: %s ...\n", file.c_str() );
    
  // Create a new DLL for each directory entry found.
  DLLPtr dll( new DLL( file.c_str() ) );

  // Test if the dll was successfully loaded.
  if ( !(*dll) ) {
    return false;
  }
  
  g_log( "- Successfully Loaded %s\n", file.c_str() );
  _dlls.push_back( dll );
  return true;

}

/*************************/
/*** get configuration ***/
ConfigurationPtr
AlgorithmFactory::getConfiguration()
{
  ConfigurationPtr config( new ConfigurationImpl("AvailableAlgorithms") );

  AlgorithmFactory& af = getInstance();

  int dll_count = af._dlls.size();

  // If there are any algorithms
  if ( dll_count > 0 ) {

    // For each algorithm
    ListDLL::iterator dll = af._dlls.begin();

    for ( ; dll != af._dlls.end(); ++dll ) {

      ConfigurationPtr alg_config( new ConfigurationImpl( "Algorithm" ) );

      ConfigurationPtr alg_meta_config( new ConfigurationImpl( "AlgorithmMetadata" ) );

      AlgMetadata const *algMetadata = (*dll)->getMetadata();

      alg_meta_config->addNameValue( "Id", algMetadata->id );
      alg_meta_config->addNameValue( "Name", algMetadata->name );
      alg_meta_config->addNameValue( "Version", algMetadata->version );
      alg_meta_config->addNameValue( "Author", algMetadata->author );
      alg_meta_config->addNameValue( "CodeAuthor", algMetadata->code_author );
      alg_meta_config->addNameValue( "Contact", algMetadata->contact );
      alg_meta_config->addNameValue( "Categorical", algMetadata->categorical );
      alg_meta_config->addNameValue( "Absence", algMetadata->absence );

      ConfigurationPtr alg_overview_config( new ConfigurationImpl( "Overview" ) );
      alg_overview_config->setValue( algMetadata->overview );
      alg_meta_config->addSubsection( alg_overview_config );

      ConfigurationPtr alg_description_config( new ConfigurationImpl( "Description" ) );
      alg_description_config->setValue( algMetadata->description );
      alg_meta_config->addSubsection( alg_description_config );

      ConfigurationPtr alg_bibliography_config( new ConfigurationImpl( "Bibliography" ) );
      alg_bibliography_config->setValue( algMetadata->biblio );
      alg_meta_config->addSubsection( alg_bibliography_config );

      alg_config->addSubsection( alg_meta_config );

      ConfigurationPtr params_config( new ConfigurationImpl( "AlgorithmParameters" ) );

      AlgParamMetadata *param = algMetadata->param;

      // Include parameters metadata
      for ( int i=0 ; i < algMetadata->nparam; param++, i++ ) {

        ConfigurationPtr param_config( new ConfigurationImpl( "Param" ) );

        param_config->addNameValue( "Id", param->id );
        param_config->addNameValue( "Name", param->name );
        param_config->addNameValue( "Type", param->type );
        param_config->addNameValue( "HasMin", param->has_min );
        param_config->addNameValue( "Min", param->min_val );
        param_config->addNameValue( "HasMax", param->has_max );
        param_config->addNameValue( "Max", param->max_val );
        param_config->addNameValue( "Typical", param->typical );

        ConfigurationPtr param_overview_config( new ConfigurationImpl( "Overview" ) );
        param_overview_config->setValue( param->overview );
        param_config->addSubsection( param_overview_config );

        ConfigurationPtr param_description_config( new ConfigurationImpl( "Description" ) );
        param_description_config->setValue( param->description );
        param_config->addSubsection( param_description_config );

        params_config->addSubsection( param_config );
      }

      alg_config->addSubsection( params_config );

      config->addSubsection( alg_config );
    }
  }

  return config;
  
}

