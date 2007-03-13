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


#include <openmodeller/Random.hh>
#include <math.h>
#include <string.h>

#include <openmodeller/Log.hh>
#include <openmodeller/Sample.hh>

#include "rules_logit.hh"
#include "regression.hh"

const double coeficientThreshold = 0.05;

// ==========================================================================
//  LogitRule implelentation
// ==========================================================================
LogitRule::LogitRule() : 
  GarpRule() 
{ }

LogitRule::LogitRule(int numGenes) : 
  GarpRule(numGenes) 
{ }

LogitRule::LogitRule(Scalar prediction, int numGenes, 
	                   const Sample& chrom1, const Sample& chrom2, 
                     const double * performances) : 
  GarpRule(prediction, numGenes, chrom1, chrom2, performances) 
{}

// ==========================================================================
LogitRule::~LogitRule() {}

// ==========================================================================
void LogitRule::initialize(const Regression& reg)
{
  int i, j;
  Random rnd;
  
  for (i = 0; i < _numGenes; i++)
    {
      j = rnd.get(_numGenes);
      
      // decide where the constant (a) will go;
      _chrom1[j] = reg.getB()[j]; 
      _chrom2[j] = reg.getC()[j];
    }  
}

// ==========================================================================
bool LogitRule::applies(const Sample& sample) const
{
  return (getStrength(sample) == 1); 
}

// ==========================================================================
int LogitRule::getStrength(const Sample& sample) const
{ 
  Scalar sum = 0.0;
  Scalar prob = 0.0;

  //static SExprType< PLUS<  <TIMES<_2<Sample>, _1<Sample> ,TIMES< _3<Sample>, SQR<_1<Sample> > > > > > >::type expr;
  //sum = expr(sample)(_chrom1)(_chrom2);
  //sum = (sample * _chrom1) + (sample * sqr(_chrom2));
  
  Sample::const_iterator si = sample.begin();
  Sample::const_iterator end = sample.end();
  Sample::const_iterator c1i = _chrom1.begin();
  Sample::const_iterator c2i = _chrom2.begin();
  
  while (si != end)
    {
      if (!equalEps( (*c1i), -1.0 ) )
	{
	  Scalar c2i2 = (*c2i); c2i2 *= c2i2;
	  sum += ( (*si) * (*c1i) ) + ( (*si) * c2i2 );
	}

      ++si; ++c1i; ++c2i;
    }
  
  prob = 1.0 / (1.0 + (double) exp(-sum));

  return (prob >= 0.5);
}

// ==========================================================================
bool LogitRule::similar(const GarpRule * rule) const
{
  Scalar thisGene, otherGene;
  
  if (type() != rule->type())
    { return false; }

  // cast to LogitRule to gain access to private data members
  LogitRule * otherRule = (LogitRule *) rule;
  
  // check rule value (presence/absence)
  if (_prediction != otherRule->_prediction) 
    { return false; }
  
  // rules are similar if they share the same relevant coeficients, 
  // i.e., abs(gene) > 0.05.
  for (int k = 0; k < _numGenes; k++)
    { 
      thisGene  = fabs(_chrom1[k]);
      otherGene = fabs(otherRule->_chrom1[k]);

      if ( ( (thisGene < coeficientThreshold) && (otherGene > coeficientThreshold) ) ||
	   ( (thisGene > coeficientThreshold) && (otherGene < coeficientThreshold) ) ) 
	{
	      return false;
	}
    }
  
  return true;
}

// ==========================================================================
void LogitRule::log()
{
  Log::instance()->info( "Logit: " );

  for (int i = 0; i < _numGenes; ++i)
    {
      if (fabs(_chrom1[i]) + fabs(_chrom2[i]) <= coeficientThreshold)
	Log::instance()->info( "****** ****** ");
      else
	      Log::instance()->info( "%+6.2f %+6.2f ", _chrom1[i], _chrom2[i] );
    }

  Log::instance()->info( "- (%.2f) : %f\n", _prediction, getPerformance(PerfSig));
}

// ==========================================================================
