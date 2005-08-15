/**
 * Declaration of DG GARP with Best Subsets Procedure
 * 
 * @file   dg_garp_bs.hh
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-12-10
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
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
 * 
 */


#ifndef _GENERIC_BEST_SUBSETS_HH_
#define _GENERIC_BEST_SUBSETS_HH_

#include <openmodeller/om.hh>

#include "AbstractBestSubsets.hh"


class GenericBestSubsets : public AbstractBestSubsets
{
public:
  GenericBestSubsets();
  virtual ~GenericBestSubsets();

private:
};


#endif

