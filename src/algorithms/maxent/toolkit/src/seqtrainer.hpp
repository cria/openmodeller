/**
 * Declaration of a conditional ME trainer using a sequential method.Modified from gistrainer.hpp 
 * 
 * @author Elisangela S. da C. Rodrigues (elisangela . rodrigues [at] poli . usp . br)
 * $Id $
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

#ifndef SEQTRAINER_H
#define SEQTRAINER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "trainer.hpp"

namespace maxent{
  
  class SEQTrainer : public Trainer {
  public:
    
    void train(size_t iter = 500, double tol = 1E-05);
    
  private:
    void init_trainer();

    void calc_q_lambda_x();

    void calc_q_lambda_f();

#if !defined(_STLPORT_VERSION) && defined(_MSC_VER) && (_MSC_VER >= 1300)
    // for MSVC7's hash_map declaration
    class featid_hasher : public stdext::hash_compare<pair<size_t, size_t> > {
    public:
      size_t operator()(const pair<size_t, size_t>& p) const {
	return p.first + p.second;
      }
      
      bool operator()(const pair<size_t, size_t>& k1,
		      const  pair<size_t, size_t>& k2) {
	return k1 < k2;
      }
    };
#else
    // for hash_map of GCC & STLPORT
    struct featid_hasher {
      size_t operator()(const pair<size_t, size_t>& p) const {
	return p.first + p.second;
      }
    };
    
#endif
    
    shared_array<double> regularization_parameters;

    vector<double > q_lambda_x;

    double *q_lambda_f;
    
    double *features_mean;

  };
  
} // namespace maxent

#endif /* ifndef SEQTRAINER_H */
