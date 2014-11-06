/**
 * Declaration of Virtual Niche Generator.
 * 
 * @author Renato De Giovanni <renato at cria.org.br>
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2012 by CRIA -
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

#ifndef _VIRTUALNICHEHH_
#define _VIRTUALNICHEHH_

#include <openmodeller/om.hh>

/************************************************************/
/*********************** VirtualNiche ***********************/

/** 
 * Virtual Niche Generator algorithm
 *
 */
class VirtualNicheGenerator : public AlgorithmImpl
{
public:

  VirtualNicheGenerator();
  ~VirtualNicheGenerator();

  int initialize();
  int iterate();
  int done() const;

  Scalar getValue( const Sample& x ) const;
  int    getConvergence( Scalar * const val ) const;

protected:
  virtual void _getConfiguration( ConfigurationPtr& ) const;
  virtual void _setConfiguration( const ConstConfigurationPtr & );

private:

  void dump();

  // Probability density function for a Gaussian distribution
  Scalar pdf(Scalar avg, Scalar std, Scalar val) const;

  bool _done;  ///> true if the algorithm is finished.

  OccurrencesPtr _background;
  int _num_background;

  Sample _minimum;   ///> Mininum value for each variable.
  Sample _maximum;   ///> Maximum value for each variable.
  Sample _mean;      ///> Average for the normal distribution.
  Sample _std;       ///> Standard deviation for the normal distribution.
  Sample _scale;     ///> Factors to multiply the PDF values.
  Scalar _threshold; ///> Threshold to get a binary niche.
  Scalar _std_factor;///> Factor used to ensure larger random standard deviations. [0.0, 1.0]
};


#endif //VIRTUALNICHEHH
