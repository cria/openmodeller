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
#include <list.cpp>
#include <om_log.hh>

#include <string.h>


/****************************************************************/
/*********************** Algorithm Factory **********************/

/***********************/
/*** DLL constructor ***/

AlgorithmFactory::DLL::DLL( char *file )
{
  _file   = file;
  _handle = 0;
  _alg    = 0;
}


/**********************/
/*** DLL destructor ***/

AlgorithmFactory::DLL::~DLL()
{
  if ( _file )   delete _file;
  if ( _alg )    delete _alg;
  if ( _handle ) dllClose( _handle );
}


/****************/
/*** DLL load ***/
Algorithm *
AlgorithmFactory::DLL::load()
{
  char *error;

  // Opens the DLL file.
  if ( ! (_handle = dllOpen( _file )) )
    {
      error = dllError( _handle );
      g_log.warn( "Error loading %s: %s\n", _file, error );
      return 0;
    }

  // Instantiated the algorithm.
  TAlgFactory factory;
  factory = (TAlgFactory) dllFunction( _handle,
                                       "algorithmFactory" );

  
  if ( !factory )
    {
	  error = dllError( _handle );
      g_log.warn( "%s is not openModeller compatible! ", _file );
      g_log.warn( "Error: %s\n", error );
      return 0;
    }

  return _alg = (*factory)();
}



/*******************/
/*** constructor ***/

AlgorithmFactory::AlgorithmFactory( char **dirs )
{
  _dirs = dirs;
}


/******************/
/*** destructor ***/

AlgorithmFactory::~AlgorithmFactory()
{
  DLL *dll;
  for ( _lstDLL.head(); dll = _lstDLL.get(); _lstDLL.next() )
    delete dll;
}


/****************************/
/*** available Algorithms ***/
AlgMetadata **
AlgorithmFactory::availableAlgorithms()
{
  // Reloads (refresh) the DLLs in _lstDLL.
  cleanDLLs();
  loadDLLs( _dirs );

  // Make room for the algorithms' metadatas.
  AlgMetadata **all;
  all = new AlgMetadata *[_lstDLL.length() + 1];

  // For each DLL found:
  AlgMetadata **metadata = all;
  DLL *dll;
  for ( _lstDLL.head(); dll = _lstDLL.get(); _lstDLL.next() )
    *metadata++ = dll->getAlgorithm()->getMetadata();

  // Null terminated.
  *metadata = 0;
  
  return all;
}


/********************************/
/*** num Available Algorithms ***/
int
AlgorithmFactory::numAvailableAlgorithms()
{
  // If there is no algorithm, try to read them (again).
  if ( ! _lstDLL.length() )
    availableAlgorithms();

  return _lstDLL.length();
}


/***************************/
/***  algorithm Metadata ***/
AlgMetadata *
AlgorithmFactory::algorithmMetadata( char *id )
{
  if ( ! id )
    return 0;

  // Metadata to be returned.
  AlgMetadata *metadata;

  // For each DLL opened.
  DLL *dll;
  for ( _lstDLL.head(); dll = _lstDLL.get(); _lstDLL.next() )
    {
      metadata = dll->getAlgorithm()->getMetadata();

      if ( ! strcmp( id, metadata->id ) )
	return metadata;
    }

  return 0;
}


/*********************/
/*** new Algorithm ***/
Algorithm *
AlgorithmFactory::newAlgorithm( Sampler *samp, char *id,
                                int nparam,
                                OmAlgParameter *param )
{
  Algorithm *alg;

  // If there is no algorithm, try to read them (again).
  if ( ! _lstDLL.length() )
    availableAlgorithms();

  // For each DLL in the list.
  DLL *dll;
  for ( _lstDLL.head(); dll = _lstDLL.get(); _lstDLL.next() )
    {
      alg = dll->getAlgorithm();

      if ( ! strcmp ( id, alg->getID() ) )
	{
	  alg->setSampler( samp );
	  alg->setParameters( nparam, param );
	  return alg;
	}
    }

  return 0;
}


/*****************/
/*** load DLLs ***/
int
AlgorithmFactory::loadDLLs( char **dirs )
{
  // Gets all DLLs from all directories.
  while ( *dirs )
    scanDir( *dirs++, _lstDLL );

  return _lstDLL.length();
}


/******************/
/*** clean DLLs ***/
void
AlgorithmFactory::cleanDLLs()
{
  // For each DLL in the list.
  DLL *dll;
  for ( _lstDLL.head(); dll = _lstDLL.get(); _lstDLL.next() )
    delete dll;

  _lstDLL.clear();
}


/****************/
/*** scan Dir ***/
int
AlgorithmFactory::scanDir( char *dir, ListDLL &lst )
{
  char **entries = scanDirectory( dir );

  if ( ! entries )
    return 0;

  char **pent = entries;
  while ( *pent )
    {
      g_log( "- Loading: %s ... ", *pent );

      // Create a new DLL for each directory entry found.
      DLL *dll = new DLL( *pent++ );

      // If the dll can create an algorithm, insert it in to
      // the list.
      if ( dll->load() )
        {
          g_log( "ok\n" );
          lst.append( dll );
        }

      // If it can not... :(
      // This deallocate the directory entry setted!
      else
	delete dll;
    }

  delete entries;

  return lst.length();
}

