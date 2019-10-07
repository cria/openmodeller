
#include <openmodeller/Configuration.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/Log.hh>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>  // For auto_ptr

using namespace std;

#include <expat.h>

#undef DEBUG_XML_PARSE

/***********************************************************************************
 *
 * Write xml formatted configurations
 *
 **********************************************************************************/

static void 
escapeXml( const char *data, ostream &_stream )
{
  while ( *data != 0 ) {
    switch ( *data ) {
      case '<': _stream << "&lt;"; break;
      case '>': _stream << "&gt;"; break;
      case '&': _stream << "&amp;"; break;
      case '"': _stream << "&quot;"; break;
      default: _stream << *data; break;
    }
    data += 1;
  }
}

static void
writeXmlStartTag( const string &name, Configuration::attribute_list nvlist, bool empty, ostream &_stream ) {

    _stream << endl << "<" << name;

  Configuration::attribute_list::const_iterator nvi = nvlist.begin();
  if ( nvi != nvlist.end() ) {

    while ( nvi != nvlist.end() ) {
      _stream << " " << nvi->first << "=\"";
      escapeXml( nvi->second.c_str(), _stream );
      _stream << "\"";
      ++nvi;
    }
  }

  if ( empty )
    _stream << "/";

  _stream << ">";

}

static void
writeXmlEndTag( const string &name, ostream &_stream ) {

  _stream << "</" << name << ">";

}

void
Configuration::writeXml( const ConstConfigurationPtr& config, ostream &_stream ) {

  Configuration::subsection_list subsects = config->getAllSubsections();

  string value = config->getValue();
  bool empty = subsects.empty() && value.empty();

  writeXmlStartTag( config->getName(), config->getAllAttributes(), empty, _stream );

  if ( empty )
    return;

  if ( ! value.empty() )
    escapeXml( value.c_str(), _stream );

  for ( Configuration::subsection_list::const_iterator subsecti = subsects.begin();
	  subsecti != subsects.end();
	  subsecti++ ) {
    writeXml( *subsecti, _stream );
  }

  writeXmlEndTag( config->getName(), _stream );
}

void
Configuration::writeXml( const ConstConfigurationPtr& config, char const *filename ) {

  ofstream file( filename );

  writeXml( config, file );

  file.close();

}

/***********************************************************************************
 *
 * Read xml formatted configurations
 *
 **********************************************************************************/

class expatState {

public:

  expatState( );
  expatState( expatState* parent );
  ~expatState();

  expatState* finalize( string name );

  void addChars( char const * chars, int len );

  ConfigurationPtr getConfig() {
    return config;
  }

  expatState* makeChild( string name, const char **attribs );

  void setName( string name );
  void setAttributes( const char **attribs );

private:

  string tmpChars;

  expatState *parent;

  ConfigurationPtr config;

};

expatState::expatState( ) :
  tmpChars(),
  parent (),
  config( new ConfigurationImpl() )
{
#if defined(DEBUG_XML_PARSE)
  cout << "expatState::expatState( ): " << this << endl;
#endif
}

expatState::expatState( expatState* parent) :
  tmpChars(),
  parent ( parent ),
  config( new ConfigurationImpl() )
{
#if defined(DEBUG_XML_PARSE)
  cout << "expatState::expatState( expatState* ): " << this << endl;
#endif
}

expatState::~expatState() {
  // Don't do anything!
  // The parser is free'd externally.
  // The Strings take care of themselves
  // The config needs to be controlled externally.
#if defined(DEBUG_XML_PARSE)
  cout << "expatState::~expatState(): " << this << endl;
#endif

}

expatState* expatState::makeChild( string name, const char **attribs ) {

#if defined(DEBUG_XML_PARSE)
  cout << "expatState::makeChild(): " << this << endl;
#endif

  // Create our child
  expatState *child = new expatState(this);

  // Attach it's configuration as our subsection.
  this->config->addSubsection( child->config );

  // Set the child's name:
  // Call the child's StartTag method by hand since expat won't do it for us.
  child->setName( name );
  child->setAttributes( attribs );

  return child;
}

void expatState::setName( string name ) {
#if defined(DEBUG_XML_PARSE)
  cout << "expatState::setName(): " << this << endl;
  cout << "  name: " << name << endl;
#endif
  config->setName( name );
}

void expatState::setAttributes( const char **attribs ) {
#if defined(DEBUG_XML_PARSE)
  cout << "expatState::setAttributes(): " << this << endl;
  cout << "  attribs: " << attribs << endl;
#endif

  const char **name = attribs++;
  const char **value = attribs;

  for( ; *name != NULL ;  ) {
#if defined(DEBUG_XML_PARSE)
    cout << "  name: " << *name << endl;
    cout << "  value: " << *value << endl;
#endif
    config->addNameValue( *name, *value );
    name = value +1;
    value = name +1;
  }

}

expatState* expatState::finalize( string name ) {
#if defined(DEBUG_XML_PARSE)
  cout << "expatState::finalize(): " << this << endl;
#endif

  //    cout << " - condition 2" << endl;
  // Since we don't have characters, that means we are comming out of a subsection.
  config->setValue( tmpChars );

  expatState* returnVal = parent;
  // Since this is the hook for returning from recursion,
  // we need to set the expat user data to this.
  if (parent) {
    delete this;
  }
  return returnVal;
}

