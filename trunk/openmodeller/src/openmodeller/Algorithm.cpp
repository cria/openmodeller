/**
 * Definition of Algorithm class.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-10-05
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

#include <algorithm.hh>


/****************************************************************/
/************************** Algorithm ***************************/

/*******************/
/*** constructor ***/

Algorithm::Algorithm( Sampler *samp )
{
  f_samp = samp;

  f_categ = new int[ f_samp->dim() ];
  f_samp->varTypes( f_categ );
}


/******************/
/*** destructor ***/

Algorithm::~Algorithm()
{
  delete f_categ;
}

