#include <configuration.hh>

#include <iostream>
#include <functional>
#include <algorithm>
#include <sstream>
#include <iterator>

#include <Sample.hh>
#include <Exceptions.hh>

using namespace std;

#undef DEBUG_CREATE

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
    return name == a.first;
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
static int getInt( const string& str, int defaultValue ) {
  int returnValue = defaultValue;
  sscanf( str.c_str(), "%d", &returnValue);
  return returnValue;
}

// Method to get double from string
static double getDouble( const string& str, double defaultValue ) {
  double returnValue = defaultValue;
  sscanf( str.c_str(), "%lf", &returnValue );
  return returnValue;
}

// Method to get Sample from string
static Sample getSample( const string& str ) {
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

#if defined(DEBUG_CREATE)
  cout << "In " << getName()
       << " Looking for subsection named: " << name
       << endl;
#endif

  ConfigurationNameTest tester(name);
  Configuration::subsection_list::const_iterator c = find_if( subsections.begin(), subsections.end(),
							      tester );

  //
  // Subsection found.
  //
  if ( c != subsections.end() ) {
#if defined(DEBUG_CREATE)
    cout << "Found: " << endl;
#endif
    return ConstConfigurationPtr( *c );
  }

  //
  // Subsection not found
  //
#if defined(DEBUG_CREATE)
  cout << "Not found" << endl;
#endif
  if ( throws ) {
    throw SubsectionNotFound( name );
  }

  return ConstConfigurationPtr();

}

ConfigurationPtr
ConfigurationImpl::getSubsection( const string & name, bool throws )
{

#if defined(DEBUG_CREATE)
  cout << "In " << getName()
       << " Looking for subsection named: " << name
       << endl;
#endif

  ConfigurationNameTest tester(name);
  Configuration::subsection_list::iterator c = find_if( subsections.begin(), subsections.end(),
							tester );

  //
  // Subsection found.
  //
  if ( c != subsections.end() ) {
#if defined(DEBUG_CREATE)
    cout << "Found: " << endl;
#endif
    return ConstConfigurationPtr( *c );
  }

  //
  // Subsection not found
  //
#if defined(DEBUG_CREATE)
  cout << "Not found" << endl;
#endif
  if ( throws ) {
    throw SubsectionNotFound( name );
  }

  return ConstConfigurationPtr();

}

void
ConfigurationImpl::addSubsection( const ConfigurationPtr & config )
{

#if defined(DEBUG_CREATE)
  cout << "In " << getName()
       << " adding subsection named: " << config->getName()
       << endl;
#endif

  subsections.push_back( config );

}

string
ConfigurationImpl::getAttribute( const string & name ) const
{

#if defined(DEBUG_CREATE)
  cout << "In " << getName()
       << " Looking for attribute named: " << name
       << endl;
#endif
  
  AttributeNameTest tester( name );
  Configuration::attribute_list::const_iterator nv = find_if( attributes.begin(), attributes.end(),
							      tester );

  if ( nv == attributes.end() ) {
#if defined(DEBUG_CREATE)
    cout << "Not found" << endl;
#endif
    throw AttributeNotFound( name );
  }

#if defined(DEBUG_CREATE)
    cout << "Found: " << nv->second << endl;
#endif

  return nv->second;

}

string
ConfigurationImpl::getAttribute( const string & name, const string& defaultValue ) const
{

#if defined(DEBUG_CREATE)
  cout << "In " << getName()
       << " Looking for attribute named: " << name
       << endl;
#endif
  
  AttributeNameTest tester( name );
  Configuration::attribute_list::const_iterator nv = find_if( attributes.begin(), attributes.end(),
							      tester );

  if ( nv == attributes.end() ) {
#if defined(DEBUG_CREATE)
    cout << "Not found" << endl;
#endif
    return defaultValue;
  }

#if defined(DEBUG_CREATE)
    cout << "Found: " << nv->second << endl;
#endif
  return nv->second;

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

template<typename T>
vector<T>
ConfigurationImpl::getAttributeAsVec( const string & name ) const {

  string val = getAttribute( name );
  
  stringstream ss( val, ios::in );

  vector<T> v;

  istream_iterator<T> end;
  for ( istream_iterator<T> is_it(ss);
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

    for (int i=0; i< end; i++ ) {
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

    for (int i=0; i< end; i++ ) {
      (*arry)[i] = (unsigned char) (vec[i] & 0x00FF) ;
    }
  }

}

void
ConfigurationImpl::addNameValue( const string & name, const string & value ) {

#if defined(DEBUG_CREATE)
  cout << "In " << getName()
       << " adding attribute: " << name
       << " value: " << value
       << endl;
#endif

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
ConfigurationImpl::addNameValue( const string & name, double value ) {

  stringstream ss(ios::out);
  ss.precision(25);
  ss << value;

  addNameValue( name, ss.str() );

}

void
ConfigurationImpl::addNameValue( const string & name, double const *values, int count ) {

  stringstream ss(ios::out);

  ss.precision(25);
  for( int i=0; i<count; i++ ) {
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

  for( int i=0; i<count; i++ ) {
    ss << (int)*values++ << " ";
  }

  addNameValue( name, ss.str() );

}

