/**
 * Declaration of AlgorithmMetadata structure.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2004-03-18
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


#ifndef _OM_ALGORITHM_METADATAHH_
#define _OM_ALGORITHM_METADATAHH_

#include <om_defs.hh>


/**
 * Algorithms' parameter metadata structure.
 * 
 **/
typedef struct
{
  char *name;
  char *type;
  char *description;

  int    has_min; //< Zero if the parameter has no lower limit.
  Scalar min;     //< Minimum parameter value.
  int    has_max; //< Zero if the parameter has no upper limit.
  Scalar max;     //< Maximum parameter value.
  Scalar typical; //< Typical parameter value.

} AlgorithmParameter;


/** 
 * Algorithms' metadata structure.
 * 
 */
typedef struct
{
  char *name;
  char *version;
  char *biblio;
  char *description;
  char *author;
  char *contact;

  int  categorical;
  int  absence;
  int  nparam;
  AlgorithmParameter *param;

} AlgorithmMetadata;



#endif
