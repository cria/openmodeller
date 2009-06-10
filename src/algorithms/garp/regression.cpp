/**
 * Definition of Regression class
 * 
 * @file   regression.cpp
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date   2005-01-28
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c), The Center for Research, University of Kansas, 
 *                 2385 Irving Hill Road, Lawrence, KS 66044-4755, USA.
 * Copyright(c), David R.B. Stockwell of Symbiotik Pty. Ltd.
 * Copyright(c), CRIA - Centro de Referencia em Informacao Ambiental
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
 * This is an implementation of the GARP algorithm first developed 
 * by David Stockwell
 * 
 */


#include <openmodeller/om.hh>
#include <openmodeller/Sample.hh>

#include "regression.hh"

Regression::Regression() :
  _dimensions(0),
  _resamples(0),
  _paramA(),
  _paramB(),
  _paramC()
{ }


void Regression::calculateParameters(const OccurrencesPtr& occs)
{
  //next two lines commented out since the vars arent used
  //Scalar y;         // outcome (dependent) variable for sample
  //Scalar xx;        // temp storage for x^2
  Sample x;         // predictor (independent) variable for current sample
  Scalar s_y;      // sum of y for all samples
  Sample s_x;      // sum of x for all samples
  Sample s_xy;    // sum of x*y for all samples
  Sample s_xx;     // sum of x^2 for all samples
  Sample s_xxy;   // sum of y*x^2 for all samples
  Sample s_x4;     // sum of x^4 for all samples

  int n = _resamples = occs->numOccurrences();
  
  // number of layers/genes/independent variables
  _dimensions = (*occs)[0]->environment().size();
  
  s_y = 0.0;
  s_x = s_xy = s_xx = s_xxy = s_x4 = Sample(_dimensions);

  // iterate through the samples
  OccurrencesImpl::const_iterator oc_it = occs->begin();
  OccurrencesImpl::const_iterator oc_end = occs->end();

  while (oc_it != oc_end)
    {
      Scalar y = ( (*oc_it)->abundance() > 0.0 ) ? 1.0 : 0.0;
      Sample x = (*oc_it)->environment();

      Sample::iterator xit = x.begin();
      Sample::iterator end = x.end();
      Sample::iterator s_xi = s_x.begin();
      Sample::iterator s_xxi = s_xx.begin();
      Sample::iterator s_xyi = s_xy.begin();
      Sample::iterator s_xxyi = s_xxy.begin();
      Sample::iterator s_x4i = s_x4.begin();

      s_y += y;

      //printf("Regression: Resample %5d\n", i);

      while (xit != end)
	{
	  Scalar xi = (*xit);
	  Scalar xx = xi * xi;

	  (*s_xi) += xi;
	  (*s_xxi) += xx;
	  (*s_xyi) += xi * y;
	  (*s_xxyi) += xx * y; 
	  (*s_x4i) += xx * xx;

	  ++xit;
	  ++s_xi;
	  ++s_xxi;
	  ++s_xyi;
	  ++s_xxyi;
	  ++s_x4i;
	}

      oc_it++;
    }

  //printf("Sum of abundances: %7.3f\n", s_y);

  _paramC = _paramB = _paramA = Sample(_dimensions);

  Sample::iterator pai = _paramA.begin();
  Sample::iterator pbi = _paramB.begin();
  Sample::iterator pci = _paramC.begin();
  Sample::iterator end = _paramC.end();
  Sample::iterator s_xi = s_x.begin();
  Sample::iterator s_xxi = s_xx.begin();
  Sample::iterator s_xyi = s_xy.begin();
  Sample::iterator s_xxyi = s_xxy.begin();
  Sample::iterator s_x4i = s_x4.begin();

  while (pci != end)
    {
      //printf("Regression: ParamIndex (%12d, %12d)\n", pai, end);

      (*pci) = ( n * (*s_xxyi) - ( (*s_xxi) * s_y ) ) / 
	       ( n * (*s_x4i)  - ( (*s_xxi) * (*s_xxi) ) );

      (*pbi) = ( n * (*s_xyi)  - ( (*s_xi)  * s_y ) ) / 
	       ( n * (*s_xxi)  - ( (*s_xi)  * (*s_xi) ) );

      (*pai) = ( s_y / n ) - ( (*pbi) * (*s_xi) / n );

      ++pai; 
      ++pbi; 
      ++pci;
      
      ++s_xi;
      ++s_xxi;
      ++s_xyi;
      ++s_xxyi;
      ++s_x4i;
    }
  
  /*
  Log::instance()->info("logit: a=%+8.4f b=%+8.4f c=%+8.4f n=%d\n", a, b, c, n);
  Log::instance()->info("aux: xi=%+8.4f yi=%+8.4f xiyi=%+8.4f xxi=%+8.4f xxiyi=%+8.4f xi4=%+8.4f\n", 
	s_xi, s_yi, s_xiyi, s_xxi, s_xxiyi, s_xi4);
  */
}

