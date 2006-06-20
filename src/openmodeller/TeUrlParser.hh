/**
 * Definition of TeUrlParser class.
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

#ifndef  __TE_URL_PASRSER_H
#define  __TE_URL_PASRSER_H

#include <TeDatabaseFactoryParams.h>

#include <string>
using std::string;

//! Parser an url.
/**
* Complete url is like that:
* terralib>user>password\@DatabaseName>host>DatabaseFile>port>layerName>tableName>columnName
*/
class TeUrlParser : public TeDatabaseFactoryParams
{
public:
	TeUrlParser() {}

	TeUrlParser( const string& url ) : url_(url)
	{
		isUrl_ = parser();
	}

	//! Parser the url.
	bool parser();
		
	//! If true is an url, else is a disk file name.
	bool isUrl() { return isUrl_; }

	//! Database url.
	string url_;
	//! Layer in Database.
	string layerName_;
	//! Table Name (Species)
	string tableName_;
	//! Column Name (Species)
	string columnName_;

private:
    bool isUrl_;
};

#endif
