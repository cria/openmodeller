/**
 * Declaration of Bioclimatic Envelope Algorithm - Nix, 1986.
 * 
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

#include <openmodeller/om.hh>

/****************************************************************/
/*********************** Bioclim Distance ***********************/

/** 
 * Bioclimatic Envelope Algorithm with occurrence probability
 * based on the distance to points' mean value.
 *
 */
class Bioclim : public AlgorithmImpl
{
public:

  Bioclim();
  ~Bioclim();

  // Inherited from Algorithm class.
  int initialize();
  int iterate();
  int done() const;

  Scalar getValue( const Sample& x ) const;
  int    getConvergence( Scalar * const val ) const;

protected:
  virtual void _getConfiguration( ConfigurationPtr& ) const;
  virtual void _setConfiguration( const ConstConfigurationPtr & );

private:

  /** compute the statistic member variables based on
      the data in the OccurrencesPtr */
  void computeStats( const OccurrencesPtr& );

  /** Log the bioclimatic envelopes information. */
  void logEnvelope();

  bool _done;  ///> is true if the algorithm is finished.

  Sample _minimum; ///> Mininum value for each variable.
  Sample _maximum; ///> Maximum value for each variable.
  Sample _mean;    ///> Mean of sampled points.
  Sample _std_dev; ///> Standard deviations for each variable.

};


#endif

