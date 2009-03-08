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

#include "niche_mosaic.hh"

#include <openmodeller/Configuration.hh>

#include <openmodeller/Random.hh>

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

/****************************************************************/
/********************** Algorithm's Metadata ********************/

#define NUM_PARAM 1

#define NUMITERATIONS_ID "NumberOfIterations"

/*************************************/
/*** Algorithm parameters metadata ***/

static AlgParamMetadata parameters[NUM_PARAM] = {

  // Metadata of the first parameter.
  {
    NUMITERATIONS_ID,       // Id
    "Number of iterations", // Name.
    Integer,                // Type.

    // Overview
    "Number of iterations.",

    // Description.
    "Number of iterations.",

    1,       // Not zero if the parameter has lower limit.
    1000,    // Parameter's lower limit.
    0,       // Not zero if the parameter has upper limit.
    0,       // Parameter's upper limit.
    "2000"   // Parameter's typical (default) value.
  },
};

/************************************/
/*** Algorithm's general metadata ***/

static AlgMetadata metadata = {

  "NICHE_MOSAIC",   // Id.
  "Niche Mosaic",   // Name.
  "0.1",    // Version.

  // Overview
  "This algorithm is still experimental. You may not use it in publications without the author's permission.",

  // Description.
  "This algorithm is still experimental. You may not use it in publications without the author's permission.",

  "Missae Yamamoto",  // Author

  // Bibliography.
  "",

  "Missae Yamamoto",         // Code author.
  "missae [at] dpi.inpe.br", // Code author's contact.

  0,  // Does not accept categorical data.
  0,  // Does not need (pseudo)absence points.

  NUM_PARAM,   // Algorithm's parameters.
  parameters
};

/****************************************************************/
/****************** Algorithm's factory function ****************/

OM_ALG_DLL_EXPORT
AlgorithmImpl *
algorithmFactory()
{
  return new NicheMosaic();
}

OM_ALG_DLL_EXPORT
AlgMetadata const *
algorithmMetadata()
{
  return &metadata;
}

/*************************************************************/
/************************ Niche Mosaic ***********************/

NicheMosaic::NicheMosaic() :
  AlgorithmImpl( &metadata ),
  _num_iterations(0),
  _num_points(0),
  _num_points_test(0),
  _num_points_absence_test(0),
  _num_layers(0),
  _minimum(),
  _maximum(),
  _delta(),
  _my_presences(0),
  _my_presences_test(0),
  _my_absence_test(0),
  _bestCost(0),
  _done( false ),
  _progress( 0.0 )
{
}

NicheMosaic::~NicheMosaic()
{
}

/******************/
/*** initialize ***/
int
NicheMosaic::initialize()
{
  // Check the number of presences
  int num_presences = _samp->numPresence();

  if ( num_presences == 0 ) {

    Log::instance()->warn( "No presence points inside the mask!\n" );
    return 0;
  }

  // Check number of layers
  _num_layers = _samp->numIndependent();

  if ( _num_layers < 2 ) 
  {
    std::string msg = "Tabu needs at least 2 layers.\n";

    Log::instance()->error( msg.c_str() );
    return 0;
  }

  // Get parameters
  if ( ! getParameter( NUMITERATIONS_ID, &_num_iterations ) ) {

    Log::instance()->error( "Parameter '" NUMITERATIONS_ID "' not passed.\n" );
    return 0;
  }

  if ( _num_iterations < 1000 ) {

    Log::instance()->error( "Parameter '" NUMITERATIONS_ID "' must be greater than 999.\n" );
    return 0;
  }

  int num_absences = _samp->numAbsence();

  if ( num_absences <= 0 ) {

    // generate pseudo absence points
    string alg_id = "BIOCLIM";
    AlgorithmPtr alg = AlgorithmFactory::newAlgorithm( alg_id );

    if ( ! alg ) {

      Log::instance()->error( "Could not instantiate BIOCLIM algorithm to generate pseudo-absences." );
      return 0;
    }

    ParamSetType param;
    param["StandardDeviationCutoff"] = "0.9";

    alg->setParameters( param );
    alg->createModel( _samp );

    _my_absence_test = _samp->getPseudoAbsences( 100, alg->getModel(), 0.6 );
  }
  else {

    _my_absence_test = _samp->getAbsences();
  }

  _num_points_absence_test = _my_absence_test->numOccurrences(); 

  _my_presences = new OccurrencesImpl( _my_absence_test->name(), _my_absence_test->coordSystem() );

  _my_presences_test = new OccurrencesImpl( _my_absence_test->name(), _my_absence_test->coordSystem() );

  return 1;
}


/***************/
/*** iterate ***/
int
NicheMosaic::iterate()
{
  // Split sampler in test/train
  splitOccurrences( _samp->getPresences(), _my_presences, _my_presences_test, 0.7 );

  _num_points_test = _my_presences_test->numOccurrences();

  _num_points = _my_presences->numOccurrences();

  if ( 0 == setMinMaxDelta() ) {

    return 0;
  }

  findSolution();

  _done = 1;

  return 1;
}

