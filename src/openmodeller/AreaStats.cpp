/**
 * Definition of AreaStats class 
 * 
 * @file area_stats.hh
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


#include <om_area_stats.hh>

AreaStats::AreaStats(Scalar predictionThreshold)
{
  reset(predictionThreshold);
}


AreaStats::~AreaStats()
{
}

void AreaStats::reset(Scalar predictionThreshold)
{
  _predictionThreshold = predictionThreshold;
  _areaTotal = _areaPredPresent = _areaPredAbsent = _areaNotPredicted = 0;
}


void AreaStats::addPrediction(Scalar value)
{
  _areaTotal++;
  if (value >= _predictionThreshold)
    { _areaPredPresent++; }
  else 
    { _areaPredAbsent++; }
}


void AreaStats::addNonPrediction()
{
  _areaTotal++;
  _areaNotPredicted++;
}


