/**
 * Definition of Configuration class
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

#include <openmodeller/Configuration.hh>

#include <iostream>
#include <functional>
#include <algorithm>
#include <sstream>
#include <iterator>

#include <openmodeller/Sample.hh>
#include <openmodeller/Exceptions.hh>

using namespace std;

/***********************************************************************************
 *
 * Helper code for this module
 *
 **********************************************************************************/

// Functor to test attribute name
class AttributeNameTest {
public:
  AttributeNameTest( const string& name ) :
    name( name )
  {}
  bool operator()( const Configuration::attribute & a ) {
    std::string cmp_name = a.first;
    size_t prefix_pos = cmp_name.find(":");
    if ( prefix_pos != string::npos ) {
      // if there is a namespace prefix, remove it
      cmp_name = cmp_name.substr( prefix_pos+1 );
    }
    return name == cmp_name;
  }
  string name;
};

// Functor to test a configuration's name.
class ConfigurationNameTest {
public:
  ConfigurationNameTest( const string& name ) :
    name( name )
  {}
  bool operator()( const ConfigurationPtr &config ) {
    return name == config->getName();
  }
  string name;
};
      
// Trim characters from front and back of a string.
static void trim( string& str, char const* delims = "\t\r\n ") {
  
  string::size_type index = str.find_last_not_of(delims);
  if( index != string::npos )
    str.erase(++index);

  index = str.find_first_not_of( delims );
  if( index != string::npos )
    str.erase(0,index);
  else
    str.erase();
}

// Method to get integer from string
int ConfigurationImpl::getInt( const string& str, int defaultValue ) {
  int returnValue = defaultValue;
  sscanf( str.c_str(), "%d", &returnValue);
  return returnValue;
}

// Method to get double from string
double ConfigurationImpl::getDouble( const string& str, double defaultValue ) {
  double returnValue = defaultValue;
  sscanf( str.c_str(), "%lf", &returnValue );
  return returnValue;
}

// Method to get Sample from string
Sample ConfigurationImpl::getSample( const string& str ) {
  stringstream ss( str, ios::in );
  Sample s;
  ss >> s;
  return s;
}

/***********************************************************************************
 *
 * ConfigurationImpl methods.
 *
 **********************************************************************************/
ConfigurationImpl::ConfigurationImpl() :
  ReferenceCountedObject(),
  name(),
  value(),
  subsections(),
  attributes()
{}

ConfigurationImpl::ConfigurationImpl( char const * name ) :
  ReferenceCountedObject(),
  name( name ),
  value(),
  subsections(),
  attributes()
{}

ConfigurationImpl::~ConfigurationImpl()
{}

string
ConfigurationImpl::getName() const {

  size_t prefix_pos = name.find(":");

  if ( prefix_pos != string::npos ) {

    // if there is a namespace prefix, remove it
    return name.substr( prefix_pos+1 );
  }

  return name;
}

void
ConfigurationImpl::setValue( const string& val )
{
  value = val;
  trim( value );
}

string
ConfigurationImpl::getValue() const {
  return this->value;
}

ConstConfigurationPtr
ConfigurationImpl::getSubsection( const string & name, bool throws ) const
{
  ConfigurationNameTest tester(name);
  Configuration::subsection_list::const_iterator c = find_if( subsections.begin(), subsections.end(), tester );

  // Subsection found.
  if ( c != subsections.end() ) {

    return ConstConfigurationPtr( *c );
  }

  // Subsection not found
  if ( throws ) {
 
   throw SubsectionNotFound( name );
  }

  return ConstConfigurationPtr();
}

ConfigurationPtr
ConfigurationImpl::getSubsection( const string & name, bool throws )
{
  ConfigurationNameTest tester(name);
  Configuration::subsection_list::iterator c = find_if( subsections.begin(), subsections.end(),	tester );

  // Subsection found.
  if ( c != subsections.end() ) {

    return ConstConfigurationPtr( *c );
  }

  // Subsection not found
  if ( throws ) {
    throw SubsectionNotFound( name );
  }

  return ConstConfigurationPtr();
}

void
ConfigurationImpl::addSubsection( const ConfigurationPtr & config )
{
  subsections.push_back( config );
}

string
ConfigurationImpl::getAttribute( const string & name ) const
{
  AttributeNameTest tester( name );
  Configuration::attribute_list::const_iterator nv = find_if( attributes.begin(), attributes.end(), tester );

  if ( nv == attributes.end() ) {

    throw AttributeNotFound( name );
  }

  return nv->second;
}

string
ConfigurationImpl::getAttribute( const string & name, const string& defaultValue ) const
{
  AttributeNameTest tester( name );
  Configuration::attribute_list::const_iterator nv = find_if( attributes.begin(), attributes.end(), tester );

  if ( nv == attributes.end() ) {

    return defaultValue;
  }

  return nv->second;
}

