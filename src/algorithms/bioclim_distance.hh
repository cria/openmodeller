/**
 * Declaration of Bioclimatic Envelope Algorithm - Nix, 1986.
 * 
 * @file
 * @author Mauro Muñoz <mauro@cria.org.br>
 * @date 2004-05-05
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2004 by CRIA -
 * Centro de Referência em Informação Ambiental
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

/****************************************************************/
/*********************** Bioclim Distance ***********************/

/** 
 * Bioclimatic Envelope Algorithm with occurrence probability
 * based on the distance to points' mean value.
 *
 */
class BioclimDistance : public AlgorithmImpl
{
public:

  BioclimDistance();
  ~BioclimDistance();

  // Need normalization to calculate distance.
  int needNormalization( Scalar *min, Scalar *max ) const;

  // Inherited from Algorithm class.
  int initialize();
  int iterate();
  int done() const;

  Scalar getValue( const Sample& x ) const;
  int    getConvergence( Scalar *val );

protected:
  virtual void _getConfiguration( ConfigurationPtr & ) const;
  virtual void _setConfiguration( const ConstConfigurationPtr & );

private:

  /** Calculates the average and standard deviation.
   *  There must be at least one point. */
  void computeStats( const OccurrencesPtr& );

  bool _done;  ///> true if the model has been generated.

  Scalar _max_distance; ///> Standard deviation vector module.
  Sample _minimum; ///> Minimum value for each variable.
  Sample _maximum; ///> Maximum value for each variable.
  Sample _mean;    ///> Mean of sampled points.
  Sample _std_dev; ///> Standard deviations for each variable.
};


#endif