float 
NicheMosaic::getProgress() const
{
  if (done()) {

    return 1.0;
  }
  else {

    return _progress;
  }
}

/*****************/
/*** get Value ***/
Scalar
NicheMosaic::getValue( const Sample& x ) const
{
  int i, j = 0;

  //_num_points represents the number of rules
  for (i = 0; i < _num_points; i++) {

    for (j = 0; j < _num_layers; j++) {

      if ( ( _model_min_best[i][j] <= x[j] ) && ( x[j] <= _model_max_best[i][j] ) )
        continue;
      else
        break;
    }//end for

    if ( j == _num_layers ) {
      return 1.0;
    }//end if
  }//end for

  return 0.0;
}

int 
NicheMosaic::setMinMaxDelta()
{
  OccurrencesImpl::const_iterator oc = _my_presences->begin();
  OccurrencesImpl::const_iterator end = _my_presences->end();

  Sample const & sample = (*oc)->environment();
  _minimum = sample;
  _maximum = sample;
  ++oc;

  while ( oc != end ) {
    Sample const& sample = (*oc)->environment();
    _minimum &= sample;
    _maximum |= sample;
    ++oc;
  }

  _delta = _maximum;
  _delta -= _minimum;

  for ( int j = 0; j < _num_layers; j++ ) {
    if (_delta[j] == 0.0) {
      Log::instance()->error( "No delta for layer %d\n", j );
      return 0;
    }
  }

  return 1;
}

void 
NicheMosaic::createModel( std::vector<ScalarVector> &model_min, std::vector<ScalarVector> &model_max, const std::vector<Scalar> &delta )
{
  size_t i=0;
  OccurrencesImpl::const_iterator oc = _my_presences->begin();
  OccurrencesImpl::const_iterator end = _my_presences->end();

  while ( oc != end ) {
    Sample const& sample = (*oc)->environment();

    for ( int j = 0; j < _num_layers; j++ ) {
      model_min[i][j] = sample[j] - delta[j];
      model_max[i][j] = sample[j] + delta[j];
    }//end for

    ++oc;
    ++i;
  }//end while
}

void 
NicheMosaic::editModel( std::vector<ScalarVector> &model_min, std::vector<ScalarVector> &model_max, const std::vector<Scalar> &delta, size_t i_layer )
{
  size_t i=0;
  OccurrencesImpl::const_iterator oc = _my_presences->begin();
  OccurrencesImpl::const_iterator end = _my_presences->end();

  while ( oc != end ) {
    Sample const& sample = (*oc)->environment();

    model_min[i][i_layer] = sample[i_layer] - delta[i_layer];
    model_max[i][i_layer] = sample[i_layer] + delta[i_layer];

    ++oc;
    ++i;
  }//end while
}

size_t
NicheMosaic::calculateCostPres( const std::vector<ScalarVector> &model_min, const std::vector<ScalarVector> &model_max )
{
  OccurrencesImpl::iterator it = _my_presences_test->begin(); 
  OccurrencesImpl::iterator last = _my_presences_test->end();

  int i, j, npresence = 0;

  //presence
  while ( it != last ) {

    Sample const& sample = (*it)->environment();

    //_num_points eh o numero de regras do modelo
    for (i = 0; i < _num_points; i++) {

      for (j = 0; j < _num_layers; j++) {

        if ( ( model_min[i][j] <= sample[j] ) && ( sample[j] <= model_max[i][j] ) )
          continue;
        else
          break;
      }//end for

      if ( j == _num_layers ) {
        npresence++;
	break;
      }//end if
    }//end for
    ++it;
  }//end while

  return npresence;
}

size_t
NicheMosaic::calculateCostAus( const std::vector<ScalarVector> &model_min, const std::vector<ScalarVector> &model_max )
{
  int i, j, nabsence = 0;

  //absence
  OccurrencesImpl::iterator it_absence = _my_absence_test->begin(); 
  OccurrencesImpl::iterator last_absence = _my_absence_test->end();

  while ( it_absence != last_absence ) 
  {     
    Sample const& samp = (*it_absence)->environment();

    for (i = 0; i < _num_points; i++){ //_num_points eh o numero de regras do modelo

      for (j = 0; j < _num_layers; j++){

        if ( ( model_min[i][j] <= samp[j] ) && ( samp[j] <= model_max[i][j] ) )
          continue;
        else
          break;
      }//end for

      if ( j == _num_layers )
        break;
    }//end for

    if ( i == _num_points)
      nabsence++;

    ++it_absence;
  }//end while

  return nabsence;
}

size_t 
NicheMosaic::getRandomLayerNumber()
{
  Random random;

  return random( 0, _num_layers );
}

Scalar 
NicheMosaic::getRandomPercent(const std::vector<Scalar> &delta, const size_t i_layer, size_t &costPres, size_t &costAus)
{
  int size = 22;

  Scalar percent[22] = { 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1, 0.11, 0.12, 0.13, 0.14, 0.15, 0.16, 0.17, 0.18, 0.19, 0.2, 0.21, 0.22, 0.23, 0.24, 0.25 };

  int half = (int)(size/2);

  int r;

  Random random;

  do{

    r = random( 0, size );

  } while (delta[i_layer] == _delta[i_layer]*percent[r]);

  if ( (costPres < _num_points_test) && (r < half) ){

    r = r + half;
  }

  return percent[r];
}

