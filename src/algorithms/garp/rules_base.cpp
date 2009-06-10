/**
 * Declaration of GarpRule used by GARP
 * 
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
#include <openmodeller/Occurrence.hh>

#include <openmodeller/Exceptions.hh>

#include "rules_base.hh"

#define MIN_SIG_NO 10

#ifdef WIN32
#ifndef INFINITY //in mingw this is already defined in math.h
#define INFINITY 1000000000
#endif
#endif

#ifndef WIN32
int min(int v1, int v2)
{
  return (v1 < v2)? v1 : v2;
}
#endif

bool equalEps(double v1, double v2)
{
  return (fabs(v1 - v2) < 0.000001);
}

bool between(double value, double min, double max)
{
  //Log::instance()->info("Between: %f <= %f <= %f? %d\n", min, value, max, ((value >= min) && (value <= max)));
  return ((value >= min) && (value <= max));
}

int membership(double value1, double value2, double value)
{
  if (equalEps(value1, -1.0) && equalEps(value2, +1.0))
    return 255; 
  else if (value < value1 || value > value2)
    return 0; 
  else
    return 1; 
}


/****************************************************************/
/****************** GarpRule Constructor ************************/

GarpRule::GarpRule() :
  _chrom1(), 
  _chrom2(),
  _prediction(0),
  _numGenes(0),
  _needsEvaluation(true)
{
  _origin = type();
  for (int i = 0; i < 10; i++)
    _performance[i] = 0.0;
}

GarpRule::GarpRule(int numGenes) :
  _chrom1(numGenes, -1.0), 
  _chrom2(numGenes, +1.0),
  _prediction(0),
  _numGenes(numGenes),
  _needsEvaluation(true)
{
  _origin = type();

  for (int i = 0; i < 10; i++)
    _performance[i] = 0.0;
}
  
// *****************
GarpRule::GarpRule(Scalar prediction, int numGenes, 
				   const Sample& chrom1, const Sample& chrom2, 
				   const double * performances) :
  _chrom1(chrom1), 
  _chrom2(chrom2),
  _prediction(prediction),
  _numGenes(numGenes),
  _needsEvaluation(true),
  _origin(OriginColonization)
  
{
  for (int i = 0; i < 10; i++)
  { *(_performance + i) = *(performances + i); }
}
  
/****************************************************************/
/****************** GarpRule Destructor *************************/

GarpRule::~GarpRule() 
{ }

GarpRule * GarpRule::clone() const
{
  GarpRule * newRule = objFactory();

  newRule->_numGenes = _numGenes;
  newRule->_chrom1 = _chrom1;
  newRule->_chrom2 = _chrom2;
  
  for (int i = 0; i < 10; i++) 
    newRule->_performance[i] = _performance[i];
  
  newRule->_needsEvaluation = _needsEvaluation;
  newRule->_prediction = _prediction;
  newRule->_origin = _origin;
  
  return newRule;
}

/** Copy data from another rule to the current rule.
  * @param fromRule Pointer to source rule providing data
  *        to set current rule.
  * @return 1 on success or 0 on failure (if types are different).
  */
int GarpRule::copy(const GarpRule * fromRule)
{
  if (type() != fromRule->type())
    return 0;
  
  _chrom1 = fromRule->_chrom1;
  _chrom2 = fromRule->_chrom2;
  
  for (int i = 0; i < 10; i++) 
    _performance[i] = fromRule->_performance[i];
  
  _needsEvaluation = fromRule->_needsEvaluation;
  _prediction = fromRule->_prediction;
  _origin = fromRule->_origin;
  
  return 1;
}


// ==========================================================================
double GarpRule::getPerformance(PerfIndex perfIndex) const
{
  return _performance[perfIndex];
}

// ==========================================================================
int GarpRule::getCertainty(Scalar pred) const
{ 
  return (_prediction == pred); 
}

// ==========================================================================
double GarpRule::getError(Scalar predefinedValue, Scalar pred) const
{ 
  return (double) fabs(predefinedValue - pred); 
}
 
