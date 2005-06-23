/**
 * Declaration of NegatedRangeRule class used in GARP
 * 
 * @file   rules_range.hh
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-04-01
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


#ifndef _RULES_NEGRANGE_HH_
#define _RULES_NEGRANGE_HH_

#include <openmodeller/om.hh>

#include "rules_range.hh"

/****************************************************************/
/****************** NegatedRangeRule class **********************/

/**
  * Implements Garp Negated Range rules. They are simply a negation
  * of an equivalent Range rule. Examples of Negated Range rules are:
  *
  * IF NOT (temp between 10 and 20C) THEN sp=PRESENT
  * IF NOT (temp between 10 and 20C AND 
  *    elevation between 500 and 2000m) THEN sp=ABSENT
  */
class NegatedRangeRule : public RangeRule
{
public:
  NegatedRangeRule();

  NegatedRangeRule(int numGenes);

  /// Constructor with setters 
  NegatedRangeRule(Scalar prediction, int numGenes, 
		   const Sample& chrom1, const Sample& chrom2, 
       const double * performances);
  
  virtual ~NegatedRangeRule();
  virtual GarpRule * objFactory() const { return new NegatedRangeRule(); }
  
  virtual char type() const				{ return '!'; }
  
  virtual bool applies(const Sample& sample) const;
  virtual int getStrength(const Sample& sample) const;

  void log();
};

// ====================================================================

#endif 
