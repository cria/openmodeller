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


#include <random.hh>
#include <math.h>
#include <string.h>

#include <om_log.hh>

#include "rules_range.hh"
#include "garp_sampler.hh"


// ==========================================================================
//  RangeRule implelentation
// ==========================================================================
RangeRule::RangeRule() : GarpRule() { }

// ==========================================================================
RangeRule::~RangeRule() { }

// ==========================================================================
void RangeRule::initialize(GarpCustomSampler * sampler)
{
  int i, j;
  Random rnd;

  // call parent initialize
  GarpRule::initialize(sampler);
  
  // loop iterates through variables
  for(i = 0; i < _numGenes; i++)
    {
      j = rnd.get(_numGenes);
      
      sampler->getBioclimRange(_prediction, j, &_genes[j * 2], &_genes[j * 2 + 1]);
    }
}

// ==========================================================================
bool RangeRule::applies(Scalar * values)
{
  // visit each of the genes
  int i;
  for (i = 0; i < _numGenes; i++)
    {
      if (!(equalEps(_genes[i * 2], -1.0) && equalEps(_genes[i * 2 + 1], +1.0)))
        if (!between(values[i], _genes[i * 2], _genes[i * 2 + 1]))
          return false;
    }

  return true;
}

// ==========================================================================
int RangeRule::getStrength(Scalar * values)
{
  double a, b, c;
  int i;	

  for (i = 0; i < _numGenes; i++)
    {
      a = _genes[i * 2];
      b = _genes[i * 2 + 1];
      c = values[i];
      
      if (!membership(a, b, c)) 
	return 0;
    } 
  
  return 1;
}

// ==========================================================================
void RangeRule::log()
{
  g_log( "Range: " );
  GarpRule::log();
}

// ==========================================================================
