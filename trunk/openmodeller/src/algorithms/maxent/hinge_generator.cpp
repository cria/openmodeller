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

HingeGenerator::HingeGenerator(const OccurrencesPtr& presences, const OccurrencesPtr& background, LinearFeature * feature):FeatureGenerator(presences, background, feature)
{
  _type = G_HINGE;
}

HingeGenerator::~HingeGenerator() {}

Feature * 
HingeGenerator::exportFeature(int idx)
{
  if ( _features[idx] == 0 ) {

    // Create feature if necessary
      Feature * f = new HingeFeature(_feature->getLayerIndex(), 0.0, 1.0);

    f->setSampExp( sampExp(idx) );
    f->setSampDev( sampDev(idx) );
    f->setExp( exp(idx) );
    f->setBeta( _beta );

    _features[idx] = f;
  }

  return _features[idx];
}

