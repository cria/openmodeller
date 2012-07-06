/**
 * Definition of the base (abstract) class for MaxEnt algorithms.
 * 
 * @author Elisangela S. da C. Rodrigues (elisangela . rodrigues [at] poli . usp . br)
 * @author Renato De Giovanni (renato [at] cria . org . br)
 * @author Daniel Bolgheroni (daniel [at] cria . org . br)
 * $Id$
 * 
 * LICENSE INFORMATION 
 * 
 * Copyright(c) 2007 by CRIA -
 * Centro de Referencia em Informacao Ambiental
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

#ifdef WIN32
// avoid warnings caused by problems in VC headers
#define _SCL_SECURE_NO_DEPRECATE
#endif

#include "maximum_entropy.hh"
#include <openmodeller/Sampler.hh>

#include "linear_feature.hh"
#include "quadratic_feature.hh"
#include "product_feature.hh"
#include "hinge_feature.hh"
#include "threshold_feature.hh"

#include "threshold_generator.hh"
#include "hinge_generator.hh"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cmath>
#include <algorithm>

#include <limits>
#include <math.h>
#ifdef MSVC
#include <float.h> //for _isnan
#endif

#include <string>

using namespace std;
using std::pair;

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 14

#define BACKGROUND_ID      "NumberOfBackgroundPoints"
#define USE_ABSENCES_ID    "UseAbsencesAsBackground"
#define MERGE_POINTS_ID    "IncludePresencePointsInBackground"
#define ITERATIONS_ID      "NumberOfIterations"
#define TOLERANCE_ID       "TerminateTolerance"
#define OUTPUT_ID          "OutputFormat"
#define QUADRATIC_ID       "QuadraticFeatures"
#define PRODUCT_ID         "ProductFeatures"
#define HINGE_ID           "HingeFeatures"
#define THRESHOLD_ID       "ThresholdFeatures"
#define AUTOFEATURES_ID    "AutoFeatures"
#define PT_THR_ID          "MinSamplesForProductThreshold"
#define Q_THR_ID           "MinSamplesForQuadratic"
#define HINGE_THR_ID       "MinSamplesForHinge"

#define MAXENT_LOG_PREFIX "Maxent: "

#define MINLIMIT 1.0e-6
#define MINDEV 1.0e-5

/******************************/
/*** Algorithm's parameters ***/

static AlgParamMetadata parameters[NUM_PARAM] = {
  
  // Number of background points to be generated
  {
    BACKGROUND_ID,                 // Id.
    "Number of background points", // Name.
    Integer,                       // Type.
    "Number of background points to be generated.", // Overview
    "Number of background points to be generated.", // Description.
    1,      // Not zero if the parameter has lower limit.
    0,      // Parameter's lower limit.
    1,      // Not zero if the parameter has upper limit.
    10000,  // Parameter's upper limit.
    "10000" // Parameter's typical (default) value.
  },
  // Use absence points as background
  {
    USE_ABSENCES_ID,     // Id.
    " Use absence points as background", // Name.
    Integer,  // Type.
    " Use absence points as background", // Overview
    "When absence points are provided, this parameter can be used to instruct the algorithm to use them as background points. This would prevent the algorithm to randomly generate them, also facilitating comparisons between different algorithms.", // Description.
    1,   // Not zero if the parameter has lower limit.
    0,   // Parameter's lower limit.
    1,   // Not zero if the parameter has upper limit.
    1,   // Parameter's upper limit.
    "0"  // Parameter's typical (default) value.
  },
  // Indication if presence points should be merged with background points
  {
    MERGE_POINTS_ID,               // Id.
    "Include input points in the background", // Name.
    Integer,                       // Type.
    "Include input points in the background: 0=No, 1=Yes.", // Overview
    "Include input points in the background: 0=No, 1=Yes.", // Description.
    1,   // Not zero if the parameter has lower limit.
    0,   // Parameter's lower limit.
    1,   // Not zero if the parameter has upper limit.
    1,   // Parameter's upper limit.
    "1"  // Parameter's typical (default) value.
  },
  // Number of iterations
  {
    ITERATIONS_ID,          // Id.
    "Number of iterations", // Name.
    Integer,                // Type.
    "Number of iterations.", // Overview
    "Number of iterations.", // Description.
    1,    // Not zero if the parameter has lower limit.
    1,    // Parameter's lower limit.
    0,    // Not zero if the parameter has upper limit.
    0,    // Parameter's upper limit.
    "500" // Parameter's typical (default) value.
  },
  // Terminate tolerance
  {
    TOLERANCE_ID, // Id.
    "Terminate tolerance", // Name.
    Real, // Type.
    "Tolerance for detecting model convergence.", // Overview
    "Tolerance for detecting model convergence.", // Description.
    1,        // Not zero if the parameter has lower limit.
    0.0,      // Parameter's lower limit.
    0,        // Not zero if the parameter has upper limit.
    0,        // Parameter's upper limit.
    "0.00001" // Parameter's typical (default) value.
  },
  // Output Format
  {
    OUTPUT_ID, // Id.
    "Output format", // Name
    Integer, // Type
    "Output format: " // Overview
    "1 = Raw, "
    "2 = Logistic. ",
    "Output format: " // Description
    "1 = Raw, "
    "2 = Logistic. ",
    1,  // Not zero if the parameter has lower limit
    1,  // Parameter's lower limit
    1,  // Not zero if the parameter has upper limit
    2,  // Parameter's upper limit
    "2" // Parameter's typical (default) value
  },
  // Quadratic features
  {
    QUADRATIC_ID, // Id.
    "Quadratic features", // Name
    Integer, // Type
    "Enable quadratic features (0=no, 1=yes)", // Overview
    "Enable quadratic features (0=no, 1=yes)", // Description
    1,  // Not zero if the parameter has lower limit
    0,  // Parameter's lower limit
    1,  // Not zero if the parameter has upper limit
    1,  // Parameter's upper limit
    "1" // Parameter's typical (default) value
  },
  // Product features
  {
    PRODUCT_ID, // Id.
    "Product features", // Name
    Integer, // Type
    "Enable product features (0=no, 1=yes)", // Overview
    "Enable product features (0=no, 1=yes)", // Description
    1,  // Not zero if the parameter has lower limit
    0,  // Parameter's lower limit
    1,  // Not zero if the parameter has upper limit
    1,  // Parameter's upper limit
    "1" // Parameter's typical (default) value
  },
  // Hinge features
  {
    HINGE_ID, // Id.
    "Hinge features", // Name
    Integer, // Type
    "Enable hinge features (0=no, 1=yes)", // Overview
    "Enable hinge features (0=no, 1=yes)", // Description
    1,  // Not zero if the parameter has lower limit
    0,  // Parameter's lower limit
    1,  // Not zero if the parameter has upper limit
    1,  // Parameter's upper limit
    "1" // Parameter's typical (default) value
  },
  // Threshold features
  {
    THRESHOLD_ID, // Id.
    "Threshold features", // Name
    Integer, // Type
    "Enable threshold features (0=no, 1=yes)", // Overview
    "Enable threshold features (0=no, 1=yes)", // Description
    1,  // Not zero if the parameter has lower limit
    0,  // Parameter's lower limit
    1,  // Not zero if the parameter has upper limit
    1,  // Parameter's upper limit
    "1" // Parameter's typical (default) value
  },
  // Auto features
  {
    AUTOFEATURES_ID, // Id.
    "Auto features", // Name
    Integer,         // Type
    "Enable auto features (0=no, 1=yes)", // Overview
    "Enable auto features (0=no, 1=yes)", // Description
    1,  // Not zero if the parameter has lower limit
    0,  // Parameter's lower limit
    1,  // Not zero if the parameter has upper limit
    1,  // Parameter's upper limit
    "1" // Parameter's typical (default) value
  },
  // Product/threshold threshold
  {
    PT_THR_ID,                    // Id.
    "Product/threshold threshod", // Name.
    Integer,                      // Type.
    "Number of samples at which product and threshold features start being used.", // Overview
    "Number of samples at which product and threshold features start being used (only when auto features is enabled).", // Description.
    1,      // Not zero if the parameter has lower limit.
    1,      // Parameter's lower limit.
    0,      // Not zero if the parameter has upper limit.
    0,      // Parameter's upper limit.
    "80"    // Parameter's typical (default) value.
  },
  // Quadratic threshold
  {
    Q_THR_ID,              // Id.
    "Quadratic threshold", // Name.
    Integer,               // Type.
    "Number of samples at which quadratic features start being used.", // Overview
    "Number of samples at which quadratic features start being used (only when auto features is enabled).", // Description.
    1,      // Not zero if the parameter has lower limit.
    1,      // Parameter's lower limit.
    0,      // Not zero if the parameter has upper limit.
    0,      // Parameter's upper limit.
    "10"    // Parameter's typical (default) value.
  },
  // Hinge threshold
  {
    HINGE_THR_ID,      // Id.
    "Hinge threshold", // Name.
    Integer,           // Type.
    "Number of samples at which hinge features start being used.", // Overview
    "Number of samples at which hinge features start being used (only when auto features is enabled).", // Description.
    1,      // Not zero if the parameter has lower limit.
    1,      // Parameter's lower limit.
    0,      // Not zero if the parameter has upper limit.
    0,      // Parameter's upper limit.
    "15"    // Parameter's typical (default) value.
  },
};

