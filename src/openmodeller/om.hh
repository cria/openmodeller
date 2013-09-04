/**
 * All headers necessary to use openModeller.
 * 
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-09-16
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

//
// This file must be included to let you use libopenmodeller in
// both ways:
//
// - Development of an openModeller algorithm;
// - Development of an openModeller client interface.
//

#ifndef _OMHH_
#define _OMHH_

// General includes (clients and algorithms)
//
#include <openmodeller/om_defs.hh>
#include <openmodeller/Algorithm.hh>
#include <openmodeller/AlgMetadata.hh>
#include <openmodeller/AlgParameter.hh>
#include <openmodeller/AreaStats.hh>
#include <openmodeller/ConfusionMatrix.hh>
#include <openmodeller/RocCurve.hh>
#include <openmodeller/Log.hh>
#include <openmodeller/Settings.hh>

// openModeller algorithms specific includes
//
#include <openmodeller/Sampler.hh>
#include <openmodeller/Occurrences.hh>


// openModeller clients specific includes
//
#include <openmodeller/OpenModeller.hh>
#include <openmodeller/AlgorithmFactory.hh>
#include <openmodeller/MapFormat.hh>
#include <openmodeller/env_io/Map.hh>
#include <openmodeller/env_io/RasterFactory.hh>
#include <openmodeller/occ_io/OccurrencesFactory.hh>

#endif