void
NicheMosaic::renewTabuDegree(std::vector<size_t> &tabuDegree)
{
  for (int i = 0; i < _num_layers; i++) {

    if (tabuDegree[i] > 0)
      tabuDegree[i] = tabuDegree[i] - 1;
  }
}

void
NicheMosaic::saveBestModel(const std::vector<ScalarVector> &model_min, const std::vector<ScalarVector> &model_max)
{
  for (int i = 0; i < _num_points; i++) {
    for (int j = 0; j < _num_layers; j++) {
      _model_min_best[i][j] = model_min[i][j];
      _model_max_best[i][j] = model_max[i][j];
    }
  }
}

void 
NicheMosaic::findSolution()
{
  size_t cost, cost1, cost2, cost1Aux, i_layer;
  std::vector<Scalar> delta( _num_layers );

  size_t nTabu = (size_t)floor(sqrt((double)(_num_layers)));
  std::vector<size_t> tabuDegree( _num_layers );

  for( int l = 0; l < _num_layers; l++ ) 
    tabuDegree[l] = 0;

  //model
  std::vector<ScalarVector> model_min( _num_points );
  std::vector<ScalarVector> model_max( _num_points );
  _model_min_best.resize( _num_points );
  _model_max_best.resize( _num_points );

  for ( unsigned int i = 0; i < model_min.size(); i++ ) {

    model_min[i] = ScalarVector( _num_layers );
    model_max[i] = ScalarVector( _num_layers );
    _model_min_best[i] = ScalarVector( _num_layers );
    _model_max_best[i] = ScalarVector( _num_layers );
  }//end for

  for ( int j = 0; j < _num_layers; j++ ){

    delta[j] = _delta[j] * 0.13;
  }

  createModel( model_min, model_max, delta );
  cost1 = calculateCostPres( model_min, model_max );
  cost1Aux = cost1;
  cost2 = calculateCostAus( model_min, model_max );
  cost = cost1 + cost2;
  _bestCost = cost;

  for (int iter=0; iter < _num_iterations; iter++) {

    _progress = (float)iter/(float)_num_iterations;

    i_layer = getRandomLayerNumber();

    if (tabuDegree[i_layer] == 0) {

      renewTabuDegree(tabuDegree);
      tabuDegree[i_layer] = nTabu;
      delta[i_layer] = _delta[i_layer] * getRandomPercent(delta, i_layer, cost1, cost2);
      editModel( model_min, model_max, delta, i_layer );
      cost1 = calculateCostPres( model_min, model_max );
      cost2 = calculateCostAus( model_min, model_max );
      cost = cost1 + cost2;

      if ( (cost > _bestCost) || ( (cost == _bestCost) && (cost1 >= cost1Aux) ) ) {
        cost1Aux = cost1;
        _bestCost = cost;
        saveBestModel(model_min, model_max);
        if (_bestCost == (_num_points_test + _num_points_absence_test) )
          break;
      }//end if
    }//end if
  }//end for
}

/****************************************************************/
/****************** configuration *******************************/
void
NicheMosaic::_getConfiguration( ConfigurationPtr& config ) const
{
  if ( !_done )
    return;

  ConfigurationPtr model_config( new ConfigurationImpl( "NicheMosaic" ) );
  config->addSubsection( model_config );

  model_config->addNameValue( "NumLayers", _num_layers );
  model_config->addNameValue( "NumPoints", _num_points );

  for (int i = 0; i < _num_points; i++) {

    ConfigurationPtr rule_config( new ConfigurationImpl( "Rule" ) );
    model_config->addSubsection( rule_config );

    Sample min_best( _model_min_best[i] );
    Sample max_best( _model_max_best[i] );

    rule_config->addNameValue( "Min", min_best );
    rule_config->addNameValue( "Max", max_best );
  }
}

void
NicheMosaic::_setConfiguration( const ConstConfigurationPtr& config )
{
  ConstConfigurationPtr model_config = config->getSubsection( "NicheMosaic" );

  if (!model_config)
    return;

  _num_layers = model_config->getAttributeAsInt( "NumLayers", 0 );
  _num_points = model_config->getAttributeAsInt( "NumPoints", 0 );

  _model_min_best.resize( _num_points );
  _model_max_best.resize( _num_points );

  Configuration::subsection_list subelements = model_config->getAllSubsections();

  Configuration::subsection_list::iterator subelement = subelements.begin();
  Configuration::subsection_list::iterator last_subelement = subelements.end();

  int i = 0;

  for ( ; subelement != last_subelement; ++subelement ) {

    if ( (*subelement)->getName() == "Rule" ) {

      _model_min_best[i] = (*subelement)->getAttributeAsVecDouble( "Min" );
      _model_max_best[i] = (*subelement)->getAttributeAsVecDouble( "Max" );

      ++i;
    }
  }

  _done = true;

  return;
}