/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {
  
  "MAXENT",          // Id.
  "Maximum Entropy", // Name.
  "1.0",       	     // Version.

  // Overview.
  "The principle of maximum entropy is a method for analyzing available qualitative information in order to determine a unique epistemic probability distribution. It states that the least biased distribution that encodes certain given information is that which maximizes the information entropy (content retrieved from Wikipedia on the 19th of May, 2008: http://en.wikipedia.org/wiki/Maximum_entropy).",

  // Description.
  "The principle of maximum entropy is a method for analyzing available qualitative information in order to determine a unique epistemic probability distribution. It states that the least biased distribution that encodes certain given information is that which maximizes the information entropy (content retrieved from Wikipedia on the 19th of May, 2008: http://en.wikipedia.org/wiki/Maximum_entropy). This implementation in openModeller follows the same approach of Maxent (Phillips et al. 2004). It was compared with Maxent 3.3.3e through a standard experiment using all possible combinations of parameters, generating models with the same number of iterations, at least a 90% rate of matching best features considering all iterations, distribution maps with a correlation (r) greater than 0.999 and no difference in the final loss. However, previous implementations of this algorithm (before version 1.0) used to generate quite different results. The first versions were based on an existing third-party Maximum Entropy library which produced low quality models compared with all other algorithms. After that, the algorithm was re-written a couple of times by Elisangela Rodrigues as part of her Doctorate. Finally, the EUBrazil-OpenBio project funded the remaining work to make this algorithm compatible with Maxent. Please note that not all functionality available from Maxent is available here - in particular the possibility of using collecting bias and categorical maps is not present, as well as many specific parameters for advanced users. However, you should be able to get compatible results for all other available parameters.",

  "Steven J. Phillips, Miroslav Dudík, Robert E. Schapire", // Algorithm authors.

  "1) Jaynes, E.T. (1957) Information Theory and Statistical Mechanics. In Physical Review, Vol. 106, #4 (pp 620-630). 2) Berger, A. L., Pietra, S. A. D. and Pietra, V. J. D. (1996). A maximum entropy approach to natural language processing. Computational Linguistics, 22, 39-71. 3) Darroch, J.N. and Ratcliff, D. (1972) Generalized iterative scaling for log-linear models. The Annals of Mathematical Statistics, Vol. 43: pp 1470-1480. 4) Malouf, R. (2003) A comparison of algorithms for maximum entropy parameter estimation. Proceedings of the Sixth Conference on Natural Language Learning. 5) Phillips, S.J., Dudík, M. and Schapire, R.E. (2004) A maximum entropy approach to species distribution modeling. Proceedings of the Twenty-First International Conference on Machine Learning, pp 655-662.", // Bibliography.

  "Elisangela S. da C. Rodrigues, Renato De Giovanni, Daniel Bolgheroni", // Code author.

  "elisangela.rodrigues [at] poli . usp . br, renato [at] cria . org . br, daniel [at] cria . org . br", // Code author's contact.

  0, // Does not accept categorical data.
  1, // Does not need (pseudo)absence points.

  NUM_PARAM, // Algorithm's parameters.
  parameters
};

/****************************************************************/
/****************** Algorithm's factory function ****************/

OM_ALG_DLL_EXPORT
AlgorithmImpl *
algorithmFactory()
{
  return new MaximumEntropy();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}

/****************************************************************/
/************************ Maximum Entropy ***********************/

/*******************/
/*** constructor ***/

MaximumEntropy::MaximumEntropy() :
  AlgorithmImpl(&metadata),
  _done(false),
  _iteration(0),
  _parallelUpdateFreq(30),
  _change(10),
  _updateInterval(20),
  _select(5),
  _autofeatures(false),
  _pt_thr(80),
  _q_thr(10),
  _hinge_thr(15)
{ }

/******************/
/*** destructor ***/

MaximumEntropy::~MaximumEntropy()
{
  unsigned int n = _features.size();

  Log::instance()->debug("Deallocating %d features\n", n);

  for (unsigned int i = 0; i < n; ++i) {

   // Avoid deallocating post generated features - this happens in the feature generator
   if ( _features[i]->postGenerated() ) {

     _features[i] = 0;
   }
   else {

     delete _features[i];
   }
  }

  unsigned int m = _generators.size();

  Log::instance()->debug("Deallocating %d generators\n", n);

  for (unsigned int j = 0; j < m; ++j)
    delete _generators[j];
}

/**************************/
/*** need Normalization ***/
int MaximumEntropy::needNormalization()
{
  // Normalization is performed internally based on background points
  return 0;
}

/******************/
/*** initialize ***/

