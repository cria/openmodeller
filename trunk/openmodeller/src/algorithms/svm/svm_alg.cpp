/**
 * Definition of SVM algorithm class.
 * 
 * @author Renato De Giovanni (renato [at] cria org br)
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

#include "svm_alg.hh"
#include "svm.h"
#include <openmodeller/MeanVarianceNormalizer.hh>
#include <openmodeller/Sampler.hh>
#include <openmodeller/Exceptions.hh>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//debug
#include <iostream>

using namespace std;

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 9

#define SVMTYPE_ID    "SvmType"
#define KERNELTYPE_ID "KernelType"
#define DEGREE_ID     "Degree"
#define GAMMA_ID      "Gamma"
#define COEF0_ID      "Coef0"
#define C_ID          "C"
#define NU_ID         "Nu"
#define PROB_ID       "ProbabilisticOutput"
#define PSEUDO_ID     "NumberOfPseudoAbsences"

#define SVM_LOG_PREFIX "SvmAlgorithm: "

/******************************/
/*** Algorithm's parameters ***/

static AlgParamMetadata parameters[NUM_PARAM] = {

  // SVM type
  {
    SVMTYPE_ID,      // Id.
    "SVM type",      // Name.
    Integer,         // Type.
    "Type of SVM: 0 = C-SVC, 1 = Nu-SVC, 2 = one-class SVM",   // Overview
    "Type of SVM: 0 = C-SVC, 1 = Nu-SVC, 2 = one-class SVM", // Description.
    1,         // Not zero if the parameter has lower limit.
    0,         // Parameter's lower limit.
    1,         // Not zero if the parameter has upper limit.
    2,         // Parameter's upper limit.
    "0"        // Parameter's typical (default) value.
  },
  // Kernel type
  {
    KERNELTYPE_ID,     // Id.
    "Kernel type",     // Name.
    Integer,           // Type.
    "Type of kernel function: 0 = linear: u'*v , 1 = polynomial: (gamma*u'*v + coef0)^degree , 2 = radial basis function: exp(-gamma*|u-v|^2)", // Overview
    "Type of kernel function: 0 = linear: u'*v , 1 = polynomial: (gamma*u'*v + coef0)^degree , 2 = radial basis function: exp(-gamma*|u-v|^2)", // Description.
    1,         // Not zero if the parameter has lower limit.
    0,         // Parameter's lower limit.
    1,         // Not zero if the parameter has upper limit.
    4,         // Parameter's upper limit.
    "2"        // Parameter's typical (default) value.
  },
  // Degree
  {
    DEGREE_ID,         // Id.
    "Degree",          // Name.
    Integer,           // Type.
    "Degree in kernel function (only for polynomial kernels).", // Overview
    "Degree in kernel function (only for polynomial kernels).", // Description.
    1,         // Not zero if the parameter has lower limit.
    0,         // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    0,         // Parameter's upper limit.
    "3"        // Parameter's typical (default) value.
  },
  // Gamma
  {
    GAMMA_ID,         // Id.
    "Gamma",          // Name.
    Real,             // Type.
    "Gamma in kernel function (only for polynomial and radial basis kernels). When set to zero, the default value will actually be 1/k, where k is the number of layers.", // Overview
    "Gamma in kernel function (only for polynomial and radial basis kernels). When set to zero, the default value will actually be 1/k, where k is the number of layers.", // Description.
    0,         // Not zero if the parameter has lower limit.
    0,         // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    0,         // Parameter's upper limit.
    "0"        // Parameter's typical (default) value.
  },
  // Coef0
  {
    COEF0_ID,         // Id.
    "Coef0",          // Name.
    Real,             // Type.
    "Coef0 in kernel function (only for polynomial kernels).", // Overview
    "Coef0 in kernel function (only for polynomial kernels).", // Description.
    0,         // Not zero if the parameter has lower limit.
    0,         // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    0,         // Parameter's upper limit.
    "0"        // Parameter's typical (default) value.
  },
  // C
  {
    C_ID,         // Id.
    "Cost",       // Name.
    Real,         // Type.
    "Cost (only for C-SVC types).", // Overview
    "Cost (only for C-SVC types).", // Description.
    1,         // Not zero if the parameter has lower limit.
    0.001,     // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    0,         // Parameter's upper limit.
    "1"        // Parameter's typical (default) value.
  },
  // Nu
  {
    NU_ID,         // Id.
    "Nu",          // Name.
    Real,          // Type.
    "Nu (only for Nu-SVC and one-class SVM).", // Overview
    "Nu (only for Nu-SVC and one-class SVM).", // Description.
    1,         // Not zero if the parameter has lower limit.
    0.001,     // Parameter's lower limit.
    1,         // Not zero if the parameter has upper limit.
    1,         // Parameter's upper limit.
    "0.5"      // Parameter's typical (default) value.
  },
  // Probabilistic output
  {
    PROB_ID,                 // Id.
    "Probabilistic output",  // Name.
    Integer,                 // Type.
    "Indicates if the output should be a probability instead of a binary response (only available for C-SVC and Nu-SVC).", // Overview
    "Indicates if the output should be a probability instead of a binary response (only available for C-SVC and Nu-SVC).", // Description.
    1,         // Not zero if the parameter has lower limit.
    0,         // Parameter's lower limit.
    1,         // Not zero if the parameter has upper limit.
    1,         // Parameter's upper limit.
    "1"        // Parameter's typical (default) value.
  },
  // Number of pseudo absences to be generated
  {
    PSEUDO_ID,                   // Id.
    "Number of pseudo-absences", // Name.
    Integer,                     // Type.
    "Number of pseudo-absences to be generated (only for C-SVC and Nu-SVC when no absences have been provided). When absences are needed, a zero parameter will default to the same number of presences.", // Overview
    "Number of pseudo-absences to be generated (only for C-SVC and Nu-SVC when no absences have been provided). When absences are needed, a zero parameter will default to the same number of presences.", // Description.
    1,         // Not zero if the parameter has lower limit.
    0,         // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    0,         // Parameter's upper limit.
    "0"        // Parameter's typical (default) value.
  },
};