// ==========================================================================
void GarpRule::adjustRange(Scalar& v1, Scalar& v2) const
{
  if (v1 > +1.0) v1 = +1.0;
  if (v2 > +1.0) v2 = +1.0;
  if (v1 < -1.0) v1 = -1.0;
  if (v2 < -1.0) v2 = -1.0;

  if (v1 > v2)
    {
      // swap v1 and v2
      Scalar aux = v1;
      v1 = v2;
      v2 = aux;
    }
}

// ==========================================================================
void GarpRule::crossover(GarpRule * rule, int xpt1, int xpt2)
{
  int i, diff, aux;
  Scalar temp;
  
  diff = 0;
  if (xpt1 > xpt2)
  {
    aux = xpt1;
    xpt1 = xpt2;
    xpt2 = aux;
  }

  for ( i = xpt1; i < xpt2; i++)
  {
    temp = _chrom1[i]; 
    _chrom1[i] = rule->_chrom1[i];
    rule->_chrom1[i] = temp;
    diff += (_chrom1[i] != rule->_chrom1[i]);

    temp = _chrom2[i]; 
    _chrom2[i] = rule->_chrom2[i];
    rule->_chrom2[i] = temp;
    
    diff += (_chrom2[i] != rule->_chrom2[i]);
  }

  if (diff)
  {
    _origin = OriginCrossover;
    forceEvaluation();

    rule->_origin = OriginCrossover;
    rule->forceEvaluation();
  }
}

// ==========================================================================
void GarpRule::mutate(double temperature)
{
  Scalar rnd1, rnd2;
  Random rnd;
  int j;
  
  j = rnd.get(_numGenes);

  rnd1 = rnd.get(-temperature, +temperature);
  rnd2 = rnd.get(-temperature, +temperature);

  _chrom1[j] -= rnd1;
  _chrom2[j] += rnd2;

  adjustRange(_chrom1[j], _chrom2[j]);
  
  _needsEvaluation = true;
  _origin = OriginMutation;
}

// ==========================================================================
bool GarpRule::similar(const GarpRule * otherRule) const
{
  if (type() != otherRule->type())
    { return false; }

  // check rule value (presence/absence)
  if (_prediction != otherRule->_prediction) 
  {  return false; }
  
  for (int i = 0; i < _numGenes; i++)
    {
      // rA and rB indicates whether gene <i> is being used or not
	  bool rA = (equalEps(_chrom1[i], -1.0) && 
		           equalEps(_chrom2[i], +1.0));
      
	  bool rB = (equalEps(otherRule->_chrom1[i], -1.0) && 
               equalEps(otherRule->_chrom2[i], +1.0));
      
      if ( rA != rB )
    {  return false; }
    }
  
  return true;
}

