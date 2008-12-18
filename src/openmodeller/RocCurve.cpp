/**
 * Definition of RocCurve class 
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

#include <openmodeller/Exceptions.hh>

#include <string.h>
#include <algorithm>
#include <map>

using namespace std;

/*******************/
/*** constructor ***/
RocCurve::RocCurve( int resolution, int background_points )
{
  reset( resolution, background_points );
}


/*******************/
/*** destructor ***/
RocCurve::~RocCurve()
{
}

/*************/
/*** reset ***/
void RocCurve::reset( int resolution, int background_points )
{
  _ready = false;
  _resolution = resolution;
  _background_points = background_points;
  _gen_background_points = false;
  _category.erase( _category.begin(), _category.end() );
  _prediction.erase( _prediction.begin(), _prediction.end() );
  _data.erase( _data.begin(), _data.end() );
  _true_negatives = 0;
  _true_positives = 0;
  _auc = -1.0;

  _proportions.erase( _proportions.begin(), _proportions.end() );
  _proportions.reserve( _resolution );

  _thresholds.erase( _thresholds.begin(), _thresholds.end() );
  _thresholds.reserve( _resolution );

  // Compute thresholds
  for ( int i = 0; i < _resolution; i++ ) {

    _thresholds.push_back( Scalar(i) / ( _resolution - 1 ) );
    _proportions.push_back( 0 );
  }

  _ratios.clear();
}


/*****************/
/*** calculate ***/
void RocCurve::calculate( const Model& model, const SamplerPtr& sampler )
{
  Log::instance()->info( "Calculating ROC curve\n" );

  reset( _resolution, _background_points );

  _loadPredictions( model, sampler );

  _calculateGraphPoints();

  _ready = true;
}


/************************/
/*** load Predictions ***/
void RocCurve::_loadPredictions( const Model& model, const SamplerPtr& sampler )
{
  // Check parameters

  EnvironmentPtr env = sampler->getEnvironment();

  if ( ! env ) {

    std::string msg = "No environment specified for the ROC curve\n";

    Log::instance()->error( msg.c_str() );

    throw InvalidParameterException( msg );
  }

  OccurrencesPtr presences = sampler->getPresences();
  OccurrencesPtr absences = sampler->getAbsences();

  if ( ! presences ) {

    std::string msg = "No presence points specified for the ROC curve\n";

    Log::instance()->error( msg.c_str() );

    throw InvalidParameterException( msg );
  }

  // Load predictions

  int size = presences->numOccurrences();

  if ( absences ) {

    size += absences->numOccurrences();
  }

  _category.reserve( size );
  _prediction.reserve( size );

  OccurrencesImpl::const_iterator it = presences->begin();
  OccurrencesImpl::const_iterator fin = presences->end();

  Scalar predictionValue;

  model->setNormalization( env );
  model->setNormalization( sampler );

  // Store predictions for presence points
  int i = 0;
  while( it != fin ) {

    Sample sample; 

    if ( (*it)->hasEnvironment() ) {

      sample = (*it)->environment();
    }
    else {

      sample = env->get( (*it)->x(), (*it)->y() );
    }

    if ( sample.size() > 0 ) {

      ++i;

      predictionValue = model->getValue( sample );

      _category.push_back( 1 );
      _prediction.push_back( predictionValue );

      Log::instance()->debug( "Probability for presence point %s (%f,%f): %f\n", 
                   ((*it)->id()).c_str(), (*it)->x(), (*it)->y(), predictionValue );
    }

    ++it;
  }

  // Store predictions for absence points
  i = 0;
  if ( absences && ! absences->isEmpty() ) {

    it = absences->begin();
    fin = absences->end();

    while( it != fin ) {

      Sample sample;

      if ( (*it)->hasEnvironment() ) {

	sample = (*it)->environment();
      }
      else {

	sample = env->get( (*it)->x(), (*it)->y() );
      }

      if ( sample.size() > 0 ) {

        ++i;

        predictionValue = model->getValue( sample );

	_category.push_back( 0 );
	_prediction.push_back( predictionValue );

        Log::instance()->debug( "Probability for absence point %s (%f,%f): %f\n", 
                     ((*it)->id()).c_str(), (*it)->x(), (*it)->y(), predictionValue );
      }

      ++it;
    }
  }
  else {

    // Generate background points when there are no absences

    Log::instance()->info( "No absence points specified\n" );

    Log::instance()->info( "Generating %d background points\n", _background_points );

    Scalar prob;

    int i = 0;

    do {

      Coord x,y;
      Sample s( env->getRandom( &x, &y ) );

      prob = model->getValue( s );

      for ( unsigned int j = 0; j < _thresholds.size(); j++ ) {

        if ( prob < _thresholds[j] ) {

          break;
        }

        _proportions[j] += 1;
      }

      ++i;

    } while ( i < _background_points );

    for ( unsigned int f = 0; f < _proportions.size(); f++ ) {

      _proportions[f] /= _background_points;
    }

    _gen_background_points = true;
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

    // Positivity criterion for current point
    Scalar threshold = _thresholds[i];

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

    // Ignore points with unknown sensitivity
    if ( sensitivity == -1 ) {

      continue;
    }

    // Ignore points with unknown specificity if absence points were provided
    if ( specificity == -1 && ! _gen_background_points ) {

      continue;
    }

    std::vector<Scalar> v;
    v.reserve(6);

    // Store vector contents

    // x value
    if ( _gen_background_points ) {

      v.push_back( _proportions[i] );
    }
    else {

      v.push_back(1 - specificity);
    }

    // y value
    v.push_back(sensitivity);

    v.push_back(ppvalue);
    v.push_back(npvalue);
    v.push_back(accuracy);
    v.push_back(threshold);

    // Append to data vector
    _data.push_back(v);
  }

  if ( ! _gen_background_points ) {

    // Append (0, 0) and (1, 1) points artificially.
    std::vector<Scalar> v00, v11;

    v00.reserve(6);
    v11.reserve(6);

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
  }

  VectorCompare compare;

  // Sort ROC points.
  std::sort( _data.begin(), _data.end(), compare );
}


