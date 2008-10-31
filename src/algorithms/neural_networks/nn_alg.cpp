/**
 * Definition of Neural Network algorithm class.
 * 
 * @author Fabrício Augusto Rodrigues (fabricio.rodrigues [at] poli . usp .br)
 * @author Alex Oshika Avilla (alex.avilla [at] poli . usp .br)
 * $Id:
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

#define NUM_PARAM 7

#define HIDDEN_ID        "HiddenLayerNeurons"
#define LEARNING_RATE_ID "LearningRate"
#define MOMENTUM_ID      "Momentum"
#define PROPTRAIN_ID     "Proptrain"
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
    HIDDEN_ID,		    	// Id.
    "Hidden Layer's Neurons",   // Name.
    Integer,           		// Type.
    "Amount of Hidden Layer's Neurons.\nHidden Layer => Layer additional to the input and output layers, not connected externally.", 	// Overview
    "Amount of Hidden Layer's Neurons.\nHidden Layer => Layer additional to the input and output layers, not connected externally.", 	// Description.
    1,         // Not zero if the parameter has lower limit.
    1,         // Parameter's lower limit.
    0,         // Not zero if the parameter has upper limit.
    0,         // Parameter's upper limit.
    "08"       // Parameter's typical (default) value.
  },

  // Learning rate
  {
    LEARNING_RATE_ID,         	// Id.
    "Learning Rate",           	// Name.
    Real,             		// Type.
    "Learning Rate => Training parameter that controls the size of weight and bias changes during learning.", 		// Overview
    "Learning Rate => Training parameter that controls the size of weight and bias changes during learning.", 		// Description.
    1,		// Not zero if the parameter has lower limit.
    0.0,	// Parameter's lower limit.
    1,		// Not zero if the parameter has upper limit.
    1.0,	// Parameter's upper limit.
    "0.3"	// Parameter's typical (default) value.
  },

  // Momentum
  {
    MOMENTUM_ID,	// Id.
    "Momentum",	// Name.
    Real,		// Type.
    "Momentum simply adds a fraction m of the previous weight update to the current one.\n Obs: The momentum parameter is used to prevent the system from converging to a local minimum or saddle point. A high momentum parameter can also help to increase the speed of convergence of the system. However, setting the momentum parameter too high can create a risk of overshooting the minimum, which can cause the system to become unstable. A momentum coefficient that is too low cannot reliably avoid local minima, and also can slow the training of the system.", 	// Overview
    "Momentum simply adds a fraction m of the previous weight update to the current one.\n Obs: The momentum parameter is used to prevent the system from converging to a local minimum or saddle point. A high momentum parameter can also help to increase the speed of convergence of the system. However, setting the momentum parameter too high can create a risk of overshooting the minimum, which can cause the system to become unstable. A momentum coefficient that is too low cannot reliably avoid local minima, and also can slow the training of the system.", 	// Description.
    1,         	// Not zero if the parameter has lower limit.
    0.0,     	// Parameter's lower limit.
    1,         	// Not zero if the parameter has upper limit.
    1.0,        // Parameter's upper limit.
    "0.05"      	// Parameter's typical (default) value.
  },

  // Proptrain
  {
    PROPTRAIN_ID,	// Id.
    "Proptrain",	// Name.
    Real,		// Type.
    "Proportion of training set according to total data sets.", 	// Overview
    "Proportion of training set according to total data sets.", 	// Description.
    1,         	// Not zero if the parameter has lower limit.
    0.5,     	// Parameter's lower limit.
    1,         	// Not zero if the parameter has upper limit.
    1.0,        // Parameter's upper limit.
    "0.8"      	// Parameter's typical (default) value.
  },

  // Choice
  {
    CHOICE_ID,         	// Id.
    "Choice",           	// Name.
    Integer,             		// Type.
    "Choice:\n 0 -> to training by epochs.\n 1 -> to training by minimum error", 		// Overview
    "Choice:\n 0 -> to training by epochs.\n 1 -> to training by minimum error", 		// Description.
    1,	// Not zero if the parameter has lower limit.
    0,	// Parameter's lower limit.
    1,	// Not zero if the parameter has upper limit.
    1,	// Parameter's upper limit.
    "0"	// Parameter's typical (default) value.
  },

  // Epoch
  {
    EPOCH_ID,	// Id.
    "Epoch",	// Name.
    Integer,	// Type.
    "Presentation of the patterns of training to a network and the calculation of new weights and biases (only for training by epochs).", 	// Overview
    "Presentation of the patterns of training to a network and the calculation of new weights and biases (only for training by epochs).", 	// Description.
    1,         	// Not zero if the parameter has lower limit.
    1,     	// Parameter's lower limit.
    0,         	// Not zero if the parameter has upper limit.
    0,         	// Parameter's upper limit.
    "100000"     // Parameter's typical (default) value.
  },

  // Minimun Error
  {
    MIN_ERROR_ID,	// Id.
    "Minimun Error",	// Name.
    Real,		// Type.
    "Minumum mean square error of the epoch.\nSquare root of the sum of squared differences between the network targets and actual outputs divided by number of patterns (only for training by minimum error).", 	// Overview
    "Minumum mean square error of the epoch.\nSquare root of the sum of squared differences between the network targets and actual outputs divided by number of patterns (only for training by minimum error).", 	// Description.
    1,         	// Not zero if the parameter has lower limit.
    0.0,     	// Parameter's lower limit.
    1,         	// Not zero if the parameter has upper limit.
    0.5,        // Parameter's upper limit.
    "0.01"     	// Parameter's typical (default) value.
  },

};


// Define the algorithm
/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "ANN_EPOCH",	// Id.
  "Artificial Neural Network",	// Name.
  "0.1",	// Version.

  // Overview
  "Artificial neural networks are made up of interconnecting artificial neurons (programming constructs that mimic the properties of biological neurons). Artificial neural networks may either be used to gain an understanding of biological neural networks, or for solving artificial intelligence problems without necessarily creating a model of a real biological system. Content retrieved from Wikipedia on the 06th of May, 2008: http://en.wikipedia.org/wiki/Neural_network",

  // Description.
  "An artificial neural network (ANN), also called a simulated neural network (SNN) or commonly just neural network (NN) is an interconnected group of artificial neurons that uses a mathematical or computational model for information processing based on a connectionistic approach to computation. In most cases an ANN is an adaptive system that changes its structure based on external or internal information that flows through the network.In more practical terms neural networks are non-linear statistical data modeling or decision making tools. They can be used to model complex relationships between inputs and outputs or to find patterns in data. Content retrieved from Wikipedia on the 06th of May, 2008: http://en.wikipedia.org/wiki/Neural_network",

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


  // PROPTRAIN
  if ( ! getParameter( PROPTRAIN_ID, &_nn_parameter.proptrain ) ) {

    Log::instance()->error( NN_LOG_PREFIX "Parameter '" PROPTRAIN_ID "' not passed.\n" );
    return 0;
  }

  if ( _nn_parameter.proptrain < 0.5 ||  _nn_parameter.proptrain > 1) {
    Log::instance()->warn( NN_LOG_PREFIX "Parameter out of range: %f\n", _nn_parameter.proptrain );
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
  if (_nn_parameter.choice == 0){

    if ( ! getParameter( EPOCH_ID, &_nn_parameter.epoch ) ) {

	Log::instance()->error( NN_LOG_PREFIX "Parameter '" EPOCH_ID "' not passed.\n" );
	return 0;
    }

    if ( _nn_parameter.epoch < 1) {

	Log::instance()->warn( NN_LOG_PREFIX "Parameter out of range: %d\n", _nn_parameter.epoch );
	return 0;
    }
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
  //Debug
  //cout << endl << "Number of layers: " << _num_layers << endl;


  // Check the number of presences
  num_presences = _samp->numPresence();
  //Debug
  //cout << endl << "Number of presence points: " << num_presences << endl;

  if ( num_presences == 0 ) {

    Log::instance()->warn( NN_LOG_PREFIX "No presence points inside the mask!\n" );
    return 0;
  }


  // Check the number of absences
  num_absences = _samp->numAbsence();
  //Debug
  //cout << endl << "Number of absence points: " << num_absences << endl;


  //Debug
  //cout << endl << "propTrain: " << _nn_parameter.proptrain << endl << endl;


  // Split sampler into test and trainning
  SamplerPtr training_sampler;
  SamplerPtr testing_sampler;

  splitSampler( _samp, &training_sampler, &testing_sampler, _nn_parameter.proptrain );


  training_points_cont = 0;
  testing_points_cont = 0;



  // TRAINING PRESENCE AND ABSENCE POINTS
  if ( training_sampler->numPresence() ) {

    // Load training presence points.
    training_presences = training_sampler->getPresences();
  }

  std::vector<Sample> training_presencePoints;

  cout << endl << endl;


  if ( training_sampler->numAbsence() ) {

    Log::instance()->info( NN_LOG_PREFIX "Is not necessary to generate pseudo-absences because exist absence points.\n" );

    // Load training absence points.
    training_absences = training_sampler->getAbsences();  //absences = _samp->getAbsences();
  }

  else{

    Log::instance()->info( NN_LOG_PREFIX "Generating pseudo-absences.\n" );

    training_absences = new OccurrencesImpl( training_presences->name(), training_presences->coordSystem() );

    num_pseudoabesences =  training_sampler->numPresence();

    //Debug
    //printf("\nNumber of training presence points:  %d\n", training_sampler->numPresence() );

    for ( int i = 0; i < num_pseudoabesences; ++i ) {

	OccurrencePtr oc = _samp->getPseudoAbsence();
	training_absences->insert( oc ); 
    }

    // Compute normalization with all points
    SamplerPtr mySamplerPtr = createSampler( _samp->getEnvironment(), training_presences, training_absences );


    // They generate points that are out of Map.
    _normalizerPtr->computeNormalization( mySamplerPtr );
    setNormalization( _samp );
    training_absences->normalize( _normalizerPtr );
  }

  std::vector<Sample> training_absencePoints;


  if(training_sampler->numAbsence() != training_sampler->numPresence() && num_pseudoabesences != training_sampler->numPresence()){

    for(int j = 0; j < training_sampler->numPresence(); j++){

	training_presencePoints.push_back((*training_presences)[j]->environment());

	for(int i = 0; i < _num_layers; i++){

	  training_vector_input[training_points_cont][i] = (double)training_presencePoints[j][i];
	}

    training_vector_output[training_points_cont][0] = 1.00000000; // Exist presence point
    training_points_cont ++;
    }

    for(int j = 0; j < training_sampler->numAbsence() || j < num_pseudoabesences; j++){

	training_absencePoints.push_back((*training_absences)[j]->environment());

	for(int i = 0; i < _num_layers; i++){

	  training_vector_input[training_points_cont][i] = (double)training_absencePoints[j][i];
	}

    training_vector_output[training_points_cont][0] = 0.00000000; // Exist absence point
    training_points_cont ++;
    }
  }

  else{

    for(int j = 0; j < training_sampler->numAbsence() || j < num_pseudoabesences; j++){

	training_presencePoints.push_back((*training_presences)[j]->environment());
	training_absencePoints.push_back((*training_absences)[j]->environment());

	for(int i = 0; i < _num_layers; i++){

          training_vector_input[training_points_cont][i] = (double)training_presencePoints[j][i];
          training_vector_input[training_points_cont+1][i] = (double)training_absencePoints[j][i];
	}

    training_vector_output[training_points_cont][0] = 1.00000000; // Exist presence point
    training_vector_output[training_points_cont+1][0] = 0.00000000; // Exist absence point
    training_points_cont = training_points_cont + 2;
    }
  }



  // TESTING PRESENCE AND ABSENCE POINTS
  if ( testing_sampler->numPresence() ) {

    // Load testing presence points.
    testing_presences = testing_sampler->getPresences();
  }

  std::vector<Sample> testing_presencePoints;


  if ( testing_sampler->numAbsence() ) {

    // Load testing absence points.
    testing_absences = testing_sampler->getAbsences();
  }

  std::vector<Sample> testing_absencePoints;


  if(testing_sampler->numPresence() != testing_sampler->numAbsence()){

    for(int j = 0; j < testing_sampler->numPresence(); j++){

	testing_presencePoints.push_back((*testing_presences)[j]->environment());

	for(int i = 0; i < _num_layers; i++){

	  testing_vector_input[testing_points_cont][i] = (double)testing_presencePoints[j][i];
	}

    testing_vector_output[testing_points_cont][0] = 1.00000000; // Exist presence point
    testing_points_cont ++;
    }

    for(int j = 0; j < testing_sampler->numAbsence(); j++){

	testing_absencePoints.push_back((*testing_absences)[j]->environment());

	for(int i = 0; i < _num_layers; i++){

	  testing_vector_input[testing_points_cont][i] = (double)testing_absencePoints[j][i];
	}

    testing_vector_output[testing_points_cont][0] = 0.00000000; // Exist absence point
    testing_points_cont ++;
    }
  }

  else{

    for(int j = 0; j < testing_sampler->numPresence(); j++){

	testing_presencePoints.push_back((*testing_presences)[j]->environment());
	testing_absencePoints.push_back((*testing_absences)[j]->environment());

	for(int i = 0; i < _num_layers; i++){

	  testing_vector_input[testing_points_cont][i] = (double)testing_presencePoints[j][i];
  	  testing_vector_input[testing_points_cont+1][i] = (double)testing_absencePoints[j][i];
	}

    testing_vector_output[testing_points_cont][0] = 1.00000000; // Exist presence point
    testing_vector_output[testing_points_cont+1][0] = 0.00000000; // Exist absence point
    testing_points_cont = testing_points_cont + 2;
    }  }


/************ Print to screen the Environment Layers *************/
/*
  // Debug to training
  for(int j = 0; j < training_sampler->numPresence() + training_sampler->numAbsence() || j < training_sampler->numPresence() + num_pseudoabesences; j++){

    cout << endl << endl << "Training points:" << j+1;

    for(int i = 0; i < _num_layers; i++){

	cout << " " << training_vector_input[j][i];
    }

    cout << "  " << training_vector_output[j][0];
  }


  // Debug to testing
  for(int j = 0; j < testing_sampler->numPresence() + testing_sampler->numAbsence(); j++){

    cout << endl << endl << "Testing points:" << j+1;

    for(int i = 0; i < _num_layers; i++){

	cout << " " << testing_vector_input[j][i];
    }

    cout << "  " << testing_vector_output[j][0];
  }
*/
/***************************************************************/

