/**
 * Definition of Neural Network algorithm class.
 * 
 * @author Fabrício Augusto Rodrigues (fabricio.rodrigues [at] poli . usp .br)
 * @author Alex Oshika Avilla (alex.avilla [at] poli . usp .br)
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


#include "nn_alg.hh"
#include "nn.h"

// openModeller's libraries
#include <openmodeller/Sampler.hh>
// This include is only necessary if you want to work with normalized values
// ScaleNormalizer is one of the available normalizers.
#include <openmodeller/ScaleNormalizer.hh>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>

using namespace std;

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 6

#define HIDDEN_ID        "HiddenLayerNeurons"
#define LEARNING_RATE_ID "LearningRate"
#define MOMENTUM_ID      "Momentum"
#define CHOICE_ID        "Choice"
#define EPOCH_ID         "Epoch"
#define MIN_ERROR_ID     "MinimunError"

#define NN_LOG_PREFIX "NNAlgorithm: "


// Define all parameters
/******************************/
/*** Algorithm's parameters ***/

static AlgParamMetadata parameters[NUM_PARAM] = {

  // Amount of neurons of the hidden layer
  {
    HIDDEN_ID,                               // Id.
    "Number of neurons in the hidden layer", // Name.
    Integer,                                // Type.
    "Number of neurons in the hidden layer (additional layer to the input and output layers, not connected externally).",  // Overview
    "Number of neurons in the hidden layer (additional layer to the input and output layers, not connected externally).",  // Description.
    1,         // Not zero if the parameter has lower limit.
    1,         // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    0,         // Parameter's upper limit.
    "14"        // Parameter's typical (default) value.
  },

  // Learning rate
  {
    LEARNING_RATE_ID,         // Id.
    "Learning Rate",          // Name.
    Real,                     // Type.
    "Learning Rate. Training parameter that controls the size of weight and bias changes during learning.", // Overview
    "Learning Rate. Training parameter that controls the size of weight and bias changes during learning.", // Description.
    1,    // Not zero if the parameter has lower limit.
    0.0,  // Parameter's lower limit.
    1,    // Not zero if the parameter has upper limit.
    1.0,  // Parameter's upper limit.
    "0.3" // Parameter's typical (default) value.
  },

  // Momentum
  {
    MOMENTUM_ID, // Id.
    "Momentum",  // Name.
    Real,        // Type.
    "Momentum simply adds a fraction m of the previous weight update to the current one. The momentum parameter is used to prevent the system from converging to a local minimum or saddle point.", // Overview
    "Momentum simply adds a fraction m of the previous weight update to the current one. The momentum parameter is used to prevent the system from converging to a local minimum or saddle point. A high momentum parameter can also help to increase the speed of convergence of the system. However, setting the momentum parameter too high can create a risk of overshooting the minimum, which can cause the system to become unstable. A momentum coefficient that is too low cannot reliably avoid local minima, and can also slow down the training of the system.", // Description.
    1,         // Not zero if the parameter has lower limit.
    0.0,       // Parameter's lower limit.
    1,         // Not zero if the parameter has upper limit.
    1.0,       // Parameter's upper limit.
    "0.05"     // Parameter's typical (default) value.
  },

  // Choice
  {
    CHOICE_ID,         // Id.
    "Training type",   // Name.
    Integer,           // Type.
    "0 = train by epoch, 1 = train by minimum error", // Overview
    "0 = train by epoch, 1 = train by minimum error", // Description.
    1,  // Not zero if the parameter has lower limit.
    0,  // Parameter's lower limit.
    1,  // Not zero if the parameter has upper limit.
    1,  // Parameter's upper limit.
    "1" // Parameter's typical (default) value.
  },

  // Epoch
  {
    EPOCH_ID,  // Id.
    "Epoch",   // Name.
    Integer,   // Type.
    "Determines when training will stop once the number of iterations exceeds epochs. When training by minimum error, this represents the maximum number of iterations.", // Overview
    "Determines when training will stop once the number of iterations exceeds epochs. When training by minimum error, this represents the maximum number of iterations.", // Description.
    1,         // Not zero if the parameter has lower limit.
    1,         // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    0,         // Parameter's upper limit.
    "5000000"   // Parameter's typical (default) value.
  },

  // Minimun Error
  {
    MIN_ERROR_ID,    // Id.
    "Minimun Error", // Name.
    Real,            // Type.
    "Minimum mean square error of the epoch.", // Overview
    "Minimum mean square error of the epoch. Square root of the sum of squared differences between the network targets and actual outputs divided by number of patterns (only for training by minimum error).", // Description.
    1,         // Not zero if the parameter has lower limit.
    0.0,       // Parameter's lower limit.
    1,         // Not zero if the parameter has upper limit.
    0.5,       // Parameter's upper limit.
    "0.01"     // Parameter's typical (default) value.
  },

};


