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

/* Class AreaStats counts several cell statistics collected during
 * output map generation.
 */
class dllexp AreaStats
{
public:
  /** 
   * Default constructor.
   * @param predictionThreshold Threshold for prediction values, above
   *        which a prediction value means a predicted presence.
   */
  AreaStats(Scalar predictionThreshold = 0.5);

  /** 
   * Destructor.
   */
  ~AreaStats();

  /** 
   * Resets area counters.
   * @param predictionThreshold New threshold for prediction 
   *   values, above which a prediction value means a predicted
   *   presence.
   */
  void reset(Scalar predictionThreshold = 0.5);

  /** 
   * Count another cell based on its prediction value.
   * @param predictionValue Value of the prediction in the current cell.
   */
  void addPrediction(Scalar predictionValue);

  /** 
   * Count a cell where the prediction doesn't apply.
   */
  void addNonPrediction();
  
  /** 
   * Returns total number of cells counted.
   */
  int getTotalArea()              { return _areaTotal; }

  /** 
   * Returns total number of cell predicted present.
   */
  int getAreaPredictedPresent()   { return _areaPredPresent; }

  /** 
   * Returns total number of cell predicted absent.
   */
  int getAreaPredictedAbsent()    { return _areaPredAbsent; }

  /** 
   * Returns total number of cells where prediction doesn't apply.
   */
  int getAreaNotPredicted()       { return _areaNotPredicted; }

private:
  int _areaTotal;
  int _areaPredPresent;
  int _areaPredAbsent;
  int _areaNotPredicted;

  Scalar _predictionThreshold;
};
