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

#include "feature.hh"
#include "linear_feature.hh"
#include <openmodeller/Occurrences.hh>

#define G_HINGE 1
#define G_THRESHOLD 2

#define G_MAXDIGITS 6

class FeatureGenerator {

public:

  FeatureGenerator(const OccurrencesPtr& presences, const OccurrencesPtr& background, LinearFeature * feature);

  ~FeatureGenerator();

  Scalar type() {return _type;}

  Feature * getFeature(int idx) {return _features[idx];}

  Feature * toFeature(int idx);

  virtual Feature * exportFeature(int idx) = 0;

  void setBeta( Scalar beta ){_beta = beta;}

  Scalar beta() {return _beta;}

  Scalar exp(int idx) {return _exp[idx];}

  Scalar sampExp(int idx) {return _samp_exp[idx];}

  Scalar sampDev(int idx) {return _samp_dev[idx];}

  Scalar lambda(int idx);

  int getFirstRef() const { return _firstRef; }

  int getLastRef() const { return _lastRef; }

protected:

  Scalar _getPrecision(Scalar val);

  int _type;

  OccurrencesPtr _presences;
  OccurrencesPtr _background;
  LinearFeature * _feature;

  Scalar _beta;
  
  int _firstRef;
  int _lastRef;

  std::vector<Feature*> _features;

  std::vector<Scalar> _exp;
  std::vector<Scalar> _samp_exp;
  std::vector<Scalar> _samp_dev;

  std::vector<Scalar> _thresholds;
};

#endif
