/**
 * Declaration of Minimum Distance algorithm.
 * 
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

#include <openmodeller/om.hh>

/****************************************************************/
/************************* Minimum Distance *********************/

/** 
 * Algorithm to generate models based on the cartesian distance
 * from the closest occurrence point.
 */
class MinimumDistance : public AlgorithmImpl
{
public:

  /** Occurrences within this distance will be considered
   *  the same one. */
  MinimumDistance();
  virtual ~MinimumDistance();

  int initialize();
  int iterate();
  int done() const;

  Scalar getValue( const Sample& x ) const;
  int    getConvergence( Scalar *val );

protected:
  virtual void _getConfiguration( ConfigurationPtr& ) const;
  virtual void _setConfiguration( const ConstConfigurationPtr& );

private:

  bool _done;

  /** Calculate cartesian distance between 'x' and 'pnt',
   *  with dimensions equal to dim.*/
  Scalar findDist( const Sample& x, const Sample& pnt ) const;

  Scalar _dist;  ///< Parameter for MaxDistance

  bool _hasCategorical;
  int _numLayers;
  Sample _isCategorical;

  std::vector<Sample> _envPoints;

};


#endif

