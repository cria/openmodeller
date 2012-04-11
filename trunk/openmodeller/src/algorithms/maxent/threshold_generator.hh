/**
 * Declaration of Threshold Generator class
 * 
 * @author Renato De Giovanni
 * $Id: $
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

#ifndef _THRESHOLDGENERATOR_HH
#define _THRESHOLDGENERATOR_HH

#include "feature_generator.hh"

class ThresholdGenerator : public FeatureGenerator {

public:

  ThresholdGenerator(const OccurrencesPtr& presences, const OccurrencesPtr& background, Feature * feature);

  ~ThresholdGenerator();
};

#endif
