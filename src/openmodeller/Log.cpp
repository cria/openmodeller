/**
 * Definition of Log class.
 * 
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date   2003-03-28
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

#include <openmodeller/Log.hh>

#include <stdarg.h>

#include <openmodeller/Exceptions.hh>

using std::ostream;
using std::endl;
using std::string;

// WIN32 function defines
#ifdef WIN32
#define snprintf		_snprintf
#define vsnprintf		_vsnprintf
#endif

// A default logger object.
Log g_log( Log::Default );

/* This callback class is a simple interface to the old FILE*
 * mechanism from standard C.
 */
class StdFileLogCallback : public Log::LogCallback {
public:
  StdFileLogCallback( FILE* file ) :
    file( file )
  {}
  ~StdFileLogCallback() {
  }
  void operator()( Log::Level level, const std::string& msg ) {
   if ( file != NULL ) {
     fprintf( file, "%s", msg.c_str() );
   }
  }
  FILE *file;
};

/****************************************************************/
/******************** OstreamCallback ***************************/

Log::OstreamCallback::OstreamCallback( ostream& os ) :
  os( os )
{}

void
Log::OstreamCallback::operator()(Level level, const string& msg ) {
  os << msg;
}

/****************************************************************/
/******************** output function ***************************/
const char *
LevelLabels[5] =
  {
    "[Debug] ",
    "", 
    "[Info] ",
    "[Warn] ",
    "[Error] "
  };

static void
FormatAndWrite( Log::LogCallback& lc, Log::Level level, std::string pref, const char* format, va_list ap ) {

  const int buf_size = 1024;
  char buf[buf_size];

  // Print in 'buf'.
  //
  // Header.
  snprintf( buf, buf_size, "%s%s", LevelLabels[level], pref.c_str() );

  // Print message after header.
  int len = strlen( buf );
  char *end = buf + len;
  vsnprintf( end, buf_size - len, format, ap );

  lc( level, buf );

}

/****************************************************************/
/****************************** Log *****************************/

/*******************/
/*** constructor ***/

Log::Log( Log::Level level, const char *pref ) :
  callback( new Log::OstreamCallback( std::cerr ) ),
  _level( level )
{
  setPrefix( pref );
  _deleteCallback = true;
}


/******************/
/*** destructor ***/

Log::~Log()
{
  if ( callback && _deleteCallback ) {

    delete callback;
  }
}

/******************/
/*** set all parameters ***/
void
Log::set( Log::Level level, FILE* out, char const *pref )
{
  setLevel( level );
  setCallback( new StdFileLogCallback( out ) );
  setPrefix( pref );
  _deleteCallback = true;
}

/******************/
/*** set Callback ***/
void
Log::setCallback( LogCallback *lc )
{
  if ( callback && _deleteCallback ) {

    delete callback;
    _deleteCallback = false;
  }

  callback = lc;
}


/******************/
/*** set Prefix ***/
void
Log::setPrefix( const char *pref )
{
  _pref.assign( pref );

  // If prefix is non-empty, we need a trailing space
  if ( _pref.size() > 0 ) {

    _pref.append(" ");
  }
}


/*************/
/*** debug ***/
void
Log::debug( const char *format, ... )
{
  if ( _level > Debug || !callback )
    return;

  va_list ap;
  va_start( ap, format );
  FormatAndWrite( *callback, Debug, _pref, format, ap );
  va_end( ap );

  return;
}


/************/
/*** info ***/
void
Log::info( const char *format, ... )
{
  if ( _level > Info || !callback )
    return;

  va_list ap;
  va_start( ap, format );
  FormatAndWrite( *callback, Info, _pref, format, ap );
  va_end( ap );

  return;
}


/*******************/
/*** operator () ***/
void
Log::operator()( const char *format, ... )
{
  if ( _level > Default || !callback )
    return;

  va_list ap;
  va_start( ap, format );
  FormatAndWrite( *callback, Default, _pref, format, ap );
  va_end( ap );

  return;
}


/************/
/*** warn ***/
void
Log::warn( const char *format, ... )
{
  if ( _level > Warn || !callback )
    return;

  va_list ap;
  va_start( ap, format );
  FormatAndWrite( *callback, Warn, _pref, format, ap );
  va_end( ap );

  return;
}


/*************/
/*** error ***/
void
Log::error( int exit_code, const char *format, ... )
{
  if ( _level > Error || !callback )
    return;

  va_list ap;
  va_start( ap, format );
  FormatAndWrite( *callback, Error, _pref, format, ap );
  va_end( ap );

  const int buf_size = 1024;
  char buf[buf_size];

  // Print in 'buf'.
  //
  va_start( ap, format );
  vsnprintf( buf, buf_size, format, ap );
  va_end( ap );

  throw OmException( buf );
}

