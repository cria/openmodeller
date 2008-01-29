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

#include <openmodeller/om_defs.hh>


typedef enum {

  Integer = 0,
  Real    = 1,
  String  = 2

} AlgParamDatatype;


/****************************************************************/
/***************** Algorithm Parameter Metadata *****************/

/**
 * Algorithms' parameter metadata structure.
 * 
 **/
typedef struct 
{
  char *id;              ///< Identifier to be used by programmers.
  char *name;            ///< Name to be shown to end users.
  AlgParamDatatype type; ///< Real, Integer, String.
  char *overview;        ///< Short description.
  char *description;     ///< Detailed description.

  int    has_min;    ///< Zero if the parameter has no lower limit.
  Scalar min_val;    ///< Minimum parameter value.
  int    has_max;    ///< Zero if the parameter has no upper limit.
  Scalar max_val;    ///< Maximum parameter value.
  char  *typical;    ///< Typical parameter value.

} AlgParamMetadata;


/****************************************************************/
/********************** Algorithm Metadata **********************/

/** 
 * Algorithms' metadata structure.
 * 
 */
typedef struct 
{
  char *id;          ///< Identifier to be used by programmers.
  char *name;        ///< Name to be shown to end users.
  char *version;     ///< Built version.
  char *overview;    ///< Short description.
  char *description; ///< Detailed description.

  char *author;      ///< Algorithm's author.
  char *biblio;      ///< Bibliography reference.

  char *code_author; ///< Who implemented.
  char *contact;     ///< code_author contact (eg e-mail).

  int  categorical;  ///< If not zero accept categorical maps.
  int  absence;      ///< Needs absence points to run.
  int  nparam;       ///< Number of parameters.
  AlgParamMetadata *param;

} AlgMetadata;



#endif
