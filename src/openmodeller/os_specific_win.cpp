/**
 * Definitions of Windows operating system's specific functions.
 * 
 * @file
 * @author Ricardo Scachetti Pereira (rpereira at ku.edu)
 * @date   2004-04-20
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 * Copyright(c), David R.B. Stockwell of Symbiotik Pty. Ltd.
 * Copyright(c), CRIA - Centro de Referencia em Informacao Ambiental
 *
 * http://www.nhm.ku.edu
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


/****************************************************************/
/********************* Dynamic Linking Loader *******************/

#include <stdio.h>
#include <windows.h>


/****************/
/*** dll Open ***/
DLLHandle
dllOpen( char *dll_file_name )
{
  return LoadLibrary( dll_file_name );
}


/********************/
/*** dll Function ***/
void *
dllFunction( DLLHandle handle, char *function_name )
{
  return GetProcAddress( handle, function_name );
}


/*****************/
/*** dll Close ***/
int
dllClose( DLLHandle handle )
{
  return FreeLibrary( handle );
}


/*****************/
/*** dll Error ***/
char *
dllError( DLLHandle )
{
  char * szBuf = NULL;
  long error = GetLastError();

  if (error)
  {
    szBuf = new char[1024]; 
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	               NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &szBuf, 0, NULL );
  }
  return szBuf;
}



/****************************************************************/
/********************* Scan Directory Entries *******************/

#include <io.h>
#include <string.h>
#include <stdlib.h>


/****************/
/*** scan Dir ***/
char **
scanDirectory( char *dir )
{
  char filepattern[1024];
  long dirhandle;
  struct _finddata_t fileinfo;

  // Windows findfirst and findnext calls.
  sprintf(filepattern, "%s\\*.dll", dir);
  dirhandle = _findfirst(filepattern, &fileinfo); 

  if (dirhandle == -1L)
    return 0;

  int nent = 1;
  while (!_findnext(dirhandle, &fileinfo))
  { nent++; }

  // Allocates the array to be returned.
  char **entries = new char *[ nent + 1 ];

  // Directory path size
  int dir_size = strlen( dir );

  // Windows findfirst and findnext calls.
  dirhandle = _findfirst(filepattern, &fileinfo); 

  if (dirhandle == -1L)
    return 0;

  // Copy from windows structure to the return structure.
  for ( int i = 0; i < nent; i++ )
    {
      char *found = fileinfo.name;
      char *name  = new char[dir_size + strlen(found) + 2];

      // Copy the directory path.
      // Append the library file name.
      sprintf( name, "%s\\%s", dir, found );
      entries[i] = name;

      _findnext(dirhandle, &fileinfo);
    }

  // Null terminated array.
  entries[nent] = 0;

  _findclose(dirhandle);

  return entries;
}




/****************************************************************/
/*********************** Random Generation **********************/

#include <time.h>


/*******************/
/*** init Random ***/
int
initRandom()
{
  unsigned int seed;
  seed = (unsigned int) time( NULL );
  srand( seed );
  return 1;
}
