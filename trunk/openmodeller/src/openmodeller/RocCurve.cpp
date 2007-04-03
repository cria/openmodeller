/**
 * Definition of RocCurve class 
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
 *
 * ACKNOWLEDGEMENTS
 * 
 * This program was largely based on the ROC curve routine provided
 * by the ROSETTA C++ library: http://sourceforge.net/projects/rosetta
 * Special thanks to Aleksander Ohrn for the great work.
 */

#include <stdio.h>

#include <openmodeller/RocCurve.hh>
#include <openmodeller/Sampler.hh>
#include <openmodeller/Algorithm.hh>
#include <openmodeller/Occurrences.hh>
#include <openmodeller/Environment.hh>
#include <openmodeller/Configuration.hh>
#include <openmodeller/Log.hh>

#include <string.h>

/*******************/
/*** constructor ***/
RocCurve::RocCurve( int resolution )
{
  reset( resolution );
}


/*******************/
/*** destructor ***/
RocCurve::~RocCurve()
{
}

/*************/
/*** reset ***/
void RocCurve::reset( int resolution )
{
  _ready = false;
  _resolution = resolution;
  _category.erase( _category.begin(), _category.end() );
  _prediction.erase( _prediction.begin(), _prediction.end() );
  _data.erase( _data.begin(), _data.end() );
  _true_negatives = 0;
  _true_positives = 0;
  _auc = 0.0;
}


/*****************/
/*** calculate ***/
void RocCurve::calculate( const Model& model, const SamplerPtr& sampler )
{
  reset( _resolution );

  _loadPredictions( sampler->getEnvironment(), model, 
                    sampler->getPresences(), sampler->getAbsences() );

  _calculateGraphPoints();

  bool calculated = _calculateArea();

  _ready = true;
}


/************************/
/*** load Predictions ***/
void RocCurve::_loadPredictions( const EnvironmentPtr & env,
                                 const Model& model,
                                 const OccurrencesPtr& presences, 
                                 const OccurrencesPtr& absences )
{
  int size = presences->numOccurrences() + absences->numOccurrences();

  _category.reserve( size );
  _prediction.reserve( size );

  OccurrencesImpl::const_iterator it = presences->begin();
  OccurrencesImpl::const_iterator fin = presences->end();

  model->setNormalization( env );

  int i = 0;
  while( it != fin ) {

    Sample sample; 

    if ( env ) {

      sample = env->get( (*it)->x(), (*it)->y() );
    }
    else {

      sample = (*it)->environment();
    }

    if ( sample.size() > 0 ) {

      ++i;

      _category.push_back( 1 );
      _prediction.push_back( model->getValue( sample ) );
    }

    ++it;
  }

  i = 0;
  if ( absences && ! absences->isEmpty() ) {

    it = absences->begin();
    fin = absences->end();

    while( it != fin ) {

      Sample sample;

      if ( env ) {

	sample = env->get( (*it)->x(), (*it)->y() );
      }
      else {

	sample = (*it)->environment();
      }

      if ( sample.size() > 0 ) {

        ++i;

	_category.push_back( 0 );
	_prediction.push_back( model->getValue( sample ) );
      }

      ++it;
    }
  }
}


