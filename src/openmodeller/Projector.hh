/**
 * Declaration of OpenModeller class (former ControlInterface class).
 * 
 * @file
 * @author Mauro E S Muñoz <mauro@cria.org.br>
 * @date 2003-09-25
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

#ifndef _PROJECTOR_HH_
#define _PROJECTOR_HH_

#include <openmodeller/om_defs.hh>
#include <openmodeller/CallbackWrapper.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/Model.hh>
#include <openmodeller/env_io/Header.hh>

#include <string>

class RasterFile;
class AreaStats;

class dllexp Projector {

public:

  /** Create and save distribution map to disk.
   */
  static bool createMap( const Model& model,
			 const EnvironmentPtr& env,
			 Map *map,
			 AreaStats *areaStats = 0,
			 CallbackWrapper *callbackWrapper = 0 );

private:
		   // Don't allow construction.
  Projector();

};

#endif
