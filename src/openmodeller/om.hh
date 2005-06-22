/**
 * All headers necessary to use openModeller.
 * 
 * @file
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
#include <om_defs.hh>
#include <om_algorithm.hh>
#include <om_algorithm_metadata.hh>
#include <om_alg_parameter.hh>
#include <om_area_stats.hh>
#include <om_conf_matrix.hh>
#include <om_log.hh>

// openModeller algorithms specific includes
//
#include <om_sampler.hh>
#include <om_occurrences.hh>
#include <om_occurrences.hh>


// openModeller clients specific includes
//
#include <om_control.hh>
#include <algorithm_factory.hh>
#include <map_format.hh>
#include <env_io/map.hh>
#include <env_io/raster.hh>

#endif


