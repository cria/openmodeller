
/**
* Declaration of class PreParameters
*
* @author Missae Yamamoto (missae at dpi . inpe . br)
* $Id$
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

#ifndef PRE_PARAMETERS_HH
  #define PRE_PARAMETERS_HH

  #include "PreMultiContainer.hh"
  #include <string>
  #include <openmodeller/om.hh>

  //adapted from Terralib PDI (Digital Image Processing) to OM Pre-analysis ( Missae & Emiliano - DPI/INPE )

 /**
  * This is the class to deal with parameters used by all Pre-analysis classes.
  * It is used because PreMultiContainer class is all templated.
  * author Emiliano F. Castejon <castejon@dpi.inpe.br>
  */
  class dllexp PreParameters : public 
	  PreMultiContainer< std::string >
  {
    public:
	  PreParameters();

	  ~PreParameters();

      // return the name descriptor for the current parameters. 
      std::string decName() const;    
  };

#endif //PRE_PARAMETERS_HH
