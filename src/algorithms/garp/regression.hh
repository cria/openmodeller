/**
 * Declaration of Regression class
 * 
 * @file   regression.hh
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2005-01-28
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 * Copyright(c), David R.B. Stockwell of Symbiotik Pty. Ltd.
 * Copyright(c), CRIA - Centro de Referencia em Informacao Ambiental
 *
 * http://www.nhm.ku.edu
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
 * 
 * This is an implementation of the GARP algorithm first developed 
 * by David Stockwell
 * 
 */


#ifndef _REGRESSION_HH_
#define _REGRESSION_HH_

#include <om.hh>

class Regression
{
public:
  Regression();

  void calculateParameters(const OccurrencesPtr& occs);

  const Sample getA() const { return _paramA; }
  const Sample getB() const { return _paramB; }
  const Sample getC() const { return _paramC; }

private:
  int _dimensions;
  int _resamples;
  Sample _paramA;
  Sample _paramB;
  Sample _paramC;
};

/****************************************************************/

#endif

