/**
 * Declaration of DelimitedTextOccurrences class.
 * 
 * @author Mauro E S Muñoz (mauro@cria.org.br)
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
 * The file must have 5 columns separated by TAB.
 *
 * - Column  1  : Unique identifier for the occurrence.
 * - Columns 2  : Label (a group identifier, usually the scientific name).
 * - Column  3  : longitude (-180.0 <= lat  <= 180.0).
 * - Column  4  : latitude: (-90.0 <= long <= 90.0).
 * - Column  5  : Abundance (integer).
 */
class dllexp DelimitedTextOccurrences : public OccurrencesReader
{
public:

  /** 
   * Return a new instance of this class.
   */
  static OccurrencesReader * CreateOccurrencesReaderCallback( const char * source, const char * coordSystem );

  /** Constructor.
   * @param source Source of occurrences (such as a file name, Terralib path or TAPIR URL).
   * @param coordSystem Default coordinate system in WKT.
   */
  DelimitedTextOccurrences( const char *source, const char * coordSystem );

  /** Destructor.
   */
  ~DelimitedTextOccurrences();

  /**
   * Read occurrences from a file.
   */
  bool load();

private:

  // Indicates if occurrences were already loaded.
  bool _loaded;
};

#endif