int
MaximumEntropy::initialize()
{
  // Check the number of presences.
  _num_presences = _samp->numPresence();

  if ( _num_presences == 0 ) {

    Log::instance()->error( MAXENT_LOG_PREFIX "No presence points inside the mask!\n" );
    return 0;
  }
  
  // Load presence points.
  _presences = _samp->getPresences();
  
  // Number of iterations
  if ( ! getParameter( ITERATIONS_ID, &_max_iterations ) ) {

    Log::instance()->warn( MAXENT_LOG_PREFIX "Parameter '" ITERATIONS_ID "' not passed. Using default value (500)\n" );
    _max_iterations = 500;
  }
  else {

    if ( _max_iterations <= 0 ) {
      
      Log::instance()->error( MAXENT_LOG_PREFIX "Parameter '" ITERATIONS_ID "' must be greater then zero.\n" );
      return 0;
    }
  }
  
  // Tolerance
  if ( ! getParameter( TOLERANCE_ID, &_tolerance ) ) {

    Log::instance()->warn( MAXENT_LOG_PREFIX "Parameter '" TOLERANCE_ID "' not passed. Using default value (1-E05)\n" );
    _tolerance = 0.00001;
  }
  else {
    if ( _tolerance <= 0.0 ) {

      Log::instance()->error( MAXENT_LOG_PREFIX "Parameter '" TOLERANCE_ID "' must be greater than zero\n" );
      return 0;
    }
  }

  // Quadratic features
  _quadratic = true;
  int quadratic;
  if ( getParameter( QUADRATIC_ID, &quadratic ) && quadratic == 0 ) {

    _quadratic = false;
  }

  // Product features
  _product = true;
  int product;
  if ( getParameter( PRODUCT_ID, &product ) && product == 0 ) {

    _product = false;
  }

  // Hinge features
  _hinge = true;
  int hinge;
  if ( getParameter( HINGE_ID, &hinge ) && hinge == 0 ) {

    _hinge = false;
  }

  // Threshold features
  _threshold = true;
  int threshold;
  if ( getParameter( THRESHOLD_ID, &threshold ) && threshold == 0 ) {

    _threshold = false;
  }

  // Auto features
  _autofeatures = true;
  int autofeatures;
  if ( getParameter( AUTOFEATURES_ID, &autofeatures ) && autofeatures == 0 ) {

    _autofeatures = false;
  }

  if ( _autofeatures ) {

    // Product & threshold threshold
    if ( ! getParameter( PT_THR_ID, &_pt_thr ) ) {

      _pt_thr = 80;
    }
    else {

      if ( _pt_thr < 1 ) {
      
        _pt_thr = 1;
      }
    }  

    // Quadratic threshold
    if ( ! getParameter( Q_THR_ID, &_q_thr ) ) {

      _q_thr = 10;
    }
    else {

      if ( _q_thr < 1 ) {
      
        _q_thr = 1;
      }
    }  

    // Hinge threshold
    if ( ! getParameter( HINGE_THR_ID, &_hinge_thr ) ) {

      _hinge_thr = 15;
    }
    else {

      if ( _hinge_thr < 1 ) {
      
        _hinge_thr = 1;
      }
    }  
  }

  bool use_absences_as_background = false;
  int use_abs;
  if ( getParameter( USE_ABSENCES_ID, &use_abs ) && use_abs == 1 ) {

    use_absences_as_background = true;
  }

  if ( use_absences_as_background ) {

    _num_background = _samp->numAbsence();

    int param_background;

    if ( _num_background && getParameter( BACKGROUND_ID, &param_background ) && param_background > _num_background ) {

    Log::instance()->warn( MAXENT_LOG_PREFIX "Number of absence points (%d) is less than the specified number of background points (%d). Using the first.\n", _num_background, param_background);
    }
  }
  else {

    if ( ! getParameter( BACKGROUND_ID, &_num_background ) ) {

      Log::instance()->warn( MAXENT_LOG_PREFIX "Parameter '" BACKGROUND_ID "' unspecified. Using default value (10000).\n");

      _num_background = 10000;
    }
  }

  if ( _num_background <= 0 ) {
	
    Log::instance()->warn( MAXENT_LOG_PREFIX "Parameter '" BACKGROUND_ID "' must be greater than zero.\n" );
    return 0;
  }

  bool merge_points = true; // default
  int merge_param;
  if ( getParameter( MERGE_POINTS_ID, &merge_param ) && !merge_param ) {

    merge_points = false;
  }
    
  _background = new OccurrencesImpl( _presences->name(), _presences->coordSystem() );

  if ( use_absences_as_background ) {

    _background->appendFrom( _samp->getAbsences() );
  }
  else {

    // Generate background points

    Log::instance()->info( MAXENT_LOG_PREFIX "Generating random background points.\n" );

    for ( int i = 0; i < _num_background; ++i ) {

      OccurrencePtr oc = _samp->getPseudoAbsence();
      _background->insert( oc ); 
    }
  }

  if ( merge_points ) {

    _num_background += _num_presences;

    // Mixing points with different abundance values (0 or 1) won't hurt
    _background->appendFrom( _presences );
  }

  // Output Format
  if ( ! getParameter( OUTPUT_ID, &_output_format ) ) {

    Log::instance()->warn( MAXENT_LOG_PREFIX "Parameter '" OUTPUT_ID "' not passed. Using default value (Logistic)\n" );
    _output_format = 2;
  }
  else {

    if ( _output_format != 1 && _output_format != 2 ) {

      Log::instance()->error( MAXENT_LOG_PREFIX "Parameter '" OUTPUT_ID "' must be 1 or 2\n" );
      return 0;
    }
  }

  initTrainer();
  
  return 1;
}

/***************/
/*** iterate ***/

int
MaximumEntropy::iterate()
{
  if ( _iteration == _max_iterations ) {

    endTrainer();
    return 1;
  }

  _old_loss = _new_loss;

  if ( _iteration && _parallelUpdateFreq && (_iteration % _parallelUpdateFreq == 0) ) {

    _new_loss = parallelProc();
  }
  else {

    _new_loss = sequentialProc();
  }

  _gain = log((double)_num_background) - _old_loss;

  if ( terminationTest(_new_loss) ) {

    endTrainer();
    return 1;
  }
  
  ++_iteration;

  return 1;
}

/********************/
/*** init Trainer ***/

