#ifndef _LIBNN_H
#define _LIBNN_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WIN32
   #include <sys/time.h>
   #include <sys/resource.h>
#else
   #include <time.h>
#endif


#include <ctype.h>
#include <float.h>
#include <string.h>
#include <stdarg.h>


struct nn_parameter
{
	// General Variables
	int inp;
	int hid;
	int outp;
	int pattern;
	double learning_rate;
	double momentum;

	// Choice Variable between training by epoch or minimum error
	int choice;

	// Variables especified of training by epoch
	double epoch;

	// Variables especified of training by minimum error
	double minimum_error;
};


/*
 * Neural Network was based on the algorithm created by Paras Chopra. 
 * Email: paras1987@gmail.com
 * Web: www.paraschopra.com
 * 
 * Changes made in the algorithm are shown with A&F at the beginning of the sentence.
 * Amended by Alex Oshika Avilla & Fabricio Augusto Rodrigues
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h> //For tanh


/*A&F*/// returns a float in the range -1.0f -> - 1.0f
#define RANDOM_CLAMP    (((double)rand()-(float)rand())/10000000)

/*A&F*///returns a float between 0 & 1
#define RANDOM_NUM		((double)rand()/(10000000+1))

//using namespace std;


/*A&F*/class Dendrite{

  public:

    // Weights of the neuron
    double d_weight;
    double d_weight_ancient;
    double d_weight_actual;
    unsigned long d_points_to; // The index of the neuron of the next layer to which it points

    Dendrite(double weight = 0.0, double weight_ancient = 0.0, double weight_actual = 0.0, unsigned long points_to = 0){ //Constructor

	d_weight = weight;
	d_weight_ancient = weight_ancient;
	d_weight_actual = weight_actual;
	d_points_to = points_to; // Give it a initial value
    }
};


class Neuron{

  public:
  
    unsigned long n_ID; // ID of a particular neuron in a layer. Used to find a particular neuron in an array
    double n_value; // Value which Neuron currently is holding
    double n_bias;  // Bias of the neuron
    double n_delta; // Used in back  prop. Note it is backprop specific

    Dendrite *Dendrites; // Dendrites

    // Constructor assigning initial values
    Neuron(unsigned long ID = 0,double value = 0.0,double bias = 0.0){

	n_ID = ID;
	n_value = value;
        n_bias = bias;
        n_delta = 0.0;
    }

    void SetDendrites(unsigned long dendrite){ // Set the dendrites from the neuron to given dendrite

	Dendrites = new Dendrite[dendrite];

        for(unsigned long i = 0; i < dendrite; i++){

	  Dendrites[i].d_points_to = i; // Initialize the dendrite to attach to next layer
        }
    }
};


class Layer{

  public:
  
    Neuron *Neurons; // Pointer to array of neurons

    /*Layer(int size = 1){    // Size is no. of neurons in it

	Neurons = new Neuron [size];
    }  */

    void Initialize(int size) {   // Initialize the layer

	Neurons = new Neuron [size];
    }

    ~Layer(){ // Destructor deletes Neurons from the memory

	delete Neurons;
    }

    Neuron GetNeuron(int index){  // Give the neuron at index

	return Neurons[index];
    }

    void SetNeuron(Neuron neuron, int index){ // Sets the neuron

	Neurons[index] = neuron;
    }
};


