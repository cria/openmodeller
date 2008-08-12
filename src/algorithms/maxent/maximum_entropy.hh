/**
 * Declaration of Maximum Entropy algorithm class.
 * 
 * @author Elisangela S. da C. Rodrigues (elisangela . rodrigues [at] poli . usp . br)
 * @author Renato De Giovanni (renato [at] cria . org . br)
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2007 by CRIA -
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

#ifndef _MAXIMUM_ENTROPYHH_
#define _MAXIMUM_ENTROPYHH_

#include <openmodeller/om.hh>

#include "maxentmodel.hpp"

using namespace maxent;

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

  int getConvergence( Scalar *val );

protected:

  virtual void _getConfiguration( ConfigurationPtr& ) const;

  virtual void _setConfiguration( const ConstConfigurationPtr& );

  bool _done;

  OccurrencesPtr _presences;
  OccurrencesPtr _absences;

  bool _hasCategorical;
  Sample _isCategorical;

  int _num_layers;

  MaxentModel _model;

  int _num_iterations;
  std::string _method;
  double _gaussian_coef;
  double _tolerance;
  int _linear_feat;
  int _quadratic_feat;
  int _threshold_feat;
  int _hinge_feat;
  int _product_feat;
  int _auto_feat;
  std::string _featstring;
};

#endif

