/**
 * Definition of AlgorithmRun class
 * 
 * @file   garp_run.cpp
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2004-08-25
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
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
 */

#include "AlgorithmRun.hh"
#include "threads.hh"

/****************************************************************/
/************************ GARP Run Thread ***********************/

// TODO: modify strategy to reuse threads instead of creating a 
//       new one for every garp run

THREAD_PROC_RETURN_TYPE AlgorithmRunThreadProc(void * threadData)
{
  AlgorithmRun * algRun = (AlgorithmRun *) threadData;

  algRun->createModel();

  algRun->stop();
  THREAD_PROC_RETURN_STATEMENT;
}


/****************************************************************/
/************************* GARP Run *****************************/

AlgorithmRun::AlgorithmRun(const AlgorithmPtr& algo) :
  _alg( algo ),
  _id(-1),
  _running( false ),
  _omission( -1.0 ),
  _commission( -1.0 ),
  _commission_samples( 0 ),
  _train_sampler(),
  _test_sampler()
{

}

/****************************************************************/
AlgorithmRun::~AlgorithmRun() 
{

}

/****************************************************************/
int AlgorithmRun::initialize(int id, int comm_samples,
			     const SamplerPtr& train_sampler, 
			     const SamplerPtr& test_sampler ) 
{
  _id = id;
  _commission_samples = comm_samples;

  _train_sampler = train_sampler;
  _test_sampler = test_sampler;

  return 1;
}

/****************************************************************/
void AlgorithmRun::run()
{
  _running = true;
  THREAD_START(AlgorithmRunThreadProc, this); 
}

void AlgorithmRun::stop()
{
  _running = false;
  THREAD_END();
}


/****************************************************************/
bool AlgorithmRun::running() const
{
  return _running;
}

/****************************************************************/
void AlgorithmRun::createModel()
{
  _alg->createModel( _train_sampler );

#if 1
  calculateCommission();
  calculateOmission();
#else  

  ConfusionMatrix cm;
  cm.calculate( _alg->getModel(), _train_sampler );

  _commission = cm.getCommissionError();

  _omission = cm.getOmissionError();
#endif
}

/****************************************************************/
float AlgorithmRun::getProgress() const
{ return _alg->getProgress(); }

/****************************************************************/
int AlgorithmRun::calculateCommission()           
{
  int i;
  double sum = 0.0;

  // TODO: check how to use absences in computing commission

  // get random points from the background to estimate 
  // area predicted present
  bool hasAbsences = (_train_sampler->numAbsence() != 0);
  for (i = 0; i < _commission_samples; i++)
    {
      ConstOccurrencePtr occ;
      if (hasAbsences)
	occ = _train_sampler->getAbsence();
      else
	occ = _train_sampler->getPseudoAbsence();

      Scalar value = _alg->getValue(occ->environment()); 
      // discard novalue (-1); zero is irrelevant to the sum
      if (value > 0)
	sum += value;
    }

  _commission = sum / (double) _commission_samples;

  return 1;
}

/****************************************************************/
int AlgorithmRun::calculateOmission()           
{
  // TODO: check how to use absences in computing omission

  // test which kind of test (intrinsic or extrinsic) should be performed
  SamplerPtr sampler;

  if (!_test_sampler)
    { sampler = _train_sampler; }
  else
    { sampler = _test_sampler; }

  int nomitted = 0;
  OccurrencesPtr presences = sampler->getPresences();
  OccurrencesImpl::const_iterator it  = presences->begin();
  OccurrencesImpl::const_iterator end = presences->end();

  while (it != end)
    {
      nomitted = !_alg->getValue((*it)->environment()); 
      ++it;
    }

  _omission = (double) nomitted / (double) presences->numOccurrences();

  return 1;
}

/****************************************************************/
double AlgorithmRun::getOmission() const
{ return _omission; }

/****************************************************************/
double AlgorithmRun::getCommission() const
{ return _commission; }

/****************************************************************/
double AlgorithmRun::getError(int type) const
{
  if (!type)
    { return _omission; }
  else
    { return _commission; }
}

/****************************************************************/
double AlgorithmRun::getValue(const Sample& x) const   
{ return _alg->getValue(x); }

