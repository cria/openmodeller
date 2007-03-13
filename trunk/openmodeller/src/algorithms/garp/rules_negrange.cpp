/**
 * Declaration of NegatedRangeRule used by GARP
 * 
 * @file   rules_negrange.cpp
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

#include "rules_negrange.hh"


// ==========================================================================
//  NegatedRangeRule implelentation
// ==========================================================================
NegatedRangeRule::NegatedRangeRule() : 
  RangeRule() 
{}

NegatedRangeRule::NegatedRangeRule(int numGenes) : 
  RangeRule(numGenes) 
{ }

NegatedRangeRule::NegatedRangeRule(Scalar prediction, int numGenes, 
	                          const Sample& chrom1, const Sample& chrom2, 
                            const double * performances) : 
  RangeRule(prediction, numGenes, chrom1, chrom2, performances) 
{}

// ==========================================================================
NegatedRangeRule::~NegatedRangeRule() {} 

// ==========================================================================
bool NegatedRangeRule::applies(const Sample& sample) const
{
  int i;
  
  // visit each of the genes
  for (i = 0; i < _numGenes; i++)
    {
      if (!(equalEps(_chrom1[i], -1.0) && equalEps(_chrom2[i], +1.0)))
	      if (!between(sample[i], _chrom1[i], _chrom2[i]))
	  return true;
    }
  
  return false;
}

// ==========================================================================
int NegatedRangeRule::getStrength(const Sample& sample) const
{
  int strength, neg_strength;
	
  strength     = RangeRule::getStrength(sample);
  neg_strength = 1 - strength;
  
  return neg_strength;
}

// ==========================================================================
void NegatedRangeRule::log()
{
  Log::instance()->info( "Negated: " );
  GarpRule::log();
}

// ==========================================================================