/******************************/
/*** calculate Graph Points ***/
void RocCurve::_calculateGraphPoints()
{
  int i, j, num_pairs = _prediction.size();

  // Compute a specified number of data points for the graph
  for ( i = 0; i < _resolution; i++ ) {

    // Define counters
    int num_tp = 0; // true positives
    int num_fp = 0; // false positives
    int num_tn = 0; // true negatives
    int num_fn = 0; // false negatives

    // Determine positivity criterion (threshold) for current point
    Scalar threshold = Scalar(i) / ( _resolution - 1 );

    // Process all pairs
    for ( j = 0; j < num_pairs; j++ ) {

      // Determine actual (binarized) and predicted value
      bool act_flag = ( _category[j] == 1 );
      bool prd_flag = ( _prediction[j] >= threshold );

      // Increment counters
      if ( act_flag && prd_flag ) {

        num_tp++;
      }
      else if ( !act_flag && !prd_flag ) {

        num_tn++;
      }
      else if ( !act_flag && prd_flag ) {

        num_fp++;
      }
      else {

        num_fn++;
      }
    }

    // Define counter sums
    int num_ap = num_tp + num_fn; // actual positives
    int num_pp = num_tp + num_fp; // predicted positives
    int num_an = num_tn + num_fp; // actual negatives
    int num_pn = num_tn + num_fn; // predicted negatives
    int num_tt = num_ap + num_an; // Total tally

    _true_negatives = num_an;
    _true_positives = num_ap;

    // Determine sensitivity, specificity, etc.
    Scalar sensitivity = (num_ap == 0) ? Scalar(-1) : Scalar(num_tp) / num_ap;
    Scalar specificity = (num_an == 0) ? Scalar(-1) : Scalar(num_tn) / num_an;
    Scalar ppvalue     = (num_pp == 0) ? Scalar(-1) : Scalar(num_tp) / num_pp;
    Scalar npvalue     = (num_pn == 0) ? Scalar(-1) : Scalar(num_tn) / num_pn;
    Scalar accuracy    = (num_tt == 0) ? Scalar(-1) : Scalar(num_tp + num_tn) / num_tt;

    // Is sensitivity and specificity defined?
    if ( sensitivity == -1 || specificity == -1 ) {

      continue;
    }

    std::vector<Scalar> v;
    v.reserve(6);

    // Store vector contents
    v.push_back(1 - specificity);
    v.push_back(sensitivity);
    v.push_back(ppvalue);
    v.push_back(npvalue);
    v.push_back(accuracy);
    v.push_back(threshold);

    // Append to data vector
    _data.push_back(v);

  }

  std::vector<Scalar> v00, v11;

  v00.reserve(6);
  v11.reserve(6);

  // Append (0, 0) and (1, 1) points artificially.
  v00.push_back(0.0);  // 1 - specificity
  v00.push_back(0.0);  // sensitivity
  v00.push_back(-1);   // ppvalue
  v00.push_back(-1);   // npvalue
  v00.push_back(-1);   // accuracy
  v00.push_back(-1);   // threshold

  v11.push_back(1.0);  // 1 - specificity
  v11.push_back(1.0);  // sensitivity
  v11.push_back(-1);   // ppvalue
  v11.push_back(-1);   // npvalue
  v11.push_back(-1);   // accuracy
  v11.push_back(-1);   // threshold

  _data.push_back(v00);
  _data.push_back(v11);

  VectorCompare compare;

  // Sort ROC points.
  std::sort( _data.begin(), _data.end(), compare );
}


/**********************/
/*** calculate Area ***/
bool RocCurve::_calculateArea()
{
  _auc = -1;

  int i, num_points = numPoints();

  // Verify dimensions
  if ( num_points < 2 ) {

    return false;
  }

  double area = 0.0;

  // Approximate area under ROC curve with trapezes
  for ( i = 1; i < num_points; i++ ) {

    double x1 = 1 - getSpecificity(i - 1);
    double y1 = getSensitivity(i - 1);
    double x2 = 1 - getSpecificity(i);
    double y2 = getSensitivity(i);

    if ( x2 != x1 ) {

      area += (x2 - x1) * 0.5 * (y1 + y2);
    }
  }

  _auc = area;

  return true;
}


/*************************/
/*** get Configuration ***/
ConfigurationPtr 
RocCurve::getConfiguration() const
{
  ConfigurationPtr config( new ConfigurationImpl("RocCurve") );

  config->addNameValue( "Auc", getArea() );

  int num_points = numPoints();

  double *tmp_points = new double[num_points*2];

  int cnt = 0;

  for ( int i = 0; i < num_points; ++i, ++cnt ) {

    tmp_points[cnt] = 1 - getSpecificity( i );

    ++cnt;

    tmp_points[cnt] = getSensitivity( i );
  }

  config->addNameValue( "Points", tmp_points, num_points*2 );

  delete[] tmp_points;

  return config;
}
