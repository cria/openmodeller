/**
 * Declaration of Minimum Distance algorithm.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-09-20
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

#ifndef _MINIMUM_DISTANCEHH_
#define _MINIMUM_DISTANCEHH_

#include <om.hh>


/****************************************************************/
/************************* Minimum Distance *********************/

/** 
 * Algorithm to generate models based on the cartesian distance
 * from the closest occurrence point.
 */
class MinimumDistance : public Algorithm
{
public:

  /** Occurrences within this distance will be considered
   *  the same one. */
  MinimumDistance();
  virtual ~MinimumDistance();

  // Inherited from Algorithm class.
  int needNormalization( Scalar *min, Scalar *max );

  int initialize( int ncicle );
  int iterate();
  int done();

  Scalar getValue( Scalar *x );
  int    getConvergence( Scalar *val );


private:

  /** Calculate cartesian distance between 'x' and 'pnt',
   *  with dimensions equal to dim.*/
  Scalar findDist( Scalar *x, Scalar *pnt, int dim );


  int    _done;
  Scalar _dist;

  SampledData *_presence;
};


#endif

