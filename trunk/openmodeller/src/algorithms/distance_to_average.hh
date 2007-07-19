/**
 * Declaration of cartesian DistanceToAverage algorithm.
 * 
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


#ifndef _DISTANCE_TO_AVERAGEHH__
#define _DISTANCE_TO_AVERAGEHH__

#include <openmodeller/om.hh>

/****************************************************************/
/************************ Distance To Average *******************/

/** 
 * Algorithm to generate models based on the cartesian
 * DistanceToAverage from the average related to occurrence
 * points.
 */
class DistanceToAverage : public AlgorithmImpl
{
public:

  DistanceToAverage();
  virtual ~DistanceToAverage();

  int initialize();
  int iterate();
  int done() const;

  Scalar getValue( const Sample& x ) const;
  int    getConvergence( Scalar *val );

protected:
  virtual void _getConfiguration( ConfigurationPtr& ) const;
  virtual void _setConfiguration( const ConstConfigurationPtr&  );

private:

  bool _done;
  Scalar _dist;
  mutable Scalar _min; ///< Store the minimum distance (for debug).
  mutable Scalar _max; ///< Store the maximum distance (for debug).

  Sample  _avg;  ///< Average related to occurrence points.
};


#endif