/*A&F*/class Network { // The real neural network

  public:
  
    double net_learning_rate; // Learning rate of network
    Layer *Layers; // The total layers in network
    int net_tot_layers; // Number of layers
    double *net_inputs; // Input array
    double *net_outputs;// Output layers
    int *net_layers; // Array which tells no. of neurons in each layer
    //double GetRand(void);

    double *square_error; // Mean square error of each training
    double *mean_square_error; // Total plus of each epoch

    float progress; // Percent of training

    Network() {
	// Blank Constructor
    }


    int SetData(double learning_rate, int layers[], int tot_layers) { // Function to set various parameters of the net
                
	if (tot_layers<2) return(-1); // Return error if total no. of layers < 2
                                               // Because input and output layers are necessary
                
	net_learning_rate = learning_rate;

        net_layers = new int [tot_layers]; // Initialize the layers array

        Layers = new Layer[tot_layers];


        for(int i = 0; i < tot_layers; i++){

          net_layers[i] = layers[i];

          Layers[i].Initialize(layers[i]); // Initialize each layer with the specified size
        }


	net_inputs = new double[layers[0]];

        net_outputs = new double[layers[tot_layers-1]];

        net_tot_layers = tot_layers;


        return 0;
    }

		
    void SetInputs(vector<double> inputs) const{ // Function to set the inputs
                 
	for(int i = 0; i < net_layers[0]; i++){

	  Layers[0].Neurons[i].n_value = inputs[i];
        }
    }


    void RandomizeWB(void){ // Randomize weights and biases

	int i,j,k;

        for(i = 0; i < net_tot_layers; i++){

          for(j = 0; j < net_layers[i]; j++){

            if(i != net_tot_layers-1){ // Last layer does not require weights

		Layers[i].Neurons[j].SetDendrites(net_layers[i+1]); //Initialize the dendrites

		for(k = 0; k < net_layers[i+1]; k++){

		  Layers[i].Neurons[j].Dendrites[k].d_weight = 0.000000; // Let weight be the zero value
		  Layers[i].Neurons[j].Dendrites[k].d_weight_ancient = 0.000000; // Let weight be the zero value
		  Layers[i].Neurons[j].Dendrites[k].d_weight_actual = GetRand(); // Let weight be the random value
	        }
		
             }

             if(i != 0){ // First layer does not need biases

		Layers[i].Neurons[j].n_bias = GetRand();
             }
	  }
	}
    }
	

    double * GetOutput(void) const{ // Gives the output of the net
    
	double *outputs;
	int i,j,k;

	outputs = new double[net_layers[net_tot_layers-1]]; // Temp ouput array

	for(i = 1; i < net_tot_layers; i++){

	  for(j = 0; j < net_layers[i]; j++){

            Layers[i].Neurons[j].n_value = 0;
                          
	    for(k = 0; k < net_layers[i-1]; k++){

		Layers[i].Neurons[j].n_value = Layers[i].Neurons[j].n_value + Layers[i-1].Neurons[k].n_value * Layers[i-1].Neurons[k].Dendrites[j].d_weight; // Multiply and add all the inputs
	    }

            Layers[i].Neurons[j].n_value = Layers[i].Neurons[j].n_value + Layers[i].Neurons[j].n_bias; // Add bias

            Layers[i].Neurons[j].n_value = Limiter(Layers[i].Neurons[j].n_value);  // Squash that value

          }
	}

        for(i = 0; i < net_layers[net_tot_layers-1]; i++){

	  outputs[i] = Layers[net_tot_layers-1].Neurons[i].n_value;
	}

	return outputs; // Return the outputs
    }


    /******************************************************************/
    // Used at _GetConfiguration and _SetConfiguration in the library nn_alg.cpp

    double getWeight(int i, int j, int k) const{ // Get weights

	return Layers[i].Neurons[j].Dendrites[k].d_weight;
    }


    void setWeight(int i, int j, int k, double w){ // Set weights
	
	Layers[i].Neurons[j].Dendrites[k].d_weight = w;

    }


    double getBias(int i, int j) const{ // Get bias

	return Layers[i].Neurons[j].n_bias;
    }


    void setBias(int i, int j, double b){ // Set bias

	Layers[i].Neurons[j].n_bias = b;
    }
    /******************************************************************/


    void Update(void){ // Just a dummy function

	double *temp; // Temperory pointer
        temp = GetOutput();
        //GetOutput();
        delete[] temp;
    }


     /*void SetOutputs(double outputs[]){ //Set the values of the output layer

	for(unsigned long i = 0; i < net_layers[net_tot_layers-1]; i++){

	  Layers[net_tot_layers-1].Neurons[i].n_value = outputs[i]; //Set the value
        }
    } */


    double Limiter(double value) const{ // Limiet to limit value between 1 and -1

	//return tanh(value);   // To use tanh fuction
        return (1.0/(1+exp(-value))); // To use sigmoid function
    }


    double GetRand(void){  // Return a random number between range -1 e 1 using time to seed the srand function

	int seconds;

#ifndef WIN32
        struct timeval time;
        gettimeofday( &time, (struct timezone *)NULL );
        seconds = (int)time.tv_usec;
#else
        time_t timer = time( NULL );
        struct tm *tblock = localtime(&timer);
        seconds = tblock->tm_sec;
#endif

        int seed = int(seconds + 100*RANDOM_CLAMP + 100*RANDOM_NUM);
        //srand(seconds);
        srand(seed);

#ifdef _GLIBCPP_HAVE_DRAND48
        srand48(seed);
#endif

        return ((RANDOM_CLAMP+RANDOM_NUM)/400);
   }


    double SigmaWeightDelta( unsigned long layer_no, unsigned long neuron_no){ // Calculate sum of weights * delta. Used in back prop. layer_no is layer number. Layer number and neuron number can be zero and neuron_no is neuron number.

	double result = 0.0;
 
	for(int i = 0; i < net_layers[layer_no+1]; i++) { // Go through all the neurons in the next layer

	  result = result + Layers[layer_no].Neurons[neuron_no].Dendrites[i].d_weight * Layers[layer_no+1].Neurons[i].n_delta; // Comput the summation
        }

	return result;
    }


/*
    For output layer:

    Delta = (TargetO - ActualO) * ActualO * (1 - ActualO)
    Weight = Weight + LearningRate * Delta * Input

    For hidden layers:

    Delta =  ActualO * (1-ActualO) * Summation(Weight_from_current_to_next AND Delta_of_next)
    Weight = Weight + LearningRate * Delta * Input
*/


    void setError(int max_pattern){ // Function to set the errors

	mean_square_error = new double;
	*mean_square_error = 0.000000;
	
	square_error = new double [max_pattern];

	for(int i = 0; i < max_pattern; i++){

	  square_error[i] = 0.000000;
	}
    }


    void addError(int max_pattern){ // Function to add the errors

	for(int i = 0; i < max_pattern; i++){

	  *mean_square_error += square_error[i];
	}
	
	delete[] square_error;
    }


    int Train(vector<double> inputs, vector<double> outputs, int number_pattern, int max_pattern, double momentum){ // The standard Backprop Learning algorithm

	int i,j,k;

	double *Target = new double;
	double *Delta = new double;
	double *Actual = new double;
	double *error = new double;;


        SetInputs(inputs); // Set the inputs

	Update(); // Update all the values

        //SetOutputs(outputs); // Set the outputs


	if(number_pattern == 0){
	
	  setError(max_pattern);
	}


        for(i = (net_tot_layers-1); i > 0; i--){ // Go from last layer to first layer

          for(j = 0; j < net_layers[i]; j++) {// Go thru every neuron

            if(i == (net_tot_layers-1)){ // Output layer, Needs special atential

		(*Target) = outputs[j]; // Target value
		(*Actual) = Layers[i].Neurons[j].n_value; // Actual value
                           
		(*Delta) = ((*Target) - (*Actual)) * (*Actual) * (1 - (*Actual)); // Function to compute error
                           
		Layers[i].Neurons[j].n_delta = (*Delta); // Compute the delta


                for(k = 0; k < net_layers[i-1]; k++) {

	          Layers[i-1].Neurons[k].Dendrites[j].d_weight = ( Layers[i-1].Neurons[k].Dendrites[j].d_weight_actual) + ( net_learning_rate * (*Delta) * Layers[i-1].Neurons[k].n_value) + (momentum * ( Layers[i-1].Neurons[k].Dendrites[j].d_weight_actual - Layers[i-1].Neurons[k].Dendrites[j].d_weight_ancient)); // Calculate the new weights

	          Layers[i-1].Neurons[k].Dendrites[j].d_weight_ancient = Layers[i-1].Neurons[k].Dendrites[j].d_weight_actual;
		  Layers[i-1].Neurons[k].Dendrites[j].d_weight_actual = Layers[i-1].Neurons[k].Dendrites[j].d_weight;	
                }

                Layers[i].Neurons[j].n_bias = Layers[i].Neurons[j].n_bias + (*Delta) * net_learning_rate * 1; // n_value is always 1 for bias


		*error = ((*Target) - (*Actual));
	    }
	    else { // Here
                           
		// Target value
                (*Actual) = Layers[i].Neurons[j].n_value; // Actual value

                (*Delta) =  (*Actual) * (1 - (*Actual)) * SigmaWeightDelta(i,j); // Function to compute error


                for(k = 0; k < net_layers[i-1]; k++) {

	          Layers[i-1].Neurons[k].Dendrites[j].d_weight = ( Layers[i-1].Neurons[k].Dendrites[j].d_weight_actual) + ( net_learning_rate * (*Delta) * Layers[i-1].Neurons[k].n_value) + (momentum * ( Layers[i-1].Neurons[k].Dendrites[j].d_weight_actual - Layers[i-1].Neurons[k].Dendrites[j].d_weight_ancient)); // Calculate the new weights

	          Layers[i-1].Neurons[k].Dendrites[j].d_weight_ancient = Layers[i-1].Neurons[k].Dendrites[j].d_weight_actual;
		  Layers[i-1].Neurons[k].Dendrites[j].d_weight_actual = Layers[i-1].Neurons[k].Dendrites[j].d_weight;	
                }


		if(i != 0){ // Input layer does not have a bias

		  Layers[i].Neurons[j].n_bias = Layers[i].Neurons[j].n_bias + (*Delta) * net_learning_rate * 1; // n_value is always 1 for bias
		}
	    }
	  }
	}


	square_error[number_pattern] = (*error) * (*error); 


	if(number_pattern == (max_pattern - 1)){

	  addError(max_pattern);
	}


	delete Target;
	delete Actual;
	delete Delta;
	delete error;


	return 0;
    }


    // Used to training by epoch
    void trainingEpoch( unsigned long actual_epoch, double epoch_total, int patterns){

	*mean_square_error = sqrt(*mean_square_error/patterns);


	progress = (float)(1.0-(epoch_total - (actual_epoch+1.0))/(epoch_total));

	delete mean_square_error;
    }


    // Used to training by minimum error
    int trainingMinimumError( int patterns, double min_error){

	int converg = 0;

	*mean_square_error = sqrt(*mean_square_error/patterns);

	
	if(*mean_square_error < min_error){

	  converg = 1;
	  progress = 1;
	}

	else{

	  progress = (float)(1.0-(*mean_square_error - min_error)/(*mean_square_error));
	}


	delete mean_square_error;

	return converg;
    }


    // Percent of training
    float getProgress(){

	return progress;
    }



    ~Network(){ }//delete Layers; }

};


#ifdef __cplusplus
}
#endif

#endif /* _LIBNN_H */
