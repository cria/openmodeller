/**
 * Declaration of Normalizable Interface
 * 
 * @file Normalizable.hh
 * @author Ricardo Scachetti Pereira
 * @date 2005-05-06
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


#ifndef _NORMALIZABLE_HH
#define _NORMALIZABLE_HH

#include <openmodeller/Sample.hh>

class Normalizable {

public:
  virtual ~Normalizable(){};

  virtual void getMinMax( Sample * min, Sample * max ) const = 0;

  virtual void normalize( bool useNormalization, 
			  const Sample& offsets, const Sample& scales ) = 0;

};

#endif
