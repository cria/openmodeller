/**
 * Definition of AlgorithmFactory class.
 * 
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
      const char *error = dllError( _handle );
      Log::instance()->warn( "Error loading %s: %s\n", file.c_str(), error );
      goto error;
    }

  _factory = (TAlgFactory) dllFunction( _handle, "algorithmFactory" );

  if ( ! _factory )
    {
      const char *error = dllError( _handle );
      Log::instance()->warn( "Algorithm %s is not openModeller compatible! (TAlgFactory mismatch)\n", file.c_str() );
      Log::instance()->warn( "Error: %s\n", error );
      goto error;
    }

  _metadata = (TAlgMetadata) dllFunction( _handle, "algorithmMetadata" );
  
  if ( ! _metadata )
    {
      const char *error = dllError( _handle );
      Log::instance()->warn( "Algorithm %s is not openModeller compatible! (TAlgMetadata mismatch)\n", file.c_str() );
      Log::instance()->warn( "Error: %s\n", error );
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
    Log::instance()->debug( "- Unloading: %s ...currently, dlls are not closed.\n", _file.c_str() );
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
  if ( ! id ) {

    std::string msg = "Algorithm id not specified.\n";

    Log::instance()->error( msg.c_str() );

    throw InvalidParameterException( msg );
  }

  AlgorithmFactory& af = getInstance();

  testDLLId test( id );
  ListDLL::iterator dll = find_if( af._dlls.begin(), af._dlls.end(), test );

  if ( dll != af._dlls.end() ) {

    return (*dll)->getMetadata();
  }

  string msg("Algorithm ");
  msg += id;
  msg += " not found";

  Log::instance()->error( msg.c_str() );

  throw InvalidParameterException( msg );
}

/*********************/
/*** new Algorithm ***/
AlgorithmPtr
AlgorithmFactory::newAlgorithm( char const *id )
{
  AlgorithmFactory& af = getInstance();

  int dll_count = af._dlls.size();

  if ( dll_count == 0 ) {

    std::string msg = "No algorithms loaded.\n";

    Log::instance()->error( msg.c_str() );

    throw AlgorithmException( msg );
  }

  testDLLId test( id );

  ListDLL::iterator dll = find_if( af._dlls.begin(), af._dlls.end(), test );

  if ( dll != af._dlls.end() ) {

    return (*dll)->newAlgorithm();
  }

  string msg("Algorithm ");
  msg += id;
  msg += " not found";

  Log::instance()->error( msg.c_str() );

  throw InvalidParameterException( msg );
}