template<typename T>
vector<T>
ConfigurationImpl::getAttributeAsVec( const string & name ) const {

  string val = getAttribute( name );
  
  stringstream ss( val, ios::in );

  vector<T> v;

  istream_iterator<T> end;
  for ( istream_iterator<T> is_it(ss); is_it != end; is_it ++ ) {

    v.push_back( *is_it );
  }

  return v;
}

int
ConfigurationImpl::getAttributeAsInt( const string & name, int defaultvalue ) const {

  string val = getAttribute( name, "" );

  int returnValue = defaultvalue;

  sscanf( val.c_str(), "%d", &returnValue );

  return returnValue;
}

double
ConfigurationImpl::getAttributeAsDouble( const string & name, double defaultvalue ) const {
  string val = getAttribute( name, "" );

  double returnValue = defaultvalue;

  sscanf( val.c_str(), "%lf", &returnValue );
  
  return returnValue;
}

vector<double>
ConfigurationImpl::getAttributeAsVecDouble( const string & name ) const {

  string val = getAttribute( name );
  
  stringstream ss( val, ios::in );

  vector<double> v;

  istream_iterator<double> end;
  for ( istream_iterator<double> is_it(ss);
	is_it != end;
	is_it ++ ) {

    v.push_back( *is_it );
  }

  return v;
}

void
ConfigurationImpl::getAttributeAsDoubleArray( const string & name, double **arry, int *dim ) const {

  vector<double> vec = getAttributeAsVecDouble( name );

  if ( dim ) {

    *dim = vec.size();
  }

  if ( arry ) {

    int end = vec.size();

    *arry = new double[ end ];

    for (int i=0; i<end; i++ ) {
      (*arry)[i] = vec[i];
    }
  }
}

vector<int>
ConfigurationImpl::getAttributeAsVecInt( const string & name ) const {

  string val = getAttribute( name );
  
  stringstream ss( val, ios::in );

  vector<int> v;

  istream_iterator<int> end;
  for ( istream_iterator<int> is_it(ss); is_it != end; is_it ++ ) {

    v.push_back( *is_it );
  }

  return v;
}

void
ConfigurationImpl::getAttributeAsIntArray( const string & name, int **arry, int *dim ) const {

  vector<int> vec = getAttributeAsVecInt( name );

  if ( dim ) {

    *dim = vec.size();
  }

  if ( arry ) {

    int end = vec.size();

    *arry = new int[ end ];

    for ( int i=0; i<end; i++ ) {

      (*arry)[i] = vec[i];
    }
  }
}

Sample
ConfigurationImpl::getAttributeAsSample( const string & name ) const {

  string v = getAttribute( name );

  stringstream is(v, ios::in );

  Sample sample;

  is >> sample;

  return sample;
}

void
ConfigurationImpl::getAttributeAsByteArray( const string & name, unsigned char **arry, int *dim ) const {

  vector<int> vec = getAttributeAsVec<int>( name );

  if ( dim ) {

    *dim = vec.size();
  }

  if ( arry ) {

    int end = vec.size();

    *arry = new unsigned char[ end ];

    for ( int i=0; i<end; i++ ) {

      (*arry)[i] = (unsigned char) (vec[i] & 0x00FF) ;
    }
  }
}

void
ConfigurationImpl::addNameValue( const string & name, const string & value ) {

  string theval = value;
  trim(theval);

  attributes.push_back( make_pair( name, theval ) );
}

void
ConfigurationImpl::addNameValue( const string & name, char const *value ) {
  
  string sval( (value)? value: "");

  addNameValue( name, sval );
}

void
ConfigurationImpl::addNameValue( const string & name, int value ) {

  stringstream ss(ios::out);

  ss << value;

  addNameValue( name, ss.str() );
}

void
ConfigurationImpl::addNameValue( const string & name, double value, int precision ) {

  stringstream ss(ios::out);
  ss.precision(precision);

  ss << value;

  addNameValue( name, ss.str() );
}

void
ConfigurationImpl::addNameValue( const string & name, double const *values, int count, int precision ) {

  stringstream ss(ios::out);
  ss.precision(precision);

  for ( int i=0; i<count; i++ ) {
    ss << *values++ << " ";
  }

  addNameValue( name, ss.str() );
}

void
ConfigurationImpl::addNameValue( const string & name, int const *values, int count ) {

  stringstream ss(ios::out);

  for ( int i=0; i<count; i++ ) {
    ss << *values++ << " ";
  }

  addNameValue( name, ss.str() );
}

void
ConfigurationImpl::addNameValue( const string & name, const Sample& value ) {

  stringstream ss(ios::out);

  ss << value;

  addNameValue( name, ss.str() );
}

void
ConfigurationImpl::addNameValue( const string & name, unsigned char const *values, int count ) {

  stringstream ss(ios::out);

  for ( int i=0; i<count; i++ ) {

    ss << (int)*values++ << " ";
  }

  addNameValue( name, ss.str() );
}

