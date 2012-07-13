/**
 * Declaration of Rule-sets used by GARP
 * 
 * @file   ruleset.cpp
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

#include "ruleset.hh"
#include "rules_base.hh"


/****************************************************************/
/****************** GarpRuleSet class ***************************/
/****************************************************************/


/****************************************************************/
/****************** Constructor *********************************/

GarpRuleSet::GarpRuleSet(int size)
{
  _size = size;
  _rules = new GarpRule*[size];
  _numRules = 0;
  memset(_rules, 0, size * sizeof(GarpRule*));
}


/****************************************************************/
/****************** Destructor **********************************/

GarpRuleSet::~GarpRuleSet()
{
  clear();
  delete [] _rules;
}

/****************************************************************/
/****************** size ****************************************/

int GarpRuleSet::size()
{ 
  return _size; 
}

/****************************************************************/
/****************** numRules ************************************/

int GarpRuleSet::numRules()
{ 
  return _numRules; 
}

/****************************************************************/
/****************** clear ***************************************/

void GarpRuleSet::clear()
{ 
  trim(0); 
}

/****************************************************************/
/****************** trim ****************************************/

void GarpRuleSet::trim(int rules)
{
  int i;

  if (rules >= _numRules)
    // there are less rules than specified: nothing to do
    return;

  for (i = rules; i < _size; i++)
  {
    if (_rules[i]) 
      {
        delete _rules[i];
        _rules[i] = NULL;
      }
  }

  _numRules = rules;
}

/****************************************************************/
/****************** filter **************************************/

void GarpRuleSet::filter(PerfIndex index, double threshold)
{
  int i, j;
  
  i = 0;
  while (i < _numRules)
    {
      //printf("%3d] Performance: %+8.4f (threshold=%+8.4f) - ", 
      //	    i, _rules[i]->getPerformance(index), threshold);

      if (_rules[i]->getPerformance(index) < threshold)
	{
	  //printf("deleting\n");

	  // this rule has not enough performance
	  // delete rule 
	  delete _rules[i];
	  
	  // and shift rules up
	  for (j = i; j < _numRules - 1; j++)
	    _rules[j] = _rules[j + 1];
	  
	  // remove the duplicated reference to the last rule
	  _rules[_numRules - 1] = NULL;
	  
	  // update the number of rules in this set
	  // no need to increment <i> because the rules were shifted up one position
	  _numRules--;
	}
      else
	{
	  //printf("keeping\n");

	  // this rule passed the test
	  // go to the next one
	  i++;
	}
    }
}

/****************************************************************/
/****************** insert **************************************/

int GarpRuleSet::insert(PerfIndex perfIndex, GarpRule * rule)
{
  double newRulePerformance;
  int i, j;

  // insert rule and keep set sorted by performance index specified (_performance[perfIndex])
  // find place where rule should be inserted
  newRulePerformance = rule->getPerformance(perfIndex);
  for (i = 0; i < _numRules; i++)
  { 
    /*
    printf("Perfs[%3d/%3d]: (%+8.4f > %+8.4f)? %2d\n", i, _numRules, 
	   newRulePerformance, _rules[i]->getPerformance(perfIndex),
	   (newRulePerformance > _rules[i]->getPerformance(perfIndex)));
    */

    if (newRulePerformance > _rules[i]->getPerformance(perfIndex))
      break;
  }

  // <i> has the index where new rule should be inserted
  // move remaining rules one position down
  // and insert new rule at index <i>
  for (j = _numRules - 1; j >= i; j--)
  { _rules[j + 1] = _rules[j]; }

  _rules[i] = rule;

  _numRules++;

  return i;
}

/****************************************************************/
/****************** get *****************************************/

GarpRule * GarpRuleSet::get(int index)
{ 
  if (index >= 0 || index < _numRules)
    return _rules[index];
  else
    return NULL;
}

/****************************************************************/
/****************** replace *************************************/

int GarpRuleSet::replace(int index, GarpRule * rule)
{
  if (!rule || index < 0 || index >= _numRules) 
    return 0;

  delete _rules[index];
  _rules[index] = rule;
  return 1;
}

/****************************************************************/
/****************** remove **************************************/

int GarpRuleSet::remove(int index)
{
  if (index < 0 || index >= _numRules) 
    return 0;

  delete _rules[index];

  int i;
  for (i = index; i < _numRules - 1; i++)
    _rules[i] = _rules[i + 1];

  _rules[--_numRules] = NULL;

  return 1;
}

/****************************************************************/
/****************** add *****************************************/

int GarpRuleSet::add(GarpRule * rule)
{
  if ( rule ) {

    if ( _numRules < _size ) {

      _rules[_numRules++] = rule;
      return _numRules;
    }
    else {
      // "Cannot add rule. Ruleset is full"
      return 0;
    }
  }
  else {
    // Cannot add null rule
    return 0;
  }
}

/****************************************************************/
/****************** findSimilar *********************************/

int GarpRuleSet::findSimilar(GarpRule * rule)
{
  int i;
  for ( i = 0; i < _numRules; i++ ) {

    if ( _rules[i]->similar(rule) ) { 

      return i; 
    }
  }

  return -1;
}

/****************************************************************/
/****************** getValue ************************************/


Scalar GarpRuleSet::getValue(const Sample& x) const
{
  int i;

  for ( i = 0; i < _numRules; i++ ) {

    if (_rules[i]->applies(x)) {

      //return i / (double) _numRules;
      return _rules[i]->getPrediction();
    }
  }

  return 0.0;
}

/****************************************************************/
/*************** performanceSummary *****************************/

void GarpRuleSet::performanceSummary(PerfIndex perfIndex, 
				     double * best, 
				     double * worst, 
				     double * average)
{
  int i;
  double performance;

  *worst = *best = *average = 0.0;

  if (!_numRules)
    return;

  for (i = 0; i < _numRules; i++)
    {
      performance = _rules[i]->getPerformance(perfIndex);
      
      if (performance < *worst || !i) *worst = performance;
      if (performance > *best  || !i) *best  = performance;
      *average += performance;
    }

  *average /= _numRules;
}

/****************************************************************/
/****************** log *****************************************/

void GarpRuleSet::log() const
{
  for ( int i = 0; i < _numRules; i++ )
    {
      Log::instance()->info( "%2d] ", i );
      _rules[i]->log();
    }
}

// ==============================================================

// gather rule set statistics
void GarpRuleSet::gatherRuleSetStats(int gen)
{
  char type='0'; //initialise to some invalid value

  printf("%4d]", gen);

  for (int i = 0; i < 3; i++)
  {
    switch (i)
    {
      case 0: type ='d'; break;
      case 1: type ='!'; break;
      case 2: type ='r'; break;
	//case 3: type ='a'; break;
    }

    double max = -10000;
    double sum = 0;
    int ct = 0;
    int pres = 0;
    for (int j = 0; j < _numRules; j++)
    {
      GarpRule * rule = _rules[j];
      if (rule->type() == type)
      {
        ct++;
        sum += rule->getPerformance(PerfUtil);
	pres += (int) rule->getPrediction();
	if (max < rule->getPerformance(PerfUtil))
	  max = rule->getPerformance(PerfUtil);
      }
    }

    if (max == -10000)
      max = 0;

    printf("%c %2d %+7.2f %+7.2f %2d|", type, ct, max, sum / ct, pres);
  }

  printf("\n");
}
