/**
 * Definition of GarpRun class
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
#include "om_serializable.hh"
#include "garp.hh"
#include "om_alg_parameter.hh"
#include "om_sampler.hh"

/****************************************************************/
/************************ GARP Run Thread ***********************/

// TODO: modify strategy to reuse threads instead of creating a 
//       new one for every garp run

THREAD_PROC_RETURN_TYPE GarpRunThreadProc(void * threadData)
{
  GarpRun * garpRun = (GarpRun *) threadData;

  g_log("Starting new thread (%d).\n", garpRun->getId());

  while (!garpRun->done())
    garpRun->iterate();

  garpRun->calculateCommission();
  garpRun->calculateOmission();
  garpRun->finalize();
  
  g_log("Finishing thread (%d).\n", garpRun->getId());

  THREAD_PROC_RETURN_STATEMENT;
}


/****************************************************************/
/************************* GARP Run *****************************/

GarpRun::GarpRun() 
{
  _id = -1;
  _running = false;
  _omission = -1.0;
  _commission = -1.0;
  _commission_samples = 0;
  _garp = NULL;
  _train_sampler = _test_sampler = NULL;
}

/****************************************************************/
GarpRun::~GarpRun() 
{
  if (_garp)
    delete _garp;

  if (_train_sampler)
    delete _train_sampler;

  if (_test_sampler)
    delete _test_sampler;
}

/****************************************************************/
int GarpRun::initialize(int id, int comm_samples,
			Sampler * train_sampler, 
			Sampler * test_sampler, 
			int nparam, AlgParameter * param)
{
  g_log("Initializing garp run (%d)\n", id);

  _id = id;
  _commission_samples = comm_samples;

  _train_sampler = train_sampler;
  _test_sampler = test_sampler;

  _garp = new Garp;
  _garp->setSampler(train_sampler);
  _garp->setParameters(nparam, param);
  _garp->initialize();

  return 1;
}

/****************************************************************/
int GarpRun::run()
{
  g_log("Starting new garp run (%d).\n", _id);
  _running = true;
  THREAD_START(GarpRunThreadProc, this); 
  return 1;  
}

/****************************************************************/
bool GarpRun::running()
{ return _running; }

/****************************************************************/
int GarpRun::iterate()
{
  //g_log("Iteration %6d on run %d.\n", _garp->getGeneration(), _id);
  return _garp->iterate();
}

/****************************************************************/
int GarpRun::done()
{ return _garp->done(); }

/****************************************************************/
int GarpRun::finalize()           
{
  g_log("Finishing up garp run.(%d)\n", _id);
  _running = false;
  _garp->deleteTempDataMembers();
  THREAD_END();
  return 1;
}

/****************************************************************/
int GarpRun::calculateCommission()           
{
  int i;
  double sum = 0.0;
  SampledData sample;

  // TODO: check how to use absences in computing commission

  g_log("Calculating commission error (%d).\n", _id);

  _train_sampler->getPseudoAbsence(&sample, _commission_samples);

  for (i = 0; i < _commission_samples; i++)
    { sum += _garp->getValue(sample[i]); }

  _commission = sum / (double) _commission_samples;

  return 1;
}

/****************************************************************/
int GarpRun::calculateOmission()           
{
  int i, npresences, nomitted;
  SampledData sample, absences;
  Sampler * sampler;

  // TODO: check how to use absences in computing omission

  g_log("Calculating omission error (%d).\n", _id);

  // test which kind of test (intrinsic or extrinsic) should be performed
  if (!_test_sampler)
    { sampler = _train_sampler; }
  else
    { sampler = _test_sampler; }

  npresences = sampler->getPresence(&sample);
  nomitted = 0;
  for (i = 0; i < npresences; i++)
    { nomitted = !_garp->getValue(sample[i]); }

  _omission = (double) nomitted / (double) npresences;

  return 1;
}

/****************************************************************/
double GarpRun::getOmission()     
{ return _omission; }

/****************************************************************/
double GarpRun::getCommission()   
{ return _commission; }

/****************************************************************/
double GarpRun::getError(int type)
{
  if (!type)
    { return _omission; }
  else
    { return _commission; }
}

/****************************************************************/
double GarpRun::getValue(Scalar * x)   
{ return _garp->getValue(x); }

/****************************************************************/
int GarpRun::serialize(Serializer * serializer)
{
  // TODO: finish implementation
  return 0;
}

/****************************************************************/
int GarpRun::deserialize(Deserializer * deserializer)
{
  // TODO: finish implementation
  return 0;
}

/****************************************************************/