// ==========================================================================
double GarpRule::evaluate(const OccurrencesPtr& occs)
{
  int i;

  double utility[10];

  //next line commented out since its not used
  //int dimension = (*occs)[0]->environment().size();

  int n = occs->numOccurrences();

  // value of dependent variable from the current sample point
  //next line commented out since its not used
  //Scalar pointValue = 0.0;

  // 1 if rule applies to sample point, i.e., values satisfies rule precondition
  // 0 otherwise
  int strength;

  // indicates whether rule has the same conclusion as the current point  
  // (i.e., rule prediction is equal to value dependent variable)
  int certainty;

  // error
  double error;

  // number of points that rule applies to (sum of strength)
  int pXs;

  // number of points with the same conclusion as the rule (sum of certainty)
  int pYs;

  // sum of points correctly predicted by the rule
  int pXYs;

  // number of points selected by the rule
  int no;

  // other intermediate statistics
  double pYcXs, pYcs;

  // prior probability
  double priorProb;

  // note that pXSs is always equals to no, so it has been removed

  // reset counters
  pXs = pYs = pXYs = no = 0;
  pYcXs = pYcs = 0.0;

  // reset utility values
  for (i = 1; i < 10; i++) 
    utility[i] = 0.0;
  
  utility[0] = 1.0;

  //FILE * flog = fopen("evaluate.log", "w");

  OccurrencesImpl::const_iterator it  = occs->begin();
  OccurrencesImpl::const_iterator end = occs->end();

  while (it != end)
    {	
      // environmental (independent) variables values from current sample point
      Scalar pointValue = ( (*it)->abundance() > 0.0 ) ? 1.0 : 0.0;
      Sample sample = (*it)->environment();

      strength = getStrength(sample);
      certainty = getCertainty(pointValue);
      error = getError(0, pointValue);

      pXs  += strength;
      pYs  += certainty;
      pYcs += error;
      
      if (strength > 0)
      {
        no++;
        pXYs += certainty;  // strength is always 1, then success == certainty
        pYcXs += getError(error, pointValue);
      }

      ++it;

      /*
      fprintf(flog, "Sample %5d: [%d %d %d] (%+3.2f) ", 
	    i, strength, certainty, min(strength, certainty), pointValue);
      for (u = 0; u < dimension; u++)
	{ 
	  fprintf(flog, "%+8.4f ", values[u]);
	}
      fprintf(flog, "\n");
      */
    }

  if (no != pXs)
    {
      Log::instance()->error("Assertion failed (no != pXs): %d != %d", no, pXs);
      throw AlgorithmException("Assertion failed");
    }
  
  // Priors
  utility[1] = pXs  / (double) n;		// proportion 
  utility[2] = pYs  / (double) n;		// Prior probability
  utility[3] = pYcs / (double) n;
  
  priorProb = utility[2];

  if (no > 0)
    {
      utility[4] = no    / (double) n;          // Posterior strength
      utility[5] = pXYs  / (double) no;  	// Posterior probability
      utility[6] = pYcXs / no;
      utility[7] = no    / (double) n;          // Coverage
    }

  // Crisp Significance
  if ( (no >= MIN_SIG_NO) && (priorProb > 0) && (priorProb < 1.0))	
    {
      utility[8] = (pXYs - priorProb * no) / 
	sqrt( no * priorProb * (1.0 - priorProb));
    }

  //flags!!! not implemented yet
  //if (Postflag)   
  utility[0] *= utility[5];
  
  //if (Sigflag)	
  utility[0] *= utility[8];

  //printf("%c] u0=%+7.3f u1=%+7.3f u2=%+7.3f u8=%+7.3f pXYs=%5d no=%4d n=%4d\n",
  // type(), utility[0], utility[1], utility[2], utility[8], pXYs, no, n);

  //if (Compflag) utility[0] *= utility[7];
  //if (Ecoflag) utility[0] *= ecoSpace();
  //if (Lengthflag) utility[0] /= length();
  
  // Record performance in rule
  for (i = 0; i < 10; i++) 
    _performance[i] = utility[i];


  /*
  fprintf(flog, "Sums (pXs, pYs, pXYs, n, no): %5d %5d %5d %5d %5d ([%c] pred=%+3.1f sig=%+10.4f)\n", 
	  pXs, pYs, pXYs, n, no, type(), _prediction, utility[8]);

  if ((utility[8] > 15) && (_prediction == 0.0))
    {
      Scalar *gen = _genes;
      Scalar *end = gen + 2 * _numGenes;
      fprintf(flog, "Genes: ");
      while ( gen < end )
	fprintf(flog, "%+8.4f ", *gen++ );
      
      fprintf(flog, "- (%.2f) : %f\n", _prediction, getPerformance(PerfSig) );

      fclose(flog);
      Log::instance()->error(1, "Check rule statistics"); //DEBUG
    }
  fclose(flog);
  */

  /*
  static int ii = 0;
  if (utility[8] < 2.7)
    printf("%4d] Rule performance is low: %+6.2f\n", ++ii, utility[8]);
  */

  return (utility[0]);
}

// ==========================================================================
void GarpRule::log()
{
  for (int i = 0; i < _numGenes * 2; i += 2)
    {
      if (fabs(_chrom1[i] - _chrom2[i]) >= 2.0)
	Log::instance()->info( "****** ****** ");
      else
	      Log::instance()->info( "%+6.2f %+6.2f ", _chrom1[i], _chrom2[i] );
    }

  Log::instance()->info( "- (%.2f) : %f\n", _prediction, getPerformance(PerfSig));
}

// ==========================================================================