AlgorithmPtr
AlgorithmFactory::newAlgorithm( const ConstConfigurationPtr & config ) {

  Log::instance()->debug( "Instantiating serialized algorithm\n" );

  string id = config->getAttribute( "Id" );

  Log::instance()->debug( "Algorithm id: %s \n" , id.c_str() );

  AlgorithmPtr alg( newAlgorithm( id.c_str() ) );

  if ( ! alg ) {

    return alg;
  }

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
    Log::instance()->debug( "Checking for algs in: [%s]\n" , (*it).c_str() );
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
  if ( dir.length() ) {

    vector<string> entries = scanDirectory( dir );

    if ( entries.empty() ) {

      Log::instance()->warn( "No algorithm found in directory [%s]\n", dir.c_str() );
      return 0;
    }

    vector<string>::const_iterator it = entries.begin();

    while( it != entries.end() ) {

      p_addDll( *it );

      ++it;
    }
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
  Log::instance()->debug( "- Loading: %s ...\n", file.c_str() );
    
  // Create a new DLL for each directory entry found.
  DLLPtr dll( new DLL( file.c_str() ) );

  // Test if the dll was successfully loaded.
  if ( !(*dll) ) {
    return false;
  }
  
  Log::instance()->debug( "- Successfully Loaded %s\n", file.c_str() );
  _dlls.push_back( dll );
  return true;

}

/*************************/
/*** get configuration ***/
ConfigurationPtr
AlgorithmFactory::getConfiguration()
{
  ConfigurationPtr config( new ConfigurationImpl("Algorithms") );

  AlgorithmFactory& af = getInstance();

  int dll_count = af._dlls.size();

  // If there are any algorithms
  if ( dll_count > 0 ) {

    // For each algorithm
    ListDLL::iterator dll = af._dlls.begin();

    for ( ; dll != af._dlls.end(); ++dll ) {

      ConfigurationPtr alg_meta_config( new ConfigurationImpl( "Algorithm" ) );

      AlgMetadata const *algMetadata = (*dll)->getMetadata();

      alg_meta_config->addNameValue( "Id", algMetadata->id );
      alg_meta_config->addNameValue( "Version", algMetadata->version );

      ConfigurationPtr alg_name_config( new ConfigurationImpl( "Name" ) );
      alg_name_config->setValue( algMetadata->name );
      alg_meta_config->addSubsection( alg_name_config );

      ConfigurationPtr alg_overview_config( new ConfigurationImpl( "Overview" ) );
      alg_overview_config->setValue( algMetadata->overview );
      alg_meta_config->addSubsection( alg_overview_config );

      ConfigurationPtr alg_description_config( new ConfigurationImpl( "Description" ) );
      alg_description_config->setValue( algMetadata->description );
      alg_meta_config->addSubsection( alg_description_config );

      ConfigurationPtr alg_designers_config( new ConfigurationImpl( "Designers" ) );
      ConfigurationPtr alg_designer_config( new ConfigurationImpl( "Designer" ) );
      alg_designer_config->addNameValue( "Name", algMetadata->author );
      alg_designers_config->addSubsection( alg_designer_config );
      alg_meta_config->addSubsection( alg_designers_config );

      ConfigurationPtr alg_bibliography_config( new ConfigurationImpl( "Bibliography" ) );
      alg_bibliography_config->setValue( algMetadata->biblio );
      alg_meta_config->addSubsection( alg_bibliography_config );

      ConfigurationPtr alg_developers_config( new ConfigurationImpl( "Developers" ) );
      ConfigurationPtr alg_developer_config( new ConfigurationImpl( "Developer" ) );
      alg_developer_config->addNameValue( "Name", algMetadata->code_author );
      alg_developer_config->addNameValue( "Contact", algMetadata->contact );
      alg_developers_config->addSubsection( alg_developer_config );
      alg_meta_config->addSubsection( alg_developers_config );

      ConfigurationPtr alg_categorical_config( new ConfigurationImpl( "AcceptsCategoricalMaps" ) );
      char const *accepts_categorical = ( algMetadata->categorical ) ? "1" : "0";
      alg_categorical_config->setValue( accepts_categorical );
      alg_meta_config->addSubsection( alg_categorical_config );

      ConfigurationPtr alg_absences_config( new ConfigurationImpl( "RequiresAbsencePoints" ) );
      char const *requires_absences = ( algMetadata->absence ) ? "1" : "0";
      alg_absences_config->setValue( requires_absences );
      alg_meta_config->addSubsection( alg_absences_config );

      ConfigurationPtr params_config( new ConfigurationImpl( "Parameters" ) );

      AlgParamMetadata *param = algMetadata->param;

      // Include parameters metadata
      for ( int i=0 ; i < algMetadata->nparam; param++, i++ ) {

        ConfigurationPtr param_config( new ConfigurationImpl( "Parameter" ) );

        param_config->addNameValue( "Id", param->id );

        ConfigurationPtr param_name_config( new ConfigurationImpl( "Name" ) );
        param_name_config->setValue( param->name );
        param_config->addSubsection( param_name_config );

        string datatype("?");

        if ( param->type == Integer ) {

          datatype = "Integer";
        }
        else if ( param->type == Real ) {

          datatype = "Real";
        }
        else if ( param->type == String ) {

          datatype = "String";
        }

        ConfigurationPtr param_type_config( new ConfigurationImpl( "Type" ) );
        param_type_config->setValue( datatype.c_str() );
        param_config->addSubsection( param_type_config );

        ConfigurationPtr param_overview_config( new ConfigurationImpl( "Overview" ) );
        param_overview_config->setValue( param->overview );
        param_config->addSubsection( param_overview_config );

        ConfigurationPtr param_description_config( new ConfigurationImpl( "Description" ) );
        param_description_config->setValue( param->description );
        param_config->addSubsection( param_description_config );

        if ( param->has_min || param->has_max ) {

          ConfigurationPtr param_range_config( new ConfigurationImpl( "AcceptedRange" ) );

          if ( param->has_min ) {

            param_range_config->addNameValue( "Min", param->min_val );
          }
          if ( param->has_max ) {

            param_range_config->addNameValue( "Max", param->max_val );
          }

          param_config->addSubsection( param_range_config );
        }

        ConfigurationPtr param_default_config( new ConfigurationImpl( "Default" ) );
        param_default_config->setValue( param->typical );
        param_config->addSubsection( param_default_config );

        params_config->addSubsection( param_config );
      }

      alg_meta_config->addSubsection( params_config );

      config->addSubsection( alg_meta_config );
    }
  }

  return config;
}

