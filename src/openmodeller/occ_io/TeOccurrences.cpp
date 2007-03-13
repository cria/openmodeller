/**
 * Definition of TeOccurrences class.
 * 
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

#include <openmodeller/occ_io/TeOccurrences.hh>

#include <openmodeller/TeDatabaseManager.hh>
#include <openmodeller/TeUrlParser.hh>

#include <openmodeller/Occurrences.hh>  // List of occurrences.
#include <openmodeller/Log.hh>

#include <TeDatabase.h>
#include <TeUtils.h>

#include <vector>
using std::vector;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using std::string;

// Win32 defines
#ifdef WIN32
#include <fcntl.h>
#include <io.h>
#define strcasecmp _stricmp

#else
#include <fcntl.h>
#include <unistd.h>
#endif


//! Return a new instance of TeOccurrences.
/**
* OccurrencesFactory calls this function to build a new OccurrencesReader.
*/
OccurrencesReader* 
TeOccurrences::CreateOccurrencesReaderCallback(const char *url, const char *coord_system)
{
	return new TeOccurrences( url, coord_system );
}

/*******************/
/*** Constructor ***/
TeOccurrences::TeOccurrences( const char *url, const char *coord_system ): 
	db_()
{
	_coord_system = (char *) coord_system;
	loadOccurrences( url );
}

/******************/
/*** Destructor ***/
TeOccurrences::~TeOccurrences()
{
	/*if( db_ )
	{
		db_->close();
		delete db_;
	}*/
}

/**********************/
/*** add Ocurrences ***/
int
TeOccurrences::loadOccurrences( const char *url )
{
	te_url_parser_ = new TeUrlParser();
	te_url_parser_->url_ = url;
	// Parser the Url.
	if( !te_url_parser_->parser() )
	{
		Log::instance()->error( 1, "TeOccurrences::addOccurrences - Invalid database url" );
		return 1;
	}

	// Connect to the database
	db_ = TeDatabaseManager::instance().create( *te_url_parser_ );

	if ( !db_->isConnected() )
	{
		Log::instance()->info("TeOccurrences::addOccurrences - Cannot connect to the database." );
		Log::instance()->error( 1, db_->errorMessage().c_str() );
		//delete db_;
		return 1;
	}

	// Get the layer
	if (!db_->layerExist( te_url_parser_->layerName_ ))
	{
		Log::instance()->error( 1, "TeOccurrences::addOccurrences - Cannot open the layer." );
		//delete db_;
		return 1;
	}
	TeLayer* layer = new TeLayer(te_url_parser_->layerName_, db_);

	// Check Species Table
	TeTable speciesTable;
	// Get the first table in layer, if species table is not specified.
	if(te_url_parser_->tableName_.length() == 0)
	{
		TeAttrTableVector attr;
		layer->getAttrTables(attr);
		speciesTable = attr[0];
	}
	else
	{
		// Get species table by name.
		if (!layer->getAttrTablesByName(te_url_parser_->tableName_, speciesTable))
		{
			Log::instance()->error( 1, "TeOccurrences::addOccurrences - Cannot open the species table." );
			//delete db_;
			return 1;
		}
	}

	// Get TablePoints
	string tablePoints = layer->tableName(TePOINTS);

	// If column name is not specified, default column name is "Species".
	if( te_url_parser_->columnName_.length() == 0 )
		te_url_parser_->columnName_ = "Species";

	// Building the sql statement
	string object_id = speciesTable.linkName();

	string sql = "select " + tablePoints + ".x, " + tablePoints + ".y, " + te_url_parser_->tableName_ + "." + te_url_parser_->columnName_ ;
	sql += " from " + te_url_parser_->tableName_ + " inner join " + tablePoints + " on " + te_url_parser_->tableName_ + "." + object_id;
	sql+= " = " + tablePoints +".object_id";

	// Executing the select statement
	TeDatabasePortal* portal = db_->getPortal();
	if (!portal || !portal->query(sql))
	{
		Log::instance()->error( 1, "TeOccurrences::addOccurrences - Cannot execut the select statement." );
		delete portal;
		portal = 0;
		return 1;
	}

	// Fixme: read this from file.
	Scalar error     	= -1.0;
	Scalar abundance 	= 1.0;
	int num_attributes  = 0;
	Scalar *attributes  = 0;
	string sp;
	int sequence = 0;

	// Get the occurrences.
	while (portal->fetchRow())
	{
		++sequence;
		sp = portal->getData(2);

		Coord lg = Coord( portal->getDouble(0) );
		Coord lt = Coord( portal->getDouble(1) );
		addOccurrence( Te2String(sequence).c_str(), sp.c_str(), lg, lt, error, abundance, num_attributes, attributes );
	}

	delete portal;
	portal = 0;
	delete te_url_parser_;
	
	return 0;
}
