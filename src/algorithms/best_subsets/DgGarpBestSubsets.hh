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


#ifndef _DG_GARP_BEST_SUBSETS_HH_
#define _DG_GARP_BEST_SUBSETS_HH_

#include <openmodeller/om.hh>

#include "GarpAlgorithm.h"
#include "AbstractBestSubsets.hh"


/****************************************************************/
/************************* GARP Algorithm ***********************/

/**
  * Implements GARP with Best Subset Procedure (Anderson et al. 2003).
  * 
  * It runs "n" Garp models, filter the best ones based on the
  * procedure defined by Anderson et al. 2003 and sum them to obtain
  * a probability map for the species distribution.
  * 
  * Current implementation uses multi-threading to run multiple 
  * Garp runs in parallel. Takes advantage of multi-processor 
  * servers and workstations.
  */
class DgGarpBestSubsets : public AbstractBestSubsets
{
public:
  DgGarpBestSubsets();
  virtual ~DgGarpBestSubsets();

private:
};


#endif

