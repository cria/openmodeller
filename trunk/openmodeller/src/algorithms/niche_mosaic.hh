/**
* Declaration of class NicheMosaic
*
* @author Missae Yamamoto (missae at dpi . inpe . br)
* $Id$
*
* LICENSE INFORMATION
* 
* Copyright(c) 2009 by INPE -
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

#ifndef NICHEMOSAIC_HH
#define NICHEMOSAIC_HH

#include <openmodeller/om.hh>
#include <openmodeller/Exceptions.hh>

typedef std::vector<Scalar> ScalarVector;

class NicheMosaic : public AlgorithmImpl
{
  private:

    int _num_iterations;               // number of iterations
    int _num_points;                   // number of points.
    size_t _num_points_test;           // number of points (test).
    size_t _num_points_absence_test;   // number of points (absence test).
    int _num_layers;                   // number of layers.
    Sample _minimum;                   // minimum of sampled points.
    Sample _maximum;	               // maximum of sampled points.
    Sample _delta;                     // delta of sampled points.
    OccurrencesPtr _my_presences;      // occurrence points of species.
    OccurrencesPtr _my_presences_test; // occurrence points of species (test).
    OccurrencesPtr _my_absence_test;   // occurrence points of species (absence test).
    Scalar _bestCost;                  // best cost
    bool _done;                        // is true if the algorithm is finished.
    float _progress;                   // iteration progress
    std::vector<ScalarVector> _model_min_best;
    std::vector<ScalarVector> _model_max_best;

  protected:

    void _getConfiguration( ConfigurationPtr& ) const;
    void _setConfiguration( const ConstConfigurationPtr & );

  public:

   NicheMosaic(); //constructor
   ~NicheMosaic(); //destructor

   int initialize();
   int iterate();
   int done() const { return _done; }
   float getProgress() const;

   Scalar getValue( const Sample& x ) const;

   //set minimum, maximum and delta for each layer.
   int setMinMaxDelta();

   //create rules
   void createModel( std::vector<ScalarVector> &_model_min, std::vector<ScalarVector> &_model_max, const std::vector<Scalar> &delta );

   //edit rules
   void editModel( std::vector<ScalarVector> &model_min, std::vector<ScalarVector> &model_max, const std::vector<Scalar> &delta, size_t i_layer );

   //verify test data for presence
   size_t calculateCostPres( const std::vector<ScalarVector> &_model_min, const std::vector<ScalarVector> &_model_max );

   //verify test data for absence
   size_t calculateCostAus( const std::vector<ScalarVector> &_model_min, const std::vector<ScalarVector> &_model_max );

   //generate random layer number.
   size_t getRandomLayerNumber();

   //return random percent.
   Scalar getRandomPercent(const std::vector<Scalar> &delta, const size_t i_layer, size_t &cost1);

   //renew tabu degree list
   void renewTabuDegree(std::vector<size_t> &tabuDegree);

   //save best model
   void saveBestModel(const std::vector<ScalarVector> &model_min, const std::vector<ScalarVector> &model_max);

   //improve model
   void improveModel(const std::vector<Scalar> &deltaBest);

   //Find solution.
   void findSolution(size_t &costBest, std::vector<Scalar> &deltaBest, int &bestIter, size_t &bestCost2);

};


#endif