void
MaximumEntropy::initTrainer()
{
  _convergence_test_frequency = 20;
  _previous_loss = std::numeric_limits<double>::infinity();

  bool deactivated_threshold_generator = false;
  bool deactivated_hinge_generator = false;

  // Instantiate features 
  // Note: this sequence of loops is not the most efficient way of doing this, 
  // but this seems to be important in terms of getting the same results as Maxent.
  for (int i = 0; i < _samp->numIndependent(); i++) {

    _features.push_back(new LinearFeature(i));
  }

  for (int i = 0; i < _samp->numIndependent(); i++) {

    if (_quadratic) {

      _features.push_back(new QuadraticFeature(i));
    }
  }

  for (int i = 0; i < _samp->numIndependent(); i++) {

    if (_product) {

      for (int j = i + 1; j < _samp->numIndependent(); j++) {

	_features.push_back(new ProductFeature(i, j));
      }
    }
  }

  for (int i = 0; i < _samp->numIndependent(); i++) {

    if (_threshold) {

      if ( (!_autofeatures) || _num_presences >= _pt_thr ) {

        _generators.push_back(new ThresholdGenerator(_presences, _background, new LinearFeature(i), false));
      }
      else {

        if ( _autofeatures ) {

          deactivated_threshold_generator = true;
        }
      }
    }
    if (_hinge) {

      if ( (!_autofeatures) || _num_presences >= _hinge_thr ) {

        _generators.push_back(new HingeGenerator(_presences, _background, new LinearFeature(i), false));
        _generators.push_back(new HingeGenerator(_presences, _background, new LinearFeature(i), true));
      }
      else {

        if ( _autofeatures ) {

          deactivated_hinge_generator = true;
        }
      }
    }
  }

  if ( deactivated_threshold_generator ) {

    Log::instance()->warn( MAXENT_LOG_PREFIX "Threshold feature generator deactivated due to low number of presence points.\n" );
  }

  if ( deactivated_hinge_generator ) {

    Log::instance()->warn( MAXENT_LOG_PREFIX "Hinge feature generator deactivated due to low number of presence points.\n" );
  }

  int i = 0;
  vector<MxFeature*>::iterator it;

  unsigned int active_features = _features.size();

  bool deactivated_product_features = false;
  bool deactivated_quadratic_features = false;

  // Deactivate features if necessary
  if ( _autofeatures ) {

    for ( it = _features.begin(); it != _features.end(); ++it ) {

      if ( (*it)->type() == F_PRODUCT && _num_presences < _pt_thr ) {

        (*it)->deactivate();
        --active_features;
        deactivated_product_features = true;
      }
      else if ( (*it)->type() == F_QUADRATIC && _num_presences < _q_thr ) {

        (*it)->deactivate();
        --active_features;
        deactivated_quadratic_features = true;
      }

      ++i;
    }
  }

  if ( deactivated_product_features ) {

    Log::instance()->warn( MAXENT_LOG_PREFIX "Product features deactivated due to low number of presence points.\n" );
  }

  if ( deactivated_quadratic_features ) {

    Log::instance()->warn( MAXENT_LOG_PREFIX "Quadratic features deactivated due to low number of presence points.\n" );
  }

  if ( active_features == 0 ) {

    Log::instance()->error( MAXENT_LOG_PREFIX "No features available. Select more feature types or deselect auto features.\n" );
  }

  unsigned int n = active_features;
  Log::instance()->debug("Using %d features\n", active_features);

  // Normalize features
  OccurrencesImpl::const_iterator b_iterator = _background->begin();
  OccurrencesImpl::const_iterator b_end = _background->end();
  Sample ref(n);
  Sample smin(n);
  Sample smax(n);
  Scalar raw_val;
  i = 0;

  while ( b_iterator != b_end ) {
    
    Sample sample = (*b_iterator)->originalEnvironment();

    // Get raw values for each feature and put them in ref
    i = 0;
    for ( it = _features.begin(); it != _features.end(); ++it ) {

      if ( !(*it)->isActive() ) {

        continue;
      }

      if ( (*it)->isNormalizable() ) {

        raw_val = (*it)->getRawVal(sample);

        ref[i] = raw_val;
      }

      ++i;
    }

    if ( b_iterator == _background->begin() ) {

      // First iteration: use ref for smin and smax
      smin = ref;
      smax = ref;
    }
    else {

      // Subsequent iterations: determine min and max 
      smin &= ref;
      smax |= ref;
    }

    ++b_iterator;
  }

  // Set final min and max values
  i = 0;
  for ( it = _features.begin(); it != _features.end(); ++it ) {

    if ( !(*it)->isActive() ) {

      continue;
    }

    if ( (*it)->isNormalizable() ) {

      (*it)->setMinMax( smin[i], smax[i] );
    }

    ++i;
  }

  setLinearPred();

  assignBetas();

  // calculate observed feature expectations - pi~[f] (empirical average of f)
  OccurrencesImpl::const_iterator p_iterator = _presences->begin();
  OccurrencesImpl::const_iterator p_end = _presences->end();

  // sum feature values to calculate mean and std
  Scalar val;
  while ( p_iterator != p_end ) {

    Sample sample = (*p_iterator)->originalEnvironment();

    for ( it = _features.begin(); it != _features.end(); ++it ) {

      val = (*it)->getVal(sample);
      (*it)->setMean( (*it)->mean() + val );
      (*it)->setStd( (*it)->std() + pow(val, 2) );
    }
    ++p_iterator;
  }

  // calculate mean, std and expected values for each feature
  double margin;
  Scalar minval = 0.0;
  Scalar maxval = 1.0;
  Scalar std_samp_dev;
  for ( it = _features.begin(); it != _features.end(); ++it ) {

    if ( _num_presences == 1 ) {

      (*it)->setStd( 0.5*(maxval-minval) );
    }
    else {

      (*it)->setMean( (*it)->mean() / (double)_num_presences );

      if ( (*it)->std() < (double)_num_presences * pow((*it)->mean(), 2) ) {

        (*it)->setStd( minval );
      }
      else {

        (*it)->setStd( sqrt( ((*it)->std() - (double)_num_presences * pow((*it)->mean(), 2)) / (double)(_num_presences - 1) ) );
      }

      if ( (*it)->std() > 0.5*(maxval-minval) ) {

        (*it)->setStd( 0.5*(maxval-minval) );
      }
    }

    margin = ((*it)->beta() / sqrt((double)_num_presences)) * (*it)->std();

    (*it)->setLower( (*it)->mean() - margin );
    (*it)->setUpper( (*it)->mean() + margin );
 
    if ( (*it)->lower() < minval ) {

      (*it)->setLower( minval );
    }
    if ( (*it)->upper() > maxval ) {

      (*it)->setUpper( maxval );
    }

    (*it)->setSampExp( 0.5*((*it)->upper()+(*it)->lower()) );

    std_samp_dev = 0.5*((*it)->upper()-(*it)->lower());

    if ( std_samp_dev < MINDEV ) {

      if ( (*it)->isBinary() && (*it)->sampExp() == 1.0 ) {

        (*it)->setSampDev( 1.0 / (2.0 * (double)_num_presences) );
      }
      else {

        (*it)->setSampDev( MINDEV );
      }
    }
    else {

      (*it)->setSampDev( std_samp_dev );
    }
  }

  // Also set generators expectations
  vector<FeatureGenerator*>::iterator git;
  for ( git = _generators.begin(); git != _generators.end(); ++git ) {

    (*git)->setSampExp( MINDEV );
  }

  _density = new double[_num_background];

  calcDensity();

  updateReg();

  _new_loss = getLoss();

  Log::instance()->debug( "Initial loss = %.16f \n", _new_loss );

  _gain = 0.0;
}

/*******************/
/*** end Trainer ***/

void
MaximumEntropy::endTrainer()
{
  // Calculate entropy
  _entropy = 0.0;

  for ( int j = 0; j < _num_background; ++j ) {
    
    double dd = _density[j] /= _z_lambda;

    if ( dd > 0.0 ) {

      _entropy += -dd * log(dd);
    }
  }

  Log::instance()->info( MAXENT_LOG_PREFIX "Gain\t %.16f \n", _gain );
  Log::instance()->info( MAXENT_LOG_PREFIX "Entropy\t %.2f \n", _entropy );

  delete[] _density;
  delete[] _linear_pred;

  _done = true;
}

/***********************/
/*** sequential Proc ***/

