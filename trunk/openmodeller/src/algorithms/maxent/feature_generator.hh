/**
 * Declaration of Feature Generator class
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

#ifndef _FEATUREGENERATOR_HH
#define _FEATUREGENERATOR_HH

#include <vector>
using std::pair;

#include "feature.hh"
#include "linear_feature.hh"
#include <openmodeller/Occurrences.hh>

#define G_HINGE 1
#define G_THRESHOLD 2

#define G_MAXDIGITS 6

class FeatureGenerator {

public:

  FeatureGenerator(const OccurrencesPtr& presences, const OccurrencesPtr& background, LinearFeature * feature, int type, bool reverse);

  ~FeatureGenerator();

  int type() {return _type;}

  virtual void setSampExp( double mindev ) = 0;

  virtual void updateExp( double * density, double z_lambda ) = 0;

  Feature * getFeature(int idx) {return _features[idx];}

  Feature * toFeature(int idx);

  virtual Feature * exportFeature(int idx) = 0;

  void setBeta( Scalar beta ){_beta = beta;}

  Scalar beta() {return _beta;}

  Scalar exp(int idx) {return _exp[idx];}

  Scalar sampExp(int idx) {return _samp_exp[idx];}

  Scalar sampDev(int idx) {return _samp_dev[idx];}

  Scalar lambda(int idx);

  int getFirstRef() const { return _first_ref; }

  int getLastRef() const { return _last_ref; }

protected:

  Scalar getVal(Sample s);

  // Custom struct to sort pair by second value
  struct by_second_value {

    bool operator()(const pair<int,Scalar> &left, const pair<int,Scalar> &right) {
        return left.second < right.second;
    }
  };

  Scalar getPrecision(Scalar val);

  int _type;

  bool _reverse;

  OccurrencesPtr _presences;
  OccurrencesPtr _background;
  LinearFeature * _feature;

  Scalar _beta;
  
  int _first_ref;
  int _last_ref;

  std::vector<Feature*> _features;

  std::vector<Scalar> _exp;
  std::vector<Scalar> _samp_exp;
  std::vector<Scalar> _samp_dev;

  std::vector< pair<int, Scalar> > _vals; // position, feature value

  std::vector<Scalar> _thresholds;
  std::vector<Scalar> _threshold_index;

  Scalar _first_samp_val;
  Scalar _last_samp_val;
};

#endif