/************ Print to file the Environment Layers *************/
/*
  char name_of_file[127];
                
  printf("\n\nType the name of the file (.txt) that you want to save the environment layers: ");
  scanf("%s", name_of_file);
				         
  strcat(name_of_file,".txt"); // add two strings
                                    
  FILE *getout;
				                    
  if((getout=fopen(name_of_file,"w"))==NULL){

    printf("\n\nFailed to try to save the file");
    return 0;
  }	


  // Training
  for(int j = 0; j < training_sampler->numPresence() + training_sampler->numAbsence() || j < training_sampler->numPresence() + num_pseudoabesences; j++){

    fprintf (getout, "#Training env_layer %d\n", j+1);

    for(int i = 0; i < _num_layers; i++){

	fprintf (getout, "%f ", training_vector_input[j][i]);
    }

    fprintf (getout, "\n%f\n\n", training_vector_output[j][0]);	
  }


  // Testing
  for(int j = 0; j < testing_sampler->numPresence() + testing_sampler->numAbsence(); j++){

    fprintf (getout, "#Testing env_layer %d\n", j+1);

    for(int i = 0; i < _num_layers; i++){

	fprintf (getout, "%f ", testing_vector_input[j][i]);
    }

    fprintf (getout, "\n%f\n\n", testing_vector_output[j][0]);	
  }


  fclose(getout);
                         
  printf("\nEnvironment layers saved.");
*/
/**************************************************/


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
 
   int layers[3];

  layers[0] = _num_layers;
  layers[1] = _nn_parameter.hid;
  layers[2] = 1; // or layer[2] = _nn_parameter.outp = 1;

				
  network.SetData(_nn_parameter.learning_rate,layers,3);

  network.RandomizeWB();
			

  _nn_parameter.inp = _num_layers;

  _nn_parameter.outp = 1;


  //Debug
  cout<<"\n\n/***  Features of the neural network's algorithm  ***/";
  cout<<"\nNumber of presence: "<<num_presences;
  if(num_absences != 0){
    cout<<"\nNumber of absence: "<<num_absences;
  }
  else{
    cout<<"\nNumber of pseudoabsence: "<<num_pseudoabesences; 
  }
  cout<<"\nInput neurons: "<<_nn_parameter.inp;
  cout<<"\nHidden neurons: "<<_nn_parameter.hid;
  cout<<"\nOutput neuron: "<<_nn_parameter.outp;
  cout<<"\nProptrain: "<<_nn_parameter.proptrain;
  cout<<"\nLearning Patterns: "<<training_points_cont;
  cout<<"\nTesting Patterns: "<<testing_points_cont;
  cout<<"\nLearning rate: "<<_nn_parameter.learning_rate;
  cout<<"\nMomentum: "<<_nn_parameter.momentum;
  cout<<"\nChoice: "<<_nn_parameter.choice;
  if(_nn_parameter.choice == 0){
    cout<<"\nEpochs : "<<_nn_parameter.epoch;
  }
  if(_nn_parameter.choice == 1){
    cout<<"\nMinimum mean square error: "<<_nn_parameter.minimum_error;
  }


				         
  /*                 TRAINING                */
  /*******************************************/
  cout<<"\n\n\nStarting Training... ";

  char name_file[127];
                
  unsigned long print_points_wished = 0;

  
  printf("\n\nType the name of the file (.xls) that you want to save: ");
  scanf("%s", name_file);
				         
  strcat(name_file,".xls"); //Add two strings
                                    
  FILE *exit;
				                    
  if((exit=fopen(name_file,"w"))==NULL){

    printf("\n\nFailed to try to save the file");
    return 0;
  }	


  fprintf (exit, "Epoch\tOutput\n");

  // Training by epoch
  if(_nn_parameter.choice == 0){

    for(unsigned long i = 0; i < (unsigned long)_nn_parameter.epoch; i++){

	for(int j = 0; j < training_points_cont; j++){

	  network.Train(training_vector_input[j], training_vector_output[j], j, training_points_cont, _nn_parameter.momentum);
	}

	//Debug
	//printf("\nNumber of the epoch: %lu", i+1);

	network.print_to_file(exit, i, _nn_parameter.epoch, training_points_cont, print_points_wished);

	if(i == print_points_wished){

	  print_points_wished += 300; //Decides which points are printed
	}
    }
  }


  // Training by minimum error
  if(_nn_parameter.choice == 1){

    unsigned long amount_epoch = 1;
    int converged = 0;


    while(converged == 0){

	for(int j = 0; j < training_points_cont; j++){

	  network.Train(training_vector_input[j], training_vector_output[j], j, training_points_cont, _nn_parameter.momentum );
	}

	converged = network.convergence(exit, amount_epoch, training_points_cont, _nn_parameter.minimum_error, print_points_wished);


	if(amount_epoch == print_points_wished){

	  print_points_wished += 300; //Decides which points are printed
	}

	amount_epoch++;
    }

    printf("\n\nFinal number of the epoch: %lu\n", amount_epoch-1);
  }


  fclose(exit);
                         
  printf("\n\nDatas saved.\n");

  cout<<"\n\nEnding Training. ";
  /*******************************************/



  /*                 TESTING	             */
  /*******************************************/
  info("\n\n\nStarting Testing... ");

  for(int j = 0; j < testing_points_cont; j++){

    info("\n\nPattern number :  %d",j+1);
		
    network.SetInputs(testing_vector_input[j]);

    outputs = network.GetOutput();

/*
    // Debug	
    for(int i = 0; i < _num_layers; i++){

	info("\nInput %d   :  ",i+1);
	info("%.8f",testing_vector_input[j][i]); 
    }
*/

    for(int i = 0; i < _nn_parameter.outp; i++){

	info("\nOutput %d  :  ",i+1);
	info("%.8f",outputs[i]); 
    }

    delete outputs;
    //double *outputs;
  }

  info("\n\nEnd Testing.\n\n");
  /*******************************************/

  //cin.get();


  _done = true;

  return 1;
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
  return _done;
}