/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "SVM", 	                   // Id.
  "SVM (Support Vector Machines)", // Name.
  "0.5",       	                   // Version.

  // Overview
  "Support vector machines (SVMs) are a set of related supervised learning methods that belong to a family of generalized linear classifiers. They can also be considered a special case of Tikhonov regularization. A special property of SVMs is that they simultaneously minimize the empirical classification error and maximize the geometric margin; hence they are also known as maximum margin classifiers. Content retrieved from Wikipedia on the 13th of June, 2007: http://en.wikipedia.org/w/index.php?title=Support_vector_machine&oldid=136646498.",

  // Description.
  "Support vector machines map input vectors to a higher dimensional space where a maximal separating hyperplane is constructed. Two parallel hyperplanes are constructed on each side of the hyperplane that separates the data. The separating hyperplane is the hyperplane that maximises the distance between the two parallel hyperplanes. An assumption is made that the larger the margin or distance between these parallel hyperplanes the better the generalisation error of the classifier will be. The model produced by support vector classification only depends on a subset of the training data, because the cost function for building the model does not care about training points that lie beyond the margin. Content retrieved from Wikipedia on the 13th of June, 2007: http://en.wikipedia.org/w/index.php?title=Support_vector_machine&oldid=136646498. The openModeller implementation of SVMs makes use of the libsvm library version 2.85: Chih-Chung Chang and Chih-Jen Lin, LIBSVM: a library for support vector machines, 2001. Software available at http://www.csie.ntu.edu.tw/~cjlin/libsvm.\n\nRelease history:\n version 0.1: initial release\n version 0.2: New parameter to specify the number of pseudo-absences to be generated; upgraded to libsvm 2.85; fixed memory leaks\n version 0.3: when absences are needed and the number of pseudo absences to be generated is zero, it will default to the same number of presences\n version 0.4: included missing serialization of C\n version 0.5: the indication if the algorithm needed normalized environmental data was not working when the algorithm was loaded from an existing model.",

  "Vladimir N. Vapnik", // Algorithm author.
  "1) Vapnik, V. (1995) The Nature of Statistical Learning Theory. SpringerVerlag. 2) Schölkopf, B., Smola, A., Williamson, R. and Bartlett, P.L.(2000). New support vector algorithms. Neural Computation, 12, 1207-1245. 3) Schölkopf, B., Platt, J.C., Shawe-Taylor, J., Smola A.J. and Williamson, R.C. (2001). Estimating the support of a high-dimensional distribution. Neural Computation, 13, 1443-1471. 4) Cristianini, N. & Shawe-Taylor, J. (2000). An Introduction to Support Vector Machines and other kernel-based learning methods. Cambridge University Press.", // Bibliography.

  "Renato De Giovanni in collaboration with Ana Carolina Lorena", // Code author.
  "renato [at] cria . org . br", // Code author's contact.

  0, // Does not accept categorical data.
  0, // Does not need (pseudo)absence points.

  NUM_PARAM, // Algorithm's parameters.
  parameters
};

