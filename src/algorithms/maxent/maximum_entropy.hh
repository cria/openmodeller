/**
 * Declaration of Maximum Entropy algorithm class.
 * 
 * @author Elisangela S. da C. Rodrigues (elisangela.rodrigues [at] poli.usp.br)
 * @date   2008-02-24
 * $Id: maximum_entropy.hh $//////////////////////////COMPLETAR////////////////
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) ///////////////////////COMPLETAR///////////////////////
 *
 * http://www.poli.usp.br ////////////////////VER///////////////////
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

#ifndef _MAXIMUM_ENTROPYHH_
#define _MAXIMUM_ENTROPYHH_

#include <openmodeller/om.hh>

#include "maxent.h"

/*********************************************************/
/******************** Maximum Entropy ********************/

/*
 * Algorithm to generate models based on Maximum Entropy.
 */
class MaximumEntropy : public AlgorithmImpl
{
public:

  MaximumEntropy();

  ~MaximumEntropy();

  int initialize();
  int iterate();
  int done() const;

  void add_samples();

  Scalar getValue( const Sample& x ) const;

  //int getConvergence( Scalar *val );

protected:

  virtual void _getConfiguration( ConfigurationPtr& ) const;

  virtual void _setConfiguration( const ConstConfigurationPtr& );

  bool _done;

  int num_layers;

  ME_Model model;

  OccurrencesPtr absences;
  OccurrencesPtr presences;
};

#endif

