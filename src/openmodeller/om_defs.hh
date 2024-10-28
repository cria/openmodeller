/**
 * Definition of types and macros of general use.
 * 
 * @author Mauro E S Munoz
 * @date   2003-01-24
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

#ifndef _DEFSHH_
#define _DEFSHH_

//preprocessor trick to convert anything to a quoted string
//e.g. myString.append(WRITESTRING(OM_MAJOR_VERSION))
#define WRITESTRING(x) #x

// Math types.
//
// Must be 'float' or 'double':
typedef double Coord;    ///< Type of map coordinates.
typedef double Scalar;   ///< Type of map values.

// C++17 removed the register keyword, so we define it to nothing so that
// it doesn't cause errors.
#if __cplusplus >= 201703L
#define register
#endif

/** Model creation callback function.
 * @param progress A number between 0.0 and 1.0 reflecting the
 *  avance of the map creating task. 0.0 is the begin and
 *  1.0 is finished. Note that in model generation it is not 
 * always possible to know how many steps the algorithm will need.
 * @param extra_param A parameter set by user when setModelCallback() 
 *  is called in the controller class.
 */
typedef void (*ModelCreationCallback)( float progress, void *extra_param );

/** Model projection callback function.
 * @param progress A number between 0.0 and 1.0 reflecting the
 *  avance of the map creating task. 0.0 is the begin and
 *  1.0 is finished.
 * @param extra_param A parameter set by user when setMapCallback() 
 *  is called in the controller class.
 */
typedef void (*ModelProjectionCallback)( float progress, void *extra_param );

/** Abortion callback function.
 * @param extra_param A parameter set by user when setAbortionCallback() is called.
 * @return Indicates if the current job must be aborted.
 */
typedef bool (*AbortionCallback)( void *extra_param );


// Windows only defs
//
#ifdef WIN32

// Strangely enough, cygwin/mingw windows libs have strcasecmp() function
// while native VC++7 lib doesn't. Go figure... 
#  ifdef MSVC
#    define strcasecmp _stricmp
#  endif

// disable warnings about lack of DLL export in inner class members
#ifdef MSVC
#pragma warning( disable : 4251)
#pragma warning( disable : 4275)
// disable warning on deprecation of standard C functions and others
#pragma warning( disable : 4996)
#endif //MSVC


#endif //WIN32

#endif //_DEFSHH_




