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

#ifndef TE_DATABASE_MANAGER_HH
#define TE_DATABASE_MANAGER_HH

#include <TeDatabaseFactory.h>
#include <TeDatabaseFactoryParams.h>

#include <string>
using std::string;

#include <map>
using std::map;

//! Manages TeDatabases connections.
/**
* Verify if exists one connection to a specified TeDatabaseFactoryParams and return it or a new connection.
* Obs: Don't delete the TeDatabase pointer or close the connection.
*/
class TeDatabaseManager
{
private:
	//! Map of TeDatabase and identifiers.
	typedef map<string, TeDatabase*> MapManager;

public:
	//! Singleton pattern
	/**
	* Returns the unique instance of TeDatabaseManager.
	*/
	static TeDatabaseManager& instance();

	//! Return a TeDatabase pointer if it exists or create a new one.
	/**
	* \param params TeDatabaseFactoryParams.
	* Obs: The TeDatabase is already connected.
	*/
	TeDatabase* create( const TeDatabaseFactoryParams& params );

private:
	//! Map of TeDatabase and identifiers.
	MapManager mapManager_;

	//! No build allowed.
	/**
	* Singleton pattern doesn't allows a public constuctor.
	*/
	TeDatabaseManager() {};
	//! No copy allowed.
	TeDatabaseManager(const TeDatabaseManager&);
	//! No copy allowed.
	TeDatabaseManager& operator=(const TeDatabaseManager&);
	//! No destruct allowed.
	~TeDatabaseManager();
};

#endif
