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


/****************************************************************/
/********************* Dynamic Linking Loader *******************/

typedef void *DLLHandle;

extern DLLHandle dllOpen    ( char *dll_file_name );
extern void     *dllFunction( DLLHandle, char *function_name );
extern int       dllClose   ( DLLHandle );
extern char     *dllError   ( DLLHandle );


/****************************************************************/
/********************* Scan Directory Entries *******************/

/**
 * Scan directory entries.
 * @param dir Path of the directory to be scanned.
 * @return A null terminated array of allocated strings with the
 * directory scanned entries. Return null if an error occurs.
 */
char **scanDirectory( char *dir );



#endif
