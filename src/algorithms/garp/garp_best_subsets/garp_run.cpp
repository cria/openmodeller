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
#include "serialization/serializable.hh"
#include "garp.hh"
#include "sampler.hh"

/****************************************************************/
/************************ GARP Run Thread ***********************/

// TODO: modify strategy to reuse threads instead of creating a 
//       new one for every garp run

THREAD_PROC_RETURN_TYPE GarpRunThreadProc(void * garpRun)
{
  while (!garpRun->done())
    garpRun->iterate();

  garpRun->project();
  garpRun->test();
  garpRun->finalize();

  THREAD_PROC_RETURN_STATEMENT;
}


/****************************************************************/
/************************* GARP Run *****************************/

GarpRun::GarpRun() 
{
  _id = -1;
  _active = false;
  _omission = -1.0;
  _commission = -1.0;
  _garp = NULL;      
}

GarpRun::~GarpRun() 
{
  if (_garp)
    delete _garp;
}

int
GarpRun::initialize(int id, Sampler * sampler, int nparam)
{
  _id = id;
  // TODO: finish implementation
}

int 
GarpRun::run()
{
  _active = true;
  THREAD_START(GarpRunThreadProc, this); 
}

int 
GarpRun::iterate()
{ _garp->iterate(); }

int 
GarpRun::done()
{ return _garp->done(); }

int 
GarpRun::finalize()           
{
  _active = false;
  _done = true; 
  THREAD_END();
}

double 
GarpRun::getOmission()     
{ return _omission; }

double 
GarpRun::getCommission()   
{ return _commission; }

int 
GarpRun::serialize(Serializer * serializer)
{
  // TODO: finish implementation
}

int
GarpRun::deserialize(Deserializer * deserializer)
{
  // TODO: finish implementation
}

