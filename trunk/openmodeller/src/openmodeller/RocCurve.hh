/**
 * Declaration of RocCurve class 
 * 
 * @author Renato De Giovanni (renato at cria . org . br)
 * $Id: $
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

#include <openmodeller/om_defs.hh>
#include <openmodeller/Configuration.hh>

#include <openmodeller/Model.hh>

#include <openmodeller/Occurrences.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/Sampler.hh>

/**
 * Class ROC curve
 */
class dllexp RocCurve
{
public:
  /** 
   * Default constructor.
   * @param resolution Number of points to be calculated for the curve.
   */
  RocCurve( int resolution=15 );

  /** 
   * Destructor.
   */
  ~RocCurve();

  /** 
   * Reset all internal values.
   * @param resolution New resolution (number of points to be calculated).
   */
  void reset( int resolution=15 );

  /** 
   * Calculate ROC curve based on an abstract Sampler object. This method loads model
   * predictions for all sampler points, calculates all points for the curve and calculates
   * the area under the curve.
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
   * Return the specificity of a particular point. Need to call "calculate" first.
   * @param point_index Point index.
   * @return Specificity for the point.
   */
  double getSpecificity( int point_index ) const { return 1 - _data[point_index][0]; }

  /** 
   * Return the sensitivity of a particular point. Need to call "calculate" first.
   * @param point_index Point index.
   * @return Sensitivity for the point.
   */
  double getSensitivity( int point_index ) const { return _data[point_index][1]; }

  /** 
   * Return the area under the curve. Need to call "calculate" first.
   * @return Area under the curve.
   */
  double getArea() const { return _auc; }

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
  * @param env Pointer to Environment object containing the environmental data to be used.
  * @param model Model object to be evaluated.
  * @param presences Pointer to an Occurrences object storing the presence points being evaluated.
  * @param absences Pointer to an Occurrences object storing the absence points being evaluated.
  */
  void _loadPredictions( const EnvironmentPtr & env, const Model& model,
                         const OccurrencesPtr& presences, 
                         const OccurrencesPtr& absences = OccurrencesPtr() );
 
  /** 
   * Calculate all points of the curve.
   */
  void _calculateGraphPoints(); 

  /** 
   * Calculate the area under the curve.
   */
  bool _calculateArea(); 
  
  std::vector<int> _category;      // 0=absence, 1=presence
  std::vector<Scalar> _prediction; // associated probabilities

  std::vector< std::vector<Scalar> > _data; // Main data structure to store all points
  
  int _resolution; // Number of points on the curve

  int _true_negatives; // Number of true negatives (binarized)
  int _true_positives; // Number of true positives (binarized)

  double _auc; // Area under the curve

};