//Debug
//int m=0;

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

  Scalar env_input[1][500]; // [1][_num_layers]


  //Debug
  //cout << "\nPoint" << m << " ";

  for(int i = 0; i < _num_layers; i++){

    env_input[0][i] = (double)x[i];
    //cout << env_input[0][i] << " ";
  }


  double *output;

  network.SetInputs(env_input[0]); // Carrega os valores de cada layer

  output = network.GetOutput();

  //Debug
  //cout <<"  Output: " << output[0];

/*
  //Debug
  // Print all presence points that were used to train
  for(int j = 0; j < num_presences; j++){

    if(env_input[0][0] == vector_input[j][0]  && env_input[0][1] == vector_input[j][1]){ ... to be continued ...

    cout<<"Presence point: "<< x << "Output: "<< output[0] << "\n";
    }
  }

  //Debug
  // Print all (pseudo)absence points that were used to train
  for(int j = num_presences; j < _nn_parameter.pattern; j++){

    if(env_input[0][0] == vector_input[j][0]  && env_input[0][1] == vector_input[j][1]){ ... to be continued ...

    cout<<"PseudoAbsence point: "<< x << "Output: "<< output[0] << "\n";
    }
  }
*/

  //Debug
  //m++;

  return (Scalar)*output;
}


/***********************/
/*** get Convergence ***/
/** Returns a value that represents the convergence of the algorithm
 * expressed as a number between 0 and 1 where 0 represents model
 * completion. 
 * @return 
 * @param Scalar *val 
 */
