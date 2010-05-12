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

#include <set>
#include <map>

#include <openmodeller/om.hh>

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

  int needNormalization();
  int initialize();
  int iterate();
  int done() const;

  void add_samples();

  Scalar getValue( const Sample& x ) const;

  int getConvergence( Scalar * const val );

private:

  void init_trainer();
  void end_trainer();

  void calc_q_lambda_x();
  
  void calc_q_lambda_f();

  double interpol( char type_feat );

  // Dump the given maxent vars related to a specfic iteration
  void displayInfo(int best_id, double new_loss, double delta_loss, double alpha, int iteration);
  bool terminationTest(double newLoss);
  double reduceAlpha(double alpha);
  double getLoss();

  double *regularization_parameters;
  double *features_mean;
  double *feat_stan_devi;
  double *q_lambda_f;
  double *lambda;
  double *q_lambda_x;

  double Z_lambda;
  double entropy;
 
protected:

  virtual void _getConfiguration( ConfigurationPtr& ) const;

  virtual void _setConfiguration( const ConstConfigurationPtr& );

  bool _done;

  OccurrencesPtr _presences;
  OccurrencesPtr _background;

  Sample _is_categorical;
  Sample _min;
  Sample _max;

  int _num_layers;
  int _num_presences;
  int _num_background;
  int _num_samples;
  int _num_values_cat;
  int _len;
  double beta_l;
  double beta_c;
  bool _hasCategorical;

  int _max_iterations;
  int iteration;
  double previousLoss;
  double old_loss;
  int convergenceTestFrequency;

  double _tolerance;
  int _output_format;

  std::map< int, std::set<Scalar> > _cat_values; // layer index => set of values (used in getValue)
  std::map< int, std::map< Scalar, std::vector< bool > > > _categorical_values; // layer index => set of values => binary values of samples according to feature values.
};

#endif

