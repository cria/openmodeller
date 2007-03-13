/**
 * Declaration of RangeRule used by GARP
 * 
 * @file   rules_range.cpp
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

#include "rules_range.hh"
#include "bioclim_histogram.hh"


// ==========================================================================
//  RangeRule implelentation
// ==========================================================================
RangeRule::RangeRule() : 
  GarpRule() 
{}

RangeRule::RangeRule(int numGenes) : 
  GarpRule(numGenes) 
{ }

RangeRule::RangeRule(Scalar prediction, int numGenes, 
	                   const Sample& chrom1, const Sample& chrom2, 
                     const double * performances) : 
  GarpRule(prediction, numGenes, chrom1, chrom2, performances) 
{}

// ==========================================================================
RangeRule::~RangeRule() { }

// ==========================================================================
void RangeRule::initialize(const BioclimHistogram& histogram)
{
  int i, j;
  Random rnd;

  // loop iterates through variables
  for(i = 0; i < _numGenes; i++)
    {
      j = rnd.get(_numGenes);
      
      Scalar a = 0 , b = 0;
      histogram.getBioclimRange(_prediction, j, a, b);
      _chrom1[j] = a;
      _chrom2[j] = b;
    }
}

// ==========================================================================
bool RangeRule::applies(const Sample& sample) const
{
  // visit each of the genes
  for (int i = 0; i < _numGenes; i++)
    {
      if (!(equalEps(_chrom1[i], -1.0) && equalEps(_chrom2[i], +1.0)))
        if (!between(sample[i], _chrom1[i], _chrom2[i]))
          return false;
    }

  return true;
}

// ==========================================================================
int RangeRule::getStrength(const Sample& sample) const
{
  //printf("GetStrength(%+7.3f)\n", _prediction);
  for (int i = 0; i < _numGenes; i++)
    {
      if (!membership(_chrom1[i], _chrom2[i], sample[i])) 
	{ 
	  //printf("Strength = 0\n"); 
	  return 0; 
	}
    } 
  
  //printf("Strength = 1\n");
  return 1;
}

// ==========================================================================
void RangeRule::log()
{
  Log::instance()->info( "Range: " );
  GarpRule::log();
}

// ==========================================================================
