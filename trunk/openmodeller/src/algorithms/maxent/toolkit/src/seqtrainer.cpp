/*
 * seqtrainer.cpp  -  trainer for conditional ME with a sequential method
 *
 * @author Elisangela S. da C. Rodrigues (elisangela . rodrigues [at] poli . usp . br)
 * * $Id $
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
 
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cassert>
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <boost/timer.hpp>
#include "seqtrainer.hpp"
#include "finite.h"

namespace maxent{

void
SEQTrainer::init_trainer()
{
  assert( m_params->size() > 0 ); // num_features
  assert( m_es->size() > 0 ); // num_examples

  regularization_parameters.reset( new double[m_params->size()] );

  // initialize all regularization_parameters with 0.0
  std::fill( regularization_parameters.get(), regularization_parameters.get() + m_params->size(), 0.0 );

  features_mean = new double[m_params->size()]; // mean of each feature
  std::vector< double > feat_stan_devi( m_params->size() );// standard deviation of each feature
  q_lambda_f = new double[m_params->size()];

  // initialize all thetas with 0.0
  for ( size_t i = 0; i < m_params->size(); ++i ) {
    std::vector<pair<size_t, size_t> >& param = (*m_params)[i];

    for (size_t j = 0; j < param.size(); ++j) {
      m_theta[param[j].second] = 0.0; // the lenght of m_theta is m_n_theta

    }
  }
  
  m_N = 0;
  
  // calculate observed feature expectations - pi~[f] (empirical average of f)

  typedef hash_map <pair<size_t, size_t>, float, featid_hasher> FeatSumMap;
  FeatSumMap feat_sum;
  
  for ( vector<Event>::const_iterator it = m_es->begin(); it != m_es->end(); ++it ) {
     size_t len = it->context_size();
    
    for (size_t i = 0; i < len; ++i) {

      // sum the features values and store them according to the feature and the outcome, that is, feature 0 with outcome 0, feature 0 with outcome 1, feature 1 with outcome 0, feature 1 with outcome 1 etc.
      feat_sum[make_pair(it->m_context[i].first,it->m_outcome)] +=
	
	it->m_count * it->m_context[i].second;

      // sum the features values to calculate the mean after.
      features_mean[i] += it->m_count * it->m_context[i].second;
    }
    m_N += it->m_count; // m_N is the number of occurrence points
  }

  // calculate the features mean
  for ( size_t pid = 0; pid < m_params->size(); ++pid ) {
    features_mean[pid] /= m_N;
  }

  // calculate the variance of each feature
  for ( vector<Event>::const_iterator it = m_es->begin(); it != m_es->end(); ++it ) {
    size_t len = it->context_size();
    
    for (size_t i = 0; i < len; ++i) {
      feat_stan_devi[i] += pow((it->m_context[i].second - features_mean[i]),2);
    }
  }

  // calculate the standard deviation of each feature
  for ( size_t i = 0; i < m_params->size(); ++i ) {
    feat_stan_devi[i] = sqrt((1.0/(m_N-1))*feat_stan_devi[i]);
  }

  // initialize the regularization parameters
  for ( size_t i = 0; i < m_params->size(); ++i ) {
    regularization_parameters[i] = (0.1 * feat_stan_devi[i]) / sqrt(m_N);
  }

} //init_trainer();
  
void
SEQTrainer::calc_q_lambda_x()
{
  double Z_lambda = 0.0;

  for ( vector<Event>::iterator it = m_es->begin(); it != m_es->end(); ++it ) {
    double sum_lambdaj_fj = 0.0;

    // calculate the sum of each weight multiplied by its corresponding feature value.
    for ( size_t j = 0; j < it->context_size(); ++j ) {

      double fval = it->m_context[j].second; // feature value
      std::vector<pair<size_t, size_t> >& param = (*m_params)[j];

      sum_lambdaj_fj += m_theta[param[1].second] * fval; // exponent sum
    }
    
    q_lambda_x.push_back(exp(sum_lambdaj_fj));
    Z_lambda += q_lambda_x.back(); // normalization constant
  }

  // normalize all q_lambda_x  
  for ( size_t j = 0; j < q_lambda_x.size(); ++j ) {
    q_lambda_x[j] /= Z_lambda;
  }
}

void
SEQTrainer::calc_q_lambda_f()
{
  size_t i = 0;

  for ( vector<Event>::iterator it = m_es->begin(); it != m_es->end(); ++it ) {

    for ( size_t j = 0; j < it->context_size(); ++j ) {

      double fval = it->m_context[j].second; // feature value

      q_lambda_f[j] += q_lambda_x[i] * fval;
    }
    ++i;
  }
}

void
SEQTrainer::train(size_t iter, double tol)
{
  if ( !m_params || !m_es ) {
    throw runtime_error("Can not train on an empty model");
  }

  init_trainer();

  vector<double> q(m_n_outcomes); // m_n_outcomes is the number of outcomes
  double F = 0.0;
  double min_F = 9999.0;
  size_t id_best_feature = 0;
  size_t id_best_alfa = 0;
  vector<double> alfa(3*m_params->size());

  for ( size_t niter = 0; niter < iter; ++niter ) {
    
    q_lambda_x.clear();
    calc_q_lambda_x();
    calc_q_lambda_f();

    for ( size_t i = 0; i < m_params->size(); ++i ) {

      std::vector<pair<size_t, size_t> >& param = (*m_params)[i];
      
      if ( m_theta[param[1].second] != 0.0 ) {
	alfa[3*i] = -(m_theta[param[1].second]);
	
	F = -( alfa[3*i] * features_mean[i] )
	    + log( 1 + ( exp(alfa[3*i]) - 1 ) * q_lambda_f[i] )
	    + regularization_parameters[i] * ( fabs(m_theta[param[1].second]
	    + alfa[3*i]) - fabs(m_theta[param[1].second]) );

	if ( min_F > F ){
	  min_F = F;
	  id_best_feature = i;
	  id_best_alfa = 3*i;
	}
      }

      alfa[3*i+1] = log ((( features_mean[i] - regularization_parameters[i] )
		    * ( 1 - q_lambda_f[i] )) / (( 1 - features_mean[i]
		    + regularization_parameters[i] ) * q_lambda_f[i]));

      if ( (m_theta[param[1].second] + alfa[3*i+1]) >= 0.0 ) {
	
	F = -( alfa[3*i+1] * features_mean[i] )
	    + log( 1 + ( exp(alfa[3*i+1]) - 1 ) * q_lambda_f[i] )
	    + regularization_parameters[i] * ( fabs(m_theta[param[1].second]
            + alfa[3*i+1]) - fabs(m_theta[param[1].second]) );
	
	if ( min_F > F ){
	  min_F = F;
	  id_best_feature = i;
	  id_best_alfa = 3*i+1;
	}
      }

      alfa[3*i+2] = log ((( features_mean[i] + regularization_parameters[i] )
		    * ( 1 - q_lambda_f[i] )) / (( 1 - features_mean[i]
		    - regularization_parameters[i] ) * q_lambda_f[i]));

      if ( (m_theta[param[1].second] + alfa[3*i+2]) <= 0.0 ) {

	F = -( alfa[3*i+2] * features_mean[i] )
	    + log( 1 + ( exp(alfa[3*i+2]) - 1 ) * q_lambda_f[i] )
	    + regularization_parameters[i] * ( fabs(m_theta[param[1].second]
            + alfa[3*i+2]) - fabs(m_theta[param[1].second]) );
	
	if ( min_F > F ){
	  min_F = F;
	  id_best_feature = i;
	  id_best_alfa = 3*i+2;
	}
      }
      q_lambda_f[i] = 0.0; // clear q_lambda_f for next iteration
    }

    m_theta[id_best_feature] += alfa[id_best_alfa];

  } // END for ( size_t niter = 0; niter < iter; ++niter )
} // train
} // namespace maxent
