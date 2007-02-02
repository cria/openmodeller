/**
 * Declaration of Configurable Interface
 * 
 * @file configurable.hh
 * @author Kevin Ruland kruland@ku.edu
 * @date 2004-11-14
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


#ifndef _CONFIGURABLE_HH
#define _CONFIGURABLE_HH

#include <openmodeller/Configuration.hh>

class Configurable {

public:
  virtual ~Configurable() {};
  virtual ConfigurationPtr getConfiguration() const = 0;

  virtual void setConfiguration( const ConstConfigurationPtr & ) = 0;

};

#endif
