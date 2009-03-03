
/**
* Declaration of class PreChiSquare
*
* @author Missae Yamamoto (missae at dpi . inpe . br)
* $Id$
*
* LICENSE INFORMATION
* 
* Copyright(c) 2008 by INPE -
* Instituto Nacional de Pesquisas Espaciais
*
* http://www.inpe.br
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

#ifndef PRE_CHISQUARE_HH
#define PRE_CHISQUARE_HH

#include "PreAlgorithm.hh"

#include <openmodeller/om.hh>
#include <openmodeller/Exceptions.hh>
#include <openmodeller/Sample.hh>
#include <openmodeller/Sampler.hh>

#define classLimit 16

/*
 * Chi Square algorithm described at Li, L., et al. (2006). "An Integrated
 * Bayesian Modelling Approach for Predicting Mosquito Larval Habitats". The
 * authors tested the independence between all pairs of layers. They crossed
 * two layers by using a contingence matrix to detect the dependence between
 * them conditioning on the presence of habitat. The rows of the contingence
 * matrix are values of one layer, and the columns of the matrix are values of
 * the other variable.  Each cell of the matrix records the number of
 * occurrence of specie , that is calculated by considering the class of the
 * two layers. Given the significance level of 0.05, the Chi-square test is
 * then applied to evaluate the independence between two crossed layers.
 *
 * input: SamplerPtr output: shows for each variable the amount of layers that
 * maintain correlation at the significance level of 0.05
 */

class dllexp PreChiSquare : public PreAlgorithm 
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
	std::vector< size_t > statistic1; //dependency between layers.
	std::vector< size_t > statistic2; //dependency between layers.

  public:

    PreChiSquare(); //constructor
    ~PreChiSquare(); //destructor

    //Checks if the supplied parameters fits the requirements of PRE algorithm implementation.
    //return true if the parameters are OK. false if not.
	bool checkParameters( const PreParameters& parameters ) const;

    //Runs the current algorithm implementation.
    //return true if OK. false on error.
    bool runImplementation();

 	//get input parameters
	void getAcceptedParameters ( stringMap& info );

	//get output information
	void getLayersetResultSpec ( stringMap& info);

	//get output information for each layer
	void getLayerResultSpec ( stringMap& info);

    //initialize attributes: num_points, num_layers, nclass, minimum, delta, my_presences.
    void init();

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
    void run();

    //set zero to all elements of the matrix "measured".
    void setMeasuredZero();

};


#endif

