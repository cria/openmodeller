/**
 * Declaration of LogitRule class used in GARP
 * 
 * @file   rules_logit.hh
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


#ifndef _RULES_LOGIT_HH_
#define _RULES_LOGIT_HH_

#include <om.hh>

#include "rules_base.hh"
#include "regression.hh"

/**
  * Implements Garp Logistic Regression rules. Also known as 
  * Logit rules. 
  */
class LogitRule : public GarpRule
{
public:
  LogitRule();

  LogitRule(int numRules);

  /// Constructor with setters 
  LogitRule(Scalar prediction, int numGenes, 
            const Sample& chrom1, const Sample& chrom2, 
            const double * performances);
  
  virtual ~LogitRule();
  virtual GarpRule * objFactory() const   { return new LogitRule(); }
  
  virtual char type() const				{ return 'r'; }
  
  virtual void initialize(const Regression& regression);
  virtual bool applies(const Sample& sample) const;
  virtual int getStrength(const Sample& sample) const;
  
  virtual bool similar(const GarpRule * objOtherRule) const;

  void log();
};

// ====================================================================

#endif 
