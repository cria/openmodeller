/**
 * Declaration of Log class.
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

#ifndef _LOGHH_
#define _LOGHH_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


//   Obs: Need to implement compatibility with sockets.


/****************************************************************/
/****************************** Log *****************************/

/** 
 * Class that allows sending of log messages to "stream" devices.
 * A system interface class with more advanced logging 
 * mechanisms (libraries).
 */
class Log
{
public:

  typedef enum {
    Debug, Info, Warn, Error
  } Level;


public:

  /** 
   * @param name Name of the output log file.
   * @param pref Prefix to be shown on every message.
   * @param overwrite If != 0 truncates the file before usage,
   *  otherwise begin from the end.
   */
  Log( char *name, char *pref="", int overwrite=0 );

  /**
   * @param level Log level.
   * @param out Where to output logs. Will not be closed in
   *  destructor.
   * @param pref Prefix to be shown on every message.
   */
  Log( Level level=Warn, FILE *out=stdout, char *pref="" );

  ~Log();


  /** Configure the logger.
   * 
   * @param level Log level.
   * @param out Where to output logs. Will not be closed in
   *  destructor.
   * @param pref Prefix to be shown on every message.
   */
  void set( Level level, FILE *out, char *pref="" );

  /** Change log level.*/
  void setLevel( Level level )  { _level = level; }

  /** Set the log output. Do not closes the file in the
   * destructor.
   * @param out Log output file handle
   */
  void setOutput( FILE *out );

  /** Change prefix to be shown befeore any message.*/
  void setPrefix( char *pref );


  // Not necessarily printed (depend on current log level).
  //
  int debug( char *format, ... );  ///< 'Debug' level.
  int info ( char *format, ... );  ///< 'Info' level.

  /** 'Info' level, but does not print "Info:<prefix>" before. */
  int operator()( char *format, ... );

  // Are necessarily printed in log.
  int warn ( char *format, ... );  ///< stderr and continue.
  int error( int exit_code, char *format, ... ); ///< stderr and exit.

  /** Print 'size' bytes from memory counting from 'buf'.
   * 'length' informs the number of bytes per line.
   * Available only during 'Debug' level.*/
  int buffer( void *buf, int size, int length=16 );


private:

  FILE *_log;
  int   _close; ///< If not zero closes '_log' in destructor.

  Level  _level;
  char  *_pref;
};


extern Log g_log;


#endif

