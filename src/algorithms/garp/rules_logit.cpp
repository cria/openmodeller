/**
 * Declaration of LogitRule used by GARP
 * 
 * @file   rules_logit.cpp
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-04-02
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 * Copyright(c), David R.B. Stockwell of Symbiotik Pty. Ltd.
 * Copyright(c), CRIA - Centro de Referencia em Informacao Ambiental
 *
 * http://www.nhm.ku.edu
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
 * 
 * This is an implementation of the GARP algorithm first developed 
 * by David Stockwell
 * 
 */


#include <random.hh>
#include <math.h>
#include <string.h>

#include <om_log.hh>

#include "rules_logit.hh"
#include "garp_sampler.hh"


// ==========================================================================
//  LogitRule implelentation
// ==========================================================================
LogitRule::LogitRule() : GarpRule() {}

// ==========================================================================
LogitRule::~LogitRule() {}

// ==========================================================================
void LogitRule::initialize(GarpCustomSampler * sampler)
{
  int i, j;
  double a, b, c;
  Random rnd;
  
  // call inherited initialize
  GarpRule::initialize(sampler);

  for (i = 0; i < _numGenes; i++)
    {
      j = rnd.get(_numGenes);
      
      regression(sampler, j, a, b, c);
      
      // decide where the constant (a) will go;
      _genes[j * 2]     = b; 
      _genes[j * 2 + 1] = c;
    }  
}

// ==========================================================================
int LogitRule::regression(GarpCustomSampler * sampler, int pred, 
			  double& a, double& b, double& c)
{
  double x;         // predictor (independent) variable for current sample
  double y;         // outcome (dependent) variable for sample
  double xx;        // temp storage for x^2
  double s_xi;      // sum of x for all samples
  double s_yi;      // sum of y for all samples
  double s_xiyi;    // sum of x*y for all samples
  double s_xxi;     // sum of x^2 for all samples
  double s_xxiyi;   // sum of y*x^2 for all samples
  double s_xi4;     // sum of x^4 for all samples

  int i;            // index of sample being summed 
  int n;            // total number of samples
  
  Scalar * values, pointValue = 0.0;
  
  n = sampler->resamples();
  
  s_xi = s_yi = s_xiyi = s_xxi = s_xxiyi = s_xi4 = 0.0;
  
  for (i = 0; i < n; i++) 
    {
      values = sampler->getSample(&pointValue);
      
      y = pointValue;
      x = values[pred];
      xx =  x * x;

      s_xi += x;
      s_yi += y;
      s_xxi += xx;
      s_xiyi += x * y;
      s_xxiyi += xx * y;
      s_xi4 += xx * xx;
    }
  
  c = (n * s_xxiyi - s_xxi * s_yi) / (n * s_xi4 - (s_xxi * s_xxi));
  b = (n * s_xiyi - s_xi * s_yi) / (n * s_xxi - (s_xi * s_xi));
  a = s_yi / n - b * s_xi / n;
  
  /*
  g_log("logit: a=%+8.4f b=%+8.4f c=%+8.4f n=%d\n", a, b, c, n);
  g_log("aux: xi=%+8.4f yi=%+8.4f xiyi=%+8.4f xxi=%+8.4f xxiyi=%+8.4f xi4=%+8.4f\n", 
	s_xi, s_yi, s_xiyi, s_xxi, s_xxiyi, s_xi4);
  */
  
  return n;
}

// ==========================================================================
bool LogitRule::applies(Scalar * values)
{ 
  return (getStrength(values) == 1); 
}

// ==========================================================================
int LogitRule::getStrength(Scalar * values)
{
  int i;
  Scalar Sum, prob, r;
  
  Sum = 0.0;
  prob = 0.0;
  
  for (i = 0; i < _numGenes; i++)
    {
      //g_log("i:%2d ", i);
      
      if (!equalEps(_genes[i * 2], -1.0))
	{
	  r = values[i];
    	  
	  Sum += _genes[i * 2] * r;
	  Sum += _genes[i * 2 + 1] * r * r;
	  
	  //g_log("gene:%7.4f r:%7.4f v:%7.4f Sum:%7.4f\n ", _genes[i * 2], r, values[i], Sum);
	}

      //Sum += _constant;
    }
  
  prob = 1.0 / (1.0 + (double) exp(-Sum));
  //g_log("prob: %7.4f\n", prob);

  return (prob >= 0.5);
}

// ==========================================================================
bool LogitRule::similar(GarpRule * objRule)
{
  int ct, k;
  
  LogitRule * objOtherRule = (LogitRule *) objRule;
  
  if (type() == objOtherRule->type())
    {
      // check rule value (presence/absence)
      if (_prediction != objOtherRule->_prediction) 
	return 0;
      
      ct = 0;
      for (k = 0; k < _numGenes * 2; k ++)
	{ ct += fabs(_genes[k] - objOtherRule->_genes[k]) < 0.2; }
      
      // rule is similar if more than half of the aleles
      // are within 0.2 distance of each other
      return (ct > _numGenes);
    }
  
  return false;
}

// ==========================================================================
void LogitRule::log()
{
  g_log( "Logit: " );
  GarpRule::log();
}

// ==========================================================================
