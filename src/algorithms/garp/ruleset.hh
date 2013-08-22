/**
 * Declaration of Rule-Set classes used in GARP
 * 
 * @file   ruleset.hh
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


#ifndef _RULESET_HH_
#define _RULESET_HH_

#include <openmodeller/om.hh>
#include "rules_base.hh"


/****************************************************************/
/************************** GARP Rule-Set ***********************/

class GarpRule;

/** 
  * Stores a set of Garp rules. Consists of a valid Garp model.
  * Provide basic methods for manipulating the set of rules as 
  * a whole.
  */
class GarpRuleSet
{
public:
/** Constructor.
  * @param size Defines the maximum number of rules that can be 
  *        stored in the rule set
  */
  GarpRuleSet(int size);

  ~GarpRuleSet();

/** Returns size of rule set, i.e., the maximum number of rules
  *  that can be stored in the rule set.
  */
  int size();


/** Returns the number of rules currently stored in the rule set.
  */
  int numRules();


/** Remove all rules from rule set.
  */
  void clear();


/** Remove last rules in a rule set, trimming it to a certain
  *  number of rules.
  * @param rules Number of rules that will remain in the rule
  *        set. It will preserve the first rules and will
  *        eliminate remaining ones from rule set.
  */
  void trim(int rules);


/** Eliminates rules with performance below a certain threshold
  *  from the rule set.
  * @param perfIndex Index on performance array that will be 
            considered for elimination of rules.
  * @param threshold Minimum performance value a rule must
            present to remain in the rule set. Rules with
			performance value below threshold are removed
			from rule set.
  */
  void filter(PerfIndex perfIndex, double threshold);
  

/** Returns pointer to rule at a given position in rule set
  * @param index Index of rule to be returned
  * @return Pointer to rule at given position. This pointer
  *         should not be deallocated by caller.
  */
  GarpRule * get(int index);


/** Inserts a rule in the rule set, preserving sort order
  * @param perfIndex Index on performance array that will be 
  *         considered during insertion to find correct
  * 		position for new rule in the rule set.
  * @param rule pointer to the new rule being inserted. Rule
  *			set gains control over memory allocated to rule
  *			after insertion, i.e., rule set will deallocate
  *			memory for that rule when rule is eliminated.
  * @return Index where rule was inserted in the rule set.
  */
  int insert(PerfIndex perfIndex, GarpRule * rule);


/** Replaces rule at a given position in rule set by another 
  *  rule provided
  * @param index Index of rule to be replaced. 
  * @param rule Pointer to rule that will replace old one. Once 
  *         rule is added, rule set gains control over memory allocated 
  *         to rule after insertion, i.e., rule set will deallocate
  *			memory for that rule when rule is eliminated.
  * @return 1 on success, 0 on failure.
  * @note This method can break the rule set sort order.
  */
  int replace(int index, GarpRule * rule);

/** Remove rule at a given position in rule set
  * @param index Index of rule to be removed. 
  * @return 1 on success, 0 on failure.
  */
  int remove(int index);

/** Adds new rule at the end of rule set.
  * @param rule Pointer to rule to be added to rule set. Once 
  *         rule is added, rule set gains control over memory allocated 
  *         to rule after insertion, i.e., rule set will deallocate
  *			memory for that rule when rule is eliminated.
  * @return Number of rules after addition on success, 0 on failure.
  */
  int add(GarpRule * rule);


/** Checks whether there is rule in the rule set that is similar to 
  *  a given rule.
  * @param rule Pointer to rule
  * @return Index of first similar rule found in rule set or -1
  *         if there are no similar rules in rule set.
  * @note: Similarity between rules is defined on the Rule class
  *        hierarchy by abstract method Rule->similar() and its
  *        concrete implementations in subclasses.
  */
  int findSimilar(GarpRule * rule);


/** Returns rule prediction for a given data vector.
  * @return Prediction value for the data vector based on GARP model
  */
  Scalar getValue( const Sample& x ) const;
  

/** Returns performance statistics of the rule set.
  * @param perfIndex Base calculation on this performance array index.
  * @param best Returns best performance value found in rule set.
  * @param worst Returns worst performance value found in rule set.
  * @param average Returns average performance of rules in rule set.
  */
  void performanceSummary(PerfIndex perfIndex, 
			  double * best, double * worst, double * average);


  void gatherRuleSetStats(int gen);
  void log() const;
  

private:
  GarpRule ** _rules;
  int _size;
  int _numRules;
};

// ====================================================================

#endif 
