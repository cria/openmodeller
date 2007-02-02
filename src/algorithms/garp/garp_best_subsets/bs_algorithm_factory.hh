/**
 * Declaration of BSAlgorithmFactory class
 * 
 * @file   bs_algorithm_factory.hh
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-12-16
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


#ifndef _BS_ALGORITHM_FACTORY_HH_
#define _BS_ALGORITHM_FACTORY_HH_

#include <openmodeller/om.hh>

class BSAlgorithmFactory
{
public:
  virtual ~BSAlgorithmFactory(){};
  virtual AlgorithmImpl * getBSAlgorithm() = 0;
};


#endif

