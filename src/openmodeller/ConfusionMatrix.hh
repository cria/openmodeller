/**
 * Declaration of ConfusionMatrix class 
 * 
 * @file om_conf_matrix.hh
 * @author Ricardo Scachetti Pereira (rpereira@ku.edu)
 * @date 2004-10-18
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
 */

#include <om_defs.hh>

class Sampler;
class Algorithm;

class ConfusionMatrix
{
public:
  ConfusionMatrix(Scalar predictionThreshold = 0.5);
  ~ConfusionMatrix();

  void reset(Scalar predictionThreshold = 0.5);

  void calculate(Sampler * sampler, Algorithm * alg);
  
  int getValue(Scalar predictionValue, Scalar actualValue);

  double getAccuracy();
  double getOmissionError();
  double getCommissionError();

  bool ready() { return _ready; }

private:
  /* 
   * Confusion Matrix:
   *  1st row is predicted present
   *  2nd row is predicted absent
   *  1st column is actual present
   *  2nd column is actual absent
   */
  int _confMatrix[2][2];

  Scalar _predictionThreshold;

  bool _ready;
};