// Define the algorithm
/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "ANN",  // Id.
  "Artificial Neural Network", // Name.
  "0.2", // Version.

  // Overview
  "Artificial neural networks are made up of interconnecting artificial neurons (programming constructs that mimic the properties of biological neurons). Artificial neural networks may either be used to gain an understanding of biological neural networks, or for solving artificial intelligence problems without necessarily creating a model of a real biological system. Content retrieved from Wikipedia on the 06th of May, 2008: http://en.wikipedia.org/wiki/Neural_network",

  // Description.
  "An artificial neural network (ANN), also called a simulated neural network (SNN) or commonly just neural network (NN), is an interconnected group of artificial neurons that uses a mathematical or computational model for information processing based on a connectionistic approach to computation. In most cases an ANN is an adaptive system that changes its structure based on external or internal information that flows through the network. In more practical terms, neural networks are non-linear statistical data modeling or decision making tools. They can be used to model complex relationships between inputs and outputs or to find patterns in data. Content retrieved from Wikipedia on the 06th of May, 2008: http://en.wikipedia.org/wiki/Neural_network",

  "Chopra, Paras, modified by Alex Oshika Avilla and Fabrício Augusto Rodrigues", // Algorithm author.
  "", // Bibliography.

  "Alex Oshika Avilla, Fabricio Augusto Rodrigues", // Code author.
  "alex.avilla [at] poli . usp .br, fabricio.rodrigues [at] poli . usp .br", // Code author's contact.

  0, // Does not accept categorical data.
  0, // Does not need (pseudo)absence points.

  NUM_PARAM, // Algorithm's parameters.
  parameters
};


/****************************************************************/
/****************** Algorithm's factory function ****************/
/*
* Needed code to link this to openModeller
*/

OM_ALG_DLL_EXPORT
AlgorithmImpl *
algorithmFactory()
{
  return new NNAlgorithm();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}


/*********************************************/
/*************** NN algorithm ****************/

/*******************/
/*** constructor ***/
NNAlgorithm::NNAlgorithm() : 
  AlgorithmImpl(&metadata),
  _done( false ),
  _num_layers( 0 )
{
  // Normalize from 0.0 to 1.0
  _normalizerPtr = new ScaleNormalizer( 0.0, 1.0, true );
}


/******************/
/*** destructor ***/
NNAlgorithm::~NNAlgorithm()
{
}


/**************************/
/*** need Normalization ***/
int NNAlgorithm::needNormalization()
{
  if ( _samp->numAbsence() == 0 ) {

    // It will be necessary to generate pseudo absences, so do not waste
    // time normalizing things because normalization should ideally consider
    // all trainning points (including pseudo-absences). In this specific case, 
    // normalization will take place in initialize().
    return 0;
  }

  return 1;
}


/******************/
/*** initialize ***/
/* Initialize the model specifying a threshold / cutoff point.
 * This is optional (model dependent).
 * @note This method is inherited from the Algorithm class
 * @return 0 on error
 */
