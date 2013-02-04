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

#include <openmodeller/os_specific.hh>
#include <openmodeller/om_defs.hh>

#include <iostream>
#include <fstream>
#include <string>
#include <stdarg.h>
#include <stdio.h>

/****************************************************************/
/****************************** Log *****************************/

/** 
 * A Singleton Class that allows sending of log messages to "stream" devices.
 * A system interface class with more advanced logging 
 * mechanisms (libraries).
 */
class dllexp Log
{
  public:
    //! Returns the instance pointer, creating the object on the first call
    static Log * instance();

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

  protected:
    /**
     * Protected constructor
     */
    Log( );

  public:

    /** General function that can be used to generate formatted strings */
    static std::string format( const char *fmt, ... );

    ~Log();

    /** Configure the logger.
     * 
     * @param level Log level.
     * @param fileName Log file name.
     * @param pref Prefix to be shown on every message.
     */
    void set( Level level, std::string fileName, char const *pref="" );

    /** Change the call back mechanism
     *  The Log object takes ownership of the object
     *  and will delete it when the Log is destroyed
     *  or when setCallback is called again
     */
    void setCallback( LogCallback *lc );

    /** Change log level.*/
    void setLevel( Level level )  { _level = level; }

    /** Change prefix to be shown before any message.*/
    void setPrefix( const char *pref );

    void debug( const char *format, ... );  ///< 'Debug' level.
    void info ( const char *format, ... );  ///< 'Info' level.
    void warn ( const char *format, ... );  ///< 'Warn' level.
    void error( const char *format, ... );  ///< 'Error' level.

    /** 'Default' level. */
    void operator()( const char *format, ... );

  private:

    //write log out to callback
   void FormatAndWrite( Log::LogCallback& lc, Log::Level level, std::string pref, const char* format, va_list ap );

    static Log * mpInstance;

    LogCallback* callback;

    Level _level;

    std::string _pref;

    bool _deleteCallback; // flag indicating if the callback should be deleted by Log
};

#endif

