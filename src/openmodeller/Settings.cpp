/**
 * Definition of Settings class.
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

#include <openmodeller/Settings.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Exceptions.hh>

#include <iostream>
#include <string.h>

using namespace std;

/**************************************************************************
 *
 * Implementation of Settings class
 *
 *************************************************************************/

/*******************/
/*** constructor ***/

Settings::Settings() :
  _fp(0)
{
}

/******************/
/*** destructor ***/

Settings::~Settings()
{
  Settings& conf = _getInstance();

  if ( conf._fp != 0 ) {

    delete conf._fp;
  }
}

/**************************/
/*** Singleton accessor ***/
Settings&
Settings::_getInstance()
{
  static Settings theInstance;

  if ( theInstance._fp == 0 ) {

    // Load settings from file in current directory, if name matches CONFIG_FILE compilation constant
    std::ifstream conf_file( CONFIG_FILE, std::ios::in );

    if ( conf_file ) {

      _loadConfig( &theInstance, CONFIG_FILE );
    }
  }

  return theInstance;
}

/*******************/
/*** _loadConfig ***/
void 
Settings::_loadConfig( Settings * conf, const std::string configFile )
{
  if ( conf->_fp != 0 ) {

    delete conf->_fp;
  }

  conf->_fp = new FileParser( configFile );
}

/******************/
/*** loadConfig ***/
void
Settings::loadConfig( const std::string configFile )
{
  Log::instance()->debug( "Loading configuration from: %s\n", configFile.c_str() );

  Settings& conf = _getInstance();

  _loadConfig( &conf, configFile );
}

/***********/
/*** get ***/
std::string 
Settings::get( const std::string & key )
{
  Settings& conf = _getInstance();

  if ( conf._fp == 0 ) {

    throw OmException( "openModeller configuration not loaded" );
  }

  return conf._fp->get( key );
}

/*************/
/*** count ***/
int
Settings::count( const std::string & key )
{
  Settings& conf = _getInstance();

  if ( conf._fp != 0 ) {

    return conf._fp->count( key );
  }

  return 0;
}

/**************/
/*** getAll ***/
std::vector<std::string>
Settings::getAll( const std::string & key )
{
  Settings& conf = _getInstance();

  if ( conf._fp == 0 ) {

    throw OmException( "openModeller configuration not loaded" );
  }

  return conf._fp->getAll( key );
}