int
NNAlgorithm::initialize()
{

  // NEURONS OF THE HIDDEN LAYER
  if ( ! getParameter( HIDDEN_ID, &_nn_parameter.hid ) ) {

    Log::instance()->error( NN_LOG_PREFIX "Parameter '" HIDDEN_ID "' not passed.\n" );
    return 0;
  }

  if ( _nn_parameter.hid < 1 ) {
    Log::instance()->warn( NN_LOG_PREFIX "Parameter out of range: %d\n", _nn_parameter.hid );
    return 0;
  }


  // LEARNING RATE
  if ( ! getParameter( LEARNING_RATE_ID, &_nn_parameter.learning_rate ) ) {

    Log::instance()->error( NN_LOG_PREFIX "Parameter '" LEARNING_RATE_ID "' not passed.\n" );
    return 0;
  }

  if ( _nn_parameter.learning_rate < 0 ||  _nn_parameter.learning_rate > 1) {
    Log::instance()->warn( NN_LOG_PREFIX "Parameter out of range: %f\n", _nn_parameter.learning_rate );
    return 0;
  }


  // MOMENTUM
  if ( ! getParameter( MOMENTUM_ID, &_nn_parameter.momentum ) ) {

    Log::instance()->error( NN_LOG_PREFIX "Parameter '" MOMENTUM_ID "' not passed.\n" );
    return 0;
  }

  if ( _nn_parameter.momentum < 0 ||  _nn_parameter.momentum >= 1) {
    Log::instance()->warn( NN_LOG_PREFIX "Parameter out of range: %f\n", _nn_parameter.momentum );
    return 0;
  }


  // CHOICE
  if ( ! getParameter( CHOICE_ID, &_nn_parameter.choice ) ) {

    Log::instance()->error( NN_LOG_PREFIX "Parameter '" CHOICE_ID "' not passed.\n" );
    return 0;
  }

  if ( _nn_parameter.choice != 0 && _nn_parameter.choice != 1) {
    Log::instance()->warn( NN_LOG_PREFIX "Parameter out of range: %d\n", _nn_parameter.choice );
    return 0;
  }


  // EPOCH
  if ( ! getParameter( EPOCH_ID, &_nn_parameter.epoch ) ) {

    Log::instance()->error( NN_LOG_PREFIX "Parameter '" EPOCH_ID "' not passed.\n" );
    return 0;
  }

  if ( _nn_parameter.epoch < 1) {

    Log::instance()->warn( NN_LOG_PREFIX "Parameter out of range: %d\n", _nn_parameter.epoch );
    return 0;
  }


  // MINIMUM ERROR
  if (_nn_parameter.choice == 1){

    if ( ! getParameter( MIN_ERROR_ID, &_nn_parameter.minimum_error ) ) {

      Log::instance()->error( NN_LOG_PREFIX "Parameter '" MIN_ERROR_ID "' not passed.\n" );
      return 0;
    }

    if ( _nn_parameter.minimum_error < 0 ||  _nn_parameter.minimum_error > 0.5) {

      Log::instance()->warn( NN_LOG_PREFIX "Parameter out of range: %f\n", _nn_parameter.minimum_error );
      return 0;
    }
  }


  // Check the number of layers
  _num_layers = _samp->numIndependent();


  // Check the number of presences
  num_presences = _samp->numPresence();

  if ( num_presences == 0 ) {

    Log::instance()->warn( NN_LOG_PREFIX "No presence points inside the mask!\n" );
    return 0;
  }

  // Load presence points.
  presences = _samp->getPresences();


  // Check the number of absences
  num_absences = _samp->numAbsence();

  if ( num_absences == 0 ) {

    Log::instance()->debug( NN_LOG_PREFIX "Generating pseudo-absences.\n" );

    num_absences = num_presences;

    absences = new OccurrencesImpl( presences->name(), presences->coordSystem() );


    for ( int i = 0; i < num_absences; ++i ) {

      OccurrencePtr oc = _samp->getPseudoAbsence();
      absences->insert( oc ); 
    }

    // Compute normalization with all points
    SamplerPtr mySamplerPtr = createSampler( _samp->getEnvironment(), presences, absences );


    // They generate points that are out of Map.
    _normalizerPtr->computeNormalization( mySamplerPtr );
    setNormalization( _samp );
    absences->normalize( _normalizerPtr );
  }
  else {

    Log::instance()->debug( NN_LOG_PREFIX "It is not necessary to generate pseudo-absences because it does exist absence points.\n" );

    // should be normalized already
    absences = _samp->getAbsences();
  }

  // Presence Points
  std::vector<Sample> _presencePoints;
  
  vector_input.resize(10000); // initialize vector_input
  for (int i = 0; i < 500; ++i) {
      vector_input[i].resize(500, 0);
  }
  
  vector_output.resize(10000); // initialize vector_output
  for (int i = 0; i < 1; ++i) {
      vector_input[i].resize(1, 0);
  }

  for(int j = 0; j < num_presences; j++){

    _presencePoints.push_back((*presences)[j]->environment());

    for(int i = 0; i < _num_layers; i++){
        
      vector_input[j][i] = (double)_presencePoints[j][i];
    }
  }

  // Absence Points
  std::vector<Sample> _absencePoints;

  for(int j = 0; j < num_absences; j++){

    _absencePoints.push_back((*absences)[j]->environment());

    for(int i = 0; i < _num_layers; i++){

      vector_input[j+num_presences][i] = (double)_absencePoints[j][i];
    }
  }


  _nn_parameter.pattern = num_presences + num_absences;

  _nn_parameter.outp = 1;

  
  // Presence points
  for(int j = 0; j < num_presences; j++){

    for(int i = 0; i < _nn_parameter.outp; i++){ // Always i=0 because it has one output

      vector_output[j][i] = 1.00000000; // Exist presence point
    }
  }

  // Absence points
  for(int j = num_presences; j < _nn_parameter.pattern; j++){

    for(int i = 0; i < _nn_parameter.outp; i++){ // Always i=0 because it has one output

      vector_output[j][i] = 0.00000000; // Exist absence point
    }
  }


  // Initialize the variables of Neural Network that will be used at training
  int layers[3];

  layers[0] = _num_layers;
  layers[1] = _nn_parameter.hid;
  layers[2] = 1; // or layer[2] = _nn_parameter.outp = 1;

  network.SetData(_nn_parameter.learning_rate,layers,3);

  network.RandomizeWB();

  _nn_parameter.inp = _num_layers;

  amount_epoch = 1;

  Log::instance()->debug( NN_LOG_PREFIX "\n\n\nStarting Training... ");


  // Debugs of the features of the Neural Network
  Log::instance()->debug( NN_LOG_PREFIX "\n\n***  Features of the neural network's algorithm  ***" );
  Log::instance()->debug( NN_LOG_PREFIX "\nNumber of presence: %d", num_presences);
  if(num_absences != 0){
    Log::instance()->debug( NN_LOG_PREFIX "\nNumber of absence: %d", num_absences);
  }
  else{
    Log::instance()->debug( NN_LOG_PREFIX "\nNumber of pseudoabsence: %d", num_presences);
  }
  Log::instance()->debug( NN_LOG_PREFIX "\nInput neurons: %d", _nn_parameter.inp);
  Log::instance()->debug( NN_LOG_PREFIX "\nHidden neurons: %d", _nn_parameter.hid);
  Log::instance()->debug( NN_LOG_PREFIX "\nOutput neuron: %d", _nn_parameter.outp);
  Log::instance()->debug( NN_LOG_PREFIX "\nLearning Patterns: %d", _nn_parameter.pattern);
  Log::instance()->debug( NN_LOG_PREFIX "\nLearning rate: %f", _nn_parameter.learning_rate);
  Log::instance()->debug( NN_LOG_PREFIX "\nMomentum: %f", _nn_parameter.momentum);
  Log::instance()->debug( NN_LOG_PREFIX "\nChoice: %d", _nn_parameter.choice);
  Log::instance()->debug( NN_LOG_PREFIX "\nEpochs: %.0f", _nn_parameter.epoch);
  if(_nn_parameter.choice == 1){
    Log::instance()->debug( NN_LOG_PREFIX "\nMinimum mean square error: %f", _nn_parameter.minimum_error);
  }


  return 1;
}


