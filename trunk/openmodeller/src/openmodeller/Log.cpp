/**
 * Definition of Log class.
 * 
 * @file
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

#include <om_log.hh>

#include <stdarg.h>
#include <string.h>


// WIN32 function defines
#ifdef WIN32
#define snprintf		_snprintf
#define vsnprintf		_vsnprintf
#endif


/**************************************************************/
/***************************** Log ****************************/

/*******************/
/*** constructor ***/

Log::Log( char *name, char *pref, int overwrite )
{
  f_level = Warn;

  f_pref = 0;
  setPrefix( pref );

  const char *str_open = overwrite ? "w+" : "a";

  if ( (f_log = fopen( name, str_open )) == NULL )
    error( 1, "Log: could not create or open %s :(\n", name );
}

Log::Log( Log::Level level, FILE *log, char *pref )
{
  f_level = level;

  f_pref = 0;
  setPrefix( pref );

  f_log = log;
}


/******************/
/*** destructor ***/

Log::~Log()
{
  fclose( f_log );
}


/******************/
/*** set Prefix ***/
void
Log::setPrefix( char *pref )
{
  if ( f_pref )
    delete f_pref;

  // Without prefix.
  if ( ! pref )
    pref = "";

  f_pref = new char[ 1 + strlen(pref) ];
  strcpy( f_pref, pref );
}


/*************/
/*** debug ***/
int
Log::debug( char *format, ... )
{
  if ( f_level > Debug )
    return 0;

  va_list ap;
  va_start( ap, format );
  fprintf( f_log, "Debug:%s ", f_pref );
  vfprintf( f_log, format, ap );
  fflush( f_log );
  va_end( ap );

  return 1;
}


/************/
/*** info ***/
int
Log::info( char *format, ... )
{
  if (  f_level > Info )
    return 0;

  va_list ap;
  va_start( ap, format );
  fprintf( f_log, "Info:%s ", f_pref );
  vfprintf( f_log, format, ap );
  fflush( f_log );
  va_end( ap );

  return 1;
}


/************/
/*** warn ***/
int
Log::warn( char *format, ... )
{
  if (  f_level > Warn )
    return 0;

  const int buf_size = 1024;
  char buf[buf_size];

  // Print in 'buf'.
  //
  va_list ap;
  va_start( ap, format );

  // Header.
  snprintf( buf, buf_size, "Warn:%s ", f_pref );

  // Print message after header.
  int len = strlen( buf );
  char *end = buf + len;
  vsnprintf( end, buf_size - len, format, ap );
  va_end( ap );


  // Print 'buf' in standard error output.
  //  fprintf( stderr, "%s", buf );


  // Print 'buf' in log stream.
  if ( f_level <= Warn )
    fprintf( f_log, "%s", buf );


  return 1;
}


/*************/
/*** error ***/
int
Log::error( int exit_code, char *format, ... )
{
  const int buf_size = 1024;
  char buf[buf_size];

  // Print in 'buf'.
  //
  va_list ap;
  va_start( ap, format );

  // Header.
  snprintf( buf, buf_size, "Error:%s ", f_pref );

  // Print message after header.
  int len = strlen( buf );
  char *end = buf + len;
  vsnprintf( end, buf_size - len, format, ap );
  va_end( ap );


  // Print 'buf' in standard error output.
  //  fprintf( stderr, "%s", buf );


  // Print 'buf' in log stream.
  if ( f_level <= Warn )
    fprintf( f_log, "%s", buf );


  ::exit( exit_code );
}


/**************/
/*** buffer ***/
int
Log::buffer( void *buf, int buf_size, int length )
{
  if ( f_level > Debug )
    return 0;
  
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
      for ( int c = 0; c < length; c++ )
        {
          if ( p >= e ) break;
          debug( "%02x ", *p++ );
        }

      // Bytes shown as characters.
      p = aux;
      debug( " " );
      for ( int c = 0; c < length; c++, *p++ )
        {
          if ( p >= e ) break;
          debug( "%c", (*p < 32 ? '.' : *p) );
        }

      debug( "\n" );
      
    } while ( p < e );

  return 0;
}


