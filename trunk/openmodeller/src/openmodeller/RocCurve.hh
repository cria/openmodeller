/**
 * Declaration of RocCurve class 
 * 
 * @author Renato De Giovanni (renato at cria . org . br)
 * $Id$
 *
 * LICENSE INFORMATION
 * 
 * Copyright(c), CRIA, Reference Center on Environmental Information 
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

#ifndef _ROCCURVEHH_
#define _ROCCURVEHH_

#include <openmodeller/om_defs.hh>
#include <openmodeller/Configuration.hh>

#include <openmodeller/Model.hh>

#include <openmodeller/Occurrences.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/Sampler.hh>

#include <map>

#define ROC_DEFAULT_RESOLUTION 15
#define ROC_DEFAULT_BACKGROUND_POINTS 10000

/**
 * Class ROC curve
 */
class dllexp RocCurve
{
public:
  /** 
   * Default constructor.
   * @param resolution Number of points to be calculated for the curve.
   * @param background_points Number of background points to be generated when there are no absences.
   * @param use_absences_as_background Indicates if absence points should be used as background points to calculate the proportional area. Internally, when no absence points are provided, background points are randomly generated to use the proportional area approach anyway.
   */
    RocCurve( int resolution=ROC_DEFAULT_RESOLUTION, int background_points=ROC_DEFAULT_BACKGROUND_POINTS, bool use_absences_as_background=false );

  /** 
   * Destructor.
   */
  ~RocCurve();

  /** 
   * Reset all internal values.
   * @param resolution New resolution (number of points to be calculated).
   * @param background_points Number of background points to be generated when there are no absences.
   * @param use_absences_as_background Indicates if absence points should be used as background points to calculate the proportional area. Internally, when no absence points are provided, background points are randomly generated to use the proportional area approach anyway.
   */
  void reset( int resolution=ROC_DEFAULT_RESOLUTION, int background_points=ROC_DEFAULT_BACKGROUND_POINTS, bool use_absences_as_background=false );

  /** 
   * Calculate ROC curve given a Model and a Sampler object. This method loads model
   * predictions for all sampler points, calculates all points for the curve and 
   * calculates the area under the curve.
   * @param model Model object to be evaluated.
   * @param Sampler Pointer to a Sampler object that will provide data for evaluation.
   */
  void calculate( const Model& model, const SamplerPtr& sampler );
  
  /** 
   * Return the number of points for the curve.
   * @return Total number of points for the curve. 
   */
  int numPoints() const { return _data.size(); }

  /** 
   * Return the X axis value for a particular point (1-specificity if absence
   * points were provided, otherwise proportion of background points predicted present). 
   * Need to call "calculate" first.
   * @param point_index Point index.
   * @return X value for the point.
   */
  double getX( int point_index ) const { return _data[point_index][0]; }

  /** 
   * Return the Y axis value of a particular point (sensitivity). Need to call 
   * "calculate" first.
   * @param point_index Point index.
   * @return Y value for the point.
   */
  double getY( int point_index ) const { return _data[point_index][1]; }

  /** 
   * Return the total area under the curve. Need to call "calculate" first.
   * @return Area under the curve.
   */
  double getTotalArea();

  /** 
   * Return the ratio between the area under the curve and the area under the diagonal
   * considering only points where Y is greater than (1 - e), i.e, with omission error
   * less than a specified value. Need to call "calculate" first.
   * @param e Maximum accepted omission error [0,1].
   * @return Ratio between area under the curve and area under the diagonal for points
   * that have omission error lesss than a specified value.
   */
  double getPartialAreaRatio( double e=1.0 );

  /** 
   * Check whether the ROC curve has been calculated already
   */
  bool ready() const { return _ready; }

  /** 
   * Serialize the ROC curve
   */
  ConfigurationPtr getConfiguration() const;

private:

  /** 
   * Custom operator to help calculating the points of the curve.
   */
  struct VectorCompare {

    bool operator () ( const std::vector<Scalar> &a, const std::vector<Scalar> &b ) const {

      if ( a[0] != b[0] ) {

        return a[0] < b[0]; // Compare 1 - specificity.
      }
      else {

        return a[1] < b[1]; // Compare sensitivity (1 - specificity values are equal).
      }
    }
  };

 /**
  * Get model predictions for each sample.
  * @param model Model object to be evaluated.
  * @param sampler Sampler object with environment, presences and optionally absence points (no absences will trigger the background points approach).
  */
  void _loadPredictions( const Model& model, const SamplerPtr& sampler );
 
  /** 
   * Calculate all points of the curve.
   */
  void _calculateGraphPoints(); 

  /** 
   * Calculate the total area under the curve.
   */
  bool _calculateTotalArea(); 
  
  std::vector<int> _category;      // 0=absence, 1=presence
  std::vector<Scalar> _prediction; // associated probabilities

  std::vector< std::vector<Scalar> > _data; // Main data structure to store all points
  
  int _resolution; // Number of points on the curve
  int _background_points; // Number of background points to be generated when there are no absences
  bool _use_proportional_area; // Indicates if background points should be used to calculate the proportional area instead of using absence points

  int _true_negatives; // Number of true negatives (binarized)
  int _true_positives; // Number of true positives (binarized)

  double _auc; // Area under the curve. Need to store this to avoid recalculating in serialization.

  std::map<double, double> _ratios; // Ratios calculated via getPartialAreaRatio (max omission <=> ratio). Ratios are stored here to be used during serialization.

  std::vector<Scalar> _thresholds; // Thresholds in ascending order

  std::vector<Scalar> _proportions; // Proportional area for each point

  bool _ready;
};

#endif
