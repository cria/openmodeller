/**
 * Declaration of Maximum Entropy algorithm class.
 * 
 * @author Elisangela S. da C. Rodrigues (elisangela . rodrigues [at] poli . usp . br)
 * @author Renato De Giovanni (renato [at] cria . org . br)
 * @author Daniel Bolgheroni (daniel [at] cria . org . br)
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

#include <vector>
using std::pair;

#include <openmodeller/om.hh>

#include "feature.hh"

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
  double sequentialProc();
  double lossBound( Feature * f );
  double parallelProc();
  void setLinearPred();
  void setLinearNormalizer();
  double calcBeta( Feature * f );
  void calcDensity();
  void calcDensity( vector<Feature*> to_update );
  double getAlpha( Feature * f );
  double searchAlpha( Feature * f, double alpha );
  double lossChange( Feature * f, double alpha );
  double runNewtonStep( Feature * f );
  double getDeriv( Feature * f );
  bool terminationTest( double newLoss );
  double decreaseAlpha( double alpha );
  void updateReg();
  void updateReg( Feature * f, double alpha );
  double increaseLambda( Feature * f, double alpha, vector<Feature*> to_update );
  double increaseLambda( double* alpha, vector<Feature*> to_update );
  double getLoss();
  vector<Feature*> featuresToUpdate();

  // Dump the given maxent vars related to a specfic iteration
  void displayInfo( Feature * f, double loss_bound, double new_loss, double delta_loss, double alpha );

  double *_linear_pred; // probability of each point
  double _linear_normalizer;/// linear normalizer
  double *_density; // density for each point
  double _reg;
  double _z_lambda;
  double _entropy;

protected:

  virtual void _getConfiguration( ConfigurationPtr& ) const;
  virtual void _setConfiguration( const ConstConfigurationPtr& );

  // Custom struct to sort pair by second value
  struct by_value {

    bool operator()(const pair<int,double> &left, const pair<int,double> &right) {
        return left.second < right.second;
    }
  };

  bool _done;

  OccurrencesPtr _presences;
  OccurrencesPtr _background;

  int _num_presences;
  int _num_background;

  int _max_iterations;
  int _iteration;
  int _parallelUpdateFreq;
  int _change;
  int _updateInterval;
  int _select;
  double _previous_loss;
  double _new_loss;
  double _old_loss;
  double _gain;
  int _convergence_test_frequency;

  double _tolerance;
  int _output_format;

  bool _quadratic;
  bool _product;
  bool _hinge;
  bool _threshold;

  vector<Feature*> _features;
};

#endif
