/**
 * Definition of HingeGenerator class
 * 
 * @author Renato De Giovanni
 * $Id$
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

#include "hinge_generator.hh"
#include "hinge_feature.hh"

#include <cmath>

HingeGenerator::HingeGenerator(const OccurrencesPtr& presences, const OccurrencesPtr& background, LinearFeature * feature, bool reverse):FeatureGenerator(presences, background, feature, G_HINGE, reverse)
{
  _first_ref = 0;
  _last_ref = (int)_thresholds.size();
  _maxval = _vals[_vals.size()-1].second;
}

HingeGenerator::~HingeGenerator() {}

void
HingeGenerator::setSampExp( double mindev )
{
  int limit = _background->numOccurrences();

  int num_samples = _presences->numOccurrences();

  double wsum = 0.0;
  double sum1 = 0.0;
  double sum2 = 0.0;

  double ref_min = 0.0;
  double ref_max = 1.0;
  double margin;
  double i_avg = 0.0;
  double i_std = 0.0;
  double i_lower;
  double i_upper;

  for ( int i=(int)_vals.size()-1; i >= 0; --i ) {

    // Is this a sample?
    if ( (_vals.at(i)).first >= limit ) {

      wsum += 1.0;
      sum1 += _vals[i].second;
      sum2 += pow(_vals[i].second, 2);
    }

    int t_idx = _threshold_index[i];

    if ((t_idx < _first_ref) || (t_idx >= _last_ref)) {

      continue;
    }

    double avg = ref_max / 2.0;
    double std = ref_max / 2.0;

    if ( num_samples ) {

      double csum1 = (sum1 - _thresholds[t_idx] * wsum) / (_maxval - _thresholds[t_idx]);

      double csum2 = (sum2 - 2.0 * sum1 * _thresholds[t_idx] + pow(_thresholds[t_idx], 2) * wsum) / (_maxval - _thresholds[t_idx]) / (_maxval - _thresholds[t_idx]);

      avg = csum1 / num_samples;

      if ( num_samples > 1 ) {

        if ( csum2 < (num_samples * pow(avg, 2)) ) {

          std = 0.0;
        }
        else {

          std = sqrt((csum2 - num_samples * pow(avg, 2)) / (num_samples - 1));
        }

        if ( std > (ref_max/2.0) ) {

          std = ref_max/2.0;
        }
        if ( std < (ref_max/sqrt(num_samples)) ) {

          std = ref_max/sqrt(num_samples);
        }
      }
    }

    i_avg = avg;
    i_std = std;

    margin = (_beta / sqrt((double)num_samples)) * i_std;

    i_lower = i_avg - margin;
    i_upper = i_avg + margin;
 
    if ( i_lower < ref_min ) {

      i_lower = ref_min;
    }
    if ( i_upper > ref_max ) {

      i_upper = ref_max;
    }

    _samp_exp[t_idx] = 0.5*( i_upper + i_lower );
    _samp_dev[t_idx] = 0.5*( i_upper - i_lower );

    if ( _samp_dev[t_idx] < mindev ) {

      _samp_dev[t_idx] = mindev;
    }
  }
}

void 
HingeGenerator::updateExp( double * density, double z_lambda )
{
  double sum = 0.0;
  double wsum = 0.0;
  double d;

  int limit = _background->numOccurrences();

  for ( int i=(int)_vals.size()-1; i >= 0; --i ) {

    // Is this a background point?
    if ( _vals[i].first < limit ) {

      d = density[ _vals[i].first ];
      sum += _vals[i].second * d;
      wsum += d;
    }

    int t_idx = _threshold_index[i];

    if ((t_idx >= _first_ref) && (t_idx < _last_ref)) {

      _exp[t_idx] = ((sum - _thresholds[t_idx] * wsum) / z_lambda / (_maxval - _thresholds[t_idx]));
    }
  }

  for ( int i = _first_ref; i < _last_ref; i++ ) {

    if ( _features[i] != 0 ) {

      _features[i]->setExp( _exp[i] );
    }
  }
}

Feature * 
HingeGenerator::exportFeature(int idx)
{
  if ( _features[idx] == 0 ) {

    // Create feature if necessary
      Feature * f = new HingeFeature(_feature->getLayerIndex(), _thresholds[idx], _maxval, _reverse);

    f->setSampExp( sampExp(idx) );
    f->setSampDev( sampDev(idx) );
    f->setExp( exp(idx) );
    f->setBeta( _beta );

    _features[idx] = f;
  }

  return _features[idx];
}

