/**
 * Definitions of operating system's specific functions.
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

#include <os_specific.hh>

#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>


/****************************************************************/
/********************* Dynamic Linking Loader *******************/

/****************/
/*** dll Open ***/
DLLHandle
dllOpen( char *dll_file_name )
{
  return dlopen( dll_file_name, RTLD_NOW );
}


/********************/
/*** dll Function ***/
void *
dllFunction( DLLHandle handle, char *function_name )
{
  return dlsym( handle, function_name );
}


/*****************/
/*** dll Close ***/
int
dllClose( DLLHandle handle )
{
  return dlclose( handle );
}


/*****************/
/*** dll Error ***/
char *
dllError( DLLHandle )
{
  return dlerror();
}




/****************************************************************/
/********************* Scan Directory Entries *******************/

typedef struct dirent TDirent;


/**************/
/*** filter ***/
/**
 * Filter the file's names that refers to a dynamic shared
 * libraries.
 */
int
filter( const TDirent *dir )
{
  char *ext = ".so";
  char *found = strstr( dir->d_name, ext );

  return found ? ! strcmp( found, ext ) : 0;
}


/****************/
/*** scan Dir ***/
char **
scanDirectory( char *dir )
{
  // Unix scandir call.
  TDirent **namelist;
  int nent = scandir( dir, &namelist, filter, alphasort );
  if ( nent < 0 )
    return 0;

  // Allocates the array to be returned.
  char **entries = new (char *)[ nent + 1 ];

  // Directory path size
  int dir_size = strlen( dir );

  // Copy from unix structure to the return structure.
  for ( int i = 0; i < nent; i++ )
    {
      char *found = namelist[i]->d_name;
      char *name  = new char[dir_size + strlen(found) + 1];

      // Copy the directory path.
      strcpy( name, dir );

      // Append the library file name.
      strcat( name, found );
      entries[i] = name;

      free( namelist[i] );
    }

  // Null terminated array.
  entries[nent] = 0;

  // Free unix structure.
  free( namelist );

  return entries;
}