void expatState::addChars( const XML_Char *chars, int len ) {
#if defined(DEBUG_XML_PARSE)
  cout << "expatState::addChars(): " << this << endl;
  cout << "  prev = " << tmpChars <<endl;
#endif

  tmpChars.append(  chars, len );
#if defined(DEBUG_XML_PARSE)
  cout << "  new  = " << tmpChars <<endl;
#endif
}

/***********************************************************************************
 *
 * expat handler functions.
 *
 **********************************************************************************/

void endElementHandler( void *, const XML_Char * );
void startElementHandler( void *state, const XML_Char *name, const XML_Char **atts );

void firstStartElementHandler( void * state, const XML_Char *name, const XML_Char **atts ) {
#if defined(DEBUG_XML_PARSE)
  cout << "firstStartElementHandler: " << name << endl;
#endif
  XML_Parser parser = (XML_Parser) state;
  expatState *myState = (expatState*) XML_GetUserData(parser);
  // The first start tag is handled differently from the rest.
  // This one needs to modify the current state object
  // rather than force the recursion.
  myState->setName( name );
  myState->setAttributes( atts );
  XML_SetElementHandler( parser, startElementHandler, endElementHandler );
}

void startElementHandler( void *state, const XML_Char *name, const XML_Char **atts ) {
#if defined(DEBUG_XML_PARSE)
  cout << "startElementHandler: " << name << endl;
#endif
  XML_Parser parser = (XML_Parser) state;
  expatState *myState = (expatState*) XML_GetUserData(parser);
  //
  // At every start tag, we force a recursion.
  //
  expatState *childState = myState->makeChild( name, atts );
  // Direct all expat handlers this the child.
  XML_SetUserData( parser, (void *)childState );

}

void endElementHandler( void * state, const XML_Char *name ) {
#if defined(DEBUG_XML_PARSE)
  cout << "endElementHandler: " << name << endl;
#endif
  XML_Parser parser = (XML_Parser) state;
  expatState *myState = (expatState*) XML_GetUserData(parser);

  expatState* parent = myState->finalize( name );
  XML_SetUserData( parser, (void*)parent );
}

void charHandler( void * state, const XML_Char *name, int len ) {
#if defined(DEBUG_XML_PARSE)
  cout << "charHandler: " << endl;
  cout << "  chars: " << string( name, len ) << endl;
#endif
  XML_Parser parser = (XML_Parser) state;
  expatState *myState = (expatState*) XML_GetUserData(parser);
  myState->addChars( name, len );
}

/***********************************************************************************
 *
 * User routines
 *
 **********************************************************************************/

ConfigurationPtr
Configuration::readXml( char const *filename ) {

  ifstream file( filename, ios::in );
  ConfigurationPtr returnValue( readXml( file ) );
  file.close();
  return returnValue;
}

class ExpatAutoPtr {
public:
  ExpatAutoPtr( XML_Parser p ) :
    p(p)
  {};
  ~ExpatAutoPtr() {
    XML_ParserFree( p );
  }
  operator XML_Parser() {
    return p;
  }
private:
  XML_Parser p;
};

ConfigurationPtr
Configuration::readXml( istream &file ) {

  ExpatAutoPtr parser = XML_ParserCreate( NULL );

  // We need to have access to the raw pointer for the call into XML_SetUserData.
  // it is wrapped in an auto_ptr later for security.
  expatState *state = new expatState();
 
  XML_SetUserData( parser, state );

  // Now, let auto_ptr do it's trick for state.
  unique_ptr<expatState> apstate( state );

  XML_UseParserAsHandlerArg( parser );
  XML_SetElementHandler( parser, firstStartElementHandler, endElementHandler );
  XML_SetCharacterDataHandler( parser, charHandler );

  const int BUFF_SIZE = 1024;

  // expat's parse loop.
  Log::instance()->debug( "XML Parser at start of document\n" );
  for (;;) {
    int bytes_read =0;
    char *buf = (char *)XML_GetBuffer( parser, BUFF_SIZE );
    if ( buf == NULL ) {
      //      cout << " - no buf" << endl;
      std::string msg = "Unable to allocate buffer during XML read";
      Log::instance()->error( msg.c_str() );
      throw XmlParseException( msg.c_str() );
    }

    // iostream::read will read up to the specified number of characters
    // it does not null terminate.
    file.read( buf, BUFF_SIZE);

    // To find the number of characters read, call iostream::gcount().
    bytes_read = file.gcount();

    //    cout << "Bytes read: " << buf << endl;
    //    if ( bytes_read < 0 ) {
    //      cout << " - read" << endl;
    //      goto cleanup;
    //    }

    if ( !XML_ParseBuffer( parser, bytes_read, bytes_read == 0 ) ) {

      XML_Error x =  XML_GetErrorCode( parser );
      stringstream errormsg( ios::out );
      errormsg << XML_ErrorString(x)
	       << " at Line "
	       << XML_GetCurrentLineNumber( parser )
	       << " column "
	       << XML_GetCurrentColumnNumber( parser )
	       << ends;

      Log::instance()->error( "XML Parser fatal error: %s\n", errormsg.str().c_str() );

      throw XmlParseException( errormsg.str() );
    }

    if ( bytes_read == 0 ) {
      Log::instance()->debug( "XML Parser reached end of document\n" );
      break;
    }

  }
  
  return state->getConfig();

}
