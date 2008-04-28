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
#include <openmodeller/TeStringParser.hh>

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


/** Return a new instance of TeOccurrences.
* OccurrencesFactory calls this function to build a new OccurrencesReader.
*/
OccurrencesReader* 
TeOccurrences::CreateOccurrencesReaderCallback( const char * source, const char * coordSystem )
{
  return new TeOccurrences( source, coordSystem );
}

/*******************/
/*** Constructor ***/
TeOccurrences::TeOccurrences( const char * source, const char * coordSystem ): 
  _db()
{
  _source = (char *) source; // Terralib string

  _coord_system = (char *) coordSystem;

  _loaded = false;
}

/******************/
/*** Destructor ***/
TeOccurrences::~TeOccurrences()
{
  /*if( _db )
  {
    _db->close();
    delete _db;
  }*/
}

/**********************/
/*** add Ocurrences ***/
bool
TeOccurrences::load()
{
  if ( _loaded ) {

    return true;
  }

  _te_str_parser = new TeStringParser();
  _te_str_parser->str_ = _source;

  // Parser the string.
  if ( !_te_str_parser->parse() ) {

    Log::instance()->error( "TeOccurrences::load - Invalid TerraLib string" );
    return false;
  }

  // Connect to the database
  _db = TeDatabaseManager::instance().create( *_te_str_parser );

  if ( !_db->isConnected() ) {

    Log::instance()->error( "TeOccurrences::load - Cannot connect to database: %s.", _db->errorMessage().c_str() );
    //delete _db;
    return false;
  }

  // Get the layer
  if ( !_db->layerExist( _te_str_parser->layerName_ ) ) {

    Log::instance()->error( "TeOccurrences::load - Cannot open layer." );
    //delete _db;
    return false;
  }
  TeLayer* layer = new TeLayer(_te_str_parser->layerName_, _db);

  // Check Species Table
  TeTable speciesTable;
  // Get the first table in layer, if species table is not specified.
  if ( _te_str_parser->tableName_.length() == 0 ) {

    TeAttrTableVector attr;
    layer->getAttrTables(attr);
    speciesTable = attr[0];
  }
  else {

    // Get species table by name.
    if ( !layer->getAttrTablesByName(_te_str_parser->tableName_, speciesTable) ) {

      Log::instance()->error( "TeOccurrences::load - Cannot open species table." );
      //delete _db;
      return false;
    }
  }

  // Get TablePoints
  string tablePoints = layer->tableName(TePOINTS);

  // If column name is not specified, default column name is "Species".
  if ( _te_str_parser->columnName_.length() == 0 ) {

    _te_str_parser->columnName_ = "Species";
  }

  // Building the sql statement
  string object_id = speciesTable.linkName();

  string sql = "select " + tablePoints + ".x, " + tablePoints + ".y, " + _te_str_parser->tableName_ + "." + _te_str_parser->columnName_ ;
  sql += " from " + _te_str_parser->tableName_ + " inner join " + tablePoints + " on " + _te_str_parser->tableName_ + "." + object_id;
  sql+= " = " + tablePoints +".object_id";

  // Executing the select statement
  TeDatabasePortal* portal = _db->getPortal();
  if ( !portal || !portal->query(sql) ) {

    Log::instance()->error( "TeOccurrences::load - Cannot execute SQL statement." );
    delete portal;
    portal = 0;
    return false;
  }

  // Fixme: read this from file.
  Scalar error       = -1.0;
  Scalar abundance   = 1.0;
  int num_attributes = 0;
  Scalar *attributes = 0;
  string sp;
  int sequence = 0;

  // Get the occurrences.
  while ( portal->fetchRow() ) {

    ++sequence;
    sp = portal->getData(2);

    Coord lg = Coord( portal->getDouble(0) );
    Coord lt = Coord( portal->getDouble(1) );

    _addOccurrence( Te2String(sequence).c_str(), sp.c_str(), lg, lt, error, abundance, num_attributes, attributes );
  }

  delete portal;
  portal = 0;
  delete _te_str_parser;

  _loaded = true;

  return true;
}
