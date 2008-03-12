/**
 * Definition of TeDatabaseManager class.
 * 
 * @file
 * @author Alexandre Copertino Jardim <alexcj@dpi.inpe.br>
 * @date 2006-05-15
 * $Id$ 
 *
 * LICENSE INFORMATION
 * 
 * Copyright © 2006 INPE
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


#include <openmodeller/TeDatabaseManager.hh>

/**
* Singleton pattern
*/
TeDatabaseManager& 
TeDatabaseManager::instance()
{
	static TeDatabaseManager unique_;
	return unique_;
}

//! Return a TeDatabase pointer if it exists or create a new one.
/**
* \param params TeDatabaseFactoryParams.
* Obs: The TeDatabase is already connected.
*/
TeDatabase*
TeDatabaseManager::create( const TeDatabaseFactoryParams& params )
{
	string id = params.dbms_name_ +
		params.database_ +
		params.host_ +
		params.user_;

	MapManager::const_iterator i = mapManager_.find( id );
	if( i != mapManager_.end() )
	{
		return i->second;
	}
	else
	{
		TeDatabase* tedb = TeDatabaseFactory::make( params );
		mapManager_.insert( MapManager::value_type(id, tedb) ).second;
		return tedb;
	}
}

//! Descturctor.
/**
* Delete all opened databases.
*/
TeDatabaseManager::~TeDatabaseManager()
{
	// TODO - Test with other database (not Ado)
	/*MapManager::const_iterator i = mapManager_.begin();
	MapManager::const_iterator end = mapManager_.end();
	
	for(; i != end; ++i)
	{
		i->second->close();
		delete (i->second);
	}*/
}
