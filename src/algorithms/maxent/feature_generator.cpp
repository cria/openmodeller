/**
 * Definition of FeatureGenerator class
 * 
 * @author Renato De Giovanni
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2011 by CRIA -
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

#include "feature_generator.hh"
#include <openmodeller/Exceptions.hh>

#include <cmath>

FeatureGenerator::FeatureGenerator(const OccurrencesPtr& presences, const OccurrencesPtr& background, LinearFeature * feature)
{
  _presences = presences;
  _background = background;
  _feature = feature;

}

FeatureGenerator::~FeatureGenerator()
{
  delete _feature;

  unsigned int n = _features.size();

  Log::instance()->debug("Deallocating %d features\n", n);

  for (unsigned int i = 0; i < n; ++i)
    delete _features[i];
}

Scalar 
FeatureGenerator::lambda(int idx)
{
  if ( _features[idx] == 0 ) {

    return 0.0;
  }

  return _features[idx]->lambda();
}

Scalar
FeatureGenerator::_getPrecision(Scalar val)
{
  Scalar v = fabs(val);
  Scalar fs = floor(log(v) / log(10.0));
  Scalar cPower = pow(10.0, fs);
  int last2Digits = 0;

  for ( int i = 1; i <= G_MAXDIGITS; i++ ) {

    int digit = (int)(v / cPower);
    last2Digits = (last2Digits * 10 + digit) % 100;

    if ( i >= 2 ) {

      switch ( last2Digits ) {
        case 0:
        case 1:
        case 98:
        case 99:
          return cPower * 50.0;
      }
    }

    v -= digit * cPower;
    cPower /= 10.0;
  }

  return cPower * 5.0;
}

