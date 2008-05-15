/**
 * Definition of SerializedXmlOccurrences class.
 * 
 * @author Renato De Giovanni (renato [at] cria . org . br)
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2008 by CRIA -
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

#include <openmodeller/occ_io/SerializedXmlOccurrences.hh>

#include <openmodeller/Occurrences.hh>
#include <openmodeller/Configuration.hh>

#include <string.h>
#include <fstream>

/*****************************************/
/*** create OccurrencesReader callback ***/
OccurrencesReader * 
SerializedXmlOccurrences::CreateOccurrencesReaderCallback( const char *source, const char *coordSystem )
{
  return new SerializedXmlOccurrences( source, coordSystem );
}


/*******************/
/*** Constructor ***/
SerializedXmlOccurrences::SerializedXmlOccurrences( const char *source, const char *coordSystem )
{
  _source = (char *) source;

  _coord_system = (char *) coordSystem;

  _loaded = false;
}


/******************/
/*** Destructor ***/
SerializedXmlOccurrences::~SerializedXmlOccurrences()
{
}


/***********************/
/*** load Ocurrences ***/
bool
SerializedXmlOccurrences::load()
{
  if ( _loaded ) {

    return true;
  }

  // Try to open file and check if first char is "<". This is the simplest
  // way to avoid parsing and then having to handle exceptions for files
  // that are not om serialized XML.
  // note: in most cases, om serialized XML doesn't contain the XML header
  // and starts with a blank line.
  std::ifstream fs( _source, std::ios_base::in );

  if ( fs.fail() ) {

    return false;
  }

  std::string line("");
  bool not_found = true;

  while ( not_found && getline( fs, line ) ) {

    for ( unsigned int i = 0; i < line.size(); ++i ) {    

      // Skip carriage returns, line feeds and spaces
      if ( line[i] == '\r' || line[i] == '\n' || line[i] == ' ' ) {

        continue;
      }

      // It may be an XML file
      if ( line[i] == '<' ) {

        not_found = false;
        break;
      }
      // Definitely not an XML file
      else {

        fs.close();
        return false;
      }
    }
  }

  fs.close();

  Log::instance()->info( "Loading occurrences from file %s.\n", _source );

  ConfigurationPtr parent_conf = Configuration::readXml( _source );

  if ( ConfigurationPtr sampler_conf = parent_conf->getSubsection( "Sampler", false ) ) {

    parent_conf = sampler_conf;
  }

  if ( ConstConfigurationPtr presence_conf = parent_conf->getSubsection( "Presence", false ) ) {

    OccurrencesPtr presences( new OccurrencesImpl( 1.0 ) );
    presences->setConfiguration( presence_conf );

    _presences.push_back( presences );

    _loaded = true;
  }

  if ( ConstConfigurationPtr absence_conf = parent_conf->getSubsection( "Absence", false ) ) {

    OccurrencesPtr absences = new OccurrencesImpl( 0.0 );
    absences->setConfiguration( absence_conf );

    _absences.push_back( absences );

    _loaded = true;
  }

  return _loaded;
}
