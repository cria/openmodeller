/**
 * Declaration of Bioclimatic Envelope Algorithm - Nix, 1984.
 * 
 * @file
 * @author Ricardo Scachetti Pereira <ricardo@cria.org.br>
 * @date 2003-09-29
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c) 2003 by The University of Kansas Natural History 
 * Museum and Biodiversity Research Center.
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

#ifndef _BIOCLIMHH_
#define _BIOCLIMHH_


#include <algorithm.hh>

class Occurrences;
class Sampler;


/****************************************************************/
/************************** Bioclim Model ***********************/

/** 
 * Bioclimatic Envelope Algorithm
 *
 */
class BioclimModel : public Algorithm
{
public:

  BioclimModel( Sampler *samp, Scalar cutoff );
  virtual ~BioclimModel();

  // Inherited from Algorithm class.
  char *name()    { return "BioclimModel"; }
  int   iterate();
  int   done();

  Scalar getValue( Scalar *x );
  int    getConvergence( Scalar *val );


private:

  int    f_done;
  Scalar f_cutoff;

  Scalar *f_min;   ///> Minimum value for variables after cutoff 
  Scalar *f_max;   ///> Maximum value for variables after curoff
  Scalar *f_avg;   ///> Average value
  Scalar *f_range; ///> Range of values
  int     f_dim;   ///> Number of dimensions in environmental space
};


#endif

