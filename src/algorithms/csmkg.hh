/**
 * This is the Keiser-Gutman cutoff implementation of csm 
 * 
 * @file csm
 * @author Tim Sutton (t.sutton@reading.ac.uk)
 * @date   2003-09-12
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2003 by CRIA -
 * Centro de Referencia em Informacao Ambiental
 *
 * http://www.cria.org.br
 * 
 * Copyright(c) Neil Caithness 2004 (Model Methodology)
 * Copyright(c) Tim Sutton 2004 (C++ implementation)
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
 
#ifndef CSMKG_H
#define CSMKG_H

#include "csm.hh"

/**
 * CsmKG = Csm kaiser-gutman cutoff for discarded components
 * @author Tim Sutton
*/
class CsmKG : public Csm
{
public:

  /** Constructor for Csm */
  CsmKG();
  /** This is the descructor for the Csm class */
  ~CsmKG();


private:
  
  /** Discard unwanted components.
   * This is a pure virtual function - it must be implemented by the derived
   * class. Currently two derived classes are expected to be implemented -
   * one for kaiser-gutman cutoff and one for broken-stick cutoff.
    * @note This method must be called after center
    * @return 0 on error    
    */  
  int discardComponents();

};

#endif
