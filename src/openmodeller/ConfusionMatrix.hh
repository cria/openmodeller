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

#include <Model.hh>

#include <om_occurrences.hh>
#include <environment.hh>
#include <om_sampler.hh>

/**
 * Class ConfusionMatrix tabulates the proportions of types of
 * successes and errors generated by the model. 
 * The matrix is represented as following:
 * +---------------------------------+
 * |           |  Actual Values      |
 * + Predicted |---------------------+
 * | Values    |  Presence | Absence |
 * +---------------------------------+
 * | Presence  |     a     |    b    |
 * +---------------------------------+
 * | Absence   |     c     |    d    |
 * +---------------------------------+
 * 
 * Values a and d are correct predictions, while b and c are
 * Type I and Type II errors.
 */
class dllexp ConfusionMatrix
{
public:
  /** 
   * Default constructor.
   * @param predictionThreshold Threshold for prediction values, above
   *        which a prediction value means a predicted presence.
   */
  ConfusionMatrix(Scalar predictionThreshold = 0.5);

  /** 
   * Destructor.
   */
  ~ConfusionMatrix();

  /** 
   * Resets area counters.
   * @param predictionThreshold New threshold for prediction 
   *   values, above which a prediction value means a predicted
   *   presence.
   */
  void reset(Scalar predictionThreshold = 0.5);

  /** 
   * Calculate confusion matrix based on model and sampled data
   * from environment and occurrences objects.
   * @param env Pointer to Environment object containing the 
   *  environmental data to be used in calculation
   * @param model Model object to be evaluated
   * @param presences Pointer to an Occurrences object storing
   *  the presence points being evaluated
   * @param absences Pointer to an Occurrences object storing
   *  the absence points being evaluated
   */
  void calculate(const EnvironmentPtr & env, const Model& model, 
		 const OccurrencesPtr& presences, const OccurrencesPtr& absences = OccurrencesPtr());
  
  /** 
   * Calculate confusion matrix based on an abstract Sampler object
   * @param model Model object to be evaluated
   * @param Sampler Pointer to a Sampler object that will provide
   *  data for evaluation
   */
  void calculate(const Model& model, const SamplerPtr& sampler);
  
  /**
   * Returns a value from the confusion matrix.
   * @param predictionValue Prediction value corresponding to the 
   *  row selected in the matrix
   * @param actualValue Actual value corresponding to the 
   *  column selected in the matrix
   */
  int getValue(Scalar predictionValue, Scalar actualValue);

  /** 
   * Returns the accuracy of the model, which corresponds to the
   *  sum of successes (points predicted correctly) divided by
   *  all available points. I.e., accuracy = (a + d) / (a + b + c + d)
   */
  double getAccuracy();

  /** 
   * Returns the omission error of the model, which corresponds to the
   *  sum of presence points predicted absent divided by
   *  all available presence points. 
   *  I.e., omission = c / (a + c)
   */
  double getOmissionError();

  /** 
   * Returns the commission error of the model, which corresponds 
   *  to the sum of absence points predicted present divided by
   *  all available absence points. 
   *  I.e., omission = b / (b + d)
   */
  double getCommissionError();

  /** 
   * Check whether the confusion matrix has been calculated already
   */
  bool ready() { return _ready; }

private:
  /* 
   * Representation of the confusion matrix on _confMatrix:
   *  1st row is predicted absent
   *  2nd row is predicted present
   *  1st column is actual absent
   *  2nd column is actual present
   * 
   * Note: it is inverted from the matrix representation above
   *        so that the indexes match the actual or predicted values,
   *        i.e., index[0][1] corresponds to predicted absence (0)
   *        and actual presence (1).
   */
  int _confMatrix[2][2];

  Scalar _predictionThreshold;

  bool _ready;
};
