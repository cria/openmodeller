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


#include <random.hh>
#include <math.h>
#include <string.h>

#include <om_log.hh>

#include "rules_negrange.hh"
#include "garp_sampler.hh"


// ==========================================================================
//  NegatedRangeRule implelentation
// ==========================================================================
NegatedRangeRule::NegatedRangeRule() : RangeRule() {} 
// ==========================================================================
NegatedRangeRule::~NegatedRangeRule() {} 

// ==========================================================================
bool NegatedRangeRule::applies(Scalar * values)
{
  int i;
  
  // visit each of the genes
  for (i = 0; i < _numGenes; i++)
    {
      if (!(equalEps(_genes[i * 2], -1.0) && equalEps(_genes[i * 2 + 1], +1.0)))
	if (between(values[i], _genes[i * 2], _genes[i * 2 + 1]))
	  return false;
    }
  
  return true;
}

// ==========================================================================
int NegatedRangeRule::getStrength(Scalar * values)
{
  int strength, neg_strength;
	
  strength     = RangeRule::getStrength(values);
  neg_strength = 1 - strength;
  
  return neg_strength;
}

// ==========================================================================
void NegatedRangeRule::log()
{
  g_log( "Negated: " );
  GarpRule::log();
}

// ==========================================================================
