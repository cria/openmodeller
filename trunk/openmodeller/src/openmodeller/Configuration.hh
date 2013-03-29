/**
 * Declaration of Configuration class
 * 
 * @author Kevin Ruland
 * @date 2004-11-14
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

#ifndef _CONFIGURATION_HH_
#define _CONFIGURATION_HH_

#include <openmodeller/refcount.hh>

#include <string>
#include <vector>
#include <utility>

class Sample;

class ConfigurationImpl;

typedef ReferenceCountedPointer<ConfigurationImpl> ConfigurationPtr;
typedef ReferenceCountedPointer<const ConfigurationImpl> ConstConfigurationPtr;

struct dllexp Configuration {

  typedef std::vector<ConfigurationPtr> subsection_list;
  typedef std::pair<std::string,std::string> attribute;
  typedef std::vector<attribute> attribute_list;

  static ConfigurationPtr readXml( char const *filename );
  static ConfigurationPtr readXml( std::istream& is );

  static void writeXml( const ConstConfigurationPtr& config, char const *fileaname );
  static void writeXml( const ConstConfigurationPtr& config, std::ostream& os );

};

class dllexp ConfigurationImpl : private ReferenceCountedObject
{
  friend class ReferenceCountedPointer<ConfigurationImpl>;
  friend class ReferenceCountedPointer<const ConfigurationImpl>;

public:
  ConfigurationImpl( char const * name );
  ConfigurationImpl();

public:
  
  // Access to the Name of this Configuarion Section
  std::string getName() const;
  void setName( const std::string& );

  void setValue( const std::string& );
  std::string getValue() const;

  // Get the configuration for the named Subsection
  // If throws is true, will throw SubsectionNotFound.
  // If throws is false, will not throw but return null configuration object.
  ConstConfigurationPtr getSubsection( const std::string& name, bool throws = true ) const;
  ConfigurationPtr getSubsection( const std::string& name, bool throws = true );

  // This call violates const correctness because the elements in
  // the container are mutable.
  const Configuration::subsection_list & getAllSubsections() const;
  
  void addSubsection( const ConfigurationPtr & config );

  // This will throw AttributeNotFound exception
  std::string getAttribute( const std::string & name ) const;

  // If the attribute is not found, this will return defaultValue.
  std::string getAttribute( const std::string & name, const std::string & defaultValue ) const ;

  template< typename T>
  std::vector<T> getAttributeAsVec( const std::string& name ) const;

  int getAttributeAsInt( const std::string & name, int defaultValue ) const;
  double getAttributeAsDouble( const std::string & name, double defaultValue ) const;
  std::vector<double> getAttributeAsVecDouble( const std::string & name ) const;
  void getAttributeAsDoubleArray( const std::string & name, double **, int * ) const;
  std::vector<int> getAttributeAsVecInt( const std::string & name ) const;
  void getAttributeAsIntArray( const std::string & name, int **, int * ) const;
  Sample getAttributeAsSample( const std::string & name ) const;
  void getAttributeAsByteArray( const std::string & name, unsigned char **, int * ) const;
  
  // This call violates const correctness because the elements in
  // the container are mutable.
  const Configuration::attribute_list & getAllAttributes() const;
  
  void addNameValue( const std::string & name, const std::string & value );
  void addNameValue( const std::string & name, char const * value );
  void addNameValue( const std::string & name, int value );
  void addNameValue( const std::string & name, double value, int precision=25 );
  void addNameValue( const std::string & name, const Sample& value );
  void addNameValue( const std::string & name, double const *values, int count, int precision=25 );
  void addNameValue( const std::string & name, int const *values, int count );
  void addNameValue( const std::string & name, unsigned char const *values, int count );
  
  // Method to get int from string
  static int getInt( const std::string& str, int defaultValue );
  // Method to get double from string
  static double getDouble( const std::string& str, double defaultValue );
  // Method to get Sample from string
  static Sample getSample( const std::string& str ); 
private:
  std::string name;
  std::string value;
  Configuration::subsection_list subsections;
  Configuration::attribute_list attributes;

  ~ConfigurationImpl();
  
};


inline void
ConfigurationImpl::setName( const std::string& name ) {
  this->name = name;
}

inline const Configuration::subsection_list &
ConfigurationImpl::getAllSubsections() const {
  return subsections;
}

inline const Configuration::attribute_list &
ConfigurationImpl::getAllAttributes() const {
  return attributes;
}

#endif // _CONFIGURATION_HH_
