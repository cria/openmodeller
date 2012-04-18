/**
 * Definition of ThresholdGenerator class
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

#include "threshold_generator.hh"
#include "threshold_feature.hh"

#include <cmath>

ThresholdGenerator::ThresholdGenerator(const OccurrencesPtr& presences, const OccurrencesPtr& background, LinearFeature * feature):FeatureGenerator(presences, background, feature)
{
  _type = G_THRESHOLD;

  // Get threshold references
  unsigned int i;
  for ( i=0; i < _thresholds.size(); ++i ) {

    if ( _first_samp_val <= _thresholds[i] ) {

      break;
    }
  }

  _first_ref = i;

  for ( i=_thresholds.size(); i > 0; --i ) {

    if ( _last_samp_val > _thresholds[i-1] ) {

      break;
    }
  }

  _last_ref = i;


  Log::instance()->debug("first_ref: %d\n", _first_ref);
  Log::instance()->debug("last_ref: %d\n", _last_ref);
}

ThresholdGenerator::~ThresholdGenerator() {}

void
ThresholdGenerator::setSampExp( double mindev )
{
  Log::instance()->debug("GEN setSampExp\n");
  int limit = _background->numOccurrences();

  int num_samples = _presences->numOccurrences();

  double sum = 0.0;

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

    double avg;
    double std;

    if ( num_samples == 1 ) {

      avg = sum;
      std = ref_max / 2.0;
    }
    else {

      avg = sum / num_samples;

      if ( sum < (num_samples * avg * avg) ) {

        std = 0.0;
      }
      else {

        std = sqrt((sum - num_samples * avg * avg) / (num_samples - 1));
      }

      if ( std > (ref_max/2.0) ) {

        std = ref_max/2.0;
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
      Log::instance()->debug("TMP avg: %.16f\n", avg);
      Log::instance()->debug("TMP std: %.16f\n", std);
      Log::instance()->debug("TMP samp_exp: %.16f\n", _samp_exp[t_idx]);
      Log::instance()->debug("TMP samp_dev: %.16f\n", _samp_dev[t_idx]);
    }
  }
}

void 
ThresholdGenerator::updateExp( double * density, double z_lambda )
{
  double sum = 0.0;

  int limit = _background->numOccurrences();

  for ( int i=(int)_vals.size()-1; i >= 0; --i ) {

    // Is this a background point?
    if ( _vals[i].first < limit ) {

      sum += density[ _vals[i].first ];
    }

    int t_idx = _threshold_index[i];

    if ((t_idx >= _first_ref) && (t_idx < _last_ref)) {

      _exp[t_idx] = (sum / z_lambda);
    }
  }

  for ( int i = _first_ref; i < _last_ref; i++ ) {

    if ( _features[i] != 0 ) {

      _features[i]->setExp( _exp[i] );
    }
  }
}

Feature * 
ThresholdGenerator::exportFeature(int idx)
{
  if ( _features[idx] == 0 ) {

    // Create feature if necessary
    Feature * f = new ThresholdFeature(_feature->getLayerIndex(), _thresholds[idx]);

    f->setSampExp( sampExp(idx) );
    f->setSampDev( sampDev(idx) );
    f->setExp( exp(idx) );
    f->setBeta( _beta );

    _features[idx] = f;
  }

  return _features[idx];
}

