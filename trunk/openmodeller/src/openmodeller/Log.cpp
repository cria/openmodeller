/**
 * Definition of Log class.
 * 
 * @file
 * @author Mauro E S Mu�oz <mauro@cria.org.br>
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

#include <om_log.hh>

#include <stdarg.h>
#include <string.h>


// WIN32 function defines
#ifdef WIN32
#define snprintf		_snprintf
#define vsnprintf		_vsnprintf
#endif


// A default logger object.
Log g_log( Log::Info , stderr );


/****************************************************************/
/****************************** Log *****************************/

/*******************/
/*** constructor ***/

Log::Log( char *name, char *pref, int overwrite )
{
  _level = Warn;

  _pref = 0;
  setPrefix( pref );

  const char *str_open = overwrite ? "w+" : "a";

  if ( (_log = fopen( name, str_open )) == NULL )
    {
      fprintf( stderr, "Log: could not create or open %s :(\n",
	       name );
      exit( 1 );
    }

  // Closes '_log' in destructor.
  _close = 1;
}

/*******************/
/*** constructor ***/

Log::Log( Log::Level level, FILE *out, char *pref )
{
  _pref = 0;
  set( level, out, pref );
}


/******************/
/*** destructor ***/

Log::~Log()
{
  if ( _close )
    fclose( _log );
  if ( _pref )
    delete [] _pref;
}


/***********/
/*** set ***/
void
Log::set( Log::Level level, FILE *out, char *pref )
{
  setLevel( level );
  setOutput( out );
  setPrefix( pref );
}


/******************/
/*** set Output ***/
void
Log::setOutput( FILE *out )
{
  _log   = out;
  _close = 0;   // Does not close '_log' in destructor.
}


/******************/
/*** set Prefix ***/
void
Log::setPrefix( char *pref )
{
 if ( _pref )
    delete [] _pref;

  // Without prefix.
  if ( ! pref )
    pref = "[]";

  _pref = new char[ 1 + strlen(pref) ];
  strcpy( _pref, pref );
}


/*************/
/*** debug ***/
int
Log::debug( char *format, ... )
{
  if ( _level > Debug )
    return 0;

  va_list ap;
  va_start( ap, format );
  fprintf( _log, "[Debug] %s ", _pref );
  vfprintf( _log, format, ap );
  fflush( _log );
  va_end( ap );

  return 1;
}


/************/
/*** info ***/
int
Log::info( char *format, ... )
{
  if ( _level > Info )
    return 0;

  va_list ap;
  va_start( ap, format );
  fprintf( _log, "[Info] %s ", _pref );
  vfprintf( _log, format, ap );
  fflush( _log );
  va_end( ap );

  return 1;
}


/*******************/
/*** operator () ***/
int
Log::operator()( char *format, ... )
{
  if ( _level > Info )
    return 0;

  va_list ap;
  va_start( ap, format );
  vfprintf( _log, format, ap );
  fflush( _log );
  va_end( ap );

  return 1;
}


/************/
/*** warn ***/
int
Log::warn( char *format, ... )
{
  if ( _level > Warn )
    return 0;

  const int buf_size = 1024;
  char buf[buf_size];

  // Print in 'buf'.
  //
  va_list ap;
  va_start( ap, format );

  // Header.
  snprintf( buf, buf_size, "[Warn] %s ", _pref );

  // Print message after header.
  int len = strlen( buf );
  char *end = buf + len;
  vsnprintf( end, buf_size - len, format, ap );
  va_end( ap );

  /*
  // Print 'buf' in standard error output.
  if ( _log != stderr )
    fprintf( stderr, "%s", buf );
  */

  // Print 'buf' in log stream.
  fprintf( _log, "%s", buf );


  return 1;
}


/*************/
/*** error ***/
int
Log::error( int exit_code, char *format, ... )
{
  if ( _level > Error )
    return 0;

  const int buf_size = 1024;
  char buf[buf_size];

  // Print in 'buf'.
  //
  va_list ap;
  va_start( ap, format );

  // Header.
  snprintf( buf, buf_size, "[Error] %s ", _pref );

  // Print message after header.
  int len = strlen( buf );
  char *end = buf + len;
  vsnprintf( end, buf_size - len, format, ap );
  va_end( ap );

  /*
  // Print 'buf' in standard error output.
  if ( _log != stderr )
    fprintf( stderr, "%s", buf );
  */

  // Print 'buf' in log stream.
  fprintf( _log, "%s", buf );


  ::exit( exit_code );
}


/**************/
/*** buffer ***/
int
Log::buffer( void *buf, int buf_size, int length )
{
  if ( _level > Debug )
    return 0;
 
  int c;

  debug( "Log::buffer( %p, %d )\n", buf, buf_size );

  unsigned char *p = (unsigned char *) buf;
  unsigned char *e = p + buf_size;
  unsigned char *aux;

  do
    {
      // Hold beginning of line.
      aux = p;

      // Bytes shown as hexadecimal.
      debug( "%p: ", p );
      for ( c = 0; c < length; c++ )
        {
          if ( p >= e ) break;
          debug( "%02x ", *p++ );
        }

      // Bytes shown as characters.
      p = aux;
      debug( " " );
      for ( c = 0; c < length; c++, *p++ )
        {
          if ( p >= e ) break;
          debug( "%c", (*p < 32 ? '.' : *p) );
        }

      debug( "\n" );
      
    } while ( p < e );

  return 0;
}


