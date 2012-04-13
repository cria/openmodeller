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
}

ThresholdGenerator::~ThresholdGenerator() {}

void
ThresholdGenerator::setSampExp( double mindev )
{
  int limit = _background->numOccurrences();

  int num_samples = _presences->numOccurrences();

  double sum1 = 0.0;
  double sum2 = 0.0;

  double ref_min = 0.0;
  double ref_max = 1.0;
  double margin;
  double i_avg = 0.0;
  double i_std = 0.0;
  double i_lower;
  double i_upper;

  std::vector< pair<int, double> >::iterator vit;
  unsigned int i = _vals.size();
  for ( vit = _vals.end(); vit != _vals.begin(); --vit,--i ) {

    // Is this a sample?
    if ( vit->first >= limit ) {

      double val = 1.0 / vit->second;
      sum1 += val;
      sum2 += val * val;
    }

    int t_idx = _threshold_index[i];

    if ((t_idx < _first_ref) || (t_idx >= _last_ref)) {

      continue;
    }

    double avg;
    double std;

    if ( num_samples == 1 ) {

      avg = sum1;
      std = ref_max / 2.0;
    }
    else {

      avg = sum1 / num_samples;

      if ( sum2 < num_samples * avg * avg ) {

        std = 0.0;
      }
      else {

        std = sqrt((sum2 - num_samples * avg * avg) / (num_samples - 1));
      }

      if (std > ref_max / 2.0) {

        std = ref_max / 2.0;
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
ThresholdGenerator::updateExp( double * density, double z_lambda )
{
  double sum = 0.0;

  int limit = _background->numOccurrences();

  std::vector< pair<int, double> >::iterator vit;
  unsigned int i = _vals.size();
  for ( vit = _vals.end(); vit != _vals.begin(); --vit,--i ) {

    // Is this a background point?
    if ( vit->first < limit ) {

      sum += density[ vit->first ];
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

