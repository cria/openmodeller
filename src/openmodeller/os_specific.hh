
/**
 * Declaration of AlgorithmFactory class.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2004-03-19
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

#ifndef _OS_SPECIFICHH_
#define _OS_SPECIFICHH_

#include <vector>
#include <string>

/****************************************************************/
/********************* Dynamic Linking Loader *******************/

#ifdef WIN32 
#include <windows.h>
typedef HMODULE DLLHandle;
#else
typedef void *DLLHandle;
#endif

extern DLLHandle   dllOpen    ( char const *dll_file_name );
extern void       *dllFunction( DLLHandle, char const *function_name );
extern int         dllClose   ( DLLHandle );
extern const char *dllError   ( DLLHandle );

/****************************************************************/
/*************** Retrieve initial library path ******************/

std::vector<std::string> initialPluginPath();


/****************************************************************/
/********************* Scan Directory Entries *******************/

/**
 * Scan directory entries.
 * @param dir Path of the directory to be scanned. (pass by value,
 * routine needs local copy).
 * @return A null terminated array of allocated strings with the
 * directory scanned entries. Return null if an error occurs.
 */
std::vector<std::string> scanDirectory( std::string dir );


/****************************************************************/
/*********************** Random Generation **********************/

/**
 * Generates a pseudo-random seed and initializes the system
 * random sequence generator. The seed is based in the
 * micro-seconds of the current machine time.
 * @return Not zero if the generator was initiated.
 */
int initRandom();



#endif
