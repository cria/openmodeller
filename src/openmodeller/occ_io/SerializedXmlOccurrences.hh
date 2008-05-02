/**
 * Declaration of SerializedXmlOccurrences class.
 * 
 * @author Renato De Giovanni (renato [at] cria . org . br)
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2008 by CRIA -
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

#ifndef _SERIALIZED_XML_OCCURRENCES_HH_
#define _SERIALIZED_XML_OCCURRENCES_HH_

#include <openmodeller/occ_io/OccurrencesReader.hh>

#include <openmodeller/om_defs.hh>
#include <openmodeller/Occurrences.hh>

/**  
 * Read occurrences data from an XML file containing openModeller 
 * serialized occurrences.
 */
class dllexp SerializedXmlOccurrences : public OccurrencesReader
{
public:

  /** 
   * Return a new instance of this class.
   */
  static OccurrencesReader * CreateOccurrencesReaderCallback( const char * source, const char * coordSystem );

  /** Constructor.
   * @param source Path to file containing openModeller serialized XML.
   * @param coordSystem Default coordinate system in WKT.
   */
  SerializedXmlOccurrences( const char *source, const char * coordSystem );

  /** Destructor.
   */
  ~SerializedXmlOccurrences();

  /**
   * Read occurrences from the XML file.
   */
  bool load();

private:

  // Indicates if occurrences were already loaded.
  bool _loaded;
};

#endif

