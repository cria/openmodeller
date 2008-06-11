/**
 * Declaration of AlgorithmMetadata structure.
 * 
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
#include <string>

typedef enum AlgParamDatatype {

  Integer = 0,
  Real    = 1,
  String  = 2

} ;

/****************************************************************/
/***************** Algorithm Parameter Metadata *****************/

/**
 * Algorithms' parameter metadata structure.
 * 
 **/
struct AlgParamMetadata
{
  std::string id;          ///< Identifier to be used by programmers.
  std::string name;        ///< Name to be shown to end users.
  AlgParamDatatype type;   ///< Real, Integer, String.
  std::string overview;    ///< Short description.
  std::string description; ///< Detailed description.

  int    has_min;      ///< Zero if the parameter has no lower limit.
  Scalar min_val;      ///< Minimum parameter value.
  int    has_max;      ///< Zero if the parameter has no upper limit.
  Scalar max_val;      ///< Maximum parameter value.
  std::string typical; ///< Typical parameter value.
} ;


/****************************************************************/
/********************** Algorithm Metadata **********************/

/** 
 * Algorithms' metadata structure.
 * 
 */
struct AlgMetadata
{
  std::string id;          ///< Identifier to be used by programmers.
  std::string name;        ///< Name to be shown to end users.
  std::string version;     ///< Built version.
  std::string overview;    ///< Short description.
  std::string description; ///< Detailed description.

  std::string author;      ///< Algorithm's author.
  std::string biblio;      ///< Bibliography reference.

  std::string code_author; ///< Who implemented.
  std::string contact;     ///< code_author contact (eg e-mail).

  int  categorical;  ///< If not zero accept categorical maps.
  int  absence;      ///< Needs absence points to run.
  int  nparam;       ///< Number of parameters.

  AlgParamMetadata * param;
} ;



#endif
