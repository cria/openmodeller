//****************************************************** Chi-square **************************************************
// 
// Chi Square algorithm described at Li, L., et al. (2006). "An Integrated Bayesian Modelling Approach for Predicting 
// Mosquito Larval Habitats". The authors tested the independence between all pairs of layers. They crossed two layers 
// by using a contingence matrix to detect the dependence between them conditioning on the presence of habitat. The rows 
// of the contingence matrix are values of one layer, and the columns of the matrix are values of the other variable. 
// Each cell of the matrix records the number of occurrence of specie , that is calculated by considering the class of 
// the two layers. Given the significance level of 0.05, the Chi-square test is then applied to evaluate the independence 
// between two crossed layers.
//
// input: SamplerPtr
// output: shows for each variable the amount of layers that maintain correlation at the significance level of 0.05
//
//**********************************************************************************************************************

#ifndef _CHISQUARE_
#define _CHISQUARE_

#include <openmodeller/om.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/Sample.hh>

#define classLimit 16

class ChiSquare 
{
private:
  size_t	num_points;		  // number of points.
  size_t	num_layers;		  // number of layers.
  size_t	nclass;   // number of class
  Sample	minimum;  // Mininum value for each layer.
  Sample	delta;    // delta value for each layer. 
  OccurrencesPtr my_presences; //occurrence points of species.
  Scalar	measured[classLimit][classLimit]; //measured points between two crossed layers.
  Scalar	expected [classLimit][classLimit]; //expected points between two crossed layers.
  Scalar  	chicell [classLimit][classLimit]; //chi value for each cell formed between two class of the crossed layers. 
  size_t*	statistic1; //dependency between layers.
  size_t*	statistic2; // dependency between layers.

public:

	ChiSquare(SamplerPtr samplerPtr); //constructor
	~ChiSquare(); //destructor
	
	//initialize attributes: num_points, num_layers, nclass, minimum, delta, my_presences.
	void init(SamplerPtr samplerPtr);

	//get number of points.
	size_t getNpoints();

	//set number of points.
	void setNpoints();

	//get number of layers.
	size_t getNlayers();

	//set number of layers. 
	void setNlayers(SamplerPtr samplerPtr);

	//set number of class.
	size_t getNclass();

	//set number of class.
	void setNclass();

	//set mininum value for each layer.
	void setMinimum();

	//get minimum value of each layer.
	Sample getMinimum();

	//set delta value for each layer. 
	void setDelta();

	//get delta value of each layer. 
	Sample getDelta();

	//set measured points for crossed layers.
	void setMeasured(size_t layer1, size_t layer2);

	//set expected points for crossed layers.
	void setExpected();

	//set chi value for each cell formed between two class of the crossed layers. 
	void setChicell();

	//count layer dependency.
	void setStatistic(size_t layer1, size_t layer2);

	//control layers crossing.
	void chiMain();

	//calculate final layers dependency.
	void showResult();

	//set zero to all elements of the matrix "measured".
	void setMeasuredZero();

};


#endif

