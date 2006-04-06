/**
 * Declaration of OccurrencesFile class.
 * 
 * @author Mauro E S Mu�oz (mauro@cria.org.br)
 * @date   2003-02-25
 * @author Alexandre Copertino Jardim <alexcj@dpi.inpe.br>
 * @date 2006-03-21
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

#ifndef _OCCURRENCES_FILEHH_
#define _OCCURRENCES_FILEHH_

#include <openmodeller/occ_io/OccurrencesReader.hh>

#include <openmodeller/om_defs.hh>
#include <openmodeller/Occurrences.hh>

#include <vector>

/**  
 * Read occurrences data of one or more group from an 
 * ASCII file. The file is read and stored as a linked list
 * of objects from the Occurrences class. Allows navigation 
 * through the list.
 *
 * Format:
 *
 * Lines beginning with '#' are ignored!
 *
 * The file must have 5 columns separated by a
 * space (characters: ' ' or '\\t').
 *
 * - Columns 1,2: Strings that make up a group identifier.
 * - Column  3  : longitude (-180.0 <= lat  <= 180.0).
 * - Column  4  : latitude: (-90.0 <= long <= 90.0).
 * - Column  5  : Abundance (integer).
 */
class dllexp OccurrencesFile : public OccurrencesReader
{
public:

    /**
     * Read the occurences from the specified file.
     * @param file_name File name.
     * @\param coord_system Coordinate system.
     */
    OccurrencesFile( const char *file_name, const char *coord_system );
    ~OccurrencesFile();

    /**
     * Read occurrences from a file.
     * @param file_name File name.
     */
    int loadOccurrences( const char *file_name );
};

#endif

