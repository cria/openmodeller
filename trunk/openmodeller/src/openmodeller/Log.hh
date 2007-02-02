/**
 * Declaration of Log class.
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

#ifndef _LOGHH_
#define _LOGHH_

#include <openmodeller/om_defs.hh>

#include <iostream>
#include <string>

#include <stdio.h>

/****************************************************************/
/****************************** Log *****************************/

/** 
 * Class that allows sending of log messages to "stream" devices.
 * A system interface class with more advanced logging 
 * mechanisms (libraries).
 */
class dllexp Log
{
public:
  typedef enum {
    Debug   =0,
    Default =1,
    Info    =2,
    Warn    =3,
    Error   =4
  } Level;

#ifndef SWIG
  class LogCallback {
  public:
    virtual ~LogCallback(){};
    virtual void operator()( Level level, const std::string& msg ) = 0;
  };

  class OstreamCallback : public LogCallback {
  public:
    OstreamCallback( std::ostream& os );
    void operator()( Level level, const std::string& msg );
  private:
    std::ostream& os;
  };
#endif

public:

  /**
   * @param level Log level.
   * @param out Where to output logs. Will not be closed in
   *  destructor.
   * @param pref Prefix to be shown on every message.
   */
  Log( Level level=Default, const char *pref="" );

  ~Log();

  /** Configure the logger.
   * 
   * @param level Log level.
   * @param out Where to output logs. Will not be closed in
   *  destructor.
   * @param pref Prefix to be shown on every message.
   */
  void set( Level level, FILE *out, char const *pref="" );

  /** Change the call back mechanism
   *  The Log object takes ownership of the object
   *  and will delete it when the Log is destroyed
   *  or when setCallback is called again
   */
  void setCallback( LogCallback *lc );

  /** Change log level.*/
  void setLevel( Level level )  { _level = level; }

  /** Change prefix to be shown befeore any message.*/
  void setPrefix( const char *pref );

  // Not necessarily printed (depend on current log level).
  //
  void debug( const char *format, ... );  ///< 'Debug' level.
  void info ( const char *format, ... );  ///< 'Info' level.

  /** 'Default' level. */
  void operator()( const char *format, ... );

  // Are necessarily printed in log.
  void warn ( const char *format, ... );  ///< stderr and continue.
  void error( int exit_code, const char *format, ... ); ///< stderr and exit.

private:

  LogCallback* callback;

  Level _level;
  std::string _pref;
  bool _deleteCallback; // flag indicating if the callback should be deleted by Log
};


extern dllexp Log g_log;


#endif

