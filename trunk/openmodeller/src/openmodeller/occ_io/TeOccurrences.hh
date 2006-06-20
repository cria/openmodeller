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
 * Copyright � 2006 INPE
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

#ifndef _TE_OCCURRENCES_FILEHH_
#define _TE_OCCURRENCES_FILEHH_

#include <openmodeller/om_defs.hh>

#include <openmodeller/occ_io/OccurrencesReader.hh>
#include <openmodeller/Occurrences.hh>

class TeUrlParser;
class TeDatabase;

#include <vector>
#include <string>
using std::string;

//! Read occurrences data of one or more group from a TeTable in a TeDataBase.
/**  
 * Table is read and stored as a linked list
 * of objects from the Occurrences class.
 *
 * Allow navigation through the object list.
 *
 * Format:
 *
 * The table must have 4 columns.
 *
 * - Columns 1  : Group name (string).
 * - Column  2  : longitude (-180.0 <= lat  <= 180.0).
 * - Column  3  : latitude: (-90.0 <= long <= 90.0).
 * - Column  4  : Group identifier (string).
 */
class dllexp TeOccurrences : public OccurrencesReader
{
public:
	//! Return a new instance of TeOccurrences.
	/**
	* OccurrencesFactory calls this function to build a new OccurrencesReader.
	*/
	static OccurrencesReader* CreateOccurrencesReaderCallback(const char *url, const char *coord_system );

	//! Read the occurences.
	/**
    * \param url Database url.
	* \param coord_system coord system.
	*/
	TeOccurrences( const char *url, const char *coord_system );
	
	/**
	* Close connection, if its open.
	*/
	~TeOccurrences();

	/** Load occurrences from a TerraLib Database.
     * @param url Url of occurrences.
     */
    int loadOccurrences( const char *url );
	
private:
	//! Pointer to database connection.
	TeDatabase *db_;

	//! TerraLib DataBase Url paser.
	TeUrlParser *te_url_parser_;
};

#endif
