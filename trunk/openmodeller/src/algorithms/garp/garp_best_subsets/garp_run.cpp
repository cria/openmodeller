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

#include <openmodeller/om.hh>

#include "garp_run.hh"
#include "garp_run_thread.hh"
#include "bs_algorithm_factory.hh"
#include <openmodeller/AlgParameter.hh>

/****************************************************************/
/************************ GARP Run Thread ***********************/

// TODO: modify strategy to reuse threads instead of creating a 
//       new one for every garp run

THREAD_PROC_RETURN_TYPE AlgorithmRunThreadProc(void * threadData)
{
  AlgorithmRun * algRun = (AlgorithmRun *) threadData;

  //Log::instance()->debug("Starting new thread (%d).\n", algRun->getId());

  while (!algRun->done())
    algRun->iterate();

  algRun->calculateCommission();
  algRun->calculateOmission();
  algRun->finalize();
  
  //Log::instance()->debug("Finishing thread (%d).\n", algRun->getId());

  THREAD_PROC_RETURN_STATEMENT;
}


/****************************************************************/
/************************* GARP Run *****************************/

AlgorithmRun::AlgorithmRun() :
  _id(-1),
  _running( false ),
  _omission( -1.0 ),
  _commission( -1.0 ),
  _commission_samples( 0 ),
  _alg( NULL ),
  _train_sampler(),
  _test_sampler()
{
  //Log::instance()->info("Creating an AlgorithmRun at: %x\n",this);
}

AlgorithmRun::AlgorithmRun( const AlgorithmPtr& alg ) :
  _id(-1),
  _running( false ),
  _omission( -1.0 ),
  _commission( -1.0 ),
  _commission_samples( 0 ),
  _alg( alg ),
  _train_sampler(),
  _test_sampler()
{
  //Log::instance()->info("Creating an AlgorithmRun at: %x\n",this);
}

/****************************************************************/
AlgorithmRun::~AlgorithmRun() 
{
  //Log::instance()->info("Deleting an AlgorithmRun at: %x\n",this);
}

/****************************************************************/
int AlgorithmRun::initialize(int id, int comm_samples,
			     const SamplerPtr& train_sampler, 
			     const SamplerPtr& test_sampler, 
			     int nparam, AlgParameter * param,
			     BSAlgorithmFactory * factory)
{
  Log::instance()->debug( "Initializing garp run (%d)\n", id );

  _id = id;
  _commission_samples = comm_samples;

  _train_sampler = train_sampler;
  _test_sampler = test_sampler;

  _alg = factory->getBSAlgorithm();
  _alg->setSampler(train_sampler);
  _alg->setParameters(nparam, param);
  _alg->initialize();

  return 1;
}

/****************************************************************/
int AlgorithmRun::run()
{
  //Log::instance()->debug("Starting new garp run (%d).\n", _id);
  _running = true;
  THREAD_START(AlgorithmRunThreadProc, this); 
  return 1;  
}

/****************************************************************/
bool AlgorithmRun::running() const
{ return _running; }

/****************************************************************/
int AlgorithmRun::iterate()
{
  //Log::instance()->debug("Iteration %6d on run %d.\n", _alg->getGeneration(), _id);
  return _alg->iterate();
}

/****************************************************************/
int AlgorithmRun::done() const
{ return _alg->done(); }

/****************************************************************/
float AlgorithmRun::getProgress() const
{ return _alg->getProgress(); }

/****************************************************************/
int AlgorithmRun::finalize()           
{
  //Log::instance()->debug("Finishing up garp run.(%d)\n", _id);
  _running = false;
  //_alg->deleteTempDataMembers(); // this is not in Algorithm interface
  THREAD_END();
  return 1;
}

/****************************************************************/
int AlgorithmRun::calculateCommission()           
{
  int i;
  double sum = 0.0;

  // TODO: check how to use absences in computing commission

  //Log::instance()->debug("Calculating commission error (%d).\n", _id);

  // get random points from the background to estimate 
  // area predicted present
  bool hasAbsences = (_train_sampler->numAbsence() != 0);
  for (i = 0; i < _commission_samples; i++) {

    Scalar value;
    if (hasAbsences) {

      ConstOccurrencePtr occ = _train_sampler->getAbsence();
      value = _alg->getValue(occ->environment()); 
    }
    else {

      OccurrencePtr occ = _train_sampler->getPseudoAbsence();
      value = _alg->getValue(occ->environment()); 
    }

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

  //Log::instance()->debug("Calculating omission error (%d).\n", _id);

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

