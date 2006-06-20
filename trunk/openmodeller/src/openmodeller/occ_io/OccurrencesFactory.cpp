/**
 * Definition of OccurrencesFactory class.
 * 
 * @file
 * @author Alexandre Copertino Jardim <alexcj@dpi.inpe.br>
 * @date 2006-03-21
 * $Id$ 
 * 
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

#include <openmodeller/occ_io/OccurrencesFactory.hh>

//! Returns a OccurencesReader
/**
* \param url used to locate occurrences in a database or file disk.
* \param coord_system coord system.
*/
OccurrencesReader*
OccurrencesFactory::create( const char *url, const char *coord_system )
{
	// Another OccurrencesReader
	string name = url;
	int i = name.find( ">" );
	if( i != -1)
	{
		string libName = name.substr( 0, i );
		CreatorMap::const_iterator i = mapOccurrencesReader_.find( libName );
		if ( i != mapOccurrencesReader_.end())
		{
			return (i->second)(url, coord_system );
		}
	}
	// Default: OccurrencesFile
	return new OccurrencesFile( url, coord_system );
}

/**
* Singleton pattern
*/
OccurrencesFactory& 
OccurrencesFactory::instance()
{
	static OccurrencesFactory unique_;
	return unique_;
}

/**
* Register a OccurrencesReader.
*/
bool 
OccurrencesFactory::registerOccurrencesReader(const string& decId, CreateOccurrencesReaderCallback creator)
{
	return mapOccurrencesReader_.insert(CreatorMap::value_type(decId, creator)).second;
}

/**
* Unregister a OccurrencesReader.
*/
bool 
OccurrencesFactory::unregisterOccurrencesReader(const string& decId)
{
	return mapOccurrencesReader_.erase(decId) != 0;
}
