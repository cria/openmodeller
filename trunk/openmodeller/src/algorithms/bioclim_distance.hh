/**
 * Declaration of Bioclimatic Envelope Algorithm - Nix, 1986.
 * 
 * @file
 * @author Mauro Mu�oz <mauro@cria.org.br>
 * @date 2004-05-05
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2004 by CRIA -
 * Centro de Refer�ncia em Informa��o Ambiental
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

#ifndef _BIOCLIMHH_
#define _BIOCLIMHH_

#include <om.hh>

class Serializer;
class Deserializer;


/****************************************************************/
/*********************** Bioclim Distance ***********************/

/** 
 * Bioclimatic Envelope Algorithm with occurrence probability
 * based on the distance to points' mean value.
 *
 */
class BioclimDistance : public Algorithm
{
public:

  BioclimDistance();
  ~BioclimDistance();

  // Need normalization to calculate distance.
  int needNormalization( Scalar *min, Scalar *max );

  // Inherited from Algorithm class.
  int initialize();
  int iterate();
  int done();

  Scalar getValue( Scalar *x );
  int    getConvergence( Scalar *val );

  /*
   */
  int serialize(Serializer * serializer);
  int deserialize(Deserializer * deserializer);

private:

  /** Calculates the average point for all SampledData points.
   *  There must be at least one point. */
  Scalar *getMean( SampledData *points );

  /** Calculates the standard deviation independently for each
   *  dimension of 'points'.
   *  There must be at least two points.
   */
  Scalar *getStandardDeviation( SampledData *points,
                                Scalar *mean );


  int _dim;   ///> Number of dimensions in environmental space

  Scalar *_mean;    ///> Mean of sampled points.
  Scalar *_std_dev; ///> Standard deviations for each variable.
  Scalar _max_distance; ///> Standard deviation vector module.
};


#endif