// Note: I needed to copy this structure definition from svm.cpp, otherwise
// our custom serialization would not compile. If there's any problem with this 
// approach, then I suggest removing this definition and then moving the svm_model
// definition from svm.cpp to svm.h
struct svm_model
{
  svm_parameter param;// parameter
  int nr_class;       // number of classes, = 2 in regression/one class svm
  int l;              // total #SV
  svm_node **SV;      // SVs (SV[l])
  double **sv_coef;   // coefficients for SVs in decision functions (sv_coef[k-1][l])
  double *rho;        // constants in decision functions (rho[k*(k-1)/2])
  double *probA;      // parwise probability information
  double *probB;

  // for classification only

  int *label; // label of each class (label[k])
  int *nSV;   // number of SVs for each class (nSV[k])
              // nSV[0] + nSV[1] + ... + nSV[k-1] = l
  // XXX
  int free_sv; // 1 if svm_model is created by svm_load_model
               // 0 if svm_model is created by svm_train
};

/****************************************************************/
/****************** Algorithm's factory function ****************/

OM_ALG_DLL_EXPORT
AlgorithmImpl *
algorithmFactory()
{
  return new SvmAlgorithm();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}


/*********************************************/
/************** SVM algorithm ****************/

/*******************/
/*** constructor ***/

SvmAlgorithm::SvmAlgorithm() :
  AlgorithmImpl( &metadata ),
  _done( false ),
  _num_layers( 0 ),
  _svm_model( 0 ),
  _presence_index( -1 )
{
  _normalizerPtr = new MeanVarianceNormalizer();

  // Needs to be initialized (see destructor)
  _svm_model = 0;

  _svm_problem.l = 0;
}


/******************/
/*** destructor ***/

SvmAlgorithm::~SvmAlgorithm()
{
  if ( _svm_model ) {

    svm_destroy_model( _svm_model );
  }

  if ( _svm_problem.l > 0 ) {

    delete[] _svm_problem.y;

    for ( int i = 0; i < _svm_problem.l; ++i ) {

      delete _svm_problem.x[i];
    }

    delete[] _svm_problem.x;
  }
}

/**************************/
/*** need Normalization ***/
int SvmAlgorithm::needNormalization()
{
  int svm_type;

  if ( done() ) {

    if ( ! _normalizerPtr ) {
	    
      return 0;
    }
  }
  else {
	  
    if ( getParameter( SVMTYPE_ID, &svm_type ) && svm_type != 2 && _samp->numAbsence() == 0 ) {

      // It will be necessary to generate pseudo absences, so do not waste
      // time normalizing things because normalization should ideally consider
      // all trainning points (including pseudo-absences). In this specific case, 
      // normalization will take place in initialize().
      return 0;
    }
  }

  return 1;
}