double
MaximumEntropy::sequentialProc()
{
  double retvalue;

  // Determine best feature
  MxFeature* best_f = 0;
  double best_dlb = 1.0;
  double alpha = 0.0;

  vector<MxFeature*>::iterator it;
  for ( it = _features.begin(); it != _features.end(); ++it ) {

    double dlb = lossBound( (*it)->isActive(), (*it)->exp(), (*it)->sampExp(), (*it)->sampDev(), (*it)->lambda(), (*it)->getDescription(_samp->getEnvironment()) );

    if (!(*it)->isActive() || (*it)->postGenerated() || dlb >= best_dlb) {
      continue;
    }

    best_f = *it;
    best_dlb = dlb;
  }    

  int best_thr = -1;
  FeatureGenerator* best_gen = 0;

  // Also check generator features
  vector<FeatureGenerator*>::iterator git;

  for ( git = _generators.begin(); git != _generators.end(); ++git ) {

    for ( int j = (*git)->getFirstRef(); j < (*git)->getLastRef(); j++ ) {

      double dlb = lossBound( true, (*git)->exp(j), (*git)->sampExp(j), (*git)->sampDev(j), (*git)->lambda(j), "G" );

      if ( dlb < best_dlb ) {

        best_gen = *git;
        best_dlb = dlb;
        best_thr = j;
      }
    }
  }    

  if ( best_gen != 0 ) {

    best_f = best_gen->getFeature(best_thr);

    if ( best_f == 0 ) {

      best_f = best_gen->exportFeature(best_thr);

      _features.push_back( best_f );
    }
  }

  if ( best_f == 0 ) {

    Log::instance()->error(MAXENT_LOG_PREFIX "Could not determine best feature!\n");
    return 0.0;
  }

  // Update expectation if necessary
  if ( best_f->lastExpChange() != _iteration - 1 ) {

    OccurrencesImpl::const_iterator p_iterator = _background->begin();
    OccurrencesImpl::const_iterator p_end = _background->end();
    int i = 0;
    double sum = 0.0;

    while ( p_iterator != p_end ) {
    
      Sample sample = (*p_iterator)->originalEnvironment();

      sum += _density[i] * best_f->getVal(sample);

      ++i;
      ++p_iterator;
    }

    best_f->setExp( sum / _z_lambda );
    best_f->setLastExpChange( _iteration );
  }

  best_f->setLastChange( _iteration );

  vector<MxFeature*> to_update = featuresToUpdate();
  best_dlb = lossBound( best_f->isActive(), best_f->exp(), best_f->sampExp(), best_f->sampDev(), best_f->lambda(), best_f->getDescription(_samp->getEnvironment()) );

  double loss;
  double delta_loss;

  if ( best_f->isBinary() ) {

    alpha = getAlpha( best_f->exp(), best_f->sampExp(), best_f->sampDev(), best_f->lambda(), best_f->getDescription(_samp->getEnvironment()) );
    alpha = decreaseAlpha( alpha );
    updateReg( best_f, alpha );
    loss = increaseLambda( best_f, alpha, to_update );
  }
  else {

    alpha = runNewtonStep( best_f );
    alpha = decreaseAlpha( alpha );
    updateReg( best_f, alpha );
    loss = increaseLambda( best_f, alpha, to_update );

    delta_loss = loss - _old_loss;

    if ( delta_loss > best_dlb) {

      Log::instance()->debug("Undoing: newLoss %.16f, oldLoss %.16f, deltaLossBound %.16f\n", loss, _old_loss, best_dlb);
      updateReg( best_f, -alpha );
      vector<MxFeature*> f_update;
      f_update.push_back( best_f );
      loss = increaseLambda( best_f, -alpha, f_update );
      alpha = searchAlpha( best_f, getAlpha( best_f->exp(), best_f->sampExp(), best_f->sampDev(), best_f->lambda(), best_f->getDescription(_samp->getEnvironment()) ) );
      alpha = decreaseAlpha( alpha );
      updateReg( best_f, alpha );
      loss = increaseLambda( best_f, alpha, to_update );
    }
  }

  delta_loss = loss - _old_loss;

  displayInfo(best_f, best_dlb, loss, delta_loss, alpha);

  retvalue = loss;

  return retvalue;
}

/*****************/
/*** lossBound ***/

double 
MaximumEntropy::lossBound( bool active, double w1, double n1, double beta1, double lambda, std::string description )
{
  if ( !active ) {

     return 0.0;
  }

  // Calculate delta loss bound
  double dlb = 0;
  double w0 = 1.0 - w1;
  // Determine alpha
  double alpha = getAlpha( w1, n1, beta1, lambda, description );
  double infinity = std::numeric_limits<double>::infinity();

  if ( n1 != -1.0 ) {

    if ( alpha < infinity ) {

      dlb = -n1 * alpha + log( w0 + w1 * exp(alpha) ) +	beta1 * ( fabs(lambda + alpha) - fabs(lambda) );

#ifdef MSVC
      bool dlb_isnan = _isnan(dlb);
#else
      bool dlb_isnan = isnan(dlb);
#endif
      if ( dlb_isnan ) {

        dlb = 0.0;
      }
    }
  }

  return dlb;
}

/*********************/
/*** parallel Proc ***/

double
MaximumEntropy::parallelProc()
{
  double retvalue;

  // Calculate alphas for each feature
  double* alpha = new double[_features.size()];
  double* sum = new double[_features.size()];
  double lambda;

  for ( unsigned int i = 0; i < _features.size(); ++i ) {

    lambda = _features[i]->lambda();

    if ( lambda == 0.0 || _features[i]->isBinary() ) {

      alpha[i] = 0.0;
    }
    else {

      alpha[i] = lambda - _features[i]->prevLambda();
    }

    _features[i]->setPrevLambda( lambda );

    sum[i] = 0.0;
  }

  // Newton step

  double th = 0.0;
  double ty = 0.0;
  double ft;
  double d;
  double v;

  OccurrencesImpl::const_iterator p_iterator = _background->begin();
  OccurrencesImpl::const_iterator p_end = _background->end();
  int i = 0;

  while ( p_iterator != p_end ) {
    
    Sample sample = (*p_iterator)->originalEnvironment();

    ft = 0.0;
    d = _density[i];

    for ( unsigned int j = 0; j < _features.size(); ++j ) {

      if ( alpha[j] != 0.0 ) {

        v = _features[j]->getVal(sample);

        ft += alpha[j] * v;
        sum[j] += d * v;
      }
    }

    th += d * pow(ft, 2);
    ty += d * ft;

    ++i;
    ++p_iterator;
  }

  int cnt = 0;

  for ( unsigned int i = 0; i < _features.size(); ++i ) {

    if ( alpha[i] != 0.0 ) {

      _features[i]->setLastExpChange( _iteration );
      _features[i]->setExp( sum[i] / _z_lambda );
      ++cnt;
    }
  }

  Log::instance()->debug("Updated %u features\n", cnt);

  ty /= _z_lambda;
  th = (th/_z_lambda) - pow(ty, 2);

  double step = 0.0;

  if ( th >= 1.0e-12 ) {

    for ( unsigned int i = 0; i < _features.size(); ++i ) {

      step += getDeriv( _features[i] ) * alpha[i];
    }

    step = -step / th;

    // Reduce newton step if necessary
    for ( unsigned int i = 0; i < _features.size(); ++i ) {

      lambda = _features[i]->lambda();

      if ( (step * alpha[i] + lambda) * lambda < 0.0 ) {

        step = -lambda / alpha[i];
      }
    }
  }

  // Revise alpha
  for ( unsigned int i = 0; i < _features.size(); ++i ) {

    lambda = _features[i]->lambda();

    alpha[i] *= step;

    if ( ( alpha[i] != -lambda ) && fabs( alpha[i] + lambda ) < MINLIMIT ) {

      alpha[i] = -lambda;
    }
  }

  double prev_loss = getLoss();

  // Determine features to update
  vector<MxFeature*> to_update = featuresToUpdate();

  double loss = increaseLambda( alpha, to_update );

  if ( loss > prev_loss ) {

    for ( unsigned int i = 0; i < _features.size(); ++i ) {

      alpha[i] = -alpha[i];
    }

    loss = increaseLambda( alpha, to_update );
  }

  delete[] alpha;
  delete[] sum;

  retvalue = loss;

  return loss;
}

