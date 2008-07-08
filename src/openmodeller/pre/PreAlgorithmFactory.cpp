 
/**
* Declaration of class PreAlgorithmFactory
*
* @author Missae Yamamoto (missae at dpi . inpe . br)
* $Id: $
*
* LICENSE INFORMATION
* 
* Copyright(c) 2008 by INPE -
* Instituto Nacional de Pesquisas Espaciais
*
* http://www.inpe.br
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

#include "PreAlgorithmFactory.hh"

PreAlgorithmFactory::PreAlgorithmFactory( const std::string& factoryName )
: PreFactory< PreAlgorithm, PreParameters >( factoryName )
{
};      

PreAlgorithmFactory::~PreAlgorithmFactory()
{
};