/******************/
/*** initialize ***/
int
SvmAlgorithm::initialize()
{
  // SVM type
  if ( ! getParameter( SVMTYPE_ID, &_svm_parameter.svm_type ) ) {

    Log::instance()->error( SVM_LOG_PREFIX "Parameter '" SVMTYPE_ID "' not passed.\n" );
    return 0;
  }

  // Need to check SVM type because some types from the svm library will not be supported
  if ( _svm_parameter.svm_type != 0 && 
       _svm_parameter.svm_type != 1 && 
       _svm_parameter.svm_type != 2  ) {

    Log::instance()->error( SVM_LOG_PREFIX "Parameter '" SVMTYPE_ID "' not set properly. It must be 0, 1 or 2.\n" );
    return 0;
  }

  // Kernel type
  if ( ! getParameter( KERNELTYPE_ID, &_svm_parameter.kernel_type ) ) {

    Log::instance()->error( SVM_LOG_PREFIX "Parameter '" KERNELTYPE_ID "' not passed.\n" );
    return 0;
  }

  // Need to check Kernel type because some kernels from the svm library will not be supported
  if ( _svm_parameter.kernel_type != 0 && 
       _svm_parameter.kernel_type != 1 && 
       _svm_parameter.kernel_type != 2  ) {

    Log::instance()->error( SVM_LOG_PREFIX "Parameter '" KERNELTYPE_ID "' not set properly. It must be 0, 1 or 2.\n" );
    return 0;
  }

  // Degree
  if ( ! getParameter( DEGREE_ID, &_svm_parameter.degree ) ) {

    Log::instance()->error( SVM_LOG_PREFIX "Parameter '" DEGREE_ID "' not passed.\n" );
    return 0;
  }

  // Gamma
  if ( ! getParameter( GAMMA_ID, &_svm_parameter.gamma ) ) {

    Log::instance()->error( SVM_LOG_PREFIX "Parameter '" GAMMA_ID "' not passed.\n" );
    return 0;
  }

  _num_layers = _samp->numIndependent();

  if ( _svm_parameter.gamma == 0 ) {

    _svm_parameter.gamma = 1.0/_num_layers;
  }

  // Coef0
  if ( ! getParameter( COEF0_ID, &_svm_parameter.coef0 ) ) {

    Log::instance()->error( SVM_LOG_PREFIX "Parameter '" COEF0_ID "' not passed.\n" );
    return 0;
  }

  // C
  if ( ! getParameter( C_ID, &_svm_parameter.C ) ) {

    Log::instance()->error( SVM_LOG_PREFIX "Parameter '" C_ID "' not passed.\n" );
    return 0;
  }

  // Nu
  if ( ! getParameter( NU_ID, &_svm_parameter.nu ) ) {

    Log::instance()->error( SVM_LOG_PREFIX "Parameter '" NU_ID "' not passed.\n" );
    return 0;
  }

  // Probabilistic output
  if ( ! getParameter( PROB_ID, &_svm_parameter.probability ) ) {

    Log::instance()->error( SVM_LOG_PREFIX "Parameter '" PROB_ID "' not passed.\n" );
    return 0;
  }

  // Check if probabilistic output is 0 or 1
  if ( _svm_parameter.probability != 0 && 
       _svm_parameter.probability != 1 ) {

    Log::instance()->error( SVM_LOG_PREFIX "Parameter '" PROB_ID "' not set properly. It must be 0 or 1.\n" );
    return 0;
  }

  // Probability estimates are not available for one-class SVM
  if ( _svm_parameter.svm_type == 2  ) {

    Log::instance()->warn( SVM_LOG_PREFIX "Probability estimates are not available for one-class SVM. Ignoring parameter.\n" );
    _svm_parameter.probability = 0;
  }

  // Is this necessary?
  _svm_parameter.cache_size = 100;
  _svm_parameter.eps = 1e-3;
  _svm_parameter.p = 0.1;
  _svm_parameter.shrinking = 1;
  _svm_parameter.nr_weight = 0;
  _svm_parameter.weight_label = NULL;
  _svm_parameter.weight = NULL;

  // Remove redundant entries
  //_samp->environmentallyUnique();

  // Check the number of presences
  int num_presences = _samp->numPresence();

  if ( num_presences == 0 ) {

    Log::instance()->warn( SVM_LOG_PREFIX "No presence points inside the mask!\n" );
    return 0;
  }

  int num_absences = _samp->numAbsence();

  bool generate_pseudo_absences = false;

  // All types of SVM will need absences, except one-class SVM
  if ( num_absences <= 0 && _svm_parameter.svm_type != 2 ) {

    Log::instance()->warn( SVM_LOG_PREFIX "No absence points available.\n" );

    // Pseudo-absences will be generated later
    if ( ! getParameter( PSEUDO_ID, &num_absences ) ) {

      Log::instance()->warn( SVM_LOG_PREFIX "Number of pseudo absences unspecified. Default will be %d (same number of presences).\n", num_presences );

      num_absences = num_presences;
    }
    else if ( num_absences == 0 ) {

      Log::instance()->warn( SVM_LOG_PREFIX "Number of pseudo absences will be %d (same number of presences).\n", num_presences );

      num_absences = num_presences;
    }
    else if ( num_absences < 0 ) {

      Log::instance()->warn( SVM_LOG_PREFIX "Number of pseudo absences must be a positive number.\n" );
      return 0;
    }

    generate_pseudo_absences = true;
  }

  int num_points = num_presences;

  // Sum absence points if not dealing with one-class SVM
  if ( _svm_parameter.svm_type != 2 ) {

    num_points += num_absences;
  }

  _svm_problem.l = num_points;

  _svm_problem.y = new double[num_points];
  _svm_problem.x = new svm_node*[num_points];

  // Load SVM problem with samples

  OccurrencesPtr presences = _samp->getPresences();

  OccurrencesImpl::const_iterator p_iterator;
  OccurrencesImpl::const_iterator p_end;

  int i = 0; // shared counter

  // Absences

  if ( _svm_parameter.svm_type != 2 ) {

    OccurrencesPtr absences;

    if ( generate_pseudo_absences ) {

      Log::instance()->info( SVM_LOG_PREFIX "Generating pseudo-absences.\n" );

      absences = new OccurrencesImpl( presences->label(), presences->coordSystem() );

      for ( int i = 0; i < num_absences; ++i ) {

        OccurrencePtr oc = _samp->getPseudoAbsence();
        absences->insert( oc ); 
      }

      // Compute normalization with all points
      SamplerPtr mySamplerPtr = createSampler( _samp->getEnvironment(), presences, absences );

      _normalizerPtr->computeNormalization( mySamplerPtr );

      setNormalization( _samp );

      absences->normalize( _normalizerPtr );
    }
    else {

      // should be normalized already
      absences = _samp->getAbsences();
    }

    p_iterator = absences->begin();
    p_end = absences->end();

    while ( p_iterator != p_end ) {

      Sample point = (*p_iterator)->environment();

      _svm_problem.y[i] = -1; // absence

      _svm_problem.x[i] = new svm_node[_num_layers+1];

      _getNode( _svm_problem.x[i], point );
    
      ++p_iterator;
      ++i;
    }
  }

  // Presences (should be normalized already, in one way or another)

  p_iterator = presences->begin();
  p_end = presences->end();

  while ( p_iterator != p_end ) {

    Sample point = (*p_iterator)->environment();

    _svm_problem.y[i] = +1; // presence

    _svm_problem.x[i] = new svm_node[_num_layers+1];

    _getNode( _svm_problem.x[i], point );
    
    ++p_iterator;
    ++i;
  }

  // Check parameters using svm library logic
  const char *error_msg;
  error_msg = svm_check_parameter( &_svm_problem, &_svm_parameter );

  if ( error_msg ) {

    Log::instance()->error( error_msg );
    return 0;
  }

  return 1;
}


