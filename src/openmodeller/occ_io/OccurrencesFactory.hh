/**
 * Definition of OccurrencesFactory class.
 * 
 * @file
 * @author Alexandre Copertino Jardim <alexcj@dpi.inpe.br>
 * @date 2006-03-21
 * $Id$ 
 *
 * LICENSE INFORMATION
 *
 * Copyright © 2006 INPE
 * Copyright(c) 2003 by CRIA - Centro de Referencia em Informacao Ambiental
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


#ifndef _OCCURRENCES_FACTORY_HH_
#define _OCCURRENCES_FACTORY_HH_

#include <openmodeller/occ_io/OccurrencesFile.hh>
#include <openmodeller/occ_io/OccurrencesReader.hh>

#include <string>
using std::string;

#include <map>
using std::map;

//! Return a OccurrencesReader.
/**
* Based on the entrance url it returns a OccurrencesReader.
*/
class dllexp OccurrencesFactory
{
	private:
	//! Function pointer to builder function.
	typedef OccurrencesReader* (*CreateOccurrencesReaderCallback)(const char *url, 
								const char *coord_system );
	//! Map of OccurrencesReader and identifiers.
	typedef map<string, CreateOccurrencesReaderCallback> CreatorMap;


public:
	//! Singleton pattern
	/**
	* Returns the unique instance of OccurrencesFactory.
	*/
	static OccurrencesFactory& instance();
	
	//! Register a OccurrencesReader.
	/**
	* \param decId OccurrencesReader Identifier.
	* \param creator Function pointer to builder function.
	*/
	bool registerOccurrencesReader(const string& decId, CreateOccurrencesReaderCallback creator);
	
	//! Unregister a OccurrencesReader.
	bool unregisterOccurrencesReader(const string& decId);

	//! Returns a OccurencesReader
	/**
    * \param url used to locate the occurrences.
	* \param coord_system coord system.
	*/
	OccurrencesReader* create( const char *url, const char *coord_system );

private:
	//! Map of OccurrencesReader and identifiers.
	CreatorMap mapOccurrencesReader_;
	
	//! No build allowed.
	/**
	* Singleton pattern doesn't allows a public constuctor.
	*/
	OccurrencesFactory(){};
	//! No copy allowed.
	OccurrencesFactory(const OccurrencesFactory&);
	//! No copy allowed.
	OccurrencesFactory& operator=(const OccurrencesFactory&);
	//! No destruct allowed.
	~OccurrencesFactory(){};
};

#endif

