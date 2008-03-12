/**
 * Declaration of Jackknife class 
 * 
 * @author Fabrício Rodrigues (fabricio . poliusp at gmail . com)
 * @author Renato De Giovanni (renato at cria . org . br)
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c), CRIA, Reference Center on Environmental Information 
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

#ifndef _OM_JACKKNIFE_
#define _OM_JACKKNIFE_

#include <openmodeller/Sampler.hh>
#include <openmodeller/Algorithm.hh>

/**
 * Jackknife class
 */
class dllexp Jackknife
{
public:
  /** 
   * Default constructor.
   */
  Jackknife();

  /** 
   * Destructor.
   */
  ~Jackknife();

  /** 
   * Run jackknife.
   * @param samplerPtr Sampler with environment and points.
   * @param algorithmPtr Algorithm to be used.
   * @param propTrain Training/test proportion for the points.
   */
  void run( SamplerPtr samplerPtr, AlgorithmPtr algorithmPtr, double propTrain );

};

#endif