int
NNAlgorithm::getConvergence( Scalar *val )
{
  *val = 1.0;
  return 1;
}



/***************************************************************/
/****************** configuration *******************************/
void
NNAlgorithm::_getConfiguration( ConfigurationPtr& config ) const
{
  //avoid serialization when something went wrong
  if ( ! _done ){
    return;
  }

  //These two lines create a new XML element called "NNAlgorithm"
  ConfigurationPtr model_config( new ConfigurationImpl("NNAlgorithm") );
  config->addSubsection( model_config );

  model_config->addNameValue( "NumLayers", _num_layers );
  model_config->addNameValue( "HiddenLayerNeurons", _nn_parameter.hid );


  int *layers = new int (3);

  layers[0] = _num_layers;
  layers[1] = _nn_parameter.hid;
  layers[2] = 1; // or layer[2] = _nn_parameter.outp = 1;


  size_t size_weight;

  size_weight = (_num_layers * _nn_parameter.hid) + _nn_parameter.hid;
  
  // write weight
   
  ConfigurationPtr weight_config( new ConfigurationImpl( "Weight" ) );

  model_config->addSubsection( weight_config );

  double *weight_values = new double[size_weight];


  int siz = 0;
 
  for(int i = 0; i < 3; i++){

    for(int j = 0; j < layers[i]; j++){

	if(i != 2){ //Last layer does not require weights

	  for(int k = 0; k < layers[i+1]; k++){
	    weight_values[siz] = network.getWeight(i, j, k); //Get weights
	    siz ++;	  }
	}
    }  }

  weight_config->addNameValue( "Values", weight_values,  size_weight);


  // write bias
  size_t size_bias;

  size_bias = _nn_parameter.hid + 1;

  ConfigurationPtr bias_config( new ConfigurationImpl( "Bias" ) );

  model_config->addSubsection( bias_config );

  double *bias_values = new double[size_bias];


  siz = 0;
 
  for(int i = 0; i < 3; i++){

    for(int j = 0; j < layers[i]; j++){

	if(i != 0){ //First layer does not need biases

	  bias_values[siz] = network.getBias(i, j); //Get bias
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
  _nn_parameter.hid = model_config->getAttributeAsInt( "HiddenLayerNeurons", 8 );


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


  /*************************************/
  /* Print Values of Weights*/
  /*
  unsigned int siz = 0;

  for(int i = 0; i < 3; i++){
    for(int j = 0; j < layers[i]; j++){
	if(i != 2){ //Last layer does not require weights
	  for(int k = 0; k < layers[i+1]; k++){
  	    printf("\n[%d]: %f", siz, weight_values[siz]); 
	    siz ++;
	  }
	}
    }
  }*/
  /*************************************/

  unsigned int siz = 0;

  for(int i = 0; i < 3; i++){

    for(int j = 0; j < layers[i]; j++){

	if(i != 2){ //Last layer does not require weights

	  for(int k = 0; k < layers[i+1]; k++){

	    network.setWeight(i, j, k, weight_values[siz]); //Set weights
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


  /*************************************/
  /* Print Values of Biases*/
  /*
  siz = 0;
 
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < layers[i]; j++){
	if(i != 0){ //First layer does not need biases
	  printf("\n[%d]: %f", siz, bias_values[siz]);
	  siz++;
	}
    }
  }*/
  /*************************************/

  siz = 0;
 
  for(int i = 0; i < 3; i++){

    for(int j = 0; j < layers[i]; j++){

	if(i != 0){ //First layer does not need biases

	  network.setBias(i, j, bias_values[siz]); //Set bias
	  siz++;
	}
    }
  }

  _done = true;
}