/***********************/
/*** set Linear Pred ***/

void
MaximumEntropy::setLinearPred()
{
  _linear_pred = new double[_num_background];

  for ( int i = 0; i < _num_background; ++i ) {

    _linear_pred[i] = 0.0;
  }
  
  OccurrencesImpl::const_iterator p_iterator = _background->begin();
  OccurrencesImpl::const_iterator p_end = _background->end();
  int i = 0;

  while ( p_iterator != p_end ) {
    
    Sample sample = (*p_iterator)->originalEnvironment();

    vector<MxFeature*>::iterator it;
    for ( it = _features.begin(); it != _features.end(); ++it ) {

      if ( (*it)->lambda() != 0.0 ) {

        _linear_pred[i] += (*it)->lambda() * (*it)->getVal(sample);
      }
    }
    ++i;
    ++p_iterator;
  }

  setLinearNormalizer();
}


/***************************/
/****** assign Betas *******/

void
MaximumEntropy::assignBetas()
{
  // Default beta for each type of feature

  double beta_hinge = 0.5;

  vector<int> ts;
  vector<double> betas;

  if ( _product && ((!_autofeatures) || _num_presences >= _pt_thr) ) {

    int ts_l[] = { 0, 10, 17, 30, 100 };
    ts.assign( ts_l, ts_l + 5 );

    double betas_l[] = { 2.6, 1.6, 0.9, 0.55, 0.05 };
    betas.assign( betas_l, betas_l + 5 );
  }
  else {

    if ( _quadratic && ((!_autofeatures) || _num_presences >= _q_thr) ) {

      int ts_l[] = { 0, 10, 17, 30, 100 };
      ts.assign( ts_l, ts_l + 5 );

      double betas_l[] = { 1.3, 0.8, 0.5, 0.25, 0.05 };
      betas.assign( betas_l, betas_l + 5 );
    }
    else {

      int ts_l[] = { 10, 30, 100 };
      ts.assign( ts_l, ts_l + 3 );

      double betas_l[] = { 1.0, 0.2, 0.05 };
      betas.assign( betas_l, betas_l + 3 );
    }
  }

  double beta_common = interpol( ts, betas, _num_presences );

  double beta_threshold = 0.0;

  if ( _threshold ) {

    int ts_l[] = { 0, 100 };
    ts.assign( ts_l, ts_l + 2 );

    double betas_l[] = { 2.0, 1.0 };
    betas.assign( betas_l, betas_l + 2 );

    beta_threshold = interpol( ts, betas, _num_presences );
  }

  // Assign beta values for each feature
  vector<MxFeature*>::iterator it;
  for ( it = _features.begin(); it != _features.end(); ++it ) {

    (*it)->setBeta( beta_common );
  }

  vector<FeatureGenerator*>::iterator itg;
  for ( itg = _generators.begin(); itg != _generators.end(); ++itg ) {

    if ( (*itg)->type() == G_THRESHOLD ) {

      (*itg)->setBeta( beta_threshold );
    }
    else if ( (*itg)->type() == G_HINGE ) {

      (*itg)->setBeta( beta_hinge );
    }
  }
}

/****************/
/*** interpol ***/

double
MaximumEntropy::interpol( vector<int> ts, vector<double> betas, int num )
{
  int size = (int)ts.size();
  int i = 0;

  // Get reference index
  for ( ; i < size && num > ts[i]; ++i) {}

  if ( i == 0 ) {

    // num is less than or equals the lowest threshold
    return betas[0];
  }

  if ( i == size ) {

    // num is greater than the highest threshold
    return betas[size-1];
  }

  // num is less than or equals the i threshold
  return betas[i-1] + ((betas[i] - betas[i-1]) * (double)(num - ts[i-1])) / (double)(ts[i] - ts[i-1]);
}

/*****************************/
/*** set Linear Normalizer ***/

void
MaximumEntropy::setLinearNormalizer()
{
  _linear_normalizer = _linear_pred[0];

  for ( int i = 1; i < _num_background; ++i ) {

    if ( _linear_pred[i] > _linear_normalizer ) {

      _linear_normalizer = _linear_pred[i]; 
    }
  }
}

/********************/
/*** calc Density ***/

void
MaximumEntropy::calcDensity()
{
  vector<MxFeature*> to_update;

  vector<MxFeature*>::iterator it;
  for ( it = _features.begin(); it != _features.end(); ++it ) {

    if ( (*it)->isActive() && !(*it)->postGenerated() ) {

      to_update.push_back( (*it) );
    }
  }

  calcDensity( to_update );
}

/********************/
/*** calc Density ***/

void
MaximumEntropy::calcDensity( vector<MxFeature*> to_update )
{
  _z_lambda = 0.0;
  double d;
  double* f_sum = new double[to_update.size()];
  for ( unsigned int j = 0; j < to_update.size(); ++j ) {

    f_sum[j] = 0.0;
  }
  
  OccurrencesImpl::const_iterator p_iterator = _background->begin();
  OccurrencesImpl::const_iterator p_end = _background->end();
  int i = 0;

  while ( p_iterator != p_end ) {
    
    d = exp( _linear_pred[i] - _linear_normalizer );

    _density[i] = d;

    _z_lambda += d;

    Sample sample = (*p_iterator)->originalEnvironment();

    for ( unsigned int j = 0; j < to_update.size(); ++j ) {

      f_sum[j] += d * to_update[j]->getVal(sample);
    }

    ++i;
    ++p_iterator;
  }

  for ( unsigned int j = 0; j < to_update.size(); ++j ) {

    to_update[j]->setLastExpChange( _iteration );
    to_update[j]->setExp( f_sum[j] / _z_lambda );
  }

  // Update feature expectations for all generators
  vector<FeatureGenerator*>::iterator git;
  for ( git = _generators.begin(); git != _generators.end(); ++git ) {

    (*git)->updateExp( _density, _z_lambda );
  }    

  delete[] f_sum;
}

/*****************/
/*** get Alpha ***/

double
MaximumEntropy::getAlpha( double w1, double n1, double beta1, double lambda, std::string description )
{
  double alpha = 0.0;
  double w0 = 1.0 - w1;
  double n0 = 1.0 - n1;

  if ( ( w0 >= MINLIMIT ) && ( w1 >= MINLIMIT ) ) {

    if ( n1 - beta1 > MINLIMIT ) {

      alpha = log( (n1 - beta1) * w0 / ((n0 + beta1) * w1) );

      if ( alpha + lambda <= 0.0 ) {

	if ( n0 - beta1 > MINLIMIT ) {

	  alpha = log( (n1 + beta1) * w0 / ( (n0 - beta1) * w1) );

          if ( alpha + lambda >= 0.0 ) {

            alpha = -lambda;
          }
        }
        else {

          alpha = -lambda;
        }
      }
    }
    else {

      if ( n0 - beta1 > MINLIMIT ) {

        alpha = log( (n1 + beta1) * w0 / ( (n0 - beta1) * w1) );
	
        if ( alpha + lambda >= 0.0 ) {

          alpha = -lambda;
        }
      }
      else {

        alpha = -lambda;
      }
    }
  }

  return alpha;
}

