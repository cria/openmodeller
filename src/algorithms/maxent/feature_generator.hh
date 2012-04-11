/**
 * Declaration of Feature Generator class
 * 
 * @author Renato De Giovanni
 * $Id: $
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2012 by CRIA -
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

#ifndef _FEATUREGENERATOR_HH
#define _FEATUREGENERATOR_HH

#include "feature.hh"
#include <openmodeller/Occurrences.hh>

#define G_HINGE 1
#define G_THRESHOLD 2

#define G_MAXDIGITS 6

class FeatureGenerator {

public:

  FeatureGenerator(const OccurrencesPtr& presences, const OccurrencesPtr& background, Feature * feature);

  ~FeatureGenerator();

  Scalar type() {return _type;}

  void setBeta( Scalar beta ){_beta = beta;}

  Scalar beta() {return _beta;}

  int getFirstRef() const { return _firstRef; }

  int getLastRef() const { return _lastRef; }

protected:

  Scalar _getPrecision(Scalar val);

  int _type;

private:

  OccurrencesPtr _presences;
  OccurrencesPtr _background;
  Feature * _feature;

  Scalar _beta;
  
  int _firstRef;
  int _lastRef;
};

#endif
