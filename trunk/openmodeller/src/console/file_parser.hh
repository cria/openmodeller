/**
 * Declaration of FileParser class.
 * 
 * @file
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-09-25
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

#ifndef _FILE_PARSERHH_
#define _FILE_PARSERHH_

#include <om_defs.hh>
#include <icstring.hh>

#include <vector>
#include <string>
#include <utility>   // for std::pair

/**
 * Read key/value pairs from a configuration file.
 * 
 */
class dllexp FileParser
{
public:

  FileParser( char const *file );

  ~FileParser();

  int load( char const *file );

  /**
  * Get the value of a key (case insensitive).
  * Do not change the char* that is returned.
  */
  char *get( char const *key ) const;

  /**
  * Return the number of times that a certain key appears in the file.
  */
  int count( char const *key ) const;

  /**
   * The type of values should really be char const **.
   * Do not change the char*'s returned in values because
   * they are really pointers into string::c_str().
   */
  int getAll( char const *key, char **values) const;

  int length() const
  {
    return f_lst.size();
  }
  
private:
  typedef std::pair<icstring,std::string> Item;
  typedef std::vector<Item> ItemList;

  ItemList f_lst;
};

#endif
