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

#include <vector>

#include <limits>

#include <algorithm>
using std::pair;

FeatureGenerator::FeatureGenerator(const OccurrencesPtr& presences, const OccurrencesPtr& background, LinearFeature * feature)
{
  _presences = presences;
  _background = background;
  _feature = feature;

  _first_ref = -1;
  _last_ref = -1;

  // Store feature values in _vals
  _vals.reserve( _background->numOccurrences() + _presences->numOccurrences() );

  int i = 0;

  // Load background values first
  OccurrencesImpl::const_iterator it = _background->begin();
  OccurrencesImpl::const_iterator end = _background->end();

  while ( it != end ) {

    Sample s = (*it)->environment();

    _vals.push_back( std::make_pair(i, _feature->getRawVal(s)) );

    ++it;
    ++i;
  }

  // Then load presence values
  it = _presences->begin();
  end = _presences->end();

  while ( it != end ) {

    Sample s = (*it)->environment();

    _vals.push_back( std::make_pair(i, _feature->getRawVal(s)) );

    ++it;
    ++i;
  }

  // Sort _vals by feature value
  sort( _vals.begin(), _vals.end(), by_second_value() );

  // Get best precision among values
  Scalar best_prec = std::numeric_limits<double>::infinity();

  std::vector< pair<int, double> >::iterator vit;
  for ( vit = _vals.begin(); vit != _vals.end(); ++vit ) {

    Scalar prec = getPrecision( vit->second );

    if ( prec < best_prec ) {

      best_prec = prec;
    }
  }

  // Calculate all thresholds
  _threshold_index.reserve(_vals.size());
  _threshold_index.assign(_vals.size(), -1);

  i = 0;
  int t_idx = 0;
  Scalar v;
  Scalar last_v = _vals[0].second;
  Scalar t;
  bool got_first = false;
  int limit = _background->numOccurrences();
  for ( vit = _vals.begin(); vit != _vals.end(); ++vit,++i ) {

    v = vit->second;

    // Is this a sample?
    if ( vit->first >= limit ) {

      // Get first and last values (min/max)
      if ( !got_first ) {

        _first_samp_val = v;
        got_first = true;
      }

      _last_samp_val = v;
    }

    if ( v - last_v > best_prec ) {

      t = ((v + last_v)/2.0);
      Log::instance()->debug("T: %.16f\n", t);
      _thresholds.push_back( t );
      _threshold_index[i] = t_idx;
      last_v = v;
      ++t_idx;
    }
  }

  _features.reserve(t_idx);
  Feature* f = 0;
  _features.assign(t_idx, f);

  _exp.reserve(t_idx);
  _exp.assign(t_idx, 0.0);
  _samp_exp.reserve(t_idx);
  _samp_exp.assign(t_idx, 0.0);
  _samp_dev.reserve(t_idx);
  _samp_dev.assign(t_idx, 0.0);

  Log::instance()->debug("Num thresholds: %u\n", _thresholds.size());
}

FeatureGenerator::~FeatureGenerator()
{
  delete _feature;

  unsigned int n = _features.size();

  Log::instance()->debug("Deallocating %d features\n", n);

  for (unsigned int i = 0; i < n; ++i) {

    if ( _features[i] != 0 ) {

      delete _features[i];
    }
  }
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
FeatureGenerator::getPrecision(Scalar val)
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