/***************/
/*** iterate ***/
/** Start model execution (build the model).     
 * @note This method is inherited from the Algorithm class
 * @return 0 on error 
 */
int
NNAlgorithm::iterate()
{
 
  /*                 TRAINING                */
  /*******************************************/

  // Training by epoch
  if(_nn_parameter.choice == 0){

    if(amount_epoch != (unsigned long)_nn_parameter.epoch+1){

      for(int j = 0; j < _nn_parameter.pattern; j++){

        network.Train(vector_input[j], vector_output[j], j, _nn_parameter.pattern, _nn_parameter.momentum);
      }

      network.trainingEpoch( amount_epoch, _nn_parameter.epoch, _nn_parameter.pattern);
    }

    else _done = true; // Training ends
  }


  // Training by minimum error
  if(_nn_parameter.choice == 1){

    // Check if number of iterations exceeded the limit
    if(amount_epoch > _nn_parameter.epoch){

      _done = true; // Training ends
      Log::instance()->warn( NN_LOG_PREFIX "Exceeded maximum number of iterations.\n\n");
    }

    for(int j = 0; j < _nn_parameter.pattern; j++){

      network.Train(vector_input[j], vector_output[j], j, _nn_parameter.pattern, _nn_parameter.momentum );
    }

    converged = network.trainingMinimumError( _nn_parameter.pattern, _nn_parameter.minimum_error);

    // Case converge
    if(converged == 1){

      _done = true; // Training ends
      Log::instance()->info( NN_LOG_PREFIX "Final number of the epoch: %lu\n\n", amount_epoch);
    }
  }


  amount_epoch++;

  _progress = network.getProgress();

  return 1;
}


