/**
 * Declaration of cartesian DistanceToAverage algorithm.
 * 
 * @file
 * @author Mauro E S Muñoz (mauro@cria.org.br)
 * @date   2003-09-12
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


#ifndef _CSMHH_
#define _CSMHH_

#include <om.hh>
#include <serialization/serializable.hh>


/****************************************************************/
/************************ Distance To Average *******************/

/** 
 * Algorithm to generate models based on the cartesian
 * DistanceToAverage from the average related to occurrence
 * points.
 */
class DistanceToAverage : public Algorithm
{
public:

  // dist: Os pontos que estiver até esta distância são
  //       considerados iguais.
  //
  DistanceToAverage();
  virtual ~DistanceToAverage();

  // Inherited from Algorithm class.
  int needNormalization( Scalar *min, Scalar *max );

  int initialize();
  int iterate();
  int done();


  Scalar getValue( Scalar *x );
  int    getConvergence( Scalar *val );

  /*
   */
  int serialize(Serializer * serializer);
  int deserialize(Deserializer * deserializer);

private:

  int    _done;
  Scalar _dist;
  Scalar _min; ///< Store the minimum distance (for debug).
  Scalar _max; ///< Store the maximum distance (for debug).

  Scalar *_avg;  ///< Average related to occurrence points.
  int     _dim;  ///< Dimension of environmental space.
};


#endif