/********************/
/*** search Alpha ***/

double 
MaximumEntropy::searchAlpha( MxFeature * f, double alpha )
{
  double ini_loss = lossChange( f, alpha );
  double current_loss = ini_loss;
  double tentative_loss;

  while ( true ) {

    tentative_loss = lossChange( f, alpha * 4.0 );

    if ( !finite(tentative_loss) || tentative_loss >= current_loss ) {

      break;
    }

    current_loss = tentative_loss;
    alpha *= 4.0;
  }

  tentative_loss = lossChange( f, alpha * 2.0 );

  if ( finite(tentative_loss) && tentative_loss < current_loss ) {

    current_loss = tentative_loss;
    alpha *= 2.0;
  }

  return alpha;
}

/*******************/
/*** loss Change ***/

double 
MaximumEntropy::lossChange( MxFeature * f, double alpha )
{
  double lambda = f->lambda();
  double n1 = f->sampExp();
  double beta1 = f->sampDev();
  double change;

  OccurrencesImpl::const_iterator p_iterator = _background->begin();
  OccurrencesImpl::const_iterator p_end = _background->end();
  int i = 0;
  double zz = 0.0;

  while ( p_iterator != p_end ) {

    Sample sample = (*p_iterator)->originalEnvironment();

    zz += _density[i] * exp(alpha * f->getVal( sample ));

    ++i;
    ++p_iterator;
  }

  change = -alpha * n1 + log(zz) + (fabs(lambda + alpha) - fabs(lambda)) * beta1;
 
  return change;
}

/***********************/
/*** run Newton Step ***/

double 
MaximumEntropy::runNewtonStep( MxFeature * f ) {

  double w1 = f->exp();
  double wu = 0.0;

  OccurrencesImpl::const_iterator p_iterator = _background->begin();
  OccurrencesImpl::const_iterator p_end = _background->end();
  int i = 0;

  while ( p_iterator != p_end ) {

    Sample sample = (*p_iterator)->originalEnvironment();

    wu += _density[i] * pow( f->getVal(sample), 2 );

    ++i;
    ++p_iterator;
  }

  wu = wu / _z_lambda - pow(w1, 2);

  if ( wu < 1.0e-12 ) {

    return 0.0;
  }

  double step = -getDeriv(f) / wu;
  double lambda = f->lambda();

  if ( lambda * (step + lambda) < 0.0 ) {

    Log::instance()->debug("Reducing newton stepsize from %.16f to %.16f\n", step, -lambda);
    step = -lambda;
  }

  return step;
}

/*****************/
/*** get Deriv ***/

double
MaximumEntropy::getDeriv( MxFeature * f )
{
  double w1 = f->exp();
  double n1 = f->sampExp();
  double beta1 = f->sampDev();
  double lambda = f->lambda();
  double deriv = w1 - n1;

  Log::instance()->debug("f: %s w1=%.16f n1=%.16f beta1=%.16f lambda=%.16f\n", f->getDescription(_samp->getEnvironment()).c_str(), w1, n1, beta1, lambda);

  if ( lambda < 0.0 ) {

    return deriv - beta1;
  }

  if ( lambda > 0.0 ) {

    return deriv + beta1;
  }

  if ( deriv + beta1 > 0.0 ) {

    return deriv + beta1;
  }

  if ( deriv - beta1 < 0.0 ) {

    return deriv - beta1;
  }

  return 0.0;
}

/******************/
/*** update Reg ***/

void
MaximumEntropy::updateReg()
{
  double sum = 0.0;

  vector<MxFeature*>::iterator it;
  for ( it = _features.begin(); it != _features.end(); ++it ) {

    sum += fabs( (*it)->lambda() ) * (*it)->sampDev();
  }

  _reg = sum;
}

/******************/
/*** update Reg ***/

void
MaximumEntropy::updateReg( MxFeature * f, double alpha )
{
  double beta1 = f->sampDev();
  double lambda = f->lambda();

  _reg += ( fabs(lambda + alpha) - fabs(lambda) ) * beta1;
}

/***********************/
/*** increase Lambda ***/

double 
MaximumEntropy::increaseLambda( MxFeature * f, double alpha, vector<MxFeature*> to_update )
{
  if ( alpha != 0.0 ) {

    double lambda = f->lambda();

    f->setLambda( lambda + alpha );

    OccurrencesImpl::const_iterator p_iterator = _background->begin();
    OccurrencesImpl::const_iterator p_end = _background->end();
    int i = 0;

    while ( p_iterator != p_end ) {

      Sample sample = (*p_iterator)->originalEnvironment();

      _linear_pred[i] += alpha * f->getVal( sample );

      if ( i == 0 || _linear_pred[i] > _linear_normalizer ) {

        _linear_normalizer = _linear_pred[i];
      }

      ++i;
      ++p_iterator;
    }

    calcDensity( to_update );
  }

  return getLoss();
}

/***********************/
/*** increase Lambda ***/

double
MaximumEntropy::increaseLambda( double* alpha, vector<MxFeature*> to_update )
{
  double lambda;

  for ( unsigned int i = 0; i < _features.size(); ++i ) {

    lambda = _features[i]->lambda();

    if ( alpha[i] != 0.0 ) {

      _features[i]->setLambda( lambda + alpha[i] );

      OccurrencesImpl::const_iterator p_iterator = _background->begin();
      OccurrencesImpl::const_iterator p_end = _background->end();
      int j = 0;

      while ( p_iterator != p_end ) {

        Sample sample = (*p_iterator)->originalEnvironment();

        _linear_pred[j] += alpha[i] * _features[i]->getVal( sample );

        ++j;
        ++p_iterator;
      }
    }
  }

  setLinearNormalizer();
  calcDensity( to_update );
  updateReg();

  return getLoss();
}

/********************/
/*** display Info ***/

void
MaximumEntropy::displayInfo( MxFeature * f, double loss_bound, double new_loss, double delta_loss, double alpha )
{
  Log::instance()->debug( "-------------------------------\n" );

  Log::instance()->debug( "%d: loss = %f \n", _iteration, new_loss );

  Log::instance()->debug( "%s: lambda = %f\n", f->getDescription(_samp->getEnvironment()).c_str(), f->lambda() );

  Log::instance()->debug( "alpha = %f lossBound = %f W1 = %f N1 = %f deltaLoss = %f \n", alpha, loss_bound, f->exp(), f->sampExp(), delta_loss );

}

/************************/
/*** termination Test ***/

bool
MaximumEntropy::terminationTest(double new_loss)
{
  if ( _iteration == 0 ) {

    _previous_loss = new_loss;
    return false;
  }

  if ( _iteration % _convergence_test_frequency != 0 ) {

    return false;
  }

  if ( _previous_loss - new_loss < _tolerance ) {

    Log::instance()->info( MAXENT_LOG_PREFIX "Algorithm converged after %d iteration(s) \n", _iteration );
    return true;
  }

  _previous_loss = new_loss;

  return false;
}

