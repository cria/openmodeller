/**
 * Definition of types and macros of general use.
 * 
 * @file
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

// openModeller version number.
//
#define OM_VERSION VERSION

// Coordinate systems.
//
#define OM_WGS84 "GEOGCS[\"WGS84\", DATUM[\"WGS84\", \
  SPHEROID[\"WGS84\", 6378137.0, 298.257223563]], \
  PRIMEM[\"Greenwich\", 0.0], \
  UNIT[\"degree\",0.017453292519943295], \
  AXIS[\"Longitude\",EAST], AXIS[\"Latitude\",NORTH]]"

// Common Coordinate System for openModeller.
#define OM_COORDINATE_SYSTEM OM_WGS84


// Types.
//
typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;


// Math types.
//
typedef double Real;
typedef double Angle;

// Must be 'float' or 'double':
typedef double Coord;    ///< Type of map coordinates.
typedef double Scalar;   ///< Type of map values.


// Types for function pointers.
//
typedef double (*doubleFunc)(double);
typedef float  (*floatFunc)(float);


// General macros
//
#define Zero        (1e-8)
#define Abs(x)      ((x) < 0 ? -(x) : x)
#define IsZero(x)   ((x) > -Zero && (x) < Zero)
#define Min( a, b ) ((a) < (b) ? (a) : (b))
#define Max( a, b ) ((a) > (b) ? (a) : (b))


// Windows only defs
//
#ifdef _WINDOWS

#define strcasecmp _stricmp
#define dllexp __declspec(dllexport)

#ifdef CORE_DLL_IMPORT
#define dll_log __declspec(dllimport)
#else
#define dll_log __declspec(dllexport)
#endif

#else

#define dllexp
#define dll_log

#endif


#endif




