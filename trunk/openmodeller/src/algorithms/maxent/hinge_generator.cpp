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

HingeGenerator::HingeGenerator(const OccurrencesPtr& presences, const OccurrencesPtr& background, LinearFeature * feature, bool reverse):FeatureGenerator(presences, background, feature)
{
  _type = G_HINGE;
  _reverse = reverse;
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

  double sum = 0.0;
  double wsum1 = 0.0;
  double wsum2 = 0.0;

  double ref_min = 0.0;
  double ref_max = 1.0;
  double margin;
  double i_avg = 0.0;
  double i_std = 0.0;
  double i_lower;
  double i_upper;

  // TMP
  int control = 0;

  for ( int i=(int)_vals.size()-1; i >= 0; --i ) {

    // Is this a sample?
    if ( (_vals.at(i)).first >= limit ) {

      sum += 1.0;
      wsum1 += _vals[i].second;
      wsum2 += pow(_vals[i].second, 2);
      //Log::instance()->debug("i%u (%.13f) -> sample!\n", i, _vals[i].second);
    }
//     else {

//       Log::instance()->debug("i%u (%.13f)-> not a sample (%d)\n", i, _vals[i].second, _vals[i].first);
//     }

    int t_idx = _threshold_index[i];

    if ((t_idx < _first_ref) || (t_idx >= _last_ref)) {

      continue;
    }

    ++control; // TMP

    double avg = ref_max / 2.0;
    double std = ref_max / 2.0;

    if ( num_samples ) {

      double csum1 = (sum - _thresholds[t_idx] * wsum1) / (_maxval - _thresholds[t_idx]);

      double csum2 = (sum - 2.0 * sum * _thresholds[t_idx] + pow(_thresholds[t_idx], 2) * wsum2) / (_maxval - _thresholds[t_idx]) / (_maxval - _thresholds[t_idx]);

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

    if (control <= 2) {

      Log::instance()->debug("TMP i: %u\n",  i);
      Log::instance()->debug("TMP idx: %u\n",  t_idx);
      Log::instance()->debug("TMP max: %.16f\n", ref_max);
      Log::instance()->debug("TMP sum: %.16f\n", sum);
      Log::instance()->debug("TMP wsum1: %.16f\n", wsum1);
      Log::instance()->debug("TMP wsum2: %.16f\n", wsum2);
      Log::instance()->debug("TMP avg: %.16f\n", avg);
      Log::instance()->debug("TMP std: %.16f\n", std);
      Log::instance()->debug("TMP samp_exp: %.16f\n", _samp_exp[t_idx]);
      Log::instance()->debug("TMP samp_dev: %.16f\n", _samp_dev[t_idx]);
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