/**********************/
/*** decrease Alpha ***/

double
MaximumEntropy::decreaseAlpha(double alpha)
{
  if ( _iteration < 10 ) {

    return alpha / 50.0;
  }
  if ( _iteration < 20 ) {

    return alpha / 10.0;
  }
  if ( _iteration < 50 ) {

    return alpha / 3.0;
  }

  return alpha;
}

/****************/
/*** get Loss ***/

double
MaximumEntropy::getLoss()
{
  double sum_mid_lambda = 0.0;

  vector<MxFeature*>::iterator it;
  for ( it = _features.begin(); it != _features.end(); ++it ) {

    sum_mid_lambda += (*it)->sampExp() * (*it)->lambda();
  }

  return log(_z_lambda) - sum_mid_lambda + _linear_normalizer  + _reg;
}

/**************************/
/*** features To Update ***/

vector<MxFeature*> 
MaximumEntropy::featuresToUpdate()
{
  int j = 0;
  vector< pair<int, double> > lb;
  vector<MxFeature*> to_update;
  vector<int> to_update_idx;
  
  vector<MxFeature*>::iterator it;
  for ( it = _features.begin(); it != _features.end(); ++it,++j ) {

    if ( (*it)->isActive() && !((*it)->postGenerated()) && ((_iteration < (*it)->lastChange() + _change) || (j % _updateInterval == _iteration % _updateInterval)) ) {

      to_update.push_back(*it);
      to_update_idx.push_back(j);
    }

    lb.push_back( make_pair(j, lossBound( (*it)->isActive(), (*it)->exp(), (*it)->sampExp(), (*it)->sampDev(), (*it)->lambda(), (*it)->getDescription(_samp->getEnvironment())) ) );
  }

  sort( lb.begin(), lb.end(), by_value() );

  j = 0;
  vector<int>::iterator idxit;
  vector< pair<int, double> >::iterator lbit;
  for ( lbit = lb.begin(); lbit != lb.end() && (j < _select); ++lbit,++j ) {

    MxFeature * f = _features[(*lbit).first];

    idxit = find ( to_update_idx.begin(), to_update_idx.end(), lbit->first );

    bool contains = ( idxit == to_update_idx.end() ) ? false : true;

    if ( f->isActive() && (!f->postGenerated()) && !contains ) {

      to_update.push_back(f);
      to_update_idx.push_back(lbit->first);
    }
  }

  Log::instance()->debug("Updating %u features\n", to_update.size());

  return to_update;
}

/********************/
/*** get Progress ***/

float 
MaximumEntropy::getProgress() const
{
  if ( done() ) {

    return 1.0;
  }

  return (float)_iteration / (float)_max_iterations;
}

/************/
/*** done ***/

int
MaximumEntropy::done() const
{
  return _done;
}

/*****************/
/*** get Value ***/

Scalar
MaximumEntropy::getValue( const Sample& x ) const
{
  double prob = 0.0 ;
  double val;

  vector<MxFeature*>::const_iterator it;
  for ( it = _features.begin(); it != _features.end(); ++it ) {

    // TODO: Clamp environmental values if necessary?
    val = (*it)->getVal( x );
 
    // Force result between 0 and 1
    val = ( val < 0.0 ) ? 0.0 : val; 
    val = ( val > 1.0 ) ? 1.0 : val;

    prob += (*it)->lambda() * val;
  }

  prob = exp(prob - _linear_normalizer)/_z_lambda;

  if ( !finite(prob) ) {

    // Beware that on windows max is also defined as a macro
    // in <windows.h> so it should be enclosed in () in the
    // next line to cause the macro not to be expanded 
    prob = (std::numeric_limits<double>::max)(); // DBL_MAX;
  }
 
  if ( _output_format == 1 ) {

    return prob;
  }
  else {

    return ( ( exp(_entropy) * prob ) / ( 1 + ( exp(_entropy) * prob ) ) );
  }
}

/***********************/
/*** get Convergence ***/
int
MaximumEntropy::getConvergence( Scalar * const val )
{
  *val = 1.0;
  return 1;
}

/****************************************************************/
/****************** configuration *******************************/

void
MaximumEntropy::_getConfiguration( ConfigurationPtr& config ) const
{
  //avoid serialization when something went wrong
  if ( ! _done ) {

    return;
  }

  //These two lines create a new XML element called "MaximumEntropy"
  ConfigurationPtr model_config( new ConfigurationImpl( "MaximumEntropy" ) );

  config->addSubsection( model_config );

  model_config->addNameValue( "Z", _z_lambda );
  model_config->addNameValue( "LN", _linear_normalizer );
  model_config->addNameValue( "Entropy", _entropy);
  model_config->addNameValue( "OutputFormat", _output_format );

  ConfigurationPtr features_config( new ConfigurationImpl( "Features" ) );

  int num_active_features = 0;

  for ( unsigned int i = 0; i < _features.size(); ++i ) {

    if ( _features[i]->isActive() ) {

      features_config->addSubsection( _features[i]->getConfiguration() );
      ++num_active_features;
    }
  }

  features_config->addNameValue( "Num", num_active_features );

  model_config->addSubsection( features_config );
}

void
MaximumEntropy::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "MaximumEntropy", false );

  if ( ! model_config ) {
    return;
  }
  
  _z_lambda = model_config->getAttributeAsDouble( "Z", 0.0 );

  _linear_normalizer = model_config->getAttributeAsDouble( "LN", 0.0 );

  _entropy = model_config->getAttributeAsDouble( "Entropy", 0.0 );

  _output_format = model_config->getAttributeAsInt( "OutputFormat", 2 );
  
  // Features
  ConstConfigurationPtr features_config = model_config->getSubsection( "Features", false );

  if ( ! features_config ) {
    return;
  }

  int num_features = features_config->getAttributeAsInt( "Num", 0 );

  _features.reserve( num_features );

  Configuration::subsection_list features = features_config->getAllSubsections();

  Configuration::subsection_list::iterator feature = features.begin();
  Configuration::subsection_list::iterator last_feature = features.end();

  for ( ; feature != last_feature; ++feature ) {

    if ( (*feature)->getName() != "Feature" ) {

      continue;
    }

    int feature_type = (*feature)->getAttributeAsInt( "Type", 0 );

    ConstConfigurationPtr feature_config = *feature;

    if ( feature_type == F_LINEAR ) {

      _features.push_back( new LinearFeature( feature_config ) );
    }
    else if ( feature_type == F_QUADRATIC ) {

      _features.push_back( new QuadraticFeature( feature_config ) );
    }
    else if ( feature_type == F_PRODUCT ) {

      _features.push_back( new ProductFeature( feature_config ) );
    }
    else if ( feature_type == F_HINGE ) {

      _features.push_back( new HingeFeature( feature_config ) );
    }
    else if ( feature_type == F_THRESHOLD ) {

      _features.push_back( new ThresholdFeature( feature_config ) );
    }
    else {

      Log::instance()->error( MAXENT_LOG_PREFIX "Unknown feature type\n" );
      return;
    }
  }

  _done = true;
}
