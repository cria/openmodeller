/**
 * Definition of dummy custom sampler for for garp unit tests
 * 
 * @file   test_rules_dummy_sampler.cpp
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-04-07
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 * Copyright(c), CRIA - Centro de Referencia em Informacao Ambiental
 *
 * http://www.nhm.ku.edu
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
 * 
 */

#include <garp_sampler.hh>

SampledData::SampledData() {}
SampledData::~SampledData() {}
GarpCustomSampler::GarpCustomSampler() {}
GarpCustomSampler::~GarpCustomSampler() {}
void GarpCustomSampler::initialize(const SamplerPtr&, int) {}
int GarpCustomSampler::resamples() {}
Scalar * GarpCustomSampler::getSample(Scalar *) { g_log("#### dummy getSample() called ####\n"); }
int * GarpCustomSampler::getFrequencies(double, int *) {} 
void GarpCustomSampler::createBioclimHistogram(double, double, double) {}
void GarpCustomSampler::getBioclimRange(Scalar, int, Scalar *, Scalar *) {}
int GarpCustomSampler::dim() { g_log("#### dummy dim() called ####\n"); }