/***************/
/*** iterate ***/
int
SvmAlgorithm::iterate()
{
  _svm_model = svm_train( &_svm_problem, &_svm_parameter );

  if ( _svm_parameter.probability == 1 && svm_check_probability_model( _svm_model ) == 0 ){

    Log::instance()->error( SVM_LOG_PREFIX "Generated model cannot return probability estimates.\n" );
    return 0;
  }

  int * labels = new int[2];

  svm_get_labels( _svm_model, labels );

  _presence_index = ( labels[0] == +1 ) ? 0 : 1;

  delete labels;

  _done = true;

  // debug
  //svm_save_model( "model.svm", _svm_model );

  return 1;
}


/************/
/*** done ***/
int
SvmAlgorithm::done() const
{
  return _done;
}

/*****************/
/*** get Value ***/
Scalar
SvmAlgorithm::getValue( const Sample& x ) const
{
  svm_node * node = new svm_node[_num_layers+1];

  _getNode( node, x );

  double prob;

  if ( _svm_parameter.probability == 1 ) {

    // Probability output

    double * estimates = new double[2];

    svm_predict_probability( _svm_model, node, estimates );

    prob = estimates[_presence_index];

    delete[] estimates;
  }
  else {

    // Binary output

    double class_predicted = svm_predict( _svm_model, node );

    prob = ( class_predicted < 0.0 ) ? 0 : 1;
  }

  delete[] node;

  return prob;
}