/**********************/
/*** calculate Area ***/
bool RocCurve::_calculateTotalArea()
{
  _auc = -1.0;

  int i, num_points = numPoints();

  // Verify dimensions
  if ( num_points < 2 ) {

    return false;
  }

  double area = 0.0;

  // Approximate area under ROC curve with trapezes
  for ( i = 1; i < num_points; i++ ) {

    double x1 = getX(i - 1);
    double y1 = getY(i - 1);
    double x2 = getX(i);
    double y2 = getY(i);

    if ( x2 != x1 ) {

      area += (x2 - x1) * 0.5 * (y1 + y2);
    }
  }

  _auc = area;

  return true;
}


/**********************/
/*** get Total Area ***/
double 
RocCurve::getTotalArea() {

  if ( _auc < 0.0 ) {

    _calculateTotalArea();
  }

  return _auc; 
}


/******************************/
/*** get Partial Area Ratio ***/
double RocCurve::getPartialAreaRatio( double e )
{
  Log::instance()->info( "Calculating partial area for limit %f\n", e );

  double area = 0.0;

  double diag_area = 0.0;

  int i, num_points = numPoints();

  // Verify dimensions
  if ( num_points < 2 ) {

    return -1.0;
  }

  map<double, double>::iterator it;

  it = _ratios.find( e );

  if ( it != _ratios.end() ) {

    return _ratios[e];
  }

  // Approximate area under ROC curve with trapezes
  for ( i = 1; i < num_points; i++ ) {

    double x1 = getX(i - 1);
    double y1 = getY(i - 1);
    double x2 = getX(i);
    double y2 = getY(i);

    // Only points where Y is greater than or equals 1-e (e=maximum accepted omission)
    if ( x2 != x1 && y1 >= (1 - e) ) {

      area += (x2 - x1) * 0.5 * (y1 + y2);

      diag_area += (x2 - x1) * 0.5 * (x1 + x2);
    }
  }

  Log::instance()->debug( "Partial area calculated as: %f / %f\n", area, diag_area );

  _ratios[e] = area / diag_area;

  return _ratios[e];
}


/*************************/
/*** get Configuration ***/
ConfigurationPtr 
RocCurve::getConfiguration() const
{
  ConfigurationPtr config( new ConfigurationImpl("RocCurve") );

  config->addNameValue( "Auc", _auc );

  if ( _gen_background_points ) {

    config->addNameValue( "NumBackgroundPoints", _background_points );
  }

  int num_points = numPoints();

  double *tmp_points = new double[num_points*2];

  int cnt = 0;

  for ( int i = 0; i < num_points; ++i, ++cnt ) {

    tmp_points[cnt] = getX( i );

    ++cnt;

    tmp_points[cnt] = getY( i );
  }

  config->addNameValue( "Points", tmp_points, num_points*2 );

  for ( map<double, double>::const_iterator it = _ratios.begin(); it != _ratios.end(); it++ ) {

    ConfigurationPtr ratio( new ConfigurationImpl( "Ratio" ) );

    ratio->addNameValue( "E", (*it).first );
    ratio->addNameValue( "Value", (*it).second );

    config->addSubsection( ratio );
  }

  delete[] tmp_points;

  return config;
}