/*******************/
/*** getProgress ***/
/** Return progress so far 
  */
float
NNAlgorithm::getProgress() const
{

  if (done())
    return 1.0;
  else
    return _progress;
}


/************/
/*** done ***/
/** Use this method to find out if the model has completed (e.g. convergence
 * point has been met. 
 * @note This method is inherited from the Algorithm class
 * @return     
 * @return Implementation specific but usually 1 for completion.
 */
int
NNAlgorithm::done() const
{

  if (_done == true)
     Log::instance()->debug( NN_LOG_PREFIX "\n\nEnding Training. ");

  return _done;
}


// Returns the occurrence probability at the environment conditions x
/*****************/
/*** get Value ***/
/** This method is used when projecting the model.  
 * @note This method is inherited from the Algorithm class
 * @return     
 * @param Scalar *x a pointer to a vector of openModeller Scalar type (currently double). The vector should contain values looked up on the environmental variable layers into which the mode is being projected. */
Scalar
NNAlgorithm::getValue( const Sample& x ) const
{

  vector<vector<double> > env_input;

  env_input.resize(1);
  for (int j = 0; j < 500; ++j) {
    env_input[j].resize(500, 0);
  }


  for(int i = 0; i < _num_layers; i++){

    env_input[0][i] = (double)x[i];
  }


  double *output;

  network.SetInputs(env_input[0]); // Load the values of each layer

  output = network.GetOutput();


  return (Scalar)*output;
}



