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

#include <om.hh>
#include "garp_run.hh"
#include "garp_run_thread.hh"
#include "bs_algorithm_factory.hh"
#include "om_alg_parameter.hh"
#include "om_sampler.hh"

/****************************************************************/
/************************ GARP Run Thread ***********************/

// TODO: modify strategy to reuse threads instead of creating a 
//       new one for every garp run

THREAD_PROC_RETURN_TYPE AlgorithmRunThreadProc(void * threadData)
{
  AlgorithmRun * algRun = (AlgorithmRun *) threadData;

  //g_log.debug("Starting new thread (%d).\n", algRun->getId());

  while (!algRun->done())
    algRun->iterate();

  algRun->calculateCommission();
  algRun->calculateOmission();

  //printf("%4d] Om=%5.3f Comm=%5.3f\n", algRun->getId(), algRun->getOmission(), algRun->getCommission());

  algRun->finalize();
  
  //g_log.debug("Finishing thread (%d).\n", algRun->getId());

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
  _train_sampler(NULL),
  _test_sampler(NULL)
{
}

/****************************************************************/
AlgorithmRun::~AlgorithmRun() 
{
  if (_train_sampler)
    delete _train_sampler;

  if (_test_sampler)
    delete _test_sampler;

  if (_alg)
    delete _alg;

}

/****************************************************************/
int AlgorithmRun::initialize(int id, int comm_samples,
			     Sampler * train_sampler, 
			     Sampler * test_sampler, 
			     int nparam, AlgParameter * param,
			     BSAlgorithmFactory * factory)
{
  //g_log.debug("Initializing garp run (%d)\n", id);

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
  //g_log.debug("Starting new garp run (%d).\n", _id);
  _running = true;
  THREAD_START(AlgorithmRunThreadProc, this); 
  return 1;  
}

/****************************************************************/
bool AlgorithmRun::running()
{ return _running; }

/****************************************************************/
int AlgorithmRun::iterate()
{
  //g_log.debug("Iteration %6d on run %d.\n", _alg->getGeneration(), _id);
  return _alg->iterate();
}

/****************************************************************/
int AlgorithmRun::done()
{ return _alg->done(); }

/****************************************************************/
float AlgorithmRun::getProgress()
{
  if (_alg)
    return _alg->getProgress(); 
  else
    return 0.0;
}

/****************************************************************/
int AlgorithmRun::finalize()           
{
  //g_log.debug("Finishing up garp run.(%d)\n", _id);
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
  SampledData sample;

  // TODO: check how to use absences in computing commission

  //g_log.debug("Calculating commission error (%d).\n", _id);

  // get random points from the background to estimate 
  // area predicted present
  _train_sampler->getPseudoAbsence(&sample, _commission_samples);

  for (i = 0; i < _commission_samples; i++)
    { sum += _alg->getValue(sample[i]); }

  _commission = sum / (double) _commission_samples;

  return 1;
}

/****************************************************************/
int AlgorithmRun::calculateOmission()           
{
  int i, npresences, nomitted;
  SampledData sample, absences;
  Sampler * sampler;

  // TODO: check how to use absences in computing omission

  //g_log.debug("Calculating omission error (%d).\n", _id);

  // test which kind of test (intrinsic or extrinsic) should be performed
  if (!_test_sampler)
    { sampler = _train_sampler; }
  else
    { sampler = _test_sampler; }

  npresences = sampler->getPresence(&sample);
  nomitted = 0;
  for (i = 0; i < npresences; i++)
    { nomitted = !_alg->getValue(sample[i]); }

  _omission = (double) nomitted / (double) npresences;

  return 1;
}

/****************************************************************/
double AlgorithmRun::getOmission()     
{ return _omission; }

/****************************************************************/
double AlgorithmRun::getCommission()   
{ return _commission; }

/****************************************************************/
double AlgorithmRun::getError(int type)
{
  if (!type)
    { return _omission; }
  else
    { return _commission; }
}

/****************************************************************/
double AlgorithmRun::getValue(Scalar * x)   
{ return _alg->getValue(x); }

