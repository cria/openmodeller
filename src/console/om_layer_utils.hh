/**
 * Declaration of layers utility functions.
 * 
 * @author Renato De Giovanni (renato at cria . org . br)
 * $Id $
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2013 by CRIA -
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

#ifndef OM_LAYER_UTILS_HH
#define OM_LAYER_UTILS_HH

#include <sstream>

using namespace std;

// Recursively reads directory content to find GDAL compatible rasters
bool readDirectory( const char* dir, const char* label, ostream &xml, int depth, int * seq );

// Get a layer label from the metadata file
string getLayerLabel( const string path, const string name, bool isDir );

// Indicates if a file is compatible with GDAL
bool isValidGdalFile( const char* fileName );

// Indicates if a file has a valid GDAL projection
bool hasValidGdalProjection( const char* fileName );

#endif