/***************************************************************/
/****************** configuration *******************************/
void
NNAlgorithm::_getConfiguration( ConfigurationPtr& config ) const
{
  // Avoid serialization when something went wrong
  if ( ! _done ){
    return;
  }

  // These two lines create a new XML element called "NNAlgorithm"
  ConfigurationPtr model_config( new ConfigurationImpl("NNAlgorithm") );
  config->addSubsection( model_config );

  model_config->addNameValue( "NumLayers", _num_layers );
  model_config->addNameValue( "HiddenLayerNeurons", _nn_parameter.hid );


  int *layers = new int[3];

  layers[0] = _num_layers;
  layers[1] = _nn_parameter.hid;
  layers[2] = 1; // or layer[2] = _nn_parameter.outp = 1;


  size_t size_weight;

  size_weight = (_num_layers * _nn_parameter.hid) + _nn_parameter.hid;
  
  // Write weight
   
  ConfigurationPtr weight_config( new ConfigurationImpl( "Weight" ) );

  model_config->addSubsection( weight_config );

  double *weight_values = new double[size_weight];


  int siz = 0;

  for(int i = 0; i < 3; i++){

    for(int j = 0; j < layers[i]; j++){

      if(i != 2){ // Last layer does not require weights

        for(int k = 0; k < layers[i+1]; k++){

          weight_values[siz] = network.getWeight(i, j, k); // Get weights
          siz ++;
        }
      }
    }
  }

  weight_config->addNameValue( "Values", weight_values,  size_weight);


  // Write bias
  size_t size_bias;

  size_bias = _nn_parameter.hid + 1;

  ConfigurationPtr bias_config( new ConfigurationImpl( "Bias" ) );

  model_config->addSubsection( bias_config );

  double *bias_values = new double[size_bias];


  siz = 0;
 
  for(int i = 0; i < 3; i++){

    for(int j = 0; j < layers[i]; j++){

      if(i != 0){ // First layer does not need biases

        bias_values[siz] = network.getBias(i, j); // Get bias
        siz++;
      }
    }
  }

  bias_config->addNameValue( "ValuesBias", bias_values,  size_bias);


  delete[] weight_values;
  delete[] bias_values;
  delete[] layers;
}


void
NNAlgorithm::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "NNAlgorithm", false );

  if ( ! model_config ) {
    return;
  }

  _num_layers = model_config->getAttributeAsInt( "NumLayers", 0 );
  _nn_parameter.hid = model_config->getAttributeAsInt( "HiddenLayerNeurons", 14 );


  int *layers = new int (3);

  layers[0] = _num_layers;
  layers[1] = _nn_parameter.hid;
  layers[2] = 1; // or layer[2] = _nn_parameter.outp = 1;

  network.SetData(_nn_parameter.learning_rate,layers,3);

  network.RandomizeWB();



  // Weight
  ConstConfigurationPtr weight_config = model_config->getSubsection( "Weight", false );

  if ( ! weight_config ) {

    return;
  }

  std::vector<double> weight_values = weight_config->getAttributeAsVecDouble( "Values" );


  unsigned int siz = 0;

  for(int i = 0; i < 3; i++){

    for(int j = 0; j < layers[i]; j++){

      if(i != 2){ // Last layer does not require weights

        for(int k = 0; k < layers[i+1]; k++){

          network.setWeight(i, j, k, weight_values[siz]); // Set weights
          siz ++;
        }
      }
    }
  }


  // Bias
  ConstConfigurationPtr bias_config = model_config->getSubsection( "Bias", false );

  if ( ! bias_config ) {

    return;
  }

  std::vector<double> bias_values = bias_config->getAttributeAsVecDouble( "ValuesBias" );


  siz = 0;
 
  for(int i = 0; i < 3; i++){

    for(int j = 0; j < layers[i]; j++){

      if(i != 0){ // First layer does not need biases

        network.setBias(i, j, bias_values[siz]); // Set bias
        siz++;
      }
    }
  }

  _done = true;
}
