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
  double constant, coef[2];
  Random rnd;
  
  // call inherited initialize
  GarpRule::initialize(sampler);

  for (i = 0; i < _numGenes; i++)
    {
      j = rnd.get(_numGenes);
      
      regression(sampler, j, constant, coef[0], coef[1]);
      
      // decide where the constant will go;
      _genes[j * 2]     = coef[0]; 
      _genes[j * 2 + 1] = coef[1];
    }  
}

// ==========================================================================
int LogitRule::regression(GarpCustomSampler * sampler, int dep, 
			  double& constant, double& coef1, double& coef2)
{
  double a, b, x, y, xi, yi, xiyi, xi2, xb, xx, xxi, xxiyi, xxi2;
  int i, n, pred;
  Scalar * values, pointValue = 0.0;
  
  n = sampler->resamples();
  pred = 0;
  
  a = b = x = y = xi = yi = xiyi = xi2 = xb = xx = xxi = xxiyi = xxi2 = 0.0;
  
  for (i = 0; i < n; i++) 
    {
      values = sampler->getSample(&pointValue);
      
      y = pointValue;
      x = values[dep];
      
      xx =  x * x;
      xi += x;
      yi += y;
      xiyi += x * y;
      xi2 += x * x;
      
      xxi += xx;
      xxiyi += xx * y;
      xxi2 += xx * xx;
    }
  
  b = (n * xiyi - xi * yi) / (n * xi2 - (xi * xi));
  coef1 = ((b * 2.0) - 1.0);
  
  a = yi / n - b * xi / n;
  constant = a;
  xb = (n * xxiyi - xxi * yi) / (n * xxi2 - (xxi * xxi));
  coef2 = ((xb * 2.0) - 1.0);
  
  //printf("logit: %+8.4f %+8.4f %+8.4f %+8.4f %+8.4f %d\n",
  // a, b, xb, coef1, coef2, n);
  
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

      if (membership(_genes[i * 2], _genes[i * 2 + 1], 1) != 255) 
	    {
	      r = ((values[i] + 1.0) / 2.0);
    	  
	      Sum += (_genes[i * 2] + 1.0) * r;
	      Sum += (_genes[i * 2 + 1] + 1.0) * r * r;

        //g_log("gene:%7.4f r:%7.4f v:%7.4f Sum:%7.4f ", _genes[i * 2], r, values[i], Sum);
	    }
      
      prob = 1.0 / (1.0 + (double) exp(-Sum));
      
      //g_log("prob: %7.4f\n", prob);
    }

  //g_log("\n");
  
  if (prob > 0.5)
    return 1;
  else
    return 0;
}

// ==========================================================================
bool LogitRule::similar(GarpRule * objRule)
{
  bool found;
  int k;
  
  LogitRule * objOtherRule = (LogitRule *) objRule;
  
  if (type() == objOtherRule->type())
    {
      // check rule value (presence/absence)
      if (_prediction != objOtherRule->_prediction) 
	return 0;
      
      for (k = 0, found = true; (k < _numGenes * 2) && (found); k ++)
	{
	  found = !( ((fabs(_genes[k] - 1.0) < 0.10) && 
		      (fabs(objOtherRule->_genes[k] - 1.0) > 10)) || 
		     ((fabs(_genes[k] - 128)>10) && 
		      (fabs(objOtherRule->_genes[k] - 128)<10)) );
	}
      
      return found;
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
