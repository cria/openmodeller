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

#include <openmodeller/TeUrlParser.hh>

//! Parser a url.
/**
* Complete url is like that:
* terralib>user>password\@DatabaseName>host>DatabaseFile>port>layerName>tableName>columnName
*/
bool
TeUrlParser::parser()
{
	string input = url_;
	int i;

	// Throw out "terralib:" ************************************************************
	i = input.find( "terralib>" );
	if( i == -1)
		return false;
	input = input.substr(9, input.length());
	
	// This is not a Url, problabe it's a disk file name.
	i = input.find( "@" );
	if( i == -1)
	{		
		url_ = input;
		return false;
	}

	// Get user_ ************************************************************************
	i = input.find( ">" );
	// This is not a valid Url. 
	if( i == -1)
		return false;
	user_ = input.substr(0, i);
	input = input.substr(user_.length()+1, input.length());
	// Get password_ ************************************************************************
	i = input.find( "@" );
	// This is not a valid Url. 
	if( i == -1)
		return false;
	password_ = input.substr(0, i);
	input = input.substr(password_.length()+1, input.length());
	// Get dbms_name_ ****************************************************************
	i = input.find( ">" );
	if( i == -1)
		return false;
	dbms_name_ = input.substr(0, i);
	input = input.substr(dbms_name_.length()+1, input.length());
	// Get host_ ************************************************************************
	i = input.find( ">" );
	if( i == -1)
		return false;
	host_ = input.substr(0, i);
	input = input.substr(host_.length()+1, input.length());
	// Get dbms_name_File ************************************************************
	i = input.find( ">" );
	if( i == -1)
		return false;
	database_ = input.substr(0, i);
	input = input.substr(database_.length()+1, input.length());
	// Get port_ ************************************************************************
	i = input.find( ">" );
	if( i == -1)
		return false;
	string s_port_ = input.substr(0, i);
	input = input.substr(s_port_.length()+1, input.length());
	port_ = atoi( s_port_.c_str() );
	// Get layerName_ *******************************************************************
	i = input.find( ">" );
	if( i == -1)
	{
		layerName_ = input;
		if(layerName_.length() == 0)
			return false;
		else
			return true;
	}
	else
	{
		layerName_ = input.substr(0, i);
		input = input.substr(layerName_.length()+1, input.length());
	}

	// Get tableName_ *******************************************************************
	i = input.find( ">" );
	if( i != -1)
	{
		tableName_ = input.substr(0, i);
		input = input.substr(tableName_.length()+1, input.length());
		// Get columnName_ ***************************************************************
		columnName_ = input;
	}
	else
		tableName_ = input;

	// This is a vaid Url.
	return true;
}