/***********************/
/*** get Convergence ***/
int
SvmAlgorithm::getConvergence( Scalar * const val ) const
{
  *val = 1.0;
  return 1;
}

/****************/
/*** get Node ***/
void
SvmAlgorithm::_getNode( svm_node * node, const Sample& sample ) const
{
  for ( int j = 0; j < _num_layers; ++j ) {

    node[j].index = j+1;  // attr index (must start with 1!)
    node[j].value = sample[j]; // attr value
  }

  node[_num_layers].index = -1; // end of array
  node[_num_layers].value = 0;  // end of array
}

/****************************************************************/
/****************** configuration *******************************/
void
SvmAlgorithm::_getConfiguration( ConfigurationPtr& config ) const
{
  if ( ! _done )
    return;

  ConfigurationPtr model_config( new ConfigurationImpl("Svm") );
  config->addSubsection( model_config );

  model_config->addNameValue( "NumLayers", _num_layers );
  model_config->addNameValue( "Type", _svm_parameter.svm_type );
  model_config->addNameValue( "KernelType", _svm_parameter.kernel_type );
  model_config->addNameValue( "Probabilistic", _svm_parameter.probability );
  model_config->addNameValue( "Degree", _svm_parameter.degree );
  model_config->addNameValue( "Gamma", _svm_parameter.gamma );
  model_config->addNameValue( "Coef0", _svm_parameter.coef0 );
  model_config->addNameValue( "C", _svm_parameter.C );
  model_config->addNameValue( "Rho", _svm_model->rho[0] ); // assuming always one

  if ( _svm_parameter.probability == 1 ) {

      model_config->addNameValue( "ProbA", _svm_model->probA[0] );
      model_config->addNameValue( "ProbB", _svm_model->probB[0] );
  }

  if ( _svm_parameter.svm_type != 2 ) {

      model_config->addNameValue( "NrSv", _svm_model->nSV, 2 );
  }

  ConfigurationPtr vectors_config( new ConfigurationImpl("Vectors") );
  model_config->addSubsection( vectors_config );

  vectors_config->addNameValue( "Total", _svm_model->l );

  // Labels
  int * labels = new int[2];

  svm_get_labels( _svm_model, labels );

  model_config->addNameValue( "Labels", labels, 2 );

  delete labels;

  // Vectors
  const double * const *sv_coef = _svm_model->sv_coef;
  const svm_node * const *SV = _svm_model->SV;

  for ( int i = 0; i < _svm_model->l; i++ ) {

    ConfigurationPtr vector_config( new ConfigurationImpl("Vector") );
    vectors_config->addSubsection( vector_config );

    vector_config->addNameValue( "Coef", sv_coef[0][i] );

    const svm_node *p = SV[i];

    while ( p->index != -1 ) {

      ConfigurationPtr node_config( new ConfigurationImpl("Node") );
      vector_config->addSubsection( node_config );

      node_config->addNameValue( "Index", p->index );
      node_config->addNameValue( "Value", p->value );

      p++;
    }
  }
}

