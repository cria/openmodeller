/**
 * Declaration of Settings class.
 * 
 * @author Renato De Giovanni (renato (at) cria . org . br)
 * $Id:  $
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2013 by CRIA -
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

#ifndef _SETTINGS_HH_
#define _SETTINGS_HH_

#include <string>
#include <vector>

#include <openmodeller/os_specific.hh>
#include <openmodeller/FileParser.hh>

/*******************************************************/
/*********************** Settings **********************/

/**
 * Class used to access openModeller settings stored in a key = value configuration file.
 */
class dllexp Settings {

public:

  ~Settings();

  /** Load configuration from file.
   * @param configFile Configuration file (full path)
   */
  static void loadConfig( const std::string configFile );

  /** Get the value of a key (case insensitive).
   * @param key Key to be searched.
   * @return Corresponding Key value.
   */
  static std::string get( const std::string & key );

  /** Return the number of times that a certain key appears in the file.
   * @param key Key to be searched.
   * @return Number of times that the key was found.
   */
  static int count( const std::string & key );

  /** Get a set of values with the same key specified in the argument.
   * @param key Key to be searched.
   * @return Vector of key values.
   */
  static std::vector<std::string> getAll( const std::string & key );

private:

  Settings();

  static Settings& _getInstance();

  static void _loadConfig( Settings * settings, const std::string configFile );

  FileParser * _fp;
};

#endif
