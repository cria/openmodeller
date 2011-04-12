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
  float getProgress() const;
  int done() const;
  Scalar getValue( const Sample& x ) const;
  int getConvergence( Scalar * const val );

private:

  void initTrainer();

  void endTrainer();

  void setLinearPred();

  void setLinearNormalizer();

  void calcDensity();

  double interpol( char type_feat );

  double getAlpha( int feature_index );

  double searchAlpha( int feature_index, double alpha );

  double lossChange( int feature_index, double alpha );

  double runNewtonStep( int feature_index );

  double getDeriv( int feature_index );

  bool terminationTest(double newLoss);

  double decreaseAlpha(double alpha);

  double increaseLambda(int feature_index, double alpha);

  double getLoss();

  // Dump the given maxent vars related to a specfic iteration
  void displayInfo(int best_id, double loss_bound, double new_loss, double delta_loss, double alpha);

  double *_f_mean; // mean of each feature
  double *_f_std; // standard deviation of each feature
  double *_f_samp_exp; // sample expectation
  double *_f_samp_dev; // sample deviation
  double *_f_exp; // feature expectation
  double *_f_lambda; // weight of each feature
  double *_linear_pred; // probability of each point
  double _linear_normalizer;/// linear normalizer
  double *_density; // density for each point
  double _reg;
  double _z_lambda;
  double _entropy;
 
protected:

  virtual void _getConfiguration( ConfigurationPtr& ) const;

  virtual void _setConfiguration( const ConstConfigurationPtr& );

  bool _done;

  OccurrencesPtr _presences;
  OccurrencesPtr _background;

  Sample _min;
  Sample _max;

  int _num_layers;
  int _num_presences;
  int _num_background;

  int _len;
  double _beta_l;

  int _max_iterations;
  int _iteration;
  double _previous_loss;
  double _new_loss;
  double _old_loss;
  double _gain;
  int _convergence_test_frequency;

  double _tolerance;
  int _output_format;
};

#endif

