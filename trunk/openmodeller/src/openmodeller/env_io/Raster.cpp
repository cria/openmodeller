/**
 * Definitions of Raster and RasterFormat classes.
 * 
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-08-22
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

#include <openmodeller/env_io/Map.hh>
#include <openmodeller/env_io/Raster.hh>

#include <openmodeller/Log.hh>

#include <openmodeller/MapFormat.hh>

#include <openmodeller/Exceptions.hh>

#include <utility>
using std::pair;

using std::string;

/****************************************************************/
/**************************** Raster ****************************/

/******************/
/*** construtor ***/



/*****************/
/*** destrutor ***/

Raster::~Raster()
{}

/*******************/
/*** set Min Max ***/
void
Raster::setMinMax( Scalar min, Scalar max )
{
	f_hdr.minmax = 1;
	f_hdr.vmin = min;
	f_hdr.vmax = max;
}

