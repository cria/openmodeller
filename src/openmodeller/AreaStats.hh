/**
 * Declaration of AreaStats class 
 * 
 * @file om_area_stats.hh
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


class AreaStats
{
public:
  AreaStats(Scalar predictionThreshold = 0.5);
  ~AreaStats();

  void reset(Scalar predictionThreshold = 0.5);
  void addPrediction(Scalar predictionValue);
  void addNonPrediction();
  
  int getTotalArea()              { return _areaTotal; }
  int getAreaPredictedPresent()   { return _areaPredPresent; }
  int getAreaPredictedAbsent()    { return _areaPredAbsent; }
  int getAreaNotPredicted()       { return _areaNotPredicted; }

private:
  int _areaTotal;
  int _areaPredPresent;
  int _areaPredAbsent;
  int _areaNotPredicted;

  Scalar _predictionThreshold;
};
