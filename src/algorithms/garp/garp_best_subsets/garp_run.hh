/**
 * Declaration of GarpRun class
 * 
 * @file   garp_run.hh
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


#ifndef _GARP_RUN_HH_
#define _GARP_RUN_HH_

#include <om.hh>

#include "serialization/serializable.hh"
#include "garp_run_thread.hh"

class Garp;
class Sampler;

void GarpRunThreadProc(void * garpRun);

/****************************************************************/
/************************* GARP Run *****************************/

class GarpRun : public Serializable
{
public:
  GarpRun();
  virtual ~GarpRun();

  int initialize(Sampler *);
  int run();
  int iterate();
  int finalize();

  int done();
  double getOmission();
  double getCommission();

  int serialize(Serializer * serializer);
  int deserialize(Deserializer * deserializer);

private:

  int _id;                   /// Identified for this particular garp run
  bool _active;              /// Indicates whether the thread is running
  double _omission;          /// Omission error for this run
  double _commission;        /// Commission error, approximated by area predicted present
  Garp * _garp;              /// Garp algorithm used in this run
};


#endif

