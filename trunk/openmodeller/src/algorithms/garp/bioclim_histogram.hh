/**
 * Declaration of GARP custom sampler class
 * 
 * @file   garp_sampler.hh
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-04-01
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 * Copyright(c), David R.B. Stockwell of Symbiotik Pty. Ltd.
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
 * This is an implementation of the GARP algorithm first developed 
 * by David Stockwell
 * 
 */


#ifndef _BIOCLIM_HISTOGRAM_HH_
#define _BIOCLIM_HISTOGRAM_HH_

#include <openmodeller/om.hh>

#define HIST_BOX_RESOLUTION           0.01
#define BIOCLIM_LEVEL_RESOLUTION      0.005
#define BIOCLIM_MAX_LEVEL             0.2

#define MAX_ENV_LAYERS                256

// ===========================================================================
//  Declaration and Implementation of class BioclimHistogram
// ===========================================================================
class BioclimHistogram
{
public:
  BioclimHistogram();
  ~BioclimHistogram();

  int resamples() const;

  void initialize(const OccurrencesPtr& occs);

  void getBioclimRange(Scalar prediction, int layerIndex, 
		       Scalar& minCutLevel, Scalar& maxCutLevel) const;

private:
  void reset();

  int _resamples;

  int _matrix[2][MAX_ENV_LAYERS][256];
  int _depend[2];

  // bioclim histogram
  double * _upperLevels;
  double * _lowerLevels;
};

/****************************************************************/

#endif

