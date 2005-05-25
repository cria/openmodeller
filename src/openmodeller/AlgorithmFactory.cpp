/**
 * Definition of AlgorithmFactory class.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
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

#include <algorithm_factory.hh>
#include <om_algorithm.hh>
#include <om_log.hh>
#include <configuration.hh>
#include <Exceptions.hh>

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

  _factory = (TAlgFactory) dllFunction( _handle,
					"algorithmFactory" );
  if ( ! _factory )
    {
      char *error = dllError( _handle );
      g_log.warn( "%s is not openModeller compatible!\n", file.c_str() );
      g_log.warn( "Error: %s\n", error );
      goto error;
    }

  _metadata = (TAlgMetadata) dllFunction( _handle,
					"algorithmMetadata" );
  
  if ( ! _metadata )
    {
      char *error = dllError( _handle );
      g_log.warn( "%s is not openModeller compatible!\n", file.c_str() );
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
    g_log( "- Unloading: %s ...currently, dlls are not closed.\n", _file.c_str() );
    //    dllClose( _handle );
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

  throw InvalidParameterException( "Algorithm not found" );
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


