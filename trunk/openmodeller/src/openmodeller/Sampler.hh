/**
 * Declaration of Sampler and Samples classes.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-05-27
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2003 by CRIA -
 * Centro de Referencia em Informacao Ambiental
 *
 * http://www.cria.org.br
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
 */

#ifndef _SAMPLERHH_
#define _SAMPLERHH_

#include <om_defs.hh>


/****************************************************************/
/*************************** Sampler ****************************/

class Occurrences;
class Environment;
class SampledData;

/** 
 * Base class to create samplers of environmental variables and
 * occurrence data. Each derived class can implement different 
 * behaviors, such as proportional sampling according to the 
 * distribution of occurrence data, disproportional sampling
 * regarding presence and absence data, etc.
 */
class Sampler
{
public:

  Sampler( Environment *env, Occurrences *presence,
	   Occurrences *absence=0 );
  ~Sampler();


  /** Number of environmental variables. */
  int dimEnv();


  /** Number of presences (occurrence points). */
  int numPresence();

  /** Number of absences (localities with no occurrence). */
  int numAbsence();

  /** Get the environment values at the presence localities.
   *
   * @param npnt Maximum number of localities to get.
   *  If it is negative, gets all avaiable localities.
   * @param env Filled with the environmental vector values.
   * 
   * @return Number of presence points filled in.
   */
  virtual int getPresence( SampledData *env, int npnt=-1 );

  /** Get the environment values at the absence localities.
   *
   * @param npnt Maximum number of localities to get.
   *  If it is negative, gets all avaiable localities.
   * @param env Filled with the environmental vector values.
   *  
   * @return Number of absence points filled in.
   */
  virtual int getAbsence( SampledData *env, int npnt=-1 );

  /** Get the environment values at some random localities.
   * It is not garanteed that the environment values are different
   * from some presence data!
   * 
   * @param npnt Maximum number of localities to get (>= 0).
   * @param env Filled with the environmental vector values.
   *
   * @return Number of absence points filled in.
   */
  virtual int getPseudoAbsence( SampledData *env, int npnt );


  /** Samples presence and absence (or pseudo-absence) points.
   * By default, returns npnt/2 (upper rounded) points of presence
   * and npnt/2 (lower rounded) points of absence.
   * 
   * @param npnt Maximum number of localities to sample.
   * @param npresence Number of presences sampled.
   * @param presence Filled with environmental values of presence
   *  localities.
   * @param nabsence Number of absences sampled.
   * @param absence Filled with environmental values of absence
   *  (or pseudo-absence) localities.
   *
   * @return Total of sampled localities.
   */
  virtual int getSamples( SampledData *presence, SampledData *absence,
			  int npnt );

  /** Sets types[i] = 1 if variable associated to "i" is
   * categorical (eg: soil), otherwise set types[i] = 0.
   * i = 0, ..., dim()-1
   */
  int varTypes( int *types );


protected:

  /** Get the environment values at the given localities.
   *
   * @param occur Localities points.
   * @param npnt Maximum number of localities to get.
   * @param env Filled with the environmental vector values.
   *  env[i * dim(), j] is the j-th environmental value for the
   *  i-th point.
   *  
   * @return Number of localities found.
   */
  int getOccurrence( Occurrences *occur, SampledData *env,
		     int npnt );


  Occurrences *_presence;
  Occurrences *_absence;
  Environment *_env;
};


#endif