void
SvmAlgorithm::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "Svm", false );

  if ( ! model_config )
    return;

  _svm_model = (svm_model *)malloc( (1)*sizeof( svm_model ) );

  _svm_model->probA = NULL;
  _svm_model->probB = NULL;
  _svm_model->label = NULL;
  _svm_model->nSV   = NULL;

  _num_layers = model_config->getAttributeAsInt( "NumLayers", 0 );
  _svm_parameter.svm_type = model_config->getAttributeAsInt( "Type", 0 );
  _svm_parameter.kernel_type = model_config->getAttributeAsInt( "KernelType", 2 );
  _svm_parameter.probability = model_config->getAttributeAsInt( "Probabilistic", 1 );
  _svm_parameter.degree = model_config->getAttributeAsInt( "Degree", 3 );
  _svm_parameter.gamma = model_config->getAttributeAsDouble( "Gamma", 0 );
  _svm_parameter.coef0 = model_config->getAttributeAsDouble( "Coef0", 0 );

  try {

    // Serialization of "C" was added in version 0.4 of this algorithm
    _svm_parameter.C = model_config->getAttributeAsDouble( "C", 1 );
  }
  catch ( AttributeNotFound& e ) {

    UNUSED( e );
  }

  _svm_parameter.cache_size = 100;
  _svm_parameter.eps = 1e-3;
  _svm_parameter.p = 0.1;
  _svm_parameter.shrinking = 1;
  _svm_parameter.nr_weight = 0;
  _svm_parameter.weight_label = NULL;
  _svm_parameter.weight = NULL;

  _svm_model->nr_class = 2; // presence or absence

  _svm_model->rho = new double[1];

  _svm_model->rho[0] = model_config->getAttributeAsDouble( "Rho", 0.0 ); // assuming always one

  if ( _svm_parameter.probability == 1 ) {

      _svm_model->probA = new double[1];
      _svm_model->probB = new double[1];

      _svm_model->probA[0] = model_config->getAttributeAsDouble( "ProbA", 0.0 );
      _svm_model->probB[0] = model_config->getAttributeAsDouble( "ProbB", 0.0 );
  }

  if ( _svm_parameter.svm_type != 2 ) {

      int size;
      model_config->getAttributeAsIntArray( "NrSv", &_svm_model->nSV, &size );
  }

  ConstConfigurationPtr vectors_config = model_config->getSubsection( "Vectors", false );

  _svm_model->l = vectors_config->getAttributeAsInt( "Total", 0 );

  Configuration::subsection_list vectors = vectors_config->getAllSubsections();

  Configuration::subsection_list::iterator vec = vectors.begin();
  Configuration::subsection_list::iterator last_vec = vectors.end();

  _svm_model->sv_coef = new double*[_svm_model->nr_class - 1];
  _svm_model->sv_coef[0] = new double[_svm_model->l];

  _svm_model->SV = new svm_node*[_svm_model->l];

  int i = 0;

  for ( ; vec != last_vec; ++vec ) {

    if ( (*vec)->getName() != "Vector" ) {

      continue;
    }

    _svm_model->sv_coef[0][i] = (*vec)->getAttributeAsDouble( "Coef", 0.0 );

    _svm_model->SV[i] = new svm_node[_num_layers + 1];

    Configuration::subsection_list nodes = (*vec)->getAllSubsections();

    Configuration::subsection_list::iterator node = nodes.begin();
    Configuration::subsection_list::iterator last_node = nodes.end();

    int j = 0;

    for ( ; node != last_node; ++node ) {

      if ( (*node)->getName() != "Node" ) {

        continue;
      }

      _svm_model->SV[i][j].index = (*node)->getAttributeAsInt( "Index", 0 );
      _svm_model->SV[i][j].value = (*node)->getAttributeAsDouble( "Value", 0.0 );

      ++j;
    }

    _svm_model->SV[i][j].index = -1;
    _svm_model->SV[i][j].value = 0.0;

    ++i;
  }

  // Labels
  int size;
  model_config->getAttributeAsIntArray( "Labels", &_svm_model->label, &size );

  _presence_index = ( _svm_model->label[0] == +1 ) ? 0 : 1;

  _svm_model->param = _svm_parameter;

  _svm_model->free_sv = 1;

  _done = true;
}
