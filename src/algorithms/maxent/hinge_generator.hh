/**
 * Declaration of Hinge Generator class
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

#ifndef _HINGEGENERATOR_HH
#define _HINGEGENERATOR_HH

#include "feature_generator.hh"

class HingeGenerator : public FeatureGenerator {

public:

  HingeGenerator(const OccurrencesPtr& presences, const OccurrencesPtr& background, LinearFeature * feature, bool reverse);

  ~HingeGenerator();

  void setSampExp( double mindev );

  void updateExp( double * density, double z_lambda );

  Feature * exportFeature(int idx);

private:

  Scalar _maxval;
  bool _reverse;
};

#endif
